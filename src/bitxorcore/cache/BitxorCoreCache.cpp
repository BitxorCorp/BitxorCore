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

#include "BitxorCoreCache.h"
#include "CacheHeight.h"
#include "BitxorCoreCacheDetachedDelta.h"
#include "ReadOnlyBitxorCoreCache.h"
#include "SubCachePluginAdapter.h"
#include "bitxorcore/crypto/Hashes.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "bitxorcore/model/NetworkIdentifier.h"
#include "bitxorcore/state/BitxorCoreState.h"
#include "bitxorcore/utils/StackLogger.h"

namespace bitxorcore { namespace cache {

	namespace {
		template<typename TSubCacheViews>
		std::vector<const void*> ExtractReadOnlyViews(const TSubCacheViews& subViews) {
			std::vector<const void*> readOnlyViews;
			for (const auto& pSubView : subViews) {
				if (!pSubView) {
					readOnlyViews.push_back(nullptr);
					continue;
				}

				readOnlyViews.push_back(pSubView->asReadOnly());
			}

			return readOnlyViews;
		}

		template<typename TSubCacheViews, typename TUpdateMerkleRoot>
		std::vector<Hash256> CollectSubCacheMerkleRoots(TSubCacheViews& subViews, TUpdateMerkleRoot updateMerkleRoot) {
			std::vector<Hash256> merkleRoots;
			for (const auto& pSubView : subViews) {
				Hash256 merkleRoot;
				if (!pSubView)
					continue;

				updateMerkleRoot(*pSubView);
				if (pSubView->tryGetMerkleRoot(merkleRoot))
					merkleRoots.push_back(merkleRoot);
			}

			return merkleRoots;
		}

		Hash256 CalculateStateHash(const std::vector<Hash256>& subCacheMerkleRoots) {
			Hash256 stateHash;
			if (subCacheMerkleRoots.empty()) {
				stateHash = Hash256();
			} else {
				crypto::Sha3_256_Builder stateHashBuilder;
				for (const auto& subCacheMerkleRoot : subCacheMerkleRoots)
					stateHashBuilder.update(subCacheMerkleRoot);

				stateHashBuilder.final(stateHash);
			}

			return stateHash;
		}

		template<typename TSubCacheViews, typename TUpdateMerkleRoot>
		StateHashInfo CalculateStateHashInfo(const TSubCacheViews& subViews, TUpdateMerkleRoot updateMerkleRoot) {
			utils::SlowOperationLogger logger("CalculateStateHashInfo", utils::LogLevel::warning);

			StateHashInfo stateHashInfo;
			stateHashInfo.SubCacheMerkleRoots = CollectSubCacheMerkleRoots(subViews, updateMerkleRoot);
			stateHashInfo.StateHash = CalculateStateHash(stateHashInfo.SubCacheMerkleRoots);
			return stateHashInfo;
		}
	}

	// region BitxorCoreCacheView

	BitxorCoreCacheView::BitxorCoreCacheView(
			CacheHeightView&& cacheHeightView,
			const state::BitxorCoreState& dependentState,
			std::vector<std::unique_ptr<const SubCacheView>>&& subViews)
			: m_pCacheHeight(std::make_unique<CacheHeightView>(std::move(cacheHeightView)))
			, m_pDependentState(&dependentState)
			, m_subViews(std::move(subViews))
	{}

	BitxorCoreCacheView::~BitxorCoreCacheView() = default;

	BitxorCoreCacheView::BitxorCoreCacheView(BitxorCoreCacheView&&) = default;

	BitxorCoreCacheView& BitxorCoreCacheView::operator=(BitxorCoreCacheView&&) = default;

	Height BitxorCoreCacheView::height() const {
		return m_pCacheHeight->get();
	}

	const state::BitxorCoreState& BitxorCoreCacheView::dependentState() const {
		return *m_pDependentState;
	}

	StateHashInfo BitxorCoreCacheView::calculateStateHash() const {
		return CalculateStateHashInfo(m_subViews, [](const auto&) {});
	}

	ReadOnlyBitxorCoreCache BitxorCoreCacheView::toReadOnly() const {
		return ReadOnlyBitxorCoreCache(*m_pDependentState, ExtractReadOnlyViews(m_subViews));
	}

	// endregion

	// region BitxorCoreCacheDelta

	BitxorCoreCacheDelta::BitxorCoreCacheDelta(
			Disposition disposition,
			state::BitxorCoreState& dependentState,
			std::vector<std::unique_ptr<SubCacheView>>&& subViews)
			: m_disposition(disposition)
			, m_pDependentState(&dependentState)
			, m_subViews(std::move(subViews))
	{}

	BitxorCoreCacheDelta::~BitxorCoreCacheDelta() = default;

	BitxorCoreCacheDelta::BitxorCoreCacheDelta(BitxorCoreCacheDelta&&) = default;

