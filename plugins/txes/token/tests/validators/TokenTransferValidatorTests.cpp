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
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "tests/test/TokenCacheTestUtils.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/test/plugins/ValidatorTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS TokenTransferValidatorTests

	DEFINE_COMMON_VALIDATOR_TESTS(TokenTransfer, UnresolvedTokenId())

	namespace {
		constexpr auto Currency_Token_Id = UnresolvedTokenId(2345);

		auto CreateCache() {
			return test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		}

		model::TokenProperties CreateTokenProperties(model::TokenFlags flags) {
			return model::TokenProperties(flags, 0, BlockDuration());
		}

		state::TokenDefinition CreateTokenDefinition(Height height, const Address& owner, model::TokenFlags flags) {
			return state::TokenDefinition(height, owner, 3, CreateTokenProperties(flags));
		}

		state::TokenEntry CreateTokenEntry(TokenId tokenId, const Address& owner, model::TokenFlags flags) {
			auto tokenDefinition = CreateTokenDefinition(Height(100), owner, flags);
			return state::TokenEntry(tokenId, tokenDefinition);
		}

		void SeedCacheWithToken(cache::BitxorCoreCache& cache, const state::TokenEntry& tokenEntry) {
			auto cacheDelta = cache.createDelta();
			auto& tokenCacheDelta = cacheDelta.sub<cache::TokenCache>();
			tokenCacheDelta.insert(tokenEntry);
			cache.commit(Height());
		}

		void AssertValidationResult(
				ValidationResult expectedResult,
				const cache::BitxorCoreCache& cache,
				const model::BalanceTransferNotification& notification) {
			// Arrange:
			auto pValidator = CreateTokenTransferValidator(Currency_Token_Id);

			// Act:
			auto result = test::ValidateNotification(*pValidator, notification, cache);

			// Assert:
			EXPECT_EQ(expectedResult, result);
		}
	}

	TEST(TEST_CLASS, SuccessWhenValidatingCurrencyTokenTransfer) {
		// Arrange:
		auto notification = model::BalanceTransferNotification(Address(), UnresolvedAddress(), Currency_Token_Id, Amount(123));
		auto cache = CreateCache();

		// Assert:
		AssertValidationResult(ValidationResult::Success, cache, notification);
	}

	namespace {
		constexpr TokenId Valid_Token_Id(222);
		constexpr UnresolvedTokenId Unresolved_Unknown_Token_Id(444);

		auto CreateAndSeedCache(const Address& owner, model::TokenFlags flags) {
			// Arrange:
			auto cache = CreateCache();
			auto validTokenEntry = CreateTokenEntry(Valid_Token_Id, owner, flags);
			SeedCacheWithToken(cache, validTokenEntry);

			auto cacheDelta = cache.createDelta();
			test::AddTokenOwner(cacheDelta, Valid_Token_Id, validTokenEntry.definition().ownerAddress(), Amount());
			cache.commit(Height());

			return cache;
		}

		void AssertTokensTest(ValidationResult expectedResult, UnresolvedTokenId tokenId) {
			// Arrange:
			auto owner = test::GenerateRandomByteArray<Address>();
			auto notification = model::BalanceTransferNotification(owner, UnresolvedAddress(), tokenId, Amount(123));
			auto cache = CreateAndSeedCache(owner, model::TokenFlags::Transferable);

			// Assert:
			AssertValidationResult(expectedResult, cache, notification);
		}
	}

	TEST(TEST_CLASS, FailureWhenValidatingUnknownToken) {
		AssertTokensTest(Failure_Token_Expired, Unresolved_Unknown_Token_Id);
	}

	TEST(TEST_CLASS, SuccessWhenValidatingKnownToken) {
		AssertTokensTest(ValidationResult::Success, test::UnresolveXor(Valid_Token_Id));
	}

	namespace {
		enum : uint8_t {
			None = 0x00,
			Owner_Is_Sender = 0x01,
			Owner_Is_Recipient = 0x02
		};

		void AssertNonTransferableTokensTest(ValidationResult expectedResult, uint8_t notificationFlags) {
			// Arrange:
			auto owner = test::GenerateRandomByteArray<Address>();
			auto cache = CreateAndSeedCache(owner, model::TokenFlags::None);

			// - notice that BalanceTransferNotification holds references to sender + recipient
			auto tokenId = test::UnresolveXor(Valid_Token_Id);
			auto notification = model::BalanceTransferNotification(Address(), UnresolvedAddress(), tokenId, Amount(123));

			if (notificationFlags & Owner_Is_Sender)
				notification.Sender = owner;

			if (notificationFlags & Owner_Is_Recipient) {
				const auto& recipient = cache.createView().sub<cache::AccountStateCache>().find(owner).get().Address;
				notification.Recipient = test::UnresolveXor(recipient);
			}

			// Assert:
			AssertValidationResult(expectedResult, cache, notification);
		}
	}

	TEST(TEST_CLASS, FailureWhenTokenIsNonTransferableAndOwnerIsNotParticipant) {
		AssertNonTransferableTokensTest(Failure_Token_Non_Transferable, None);
	}

	TEST(TEST_CLASS, SuccessWhenTokenIsNonTransferableAndOwnerIsSender) {
		AssertNonTransferableTokensTest(ValidationResult::Success, Owner_Is_Sender);
	}

	TEST(TEST_CLASS, SuccessWhenTokenIsNonTransferableAndOwnerIsRecipient) {
		AssertNonTransferableTokensTest(ValidationResult::Success, Owner_Is_Recipient);
	}

	TEST(TEST_CLASS, SuccessWhenTokenIsNonTransferableAndOwnerSendsToSelf) {
		AssertNonTransferableTokensTest(ValidationResult::Success, Owner_Is_Recipient | Owner_Is_Sender);
	}
}}
