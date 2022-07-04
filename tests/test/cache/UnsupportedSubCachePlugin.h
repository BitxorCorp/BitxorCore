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

#pragma once
#include "bitxorcore/cache/SubCachePlugin.h"

namespace bitxorcore { namespace test {

	// region UnsupportedSubCacheView

	/// Unsupported sub cache view.
	class UnsupportedSubCacheView : public cache::SubCacheView {
	public:
		[[noreturn]]
		const cache::SubCacheViewIdentifier& id() const override {
			BITXORCORE_THROW_RUNTIME_ERROR("id is not supported");
		}

		[[noreturn]]
		const void* get() const override {
			BITXORCORE_THROW_RUNTIME_ERROR("get is not supported");
		}

		[[noreturn]]
		void* get() override {
			BITXORCORE_THROW_RUNTIME_ERROR("get is not supported");
		}

		[[noreturn]]
		bool supportsMerkleRoot() const override {
			BITXORCORE_THROW_RUNTIME_ERROR("supportsMerkleRoot is not supported");
		}

		[[noreturn]]
		bool tryGetMerkleRoot(Hash256&) const override {
			BITXORCORE_THROW_RUNTIME_ERROR("tryGetMerkleRoot is not supported");
		}

		[[noreturn]]
		bool trySetMerkleRoot(const Hash256&) override {
			BITXORCORE_THROW_RUNTIME_ERROR("trySetMerkleRoot is not supported");
		}

		[[noreturn]]
		void updateMerkleRoot(Height) override {
			BITXORCORE_THROW_RUNTIME_ERROR("updateMerkleRoot is not supported");
		}

		[[noreturn]]
		void prune(Height) override {
			BITXORCORE_THROW_RUNTIME_ERROR("prune is not supported");
		}

		[[noreturn]]
		void prune(Timestamp) override {
			BITXORCORE_THROW_RUNTIME_ERROR("prune is not supported");
		}

		[[noreturn]]
		const void* asReadOnly() const override {
			BITXORCORE_THROW_RUNTIME_ERROR("asReadOnly is not supported");
		}
	};

	// endregion

	// region UnsupportedSubCachePlugin

	/// Unsupported sub cache plugin.
	template<typename TCache>
	class UnsupportedSubCachePlugin : public cache::SubCachePlugin {
	public:
		/// Creates an unsupported sub cache plugin.
		UnsupportedSubCachePlugin() : m_name(TCache::Name)
		{}

	public:
		const std::string& name() const override {
			return m_name;
		}

		size_t id() const override {
			return TCache::Id;
		}

	public:
		[[noreturn]]
		std::unique_ptr<const cache::SubCacheView> createView() const override {
			BITXORCORE_THROW_RUNTIME_ERROR("createView is not supported");
		}

		[[noreturn]]
		std::unique_ptr<cache::SubCacheView> createDelta() override {
			BITXORCORE_THROW_RUNTIME_ERROR("createDelta is not supported");
		}

		[[noreturn]]
		std::unique_ptr<cache::DetachedSubCacheView> createDetachedDelta() const override {
			BITXORCORE_THROW_RUNTIME_ERROR("createDetachedDelta is not supported");
		}

		[[noreturn]]
		void commit() override {
			BITXORCORE_THROW_RUNTIME_ERROR("commit is not supported");
		}

	public:
		[[noreturn]]
		const void* get() const override {
			BITXORCORE_THROW_RUNTIME_ERROR("get is not supported");
		}

	public:
		[[noreturn]]
		std::unique_ptr<cache::CacheStorage> createStorage() override {
			BITXORCORE_THROW_RUNTIME_ERROR("createStorage is not supported");
		}

		[[noreturn]]
		std::unique_ptr<cache::CacheChangesStorage> createChangesStorage() const override {
			BITXORCORE_THROW_RUNTIME_ERROR("createChangesStorage is not supported");
		}

	private:
		std::string m_name;
	};

	// endregion
}}
