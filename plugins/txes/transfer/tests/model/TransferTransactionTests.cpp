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

#include "src/model/TransferTransaction.h"
#include "bitxorcore/utils/MemoryUtils.h"
#include "tests/test/core/TransactionTestUtils.h"
#include "tests/test/core/VariableSizedEntityTestUtils.h"
#include "tests/test/nodeps/Alignment.h"
#include "tests/test/nodeps/NumericTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace model {

#define TEST_CLASS TransferTransactionTests

	// region size + alignment + properties

#define TRANSACTION_FIELDS FIELD(RecipientAddress) FIELD(MessageSize) FIELD(TokensCount)

	namespace {
		template<typename T>
		void AssertTransactionHasExpectedSize(size_t baseSize) {
			// Arrange:
			auto expectedSize = baseSize + sizeof(uint32_t) + sizeof(uint8_t);

#define FIELD(X) expectedSize += SizeOf32<decltype(T::X)>();
			TRANSACTION_FIELDS
#undef FIELD

			// Assert:
			EXPECT_EQ(expectedSize, sizeof(T));
			EXPECT_EQ(baseSize + 5 + 27u, sizeof(T));
		}

		template<typename T>
		void AssertTransactionHasProperAlignment() {
#define FIELD(X) EXPECT_ALIGNED(T, X);
			TRANSACTION_FIELDS
#undef FIELD

			EXPECT_EQ(0u, sizeof(T) % 8);
		}

		template<typename T>
		void AssertTransactionHasExpectedProperties() {
			// Assert:
			EXPECT_EQ(Entity_Type_Transfer, T::Entity_Type);
			EXPECT_EQ(1u, T::Current_Version);
		}
	}

#undef TRANSACTION_FIELDS

	ADD_BASIC_TRANSACTION_SIZE_PROPERTY_TESTS(Transfer)

	// endregion

	// region data pointers

	namespace {
		struct TransferTransactionTraits {
			static auto GenerateEntityWithAttachments(uint8_t numTokens, uint16_t messageSize) {
				uint32_t entitySize = SizeOf32<TransferTransaction>() + messageSize + numTokens * SizeOf32<UnresolvedToken>();
				auto pTransaction = utils::MakeUniqueWithSize<TransferTransaction>(entitySize);
				pTransaction->Size = entitySize;
				pTransaction->TokensCount = numTokens;
				pTransaction->MessageSize = messageSize;
				return pTransaction;
			}

			static constexpr size_t GetAttachment1Size(uint8_t numTokens) {
				return numTokens * sizeof(UnresolvedToken);
			}

			template<typename TEntity>
			static auto GetAttachmentPointer1(TEntity& entity) {
				return entity.TokensPtr();
			}

			template<typename TEntity>
			static auto GetAttachmentPointer2(TEntity& entity) {
				return entity.MessagePtr();
			}
		};
	}

	DEFINE_DUAL_ATTACHMENT_POINTER_TESTS(TEST_CLASS, TransferTransactionTraits)

	// endregion

	// region CalculateRealSize

	TEST(TEST_CLASS, CanCalculateRealSizeWithReasonableValues) {
		// Arrange:
		TransferTransaction transaction;
		transaction.Size = 0;
		transaction.MessageSize = 100;
		transaction.TokensCount = 7;

		// Act:
		auto realSize = TransferTransaction::CalculateRealSize(transaction);

		// Assert:
		EXPECT_EQ(16u, sizeof(UnresolvedToken));
		EXPECT_EQ(sizeof(TransferTransaction) + 100 + 7 * sizeof(UnresolvedToken), realSize);
	}

	TEST(TEST_CLASS, CalculateRealSizeDoesNotOverflowWithMaxValues) {
		// Arrange:
		TransferTransaction transaction;
		test::SetMaxValue(transaction.Size);
		test::SetMaxValue(transaction.MessageSize);
		test::SetMaxValue(transaction.TokensCount);

		// Act:
		auto realSize = TransferTransaction::CalculateRealSize(transaction);

		// Assert:
		ASSERT_EQ(0xFFFFFFFF, transaction.Size);
		EXPECT_EQ(sizeof(TransferTransaction) + 0xFFFF + 0xFF * sizeof(UnresolvedToken), realSize);
		EXPECT_GT(0xFFFFFFFF, realSize);
	}

	// endregion
}}
