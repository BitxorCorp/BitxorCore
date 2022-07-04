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
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	using Notification = model::SignatureNotification;

	DECLARE_STATEFUL_VALIDATOR(GenesisSink, Notification)(
			Height additionalAllowedSignaturesHeight,
			const std::vector<Signature>& additionalAllowedSignatures) {
		return MAKE_STATEFUL_VALIDATOR(GenesisSink, ([additionalAllowedSignaturesHeight, additionalAllowedSignatures](
				const Notification& notification,
				const ValidatorContext& context) {
			auto isGenesisPublicKey = notification.SignerPublicKey == context.Network.GenesisSignerPublicKey;
			if (!isGenesisPublicKey || Height(1) == context.Height)
				return ValidationResult::Success;

			if (additionalAllowedSignaturesHeight != context.Height)
				return Failure_Core_Genesis_Account_Signed_After_Genesis_Block;

			auto isExplicitlyAllowed = additionalAllowedSignatures.cend() != std::find(
					additionalAllowedSignatures.cbegin(),
					additionalAllowedSignatures.cend(),
					notification.Signature);

			return isExplicitlyAllowed ? ValidationResult::Success : Failure_Core_Genesis_Account_Signed_After_Genesis_Block;
		}));
	}
}}
