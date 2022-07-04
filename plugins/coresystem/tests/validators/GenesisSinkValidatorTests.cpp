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

#include "src/validators/Validators.h"
#include "bitxorcore/crypto/KeyPair.h"
#include "bitxorcore/validators/ValidatorContext.h"
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

	DEFINE_COMMON_VALIDATOR_TESTS(GenesisSink, Height(), std::vector<Signature>())

#define TEST_CLASS GenesisSinkValidatorTests

	namespace {
		constexpr auto Success_Result = ValidationResult::Success;
		constexpr auto Failure_Result = Failure_Core_Genesis_Account_Signed_After_Genesis_Block;
		constexpr auto Fork_Height = Height(1000);

		enum class ExplicitlyAllowedSignatureMode { Enabled, Disabled };

		crypto::KeyPair GetGenesisAccount() {
			// note that the genesis account is fake in order to ensure that it is being retrieved from the context
			return crypto::KeyPair::FromString("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF");
		}

		void AssertValidationResult(
				ValidationResult expectedResult,
				const Key& signer,
				Height height,
				ExplicitlyAllowedSignatureMode explicitlyAllowedSignatureMode = ExplicitlyAllowedSignatureMode::Disabled) {
			// Arrange:
			auto cache = test::CreateEmptyBitxorCoreCache();
			auto cacheView = cache.createView();
			auto readOnlyCache = cacheView.toReadOnly();
			model::NetworkInfo networkInfo;
			networkInfo.GenesisSignerPublicKey = GetGenesisAccount().publicKey();

			auto signatures = test::GenerateRandomDataVector<Signature>(3);
			auto pValidator = CreateGenesisSinkValidator(Fork_Height, signatures);
			auto context = test::CreateValidatorContext(height, networkInfo, readOnlyCache);

			auto signature = ExplicitlyAllowedSignatureMode::Enabled == explicitlyAllowedSignatureMode
					? signatures[1]
					: test::GenerateRandomByteArray<Signature>();
			model::SignatureNotification notification(signer, signature, {});

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, context);

			// Assert:
			EXPECT_EQ(expectedResult, result);
		}
	}

	TEST(TEST_CLASS, SuccessWhenValidatingSignatureNotFromGenesisAccount) {
		// Arrange:
		auto signer = test::GenerateRandomByteArray<Key>();

		// Assert: signer is allowed at all heights
		AssertValidationResult(Success_Result, signer, Height(1));
		AssertValidationResult(Success_Result, signer, Height(10));
		AssertValidationResult(Success_Result, signer, Height(100));
	}

	TEST(TEST_CLASS, SuccessWhenValidatingSignatureFromGenesisAccountAtHeightOne) {
		// Arrange:
		auto signer = GetGenesisAccount().publicKey();

		// Assert: allowed at height one
		AssertValidationResult(Success_Result, signer, Height(1));
	}

	TEST(TEST_CLASS, FailureWhenValidatingSignatureFromGenesisAccountNotAtHeightOne) {
		// Arrange:
		auto signer = GetGenesisAccount().publicKey();

		// Assert: not allowed at heights greater than one
		AssertValidationResult(Failure_Result, signer, Fork_Height - Height(100));
		AssertValidationResult(Failure_Result, signer, Fork_Height);
		AssertValidationResult(Failure_Result, signer, Fork_Height + Height(100));
	}

	TEST(TEST_CLASS, FailureWhenValidatingSignatureFromGenesisAccountNotAtForkHeightWithExplicitlyAllowedSignatures) {
		// Arrange:
		auto signer = GetGenesisAccount().publicKey();

		// Assert: not allowed at non-fork heights
		AssertValidationResult(Failure_Result, signer, Fork_Height - Height(100), ExplicitlyAllowedSignatureMode::Enabled);
		AssertValidationResult(Failure_Result, signer, Fork_Height - Height(1), ExplicitlyAllowedSignatureMode::Enabled);
		AssertValidationResult(Failure_Result, signer, Fork_Height + Height(1), ExplicitlyAllowedSignatureMode::Enabled);
		AssertValidationResult(Failure_Result, signer, Fork_Height + Height(100), ExplicitlyAllowedSignatureMode::Enabled);
	}

	TEST(TEST_CLASS, FailureWhenValidatingSignatureFromGenesisAccountAtZeroHeightWithExplicitlyAllowedSignatures) {
		// Arrange:
		auto signer = GetGenesisAccount().publicKey();

		// Assert:
		AssertValidationResult(Failure_Result, signer, Height(0), ExplicitlyAllowedSignatureMode::Enabled);
	}

	TEST(TEST_CLASS, SuccessWhenValidatingSignatureFromGenesisAccountAtForkHeightWithExplicitlyAllowedSignatures) {
		// Arrange:
		auto signer = GetGenesisAccount().publicKey();

		// Assert: allowed at fork height because of explicit allowance
		AssertValidationResult(Success_Result, signer, Fork_Height, ExplicitlyAllowedSignatureMode::Enabled);
	}
}}
