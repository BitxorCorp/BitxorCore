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

#include "Observers.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/cache_core/AccountStateCacheUtils.h"
#include "bitxorcore/model/InflationCalculator.h"
#include "bitxorcore/model/Token.h"

namespace bitxorcore { namespace observers {

	namespace {
		using Notification = model::BlockNotification;

		class FeeApplier {
		public:
			FeeApplier(TokenId currencyTokenId, ObserverContext& context)
					: m_currencyTokenId(currencyTokenId)
					, m_context(context)
			{}

		public:
			void apply(const Address& address, Amount amount) {
				auto& cache = m_context.Cache.sub<cache::AccountStateCache>();
				auto feeToken = model::Token{ m_currencyTokenId, amount };
				cache::ProcessForwardedAccountState(cache, address, [&feeToken, &context = m_context](auto& accountState) {
					ApplyFee(accountState, context.Mode, feeToken, context.StatementBuilder());
				});
			}

		private:
			static void ApplyFee(
					state::AccountState& accountState,
					NotifyMode notifyMode,
					const model::Token& feeToken,
					ObserverStatementBuilder& statementBuilder) {
				if (NotifyMode::Rollback == notifyMode) {
					accountState.Balances.debit(feeToken.TokenId, feeToken.Amount);
					return;
				}

				accountState.Balances.credit(feeToken.TokenId, feeToken.Amount);

				// add fee receipt
				auto receiptType = model::Receipt_Type_Harvest_Fee;
				model::BalanceChangeReceipt receipt(receiptType, accountState.Address, feeToken.TokenId, feeToken.Amount);
				statementBuilder.addReceipt(receipt);
			}

		private:
			TokenId m_currencyTokenId;
			ObserverContext& m_context;
		};

		bool ShouldShareFees(const Notification& notification, uint8_t harvestBeneficiaryPercentage) {
			return 0u < harvestBeneficiaryPercentage && notification.Harvester != notification.Beneficiary;
		}
	}

	DECLARE_OBSERVER(HarvestFee, Notification)(const HarvestFeeOptions& options, const model::InflationCalculator& calculator) {
		return MAKE_OBSERVER(HarvestFee, Notification, ([options, calculator](const Notification& notification, ObserverContext& context) {
			auto inflationAmount = calculator.getSpotAmount(context.Height);
			auto totalAmount = notification.TotalFee + inflationAmount;

			auto networkAmount = Amount(totalAmount.unwrap() * options.HarvestNetworkPercentage / 100);
			auto controlstakeAmount = Amount(totalAmount.unwrap() * options.HarvestControlStakePercentage / 100);
			auto beneficiaryAmount = ShouldShareFees(notification, options.HarvestBeneficiaryPercentage)
					? Amount(totalAmount.unwrap() * options.HarvestBeneficiaryPercentage / 100)
					: Amount();
			auto harvesterAmount = totalAmount - networkAmount - controlstakeAmount - beneficiaryAmount;

			// always create receipt for harvester
			FeeApplier applier(options.CurrencyTokenId, context);
			applier.apply(notification.Harvester, harvesterAmount);

			// only if amount is non-zero create receipt for network sink account
			if (Amount() != networkAmount)
				applier.apply(options.HarvestNetworkFeeSinkAddress.get(context.Height), networkAmount);
			
			// only if amount is non-zero create receipt for network sink account
			if (Amount() != controlstakeAmount)
				applier.apply(options.HarvestControlStakeFeeSinkAddress.get(context.Height), controlstakeAmount);

			// only if amount is non-zero create receipt for beneficiary account
			if (Amount() != beneficiaryAmount)
				applier.apply(notification.Beneficiary, beneficiaryAmount);

			// add inflation receipt
			if (Amount() != inflationAmount && NotifyMode::Commit == context.Mode) {
				model::InflationReceipt receipt(model::Receipt_Type_Inflation, options.CurrencyTokenId, inflationAmount);
				context.StatementBuilder().addReceipt(receipt);
			}
		}));
	}
}}