	BitxorCoreCacheDelta& BitxorCoreCacheDelta::operator=(BitxorCoreCacheDelta&&) = default;

	BitxorCoreCacheDelta::Disposition BitxorCoreCacheDelta::disposition() const {
		return m_disposition;
	}

	const state::BitxorCoreState& BitxorCoreCacheDelta::dependentState() const {
		return *m_pDependentState;
	}

	state::BitxorCoreState& BitxorCoreCacheDelta::dependentState() {
		return *m_pDependentState;
	}

	StateHashInfo BitxorCoreCacheDelta::calculateStateHash(Height height) const {
		return CalculateStateHashInfo(m_subViews, [height](auto& subView) { subView.updateMerkleRoot(height); });
	}

	void BitxorCoreCacheDelta::setSubCacheMerkleRoots(const std::vector<Hash256>& subCacheMerkleRoots) {
		auto merkleRootIndex = 0u;
		for (const auto& pSubView : m_subViews) {
			if (!pSubView || !pSubView->supportsMerkleRoot())
				continue;

			if (merkleRootIndex == subCacheMerkleRoots.size())
				BITXORCORE_THROW_INVALID_ARGUMENT_1("too few sub cache merkle roots were passed", subCacheMerkleRoots.size());

			// this will always succeed because supportsMerkleRoot was checked above
			pSubView->trySetMerkleRoot(subCacheMerkleRoots[merkleRootIndex++]);
		}

		if (merkleRootIndex != subCacheMerkleRoots.size()) {
			BITXORCORE_THROW_INVALID_ARGUMENT_2(
					"wrong number of sub cache merkle roots were passed (expected, actual)",
					merkleRootIndex,
					subCacheMerkleRoots.size());
		}
	}

	void BitxorCoreCacheDelta::prune(Height height) {
		for (const auto& pSubView : m_subViews) {
			if (!pSubView)
				continue;

			pSubView->prune(height);
		}
	}

	void BitxorCoreCacheDelta::prune(Timestamp time) {
		for (const auto& pSubView : m_subViews) {
			if (!pSubView)
				continue;

			pSubView->prune(time);
		}
	}

	ReadOnlyBitxorCoreCache BitxorCoreCacheDelta::toReadOnly() const {
		return ReadOnlyBitxorCoreCache(*m_pDependentState, ExtractReadOnlyViews(m_subViews));
	}

	// endregion

	// region BitxorCoreCacheDetachableDelta

	BitxorCoreCacheDetachableDelta::BitxorCoreCacheDetachableDelta(
			CacheHeightView&& cacheHeightView,
			const state::BitxorCoreState& dependentState,
			std::vector<std::unique_ptr<DetachedSubCacheView>>&& detachedSubViews)
			// note that CacheHeightView is a unique_ptr to allow BitxorCoreCacheDetachableDelta to be declared without it defined
			: m_pCacheHeightView(std::make_unique<CacheHeightView>(std::move(cacheHeightView)))
			, m_detachedDelta(dependentState, std::move(detachedSubViews))
	{}

	BitxorCoreCacheDetachableDelta::~BitxorCoreCacheDetachableDelta() = default;

	BitxorCoreCacheDetachableDelta::BitxorCoreCacheDetachableDelta(BitxorCoreCacheDetachableDelta&&) = default;

	Height BitxorCoreCacheDetachableDelta::height() const {
		return m_pCacheHeightView->get();
	}

	BitxorCoreCacheDetachedDelta BitxorCoreCacheDetachableDelta::detach() {
		return std::move(m_detachedDelta);
	}

	// endregion

	// region BitxorCoreCacheDetachedDelta

	BitxorCoreCacheDetachedDelta::BitxorCoreCacheDetachedDelta(
			const state::BitxorCoreState& dependentState,
			std::vector<std::unique_ptr<DetachedSubCacheView>>&& detachedSubViews)
			: m_pDependentState(std::make_unique<state::BitxorCoreState>(dependentState))
			, m_detachedSubViews(std::move(detachedSubViews))
	{}

	BitxorCoreCacheDetachedDelta::~BitxorCoreCacheDetachedDelta() = default;

	BitxorCoreCacheDetachedDelta::BitxorCoreCacheDetachedDelta(BitxorCoreCacheDetachedDelta&&) = default;

	BitxorCoreCacheDetachedDelta& BitxorCoreCacheDetachedDelta::operator=(BitxorCoreCacheDetachedDelta&&) = default;

