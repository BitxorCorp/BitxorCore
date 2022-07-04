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

#include "Validators.h"
#include "ActiveTokenView.h"
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"

namespace bitxorcore { namespace validators {

	ActiveTokenView::ActiveTokenView(const cache::ReadOnlyBitxorCoreCache& cache) : m_cache(cache)
	{}

	validators::ValidationResult ActiveTokenView::tryGet(TokenId id, Height height, FindIterator& iter) const {
		// ensure that the token is active
		const auto& tokenCache = m_cache.sub<cache::TokenCache>();
		iter = tokenCache.find(id);
		return !iter.tryGet() || !iter.get().definition().isActive(height)
				? Failure_Token_Expired
				: ValidationResult::Success;
	}

	validators::ValidationResult ActiveTokenView::tryGet(TokenId id, Height height, const Address& owner, FindIterator& iter) const {
		auto result = tryGet(id, height, iter);
		if (!IsValidationResultSuccess(result))
			return result;

		return iter.get().definition().ownerAddress() != owner ? Failure_Token_Owner_Conflict : ValidationResult::Success;
	}
}}
