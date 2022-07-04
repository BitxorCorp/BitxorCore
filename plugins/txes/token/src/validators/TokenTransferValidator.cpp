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
#include "src/cache/TokenCache.h"
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	using Notification = model::BalanceTransferNotification;

	namespace {
		bool IsTokenOwnerParticipant(
				const cache::ReadOnlyBitxorCoreCache& cache,
				const Address& owner,
				const Notification& notification,
				const model::ResolverContext& resolvers) {
			if (owner == notification.Sender.resolved(resolvers))
				return true;

			// the owner must exist if the token lookup succeeded
			const auto& accountStateCache = cache.sub<cache::AccountStateCache>();
			auto ownerAccountStateIter = accountStateCache.find(owner);
			return ownerAccountStateIter.get().Address == notification.Recipient.resolved(resolvers);
		}
	}

	DECLARE_STATEFUL_VALIDATOR(TokenTransfer, Notification)(UnresolvedTokenId currencyTokenId) {
		return MAKE_STATEFUL_VALIDATOR(TokenTransfer, [currencyTokenId](
				const Notification& notification,
				const ValidatorContext& context) {
			// 0. allow currency token id
			if (currencyTokenId == notification.TokenId)
				return ValidationResult::Success;

			// 1. check that the token exists
			ActiveTokenView::FindIterator tokenIter;
			ActiveTokenView activeTokenView(context.Cache);
			auto result = activeTokenView.tryGet(context.Resolvers.resolve(notification.TokenId), context.Height, tokenIter);
			if (!IsValidationResultSuccess(result))
				return result;

			// 2. if it's transferable there's nothing else to check
			const auto& tokenEntry = tokenIter.get();
			if (tokenEntry.definition().properties().is(model::TokenFlags::Transferable))
				return ValidationResult::Success;

			// 3. if it's NOT transferable then owner must be either sender or recipient
			if (!IsTokenOwnerParticipant(context.Cache, tokenEntry.definition().ownerAddress(), notification, context.Resolvers))
				return Failure_Token_Non_Transferable;

			return ValidationResult::Success;
		});
	}
}}