	std::unique_ptr<BitxorCoreCacheDelta> BitxorCoreCacheDetachedDelta::tryLock() {
		std::vector<std::unique_ptr<SubCacheView>> subViews;
		for (const auto& pDetachedSubView : m_detachedSubViews) {
			if (!pDetachedSubView) {
				subViews.push_back(nullptr);
				continue;
			}

			auto pSubView = pDetachedSubView->tryLock();
			if (!pSubView)
				return nullptr;

			subViews.push_back(std::move(pSubView));
		}

		return std::make_unique<BitxorCoreCacheDelta>(BitxorCoreCacheDelta::Disposition::Detached, *m_pDependentState, std::move(subViews));
	}

	// endregion

	// region BitxorCoreCache

	namespace {
		template<typename TResultView, typename TSubCaches, typename TMapper>
		std::vector<std::unique_ptr<TResultView>> MapSubCaches(TSubCaches& subCaches, TMapper map, bool includeNulls = true) {
			std::vector<std::unique_ptr<TResultView>> resultViews;
			for (const auto& pSubCache : subCaches) {
				auto pSubCacheView = pSubCache ? map(pSubCache) : nullptr;
				if (!pSubCacheView && !includeNulls)
					continue;

				resultViews.push_back(std::move(pSubCacheView));
			}

			return resultViews;
		}
	}

	BitxorCoreCache::BitxorCoreCache(std::vector<std::unique_ptr<SubCachePlugin>>&& subCaches)
			: m_pCacheHeight(std::make_unique<CacheHeight>())
			, m_pDependentState(std::make_unique<state::BitxorCoreState>())
			, m_pDependentStateDelta(std::make_unique<state::BitxorCoreState>())
			, m_subCaches(std::move(subCaches))
	{}

	BitxorCoreCache::~BitxorCoreCache() = default;

	BitxorCoreCache::BitxorCoreCache(BitxorCoreCache&&) = default;

	BitxorCoreCache& BitxorCoreCache::operator=(BitxorCoreCache&&) = default;

	BitxorCoreCacheView BitxorCoreCache::createView() const {
		// acquire a height reader lock to ensure the view is composed of consistent sub cache views
		auto pCacheHeightView = m_pCacheHeight->view();
		auto subViews = MapSubCaches<const SubCacheView>(m_subCaches, [](const auto& pSubCache) { return pSubCache->createView(); });
		return BitxorCoreCacheView(std::move(pCacheHeightView), *m_pDependentState, std::move(subViews));
	}

	BitxorCoreCacheDelta BitxorCoreCache::createDelta() {
		// since only one sub cache delta is allowed outstanding at a time and an outstanding delta is required for commit,
		// sub cache deltas will always be consistent
		auto subViews = MapSubCaches<SubCacheView>(m_subCaches, [](const auto& pSubCache) { return pSubCache->createDelta(); });

		// make a copy of the dependent state after all caches are locked with outstanding deltas
		m_pDependentStateDelta = std::make_unique<state::BitxorCoreState>(*m_pDependentState);
		return BitxorCoreCacheDelta(BitxorCoreCacheDelta::Disposition::Attached, *m_pDependentStateDelta, std::move(subViews));
	}

	BitxorCoreCacheDetachableDelta BitxorCoreCache::createDetachableDelta() const {
		// acquire a height reader lock to ensure the delta is composed of consistent sub cache deltas
		auto pCacheHeightView = m_pCacheHeight->view();
		auto detachedSubViews = MapSubCaches<DetachedSubCacheView>(m_subCaches, [](const auto& pSubCache) {
			return pSubCache->createDetachedDelta();
		});
		return BitxorCoreCacheDetachableDelta(std::move(pCacheHeightView), *m_pDependentState, std::move(detachedSubViews));
	}

	void BitxorCoreCache::commit(Height height) {
		// use the height writer lock to lock the entire cache during commit
		auto cacheHeightModifier = m_pCacheHeight->modifier();

		for (const auto& pSubCache : m_subCaches) {
			if (pSubCache)
				pSubCache->commit();
		}

		// finally, update the dependent state and cache height
		m_pDependentState = std::make_unique<state::BitxorCoreState>(*m_pDependentStateDelta);
		cacheHeightModifier.set(height);
	}

	std::vector<std::unique_ptr<const CacheStorage>> BitxorCoreCache::storages() const {
		return MapSubCaches<const CacheStorage>(
				m_subCaches,
				[](const auto& pSubCache) { return pSubCache->createStorage(); },
				false);
	}

	std::vector<std::unique_ptr<CacheStorage>> BitxorCoreCache::storages() {
		return MapSubCaches<CacheStorage>(
				m_subCaches,
				[](const auto& pSubCache) { return pSubCache->createStorage(); },
				false);
	}

	std::vector<std::unique_ptr<const CacheChangesStorage>> BitxorCoreCache::changesStorages() const {
		return MapSubCaches<const CacheChangesStorage>(
				m_subCaches,
				[](const auto& pSubCache) { return pSubCache->createChangesStorage(); },
				false);
	}

	// endregion
}}
