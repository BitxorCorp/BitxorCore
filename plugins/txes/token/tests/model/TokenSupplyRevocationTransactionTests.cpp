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

#include "src/model/TokenSupplyRevocationTransaction.h"
#include "tests/test/core/TransactionTestUtils.h"
#include "tests/test/nodeps/Alignment.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace model {

#define TEST_CLASS TokenSupplyRevocationTransactionTests

	// region size + alignment + properties

#define TRANSACTION_FIELDS FIELD(SourceAddress) FIELD(Token)

	namespace {
		template<typename T>
		void AssertTransactionHasExpectedSize(size_t baseSize) {
			// Arrange:
			auto expectedSize = baseSize;

#define FIELD(X) expectedSize += SizeOf32<decltype(T::X)>();
			TRANSACTION_FIELDS
#undef FIELD

			// Assert:
			EXPECT_EQ(expectedSize, sizeof(T));
			EXPECT_EQ(baseSize + 40u, sizeof(T));
		}

		template<typename T>
		void AssertTransactionHasProperAlignment() {
#define FIELD(X) EXPECT_ALIGNED(T, X);
			TRANSACTION_FIELDS
#undef FIELD
		}

		template<typename T>
		void AssertTransactionHasExpectedProperties() {
			// Assert:
			EXPECT_EQ(Entity_Type_Token_Supply_Revocation, T::Entity_Type);
			EXPECT_EQ(1u, T::Current_Version);
		}
	}

#undef TRANSACTION_FIELDS

	ADD_BASIC_TRANSACTION_SIZE_PROPERTY_TESTS(TokenSupplyRevocation)

	// endregion

	// region CalculateRealSize

	TEST(TEST_CLASS, CanCalculateRealSizeWithReasonableValues) {
		// Arrange:
		TokenSupplyRevocationTransaction transaction;
		transaction.Size = 0;

		// Act:
		auto realSize = TokenSupplyRevocationTransaction::CalculateRealSize(transaction);

		// Assert:
		EXPECT_EQ(sizeof(TokenSupplyRevocationTransaction), realSize);
	}

	// endregion
}}
