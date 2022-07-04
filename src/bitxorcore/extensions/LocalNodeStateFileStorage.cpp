/**
*** Copyright (c) 2016-2019, Jaguar0625, gimre, BloodyRookie, Tech Bureau, Corp.
*** Copyright (c) 2020-2021, Jaguar0625, gimre, BloodyRookie.
*** Copyright (c) 2022-present, Kriptxor Corp, Microsula S.A.
*** All rights reserved.
***
*** This file is part of BitxorCore.
***
*** BitxorCore is free software: you can redistribute it and/or modify
*** it under the terms of the GNU Lesser General Public License as published by
*** the Free Software Foundation, either version 3 of the License, or
*** (at your option) any later version.
***
*** BitxorCore is distributed in the hope that it will be useful,
*** but WITHOUT ANY WARRANTY; without even the implied warranty of
*** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*** GNU Lesser General Public License for more details.
***
*** You should have received a copy of the GNU Lesser General Public License
*** along with BitxorCore. If not, see <http://www.gnu.org/licenses/>.
**/

#include "LocalNodeStateFileStorage.h"
#include "LocalNodeChainScore.h"
#include "LocalNodeStateRef.h"
#include "GenesisBlockLoader.h"
#include "bitxorcore/cache/CacheStorage.h"
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/cache/SupplementalDataStorage.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/config/NodeConfiguration.h"
#include "bitxorcore/consumers/BlockchainSyncHandlers.h"
#include "bitxorcore/io/BlockStorageCache.h"
#include "bitxorcore/io/BufferedFileStream.h"
#include "bitxorcore/io/FilesystemUtils.h"
#include "bitxorcore/io/IndexFile.h"
#include "bitxorcore/plugins/PluginManager.h"
#include "bitxorcore/utils/StackLogger.h"

namespace bitxorcore { namespace extensions {

	// region constants + utils

	namespace {
		constexpr size_t Default_Loader_Batch_Size = 100'000;
		constexpr auto Supplemental_Data_Filename = "supplemental.dat";

		std::string GetStorageFilename(const cache::CacheStorage& storage) {
			return storage.name() + ".dat";
		}
	}

	// endregion

	// region HasSerializedState

	bool HasSerializedState(const config::BitxorCoreDirectory& directory) {
		return std::filesystem::exists(directory.file(Supplemental_Data_Filename));
	}

	// endregion

	// region LoadDependentStateFromDirectory

	namespace {
		io::BufferedInputFileStream OpenInputStream(const config::BitxorCoreDirectory& directory, const std::string& filename) {
			return io::BufferedInputFileStream(io::RawFile(directory.file(filename), io::OpenMode::Read_Only));
		}

		void LoadDependentStateFromDirectory(
				const config::BitxorCoreDirectory& directory,
				cache::BitxorCoreCache& cache,
				cache::SupplementalData& supplementalData) {
			// load supplemental data
			Height chainHeight;
			{
				auto inputStream = OpenInputStream(directory, Supplemental_Data_Filename);
				cache::LoadSupplementalData(inputStream, supplementalData, chainHeight);
			}

			// commit changes
			auto cacheDelta = cache.createDelta();
			cacheDelta.dependentState() = supplementalData.State;
			cache.commit(chainHeight);
		}
	}

	void LoadDependentStateFromDirectory(const config::BitxorCoreDirectory& directory, cache::BitxorCoreCache& cache) {
		cache::SupplementalData supplementalData;
		LoadDependentStateFromDirectory(directory, cache, supplementalData);
	}

	// endregion

	// region LoadStateFromDirectory

	namespace {
		bool LoadStateFromDirectory(
				const config::BitxorCoreDirectory& directory,
				cache::BitxorCoreCache& cache,
				cache::SupplementalData& supplementalData) {
			if (!HasSerializedState(directory))
				return false;

			// 1. load cache data
			utils::StackLogger stopwatch("load state", utils::LogLevel::important);
			for (const auto& pStorage : cache.storages()) {
				auto inputStream = OpenInputStream(directory, GetStorageFilename(*pStorage));
				pStorage->loadAll(inputStream, Default_Loader_Batch_Size);
			}

			// 2. load supplemental data
			LoadDependentStateFromDirectory(directory, cache, supplementalData);
			return true;
		}
	}

