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
#include "ValidatorTypes.h"
#include "bitxorcore/thread/Future.h"

namespace bitxorcore { namespace thread { class IoThreadPool; } }

namespace bitxorcore { namespace validators {

	/// Parallel validation policy that performs parallel (stateless) validations on multiple threads.
	class ParallelValidationPolicy {
	public:
		virtual ~ParallelValidationPolicy() = default;

	public:
		/// Validates all \a entityInfos and short circuits on first failure.
		virtual thread::future<ValidationResult> validateShortCircuit(const model::WeakEntityInfos& entityInfos) const = 0;

		/// Validates all \a entityInfos and does \em NOT short circuit on failures.
		virtual thread::future<std::vector<ValidationResult>> validateAll(const model::WeakEntityInfos& entityInfos) const = 0;
	};

	/// Creates a parallel validation policy using \a pool for parallelization and \a pValidator for validation.
	std::shared_ptr<const ParallelValidationPolicy> CreateParallelValidationPolicy(
			thread::IoThreadPool& pool,
			const std::shared_ptr<const StatelessEntityValidator>& pValidator);
}}
