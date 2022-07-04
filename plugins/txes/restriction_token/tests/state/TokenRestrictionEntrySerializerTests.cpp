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

#include "src/state/TokenRestrictionEntrySerializer.h"
#include "tests/test/TokenRestrictionTestUtils.h"
#include "tests/test/core/SerializerTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS TokenRestrictionEntrySerializerTests

	namespace {
		// region raw structures

#pragma pack(push, 1)

		struct TokenAddressRestrictionHeader {
			TokenRestrictionEntry::EntryType EntryType;
			bitxorcore::TokenId TokenId;
			bitxorcore::Address Address;
			uint8_t NumValues;
		};

		struct TokenGlobalRestrictionHeader {
			TokenRestrictionEntry::EntryType EntryType;
			bitxorcore::TokenId TokenId;
			uint8_t NumValues;
		};

		struct TokenAddressRestrictionTuple {
			uint64_t Key;
			uint64_t Value;
		};

		struct TokenGlobalRestrictionTuple {
			uint64_t Key;
			TokenId ReferenceTokenId;
			uint64_t RestrictionValue;
			model::TokenRestrictionType RestrictionType;
		};

#pragma pack(pop)

		// endregion
	}

	// region traits

	namespace {
		struct AddressRestrictionTraits {
			using HeaderType = TokenAddressRestrictionHeader;
			using TupleType = TokenAddressRestrictionTuple;

			static TokenRestrictionEntry GenerateRandomEntry(uint8_t count) {
				auto tokenId = test::GenerateRandomValue<TokenId>();
				auto address = test::GenerateRandomByteArray<Address>();
				TokenAddressRestriction restriction(tokenId, address);

				for (auto i = 0u; i < count; ++i)
					restriction.set(i + 100, test::Random());

				return TokenRestrictionEntry(restriction);
			}

			static auto GetKeys(const TokenRestrictionEntry& entry) {
				return entry.asAddressRestriction().keys();
			}

			static void SetRandomValue(TokenRestrictionEntry& entry, uint64_t key) {
				entry.asAddressRestriction().set(key, test::Random());
			}

			static void AssertHeader(const TokenRestrictionEntry& entry, const TokenAddressRestrictionHeader& header) {
				auto restriction = entry.asAddressRestriction();

				EXPECT_EQ(TokenRestrictionEntry::EntryType::Address, header.EntryType);
				EXPECT_EQ(restriction.tokenId(), header.TokenId);
				EXPECT_EQ(restriction.address(), header.Address);
				EXPECT_EQ(restriction.keys().size(), header.NumValues);
			}

			static void AssertTuple(
					const TokenRestrictionEntry& entry,
					uint64_t key,
					const TokenAddressRestrictionTuple& tuple,
					const std::string& message) {
				auto restriction = entry.asAddressRestriction();

				EXPECT_EQ(restriction.get(key), tuple.Value) << message;
			}
		};

		struct GlobalRestrictionTraits {
		public:
			using HeaderType = TokenGlobalRestrictionHeader;
			using TupleType = TokenGlobalRestrictionTuple;

			static TokenRestrictionEntry GenerateRandomEntry(uint8_t count) {
				auto tokenId = test::GenerateRandomValue<TokenId>();
				TokenGlobalRestriction restriction(tokenId);

				for (auto i = 0u; i < count; ++i)
					restriction.set(i + 100, CreateRandomRule());

				return TokenRestrictionEntry(restriction);
			}

			static auto GetKeys(const TokenRestrictionEntry& entry) {
				return entry.asGlobalRestriction().keys();
			}

			static void SetRandomValue(TokenRestrictionEntry& entry, uint64_t key) {
				entry.asGlobalRestriction().set(key, CreateRandomRule());
			}

			static void AssertHeader(const TokenRestrictionEntry& entry, const TokenGlobalRestrictionHeader& header) {
				auto restriction = entry.asGlobalRestriction();

				EXPECT_EQ(TokenRestrictionEntry::EntryType::Global, header.EntryType);
				EXPECT_EQ(restriction.tokenId(), header.TokenId);
				EXPECT_EQ(restriction.keys().size(), header.NumValues);
			}

			static void AssertTuple(
					const TokenRestrictionEntry& entry,
					uint64_t key,
					const TokenGlobalRestrictionTuple& tuple,
					const std::string& message) {
				auto restriction = entry.asGlobalRestriction();
				TokenGlobalRestriction::RestrictionRule rule;
				restriction.tryGet(key, rule);

				EXPECT_EQ(rule.ReferenceTokenId, tuple.ReferenceTokenId) << message;
				EXPECT_EQ(rule.RestrictionValue, tuple.RestrictionValue) << message;
				EXPECT_EQ(rule.RestrictionType, tuple.RestrictionType) << message;
			}

		private:
			static TokenGlobalRestriction::RestrictionRule CreateRandomRule() {
				TokenGlobalRestriction::RestrictionRule rule;
				rule.ReferenceTokenId = test::GenerateRandomValue<TokenId>();
				rule.RestrictionValue = test::Random();
				rule.RestrictionType = model::TokenRestrictionType::LT;
				return rule;
			}
		};
	}

