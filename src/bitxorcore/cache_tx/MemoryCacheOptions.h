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
#include "bitxorcore/utils/FileSize.h"

namespace bitxorcore { namespace cache {

	/// Options for customizing the behavior of a memory based cache.
	class MemoryCacheOptions {
	public:
		/// Creates default options.
		constexpr MemoryCacheOptions() = default;

		/// Creates options with custom \a maxResponseSize and \a maxCacheSize.
		constexpr MemoryCacheOptions(utils::FileSize maxResponseSize, utils::FileSize maxCacheSize)
				: MaxResponseSize(maxResponseSize)
				, MaxCacheSize(maxCacheSize)
		{}

	public:
		/// Maximum response size.
		utils::FileSize MaxResponseSize;

		/// Maximum size of the cache.
		utils::FileSize MaxCacheSize;
	};
}}