	StateHeights LoadStateFromDirectory(
			const config::BitxorCoreDirectory& directory,
			const LocalNodeStateRef& stateRef,
			const plugins::PluginManager& pluginManager) {
		cache::SupplementalData supplementalData;
		if (LoadStateFromDirectory(directory, stateRef.Cache, supplementalData)) {
			stateRef.Score += supplementalData.ChainScore;
		} else {
			auto cacheDelta = stateRef.Cache.createDelta();
			GenesisBlockLoader loader(cacheDelta, pluginManager, pluginManager.createObserver());
			loader.executeAndCommit(stateRef, StateHashVerification::Enabled);
			stateRef.Score += model::ChainScore(1); // set chain score to 1 after processing genesis
		}

		StateHeights heights;
		heights.Cache = stateRef.Cache.createView().height();
		heights.Storage = stateRef.Storage.view().chainHeight();
		return heights;
	}

	// endregion

	// region LocalNodeStateSerializer

	namespace {
		io::BufferedOutputFileStream OpenOutputStream(const config::BitxorCoreDirectory& directory, const std::string& filename) {
			return io::BufferedOutputFileStream(io::RawFile(directory.file(filename), io::OpenMode::Read_Write));
		}

		void SaveStateToDirectory(
				const config::BitxorCoreDirectory& directory,
				const std::vector<std::unique_ptr<const cache::CacheStorage>>& cacheStorages,
				const state::BitxorCoreState& state,
				const model::ChainScore& score,
				Height height,
				const consumer<const cache::CacheStorage&, io::OutputStream&>& save) {
			// 1. create directory if required
			config::BitxorCoreDirectory(directory.path()).create();

			// 2. save cache data
			for (const auto& pStorage : cacheStorages) {
				auto outputStream = OpenOutputStream(directory, GetStorageFilename(*pStorage));
				save(*pStorage, outputStream);
			}

			// 3. save supplemental data
			cache::SupplementalData supplementalData{ state, score };
			auto outputStream = OpenOutputStream(directory, Supplemental_Data_Filename);
			cache::SaveSupplementalData(supplementalData, height, outputStream);
		}
	}

	LocalNodeStateSerializer::LocalNodeStateSerializer(const config::BitxorCoreDirectory& directory) : m_directory(directory)
	{}

	void LocalNodeStateSerializer::save(const cache::BitxorCoreCache& cache, const model::ChainScore& score) const {
		auto cacheStorages = cache.storages();
		auto cacheView = cache.createView();
		const auto& state = cacheView.dependentState();
		auto height = cacheView.height();
		SaveStateToDirectory(m_directory, cacheStorages, state, score, height, [&cacheView](const auto& storage, auto& outputStream) {
			storage.saveAll(cacheView, outputStream);
		});
	}

	void LocalNodeStateSerializer::save(
			const cache::BitxorCoreCacheDelta& cacheDelta,
			const std::vector<std::unique_ptr<const cache::CacheStorage>>& cacheStorages,
			const model::ChainScore& score,
			Height height) const {
		const auto& state = cacheDelta.dependentState();
		SaveStateToDirectory(m_directory, cacheStorages, state, score, height, [&cacheDelta](const auto& storage, auto& outputStream) {
			storage.saveSummary(cacheDelta, outputStream);
		});
	}

	void LocalNodeStateSerializer::moveTo(const config::BitxorCoreDirectory& destinationDirectory) {
		io::PurgeDirectory(destinationDirectory.str());
		std::filesystem::remove(destinationDirectory.path());
		std::filesystem::rename(m_directory.path(), destinationDirectory.path());
	}

	// endregion

	// region SaveStateToDirectoryWithCheckpointing

	namespace {
		void SetCommitStep(const config::BitxorCoreDataDirectory& dataDirectory, consumers::CommitOperationStep step) {
			io::IndexFile(dataDirectory.rootDir().file("commit_step.dat")).set(utils::to_underlying_type(step));
		}
	}

	void SaveStateToDirectoryWithCheckpointing(
			const config::BitxorCoreDataDirectory& dataDirectory,
			const config::NodeConfiguration& nodeConfig,
			const cache::BitxorCoreCache& cache,
			const model::ChainScore& score) {
		SetCommitStep(dataDirectory, consumers::CommitOperationStep::Blocks_Written);

		LocalNodeStateSerializer serializer(dataDirectory.dir("state.tmp"));

		if (nodeConfig.EnableCacheDatabaseStorage) {
			auto storages = const_cast<const cache::BitxorCoreCache&>(cache).storages();
			auto height = cache.createView().height();

			auto cacheDetachableDelta = cache.createDetachableDelta();
			auto cacheDetachedDelta = cacheDetachableDelta.detach();
			auto pCacheDelta = cacheDetachedDelta.tryLock();

			serializer.save(*pCacheDelta, storages, score, height);
		} else {
			serializer.save(cache, score);
		}

		SetCommitStep(dataDirectory, consumers::CommitOperationStep::State_Written);

		serializer.moveTo(dataDirectory.dir("state"));

		SetCommitStep(dataDirectory, consumers::CommitOperationStep::All_Updated);
	}

	// endregion
}}
