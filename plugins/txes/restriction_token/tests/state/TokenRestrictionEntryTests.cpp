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

#include "src/state/TokenRestrictionEntry.h"
#include "bitxorcore/crypto/Hashes.h"
#include "tests/test/TokenRestrictionTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS TokenRestrictionEntryTests

	// region test utils

	namespace {
#pragma pack(push, 1)

		struct PackedUniqueKey {
			bitxorcore::TokenId TokenId;
			bitxorcore::Address Address;
		};

#pragma pack(pop)

		Hash256 CalculateExpectedUniqueKey(TokenId tokenId, const Address& address) {
			auto packedUniqueKey = PackedUniqueKey{ tokenId, address };

			Hash256 uniqueKey;
			crypto::Sha3_256({ reinterpret_cast<const uint8_t*>(&packedUniqueKey), sizeof(PackedUniqueKey) }, uniqueKey);
			return uniqueKey;
		}
	}

	// endregion

	// region TokenRestrictionEntry - constructor

	TEST(TEST_CLASS, CanCreateEntryAroundAddressRestriction) {
		// Act:
		TokenRestrictionEntry entry(TokenAddressRestriction(TokenId(123), Address{ { 35 } }));

		// Assert:
		EXPECT_EQ(TokenRestrictionEntry::EntryType::Address, entry.entryType());

		auto expectedUniqueKey = CalculateExpectedUniqueKey(TokenId(123), Address{ { 35 } });
		EXPECT_EQ(expectedUniqueKey, entry.uniqueKey());

		EXPECT_EQ(TokenId(123), entry.asAddressRestriction().tokenId());
		EXPECT_EQ(TokenId(123), const_cast<const TokenRestrictionEntry&>(entry).asAddressRestriction().tokenId());

		EXPECT_THROW(entry.asGlobalRestriction(), bitxorcore_runtime_error);
		EXPECT_THROW(const_cast<const TokenRestrictionEntry&>(entry).asGlobalRestriction(), bitxorcore_runtime_error);
	}

	TEST(TEST_CLASS, CanCreateEntryAroundGlobalRestriction) {
		// Act:
		TokenRestrictionEntry entry(TokenGlobalRestriction(TokenId(123)));

		// Assert:
		EXPECT_EQ(TokenRestrictionEntry::EntryType::Global, entry.entryType());

		auto expectedUniqueKey = CalculateExpectedUniqueKey(TokenId(123), Address());
		EXPECT_EQ(expectedUniqueKey, entry.uniqueKey());

		EXPECT_THROW(entry.asAddressRestriction(), bitxorcore_runtime_error);
		EXPECT_THROW(const_cast<const TokenRestrictionEntry&>(entry).asAddressRestriction(), bitxorcore_runtime_error);

		EXPECT_EQ(TokenId(123), entry.asGlobalRestriction().tokenId());
		EXPECT_EQ(TokenId(123), const_cast<const TokenRestrictionEntry&>(entry).asGlobalRestriction().tokenId());
	}

	// endregion

	// region TokenRestrictionEntry - copy constructor / assignment operator

	namespace {
		template<typename TCopier>
		void AssertCopyWithAddressRestriction(TCopier copier) {
			// Arrange:
			TokenRestrictionEntry originalEntry(TokenAddressRestriction(TokenId(123), Address{ { 35 } }));
			auto uniqueKey = originalEntry.uniqueKey();

			// Act: make a copy and set a restriction
			auto entry = copier(originalEntry);
			entry.asAddressRestriction().set(1, 1);

			// Assert: properties are correct
			EXPECT_EQ(TokenRestrictionEntry::EntryType::Address, entry.entryType());
			EXPECT_EQ(uniqueKey, entry.uniqueKey());

			// - restriction is only set in copy
			EXPECT_EQ(0u, originalEntry.asAddressRestriction().size());
			EXPECT_EQ(1u, entry.asAddressRestriction().size());
		}

		template<typename TCopier>
		void AssertCopyWithGlobalRestriction(TCopier copier) {
			// Arrange:
			TokenRestrictionEntry originalEntry(TokenGlobalRestriction(TokenId(123)));
			auto uniqueKey = originalEntry.uniqueKey();

			// Act: make a copy and set a restriction
			auto entry = copier(originalEntry);
			entry.asGlobalRestriction().set(1, { TokenId(), 1, model::TokenRestrictionType::EQ });

			// Assert: properties are correct
			EXPECT_EQ(TokenRestrictionEntry::EntryType::Global, entry.entryType());
			EXPECT_EQ(uniqueKey, entry.uniqueKey());

			// - restriction is only set in copy
			EXPECT_EQ(0u, originalEntry.asGlobalRestriction().size());
			EXPECT_EQ(1u, entry.asGlobalRestriction().size());
		}
	}

	TEST(TEST_CLASS, CanCopyEntryContainingAddressRestriction) {
		AssertCopyWithAddressRestriction([](const auto& originalEntry) {
			// Act:
			return TokenRestrictionEntry(originalEntry);
		});
	}

	TEST(TEST_CLASS, CanCopyEntryContainingGlobalRestriction) {
		AssertCopyWithGlobalRestriction([](const auto& originalEntry) {
			// Act:
			return TokenRestrictionEntry(originalEntry);
		});
	}

	TEST(TEST_CLASS, CanAssignEntryContainingAddressRestriction) {
		AssertCopyWithAddressRestriction([](const auto& originalEntry) {
			// Act:
			TokenRestrictionEntry entry(TokenGlobalRestriction(TokenId(0)));
			const auto& result = (entry = originalEntry);

			// Assert:
			EXPECT_EQ(&entry, &result);
			return entry;
		});
	}

	TEST(TEST_CLASS, CanAssignEntryContainingGlobalRestriction) {
		AssertCopyWithGlobalRestriction([](const auto& originalEntry) {
			// Act:
			TokenRestrictionEntry entry(TokenAddressRestriction(TokenId(0), Address()));
			const auto& result = (entry = originalEntry);

			// Assert:
			EXPECT_EQ(&entry, &result);
			return entry;
		});
	}

	// endregion

	// region CreateTokenRestrictionEntryKey

	TEST(TEST_CLASS, CanCreateTokenRestrictionEntryKeyFromTokenId) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();

		// Act:
		auto key = CreateTokenRestrictionEntryKey(tokenId);

		// Assert:
		auto expectedUniqueKey = CalculateExpectedUniqueKey(tokenId, Address());
		EXPECT_EQ(expectedUniqueKey, key);
	}

	TEST(TEST_CLASS, CanCreateTokenRestrictionEntryKeyFromTokenIdAndAddress) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<TokenId>();
		auto address = test::GenerateRandomByteArray<Address>();

		// Act:
		auto key = CreateTokenRestrictionEntryKey(tokenId, address);

		// Assert:
		auto expectedUniqueKey = CalculateExpectedUniqueKey(tokenId, address);
		EXPECT_EQ(expectedUniqueKey, key);
	}

	// endregion
}}
