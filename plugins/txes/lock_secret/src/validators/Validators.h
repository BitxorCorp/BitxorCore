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
#include "Results.h"
#include "src/model/SecretLockNotifications.h"
#include "bitxorcore/validators/ValidatorTypes.h"

namespace bitxorcore { namespace validators {

	/// Validator that applies to secret lock notifications and validates that:
	/// - lock duration is at most \a maxSecretLockDuration
	DECLARE_STATELESS_VALIDATOR(SecretLockDuration, model::SecretLockDurationNotification)(BlockDuration maxSecretLockDuration);

	/// Validator that applies to secret lock hash algorithm notifications and validates that:
	/// - hash algorithm is valid
	DECLARE_STATELESS_VALIDATOR(SecretLockHashAlgorithm, model::SecretLockHashAlgorithmNotification)();

	/// Validator that applies to secret lock notifications and validates that:
	/// - attached hash is not present in secret lock info cache
	DECLARE_STATEFUL_VALIDATOR(SecretLockCacheUnique, model::SecretLockNotification)();

	/// Validator that applies to proof notifications and validates that:
	/// - hash algorithm is supported
	/// - proof size is within inclusive bounds of \a minProofSize and \a maxProofSize
	/// - hash of proof matches secret
	DECLARE_STATELESS_VALIDATOR(ProofSecret, model::ProofSecretNotification)(uint16_t minProofSize, uint16_t maxProofSize);

	/// Validator that applies to proof notifications and validates that:
	/// - secret obtained from proof is present in cache
	DECLARE_STATEFUL_VALIDATOR(Proof, model::ProofPublicationNotification)();
}}