#define RESTRICTION_TRAITS_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_AddressRestriction) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<AddressRestrictionTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_GlobalRestriction) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<GlobalRestrictionTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	// endregion

	// region Save

	RESTRICTION_TRAITS_BASED_TEST(CanSaveEmptyEntry) {
		// Arrange:
		std::vector<uint8_t> buffer;
		mocks::MockMemoryStream outputStream(buffer);

		auto entry = TTraits::GenerateRandomEntry(0);

		// Act:
		TokenRestrictionEntrySerializer::Save(entry, outputStream);

		// Assert:
		ASSERT_EQ(sizeof(typename TTraits::HeaderType), buffer.size());

		const auto& header = reinterpret_cast<const typename TTraits::HeaderType&>(buffer[0]);
		TTraits::AssertHeader(entry, header);
	}

	RESTRICTION_TRAITS_BASED_TEST(CanSaveEntry) {
		// Arrange:
		std::vector<uint8_t> buffer;
		mocks::MockMemoryStream outputStream(buffer);

		auto entry = TTraits::GenerateRandomEntry(3);

		// Act:
		TokenRestrictionEntrySerializer::Save(entry, outputStream);

		// Assert:
		ASSERT_EQ(sizeof(typename TTraits::HeaderType) + 3 * sizeof(typename TTraits::TupleType), buffer.size());

		const auto& header = reinterpret_cast<const typename TTraits::HeaderType&>(buffer[0]);
		TTraits::AssertHeader(entry, header);

		auto i = 0u;
		const auto* pTuple = reinterpret_cast<const typename TTraits::TupleType*>(&buffer[sizeof(typename TTraits::HeaderType)]);
		for (auto key : TTraits::GetKeys(entry)) {
			auto tupleAlignedCopy = *pTuple; // not aligned so cannot be passed by reference
			auto message = "tuple at " + std::to_string(i++);
			EXPECT_EQ(key, tupleAlignedCopy.Key) << message;
			TTraits::AssertTuple(entry, key, tupleAlignedCopy, message);
			++pTuple;
		}
	}

	// endregion

	// region Save (Ordering)

	RESTRICTION_TRAITS_BASED_TEST(SavedRestrictionKeyValuePairsAreOrdered) {
		// Arrange:
		std::vector<uint8_t> buffer;
		mocks::MockMemoryStream outputStream(buffer);

		auto entry = TTraits::GenerateRandomEntry(0);
		for (auto key : std::initializer_list<uint64_t>{ 123, 34, 57, 12 })
			TTraits::SetRandomValue(entry, key);

		// Act:
		TokenRestrictionEntrySerializer::Save(entry, outputStream);

		// Assert:
		ASSERT_EQ(sizeof(typename TTraits::HeaderType) + 4 * sizeof(typename TTraits::TupleType), buffer.size());

		auto i = 0u;
		std::vector<uint64_t> expectedKeysOrdered{ 12, 34, 57, 123 };
		const auto* pTuple = reinterpret_cast<const typename TTraits::TupleType*>(&buffer[sizeof(typename TTraits::HeaderType)]);
		for (auto key : expectedKeysOrdered) {
			auto tupleAlignedCopy = *pTuple; // not aligned so cannot be passed by reference
			auto message = "tuple at " + std::to_string(i++);
			EXPECT_EQ(key, tupleAlignedCopy.Key) << message;
			++pTuple;
		}
	}

	// endregion

	// region Load (failure)

	RESTRICTION_TRAITS_BASED_TEST(CannotLoadWithInvalidEntryType) {
		// Arrange:
		std::vector<uint8_t> buffer;
		mocks::MockMemoryStream stream(buffer);

		auto entry = TTraits::GenerateRandomEntry(3);

		TokenRestrictionEntrySerializer::Save(entry, stream);
		stream.seek(0);

		// Sanity:
		ASSERT_EQ(sizeof(typename TTraits::HeaderType) + 3 * sizeof(typename TTraits::TupleType), buffer.size());

		// - corrupt the type
		auto& header = reinterpret_cast<typename TTraits::HeaderType&>(buffer[0]);
		header.EntryType = static_cast<TokenRestrictionEntry::EntryType>(3);

		// Act + Assert:
		EXPECT_THROW(TokenRestrictionEntrySerializer::Load(stream), bitxorcore_invalid_argument);
	}

	// endregion

	// region Roundtrip

	namespace {
		template<typename TTraits>
		void AssertCanRoundtripEntry(uint8_t count) {
			// Arrange:
			auto originalEntry = TTraits::GenerateRandomEntry(count);

			// Act:
			auto result = test::RunRoundtripBufferTest<TokenRestrictionEntrySerializer>(originalEntry);

			// Assert:
			test::AssertEqual(originalEntry, result);
		}
	}

	RESTRICTION_TRAITS_BASED_TEST(CanRoundtripEmptyEntry) {
		AssertCanRoundtripEntry<TTraits>(0);
	}

	RESTRICTION_TRAITS_BASED_TEST(CanRoundtripEntry) {
		AssertCanRoundtripEntry<TTraits>(3);
	}

	// endregion
}}
