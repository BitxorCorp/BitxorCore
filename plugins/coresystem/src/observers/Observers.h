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
#include "bitxorcore/model/HeightDependentAddress.h"
#include "bitxorcore/model/Notifications.h"
#include "bitxorcore/observers/ObserverTypes.h"

namespace bitxorcore {
	namespace importance { class ImportanceCalculator; }
	namespace model { class InflationCalculator; }
}

namespace bitxorcore { namespace observers {

	// region VerifiableEntity

	/// Observes account address changes.
	DECLARE_OBSERVER(AccountAddress, model::AccountAddressNotification)();

	/// Observes account public key changes.
	DECLARE_OBSERVER(AccountPublicKey, model::AccountPublicKeyNotification)();

	// endregion

	// region Block

	/// Options for the harvest fee observer.
	struct HarvestFeeOptions {
		/// Token id used as primary chain currency.
		TokenId CurrencyTokenId;

		/// Percentage of the harvested fee that is collected by the beneficiary account.
		uint8_t HarvestBeneficiaryPercentage;

		/// Percentage of the harvested fee that is collected by the network.
		uint8_t HarvestNetworkPercentage;

		/// Percentage of the harvested fee that is collected by the Control-Stake.
		uint8_t HarvestControlStakePercentage;

		/// Address of the harvest network fee sink account(s).
		model::HeightDependentAddress HarvestNetworkFeeSinkAddress;

		/// Address of the harvest network fee sink account(s).
		model::HeightDependentAddress HarvestControlStakeFeeSinkAddress;
	};

	/// Observes block notifications and triggers importance recalculations using either \a pCommitCalculator (for commits)
	/// or \a pRollbackCalculator (for rollbacks).
	DECLARE_OBSERVER(RecalculateImportances, model::BlockNotification)(
			std::unique_ptr<importance::ImportanceCalculator>&& pCommitCalculator,
			std::unique_ptr<importance::ImportanceCalculator>&& pRollbackCalculator);

	/// Observes block notifications and credits the harvester and, optionally, additional accounts specified in \a options
	/// with the currency token given the specified inflation \a calculator.
	DECLARE_OBSERVER(HarvestFee, model::BlockNotification)(const HarvestFeeOptions& options, const model::InflationCalculator& calculator);

	/// Observes block beneficiary.
	DECLARE_OBSERVER(Beneficiary, model::BlockNotification)();

	/// Observes block statistics and updates dynamic fee multiplier given \a maxDifficultyBlocks and \a defaultDynamicFeeMultiplier.
	DECLARE_OBSERVER(BlockStatistic, model::BlockNotification)(
			uint32_t maxDifficultyBlocks,
			BlockFeeMultiplier defaultDynamicFeeMultiplier);

	/// Observes block notifications and counts transactions.
	DECLARE_OBSERVER(TotalTransactions, model::BlockNotification)();

	/// Observes block notifications and recalculates high value account information when \a mode matches.
	DECLARE_OBSERVER(HighValueAccount, model::BlockNotification)(NotifyMode mode);

	// endregion

	// region Transaction

	/// Observes balance changes triggered by balance transfer notifications.
	DECLARE_OBSERVER(BalanceTransfer, model::BalanceTransferNotification)();

	/// Observes balance changes triggered by balance debit notifications.
	DECLARE_OBSERVER(BalanceDebit, model::BalanceDebitNotification)();

	/// Observes transaction fee notifications and updates account activity information.
	DECLARE_OBSERVER(TransactionFeeActivity, model::TransactionFeeNotification)();

	// endregion

	// region SourceChange

	/// Observes source changes and changes observer source.
	DECLARE_OBSERVER(SourceChange, model::SourceChangeNotification)();

	// endregion
}}
