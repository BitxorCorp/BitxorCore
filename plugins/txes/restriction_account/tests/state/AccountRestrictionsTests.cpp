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

#include "src/state/AccountRestrictions.h"
#include "bitxorcore/model/EntityType.h"
#include "tests/test/AccountRestrictionTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS AccountRestrictionsTests

	namespace {
		constexpr auto Add = model::AccountRestrictionModificationAction::Add;

		struct AccountAddressRestrictionTraits {
			using ValueType = Address;

			static constexpr model::AccountRestrictionFlags AccountRestrictionFlags() {
				return model::AccountRestrictionFlags::Address;
			}
		};

		struct AccountAddressOutgoingRestrictionTraits {
			using ValueType = Address;

			static constexpr model::AccountRestrictionFlags AccountRestrictionFlags() {
				return model::AccountRestrictionFlags::Address | model::AccountRestrictionFlags::Outgoing;
			}
		};

		struct AccountTokenRestrictionTraits {
			using ValueType = TokenId;

			static constexpr model::AccountRestrictionFlags AccountRestrictionFlags() {
				return model::AccountRestrictionFlags::TokenId;
			}
		};

		struct AccountOperationRestrictionTraits {
			using ValueType = model::EntityType;

			static constexpr model::AccountRestrictionFlags AccountRestrictionFlags() {
				return model::AccountRestrictionFlags::TransactionType | model::AccountRestrictionFlags::Outgoing;
			}
		};

		std::vector<model::AccountRestrictionFlags> CollectAccountRestrictionFlags(const AccountRestrictions& restrictions) {
			std::vector<model::AccountRestrictionFlags> types;
			for (const auto& restriction : restrictions)
				types.push_back(restriction.first);

			return types;
		}

		void AssertIsEmpty(const std::vector<size_t>& valueSizes, bool expectedResult) {
			// Arrange:
			auto restrictions = test::CreateAccountRestrictions(AccountRestrictionOperationType::Allow, valueSizes);

			// Act + Assert:
			EXPECT_EQ(expectedResult, restrictions.isEmpty());
		}
	}

#define ACCOUNT_RESTRICTION_TRAITS_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_Address) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<AccountAddressRestrictionTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_AddressOutgoing) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<AccountAddressOutgoingRestrictionTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Token) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<AccountTokenRestrictionTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Operation) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<AccountOperationRestrictionTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	TEST(TEST_CLASS, CanCreateAccountRestrictions) {
		// Arrange:
		auto address = test::GenerateRandomByteArray<Address>();

		// Act:
		AccountRestrictions restrictions(address);

		// Assert:
		EXPECT_EQ(address, restrictions.address());
		EXPECT_EQ(4u, restrictions.size());
	}

	TEST(TEST_CLASS, CanCreateAccountRestrictionsWithDifferentAccountRestrictionOperationTypes) {
		// Arrange:
		AccountRestrictions restrictions(test::GenerateRandomByteArray<Address>());
		auto& addressRestriction = restrictions.restriction(model::AccountRestrictionFlags::Address);
		auto& tokenRestriction = restrictions.restriction(model::AccountRestrictionFlags::TokenId);

		// Act:
		addressRestriction.block({ Add, test::GenerateRandomVector(Address::Size) });
		tokenRestriction.allow({ Add, test::GenerateRandomVector(sizeof(TokenId)) });

		// Assert:
		EXPECT_EQ(1u, addressRestriction.values().size());
		EXPECT_EQ(AccountRestrictionOperationType::Block, addressRestriction.descriptor().operationType());

		EXPECT_EQ(1u, tokenRestriction.values().size());
		EXPECT_EQ(AccountRestrictionOperationType::Allow, tokenRestriction.descriptor().operationType());
	}

	TEST(TEST_CLASS, IsEmptyReturnsTrueWhenNoAccountRestrictionHasValues) {
		AssertIsEmpty({ 0, 0, 0, 0 }, true);
	}

	TEST(TEST_CLASS, IsEmptyReturnsFalseWhenAtLeastOneAccountRestrictionHasAtLeastOneValue) {
		AssertIsEmpty({ 1, 0, 0, 0 }, false);
		AssertIsEmpty({ 0, 1, 0, 0 }, false);
		AssertIsEmpty({ 0, 0, 1, 0 }, false);
		AssertIsEmpty({ 0, 0, 0, 1 }, false);

		AssertIsEmpty({ 5, 0, 0, 0 }, false);
		AssertIsEmpty({ 0, 5, 0, 0 }, false);
		AssertIsEmpty({ 0, 0, 5, 0 }, false);
		AssertIsEmpty({ 0, 0, 0, 5 }, false);

		AssertIsEmpty({ 20, 17, 13, 9 }, false);
	}

	TEST(TEST_CLASS, CanIterateThroughAccountRestrictions) {
		// Arrange:
		AccountRestrictions restrictions(test::GenerateRandomByteArray<Address>());

		// Act:
		auto flagsContainer = CollectAccountRestrictionFlags(restrictions);

		// Assert:
		std::vector<model::AccountRestrictionFlags> expectedFlagsContainer{
			model::AccountRestrictionFlags::Address,
			model::AccountRestrictionFlags::TokenId,
			model::AccountRestrictionFlags::Address | model::AccountRestrictionFlags::Outgoing,
			model::AccountRestrictionFlags::TransactionType | model::AccountRestrictionFlags::Outgoing
		};
		EXPECT_EQ(expectedFlagsContainer, flagsContainer);
	}

	ACCOUNT_RESTRICTION_TRAITS_BASED_TEST(RestrictionReturnsCorrectAccountRestrictionWhenAccountRestrictionFlagsAreKnown) {
		// Arrange:
		AccountRestrictions restrictions(test::GenerateRandomByteArray<Address>());

		// Act:
		auto& restriction = restrictions.restriction(TTraits::AccountRestrictionFlags());

		// Assert:
		EXPECT_EQ(TTraits::AccountRestrictionFlags(), restriction.descriptor().directionalRestrictionFlags());
	}

	TEST(TEST_CLASS, RestrictionThrowsWhenAccountRestrictionFlagsAreUnknown) {
		// Arrange:
		AccountRestrictions restrictions(test::GenerateRandomByteArray<Address>());

		// Act + Assert:
		EXPECT_THROW(restrictions.restriction(model::AccountRestrictionFlags::Sentinel), bitxorcore_invalid_argument);
	}

	ACCOUNT_RESTRICTION_TRAITS_BASED_TEST(RestrictionReturnsCorrectAccountRestrictionWhenAccountRestrictionFlagsAreKnown_Const) {
		// Arrange:
		AccountRestrictions restrictions(test::GenerateRandomByteArray<Address>());

		// Act:
		auto& restriction = const_cast<const AccountRestrictions&>(restrictions).restriction(TTraits::AccountRestrictionFlags());

		// Assert:
		EXPECT_EQ(TTraits::AccountRestrictionFlags(), restriction.descriptor().directionalRestrictionFlags());
	}

	TEST(TEST_CLASS, RestrictionThrowsWhenAccountRestrictionFlagsAreUnknown_Const) {
		// Arrange:
		AccountRestrictions restrictions(test::GenerateRandomByteArray<Address>());

		// Act + Assert:
		EXPECT_THROW(
				const_cast<const AccountRestrictions&>(restrictions).restriction(model::AccountRestrictionFlags::Sentinel),
				bitxorcore_invalid_argument);
	}
}}
