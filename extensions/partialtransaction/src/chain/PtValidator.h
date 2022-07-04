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
#include "bitxorcore/chain/ChainFunctions.h"
#include "bitxorcore/model/WeakEntityInfo.h"
#include "bitxorcore/validators/ValidationResult.h"

namespace bitxorcore {
	namespace cache { class BitxorCoreCache; }
	namespace model {
		struct Transaction;
		class WeakCosignedTransactionInfo;
	}
	namespace plugins { class PluginManager; }
}

namespace bitxorcore { namespace chain {

	/// Result of a partial transaction cosignatories validation.
	enum class CosignatoriesValidationResult {
		/// At least one cosignatory is missing.
		Missing,

		/// At least one cosignatory is ineligible.
		Ineligible,

		/// All cosignatories are eligible and sufficient.
		Success,

		/// Transaction failed validation and should be rejected.
		Failure
	};

	/// Validator for validating parts of a partial transaction.
	/// \note Upon completion the full aggregate transaction will be revalidated.
	class PtValidator {
	public:
		virtual ~PtValidator() = default;

	public:
		/// Validation result.
		template<typename TNormalizedResult>
		struct Result {
			/// Raw validation result.
			validators::ValidationResult Raw;

			/// Normalized validation result.
			TNormalizedResult Normalized;
		};

	public:
		/// Validates a partial transaction (\a transactionInfo) excluding ineligible cosignatories and missing cosignatures checks.
		virtual Result<bool> validatePartial(const model::WeakEntityInfoT<model::Transaction>& transactionInfo) const = 0;

		/// Validates the cosignatories of a partial transaction (\a transactionInfo).
		virtual Result<CosignatoriesValidationResult> validateCosignatories(
				const model::WeakCosignedTransactionInfo& transactionInfo) const = 0;
	};

	/// Creates a default partial transaction validator around \a cache, current time supplier (\a timeSupplier) and \a pluginManager.
	std::unique_ptr<PtValidator> CreatePtValidator(
			const cache::BitxorCoreCache& cache,
			const TimeSupplier& timeSupplier,
			const plugins::PluginManager& pluginManager);
}}
