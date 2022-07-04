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

#include "GenesisFundingObserver.h"
#include "bitxorcore/cache_core/AccountStateCache.h"

namespace bitxorcore { namespace extensions {

	using Notification = model::BalanceTransferNotification;

	DECLARE_OBSERVER(GenesisFunding, Notification)(const Address& genesisAddress, GenesisFundingState& fundingState) {
		return MAKE_OBSERVER(GenesisFunding, Notification, ([&genesisAddress, &fundingState](
				const Notification& notification,
				const observers::ObserverContext& context) {
			// since this is only used by GenesisBlockLoader, it only needs to support commit because genesis can't be rolled back
			if (observers::NotifyMode::Commit != context.Mode || Height(1) != context.Height)
				BITXORCORE_THROW_INVALID_ARGUMENT("GenesisFundingObserver only supports commit mode for genesis block");

			// never fund non-genesis accounts
			auto senderAddress = notification.Sender.resolved(context.Resolvers);
			if (genesisAddress != senderAddress)
				return;

			auto& cache = context.Cache.sub<cache::AccountStateCache>();
			cache.addAccount(senderAddress, context.Height);

			auto senderIter = cache.find(senderAddress);
			auto& senderState = senderIter.get();

			auto tokenId = context.Resolvers.resolve(notification.TokenId);
			fundingState.TotalFundedTokens.credit(tokenId, notification.Amount);

			// if the account state balance is zero when the first transfer is made, implicitly fund the genesis account
			// assume that all tokens are funded in same way (i.e. genesis cannot have mix of implicitly and explicitly funded tokens)
			if (GenesisFundingType::Unknown == fundingState.FundingType) {
				fundingState.FundingType = Amount() != senderState.Balances.get(tokenId)
						? GenesisFundingType::Explicit
						: GenesisFundingType::Implicit;
			}

			if (GenesisFundingType::Explicit == fundingState.FundingType)
				return;

			senderState.Balances.credit(tokenId, notification.Amount);
		}));
	}
}}
