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
#include "src/cache/TokenCache.h"
#include "bitxorcore/validators/ValidationResult.h"
#include "bitxorcore/types.h"

namespace bitxorcore {
	namespace cache { class ReadOnlyBitxorCoreCache; }
	namespace state { class TokenEntry; }
}

namespace bitxorcore { namespace validators {

	/// View on top of a bitxorcore cache cache for retrieving active tokens.
	class ActiveTokenView {
	public:
		/// Creates a view around \a cache.
		explicit ActiveTokenView(const cache::ReadOnlyBitxorCoreCache& cache);

	public:
		/// Iterator type returned by tryGet.
		using FindIterator = cache::TokenCacheTypes::CacheReadOnlyType::ReadOnlyFindIterator<
			cache::TokenCacheView::const_iterator,
			cache::TokenCacheDelta::const_iterator
		>;

	public:
		/// Tries to get an entry iterator (\a iter) for an active token with \a id at \a height.
		validators::ValidationResult tryGet(TokenId id, Height height, FindIterator& iter) const;

		/// Tries to get an entry iterator (\a iter) for an active token with \a id at \a height given its purported \a owner.
		validators::ValidationResult tryGet(TokenId id, Height height, const Address& owner, FindIterator& iter) const;

	private:
		const cache::ReadOnlyBitxorCoreCache& m_cache;
	};
}}
