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
#include "BitxorCoreCache.h"
#include "SubCachePluginAdapter.h"
#include "bitxorcore/exceptions.h"

namespace bitxorcore { namespace cache {

	/// Builder for creating a bitxorcore cache around sub caches.
	class BitxorCoreCacheBuilder {
	public:
		/// Adds \a pSubCache to the builder with the specified storage traits.
		template<typename TStorageTraits, typename TCache>
		void add(std::unique_ptr<TCache>&& pSubCache) {
			add(std::make_unique<SubCachePluginAdapter<TCache, TStorageTraits>>(std::move(pSubCache)));
		}

		/// Adds \a pSubCachePlugin to the builder.
		void add(std::unique_ptr<SubCachePlugin>&& pSubCachePlugin) {
			auto id = pSubCachePlugin->id();
			m_subCaches.resize(std::max(m_subCaches.size(), id + 1));
			if (m_subCaches[id])
				BITXORCORE_THROW_INVALID_ARGUMENT_1("sub cache has already been registered with id", id);

			m_subCaches[id] = std::move(pSubCachePlugin);
		}

	public:
		/// Builds a bitxorcore cache.
		BitxorCoreCache build() {
			BITXORCORE_LOG(debug) << "creating BitxorCoreCache with " << m_subCaches.size() << " sub caches";
			return BitxorCoreCache(std::move(m_subCaches));
		}

	private:
		std::vector<std::unique_ptr<SubCachePlugin>> m_subCaches;
	};
}}
