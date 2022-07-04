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
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/state/BitxorCoreState.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	using BalanceTransferNotification = model::BalanceTransferNotification;
	using BalanceDebitNotification = model::BalanceDebitNotification;

	namespace {
		bool TryGetEffectiveAmount(
				const BalanceTransferNotification& notification,
				BlockFeeMultiplier feeMultiplier,
				Amount& effectiveAmount) {
			effectiveAmount = notification.Amount;
			if (BalanceTransferNotification::AmountType::Static == notification.TransferAmountType)
				return true;

			effectiveAmount = Amount(notification.Amount.unwrap() * feeMultiplier.unwrap());
			if (BlockFeeMultiplier() == feeMultiplier)
				return true;

			return std::numeric_limits<Amount::ValueType>::max() / feeMultiplier.unwrap() >= notification.Amount.unwrap();
		}

		bool TryGetEffectiveAmount(const BalanceDebitNotification& notification, BlockFeeMultiplier, Amount& effectiveAmount) {
			effectiveAmount = notification.Amount;
			return true;
		}

		bool FindAccountBalance(const cache::ReadOnlyAccountStateCache& cache, const Address& address, TokenId tokenId, Amount& amount) {
			auto accountStateAddressIter = cache.find(address);
			if (accountStateAddressIter.tryGet()) {
				amount = accountStateAddressIter.get().Balances.get(tokenId);
				return true;
			}

			return false;
		}

		template<typename TNotification>
		ValidationResult CheckBalance(const TNotification& notification, const ValidatorContext& context) {
			const auto& cache = context.Cache.sub<cache::AccountStateCache>();

			Amount amount;
			auto tokenId = context.Resolvers.resolve(notification.TokenId);
			if (FindAccountBalance(cache, notification.Sender.resolved(context.Resolvers), tokenId, amount)) {
				Amount effectiveAmount;
				auto dynamicFeeMultiplier = context.Cache.dependentState().DynamicFeeMultiplier;
				if (TryGetEffectiveAmount(notification, dynamicFeeMultiplier, effectiveAmount) && amount >= effectiveAmount)
					return ValidationResult::Success;
			}

			return Failure_Core_Insufficient_Balance;
		}
	}

	DEFINE_STATEFUL_VALIDATOR_WITH_TYPE(BalanceTransfer, BalanceTransferNotification, CheckBalance<BalanceTransferNotification>)
	DEFINE_STATEFUL_VALIDATOR_WITH_TYPE(BalanceDebit, BalanceDebitNotification, CheckBalance<BalanceDebitNotification>)
}}
