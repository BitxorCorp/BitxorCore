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

#include "src/builders/TokenSupplyChangeBuilder.h"
#include "sdk/tests/builders/test/BuilderTestUtils.h"

namespace bitxorcore { namespace builders {

#define TEST_CLASS TokenSupplyChangeBuilderTests

	namespace {
		using RegularTraits = test::RegularTransactionTraits<model::TokenSupplyChangeTransaction>;
		using EmbeddedTraits = test::EmbeddedTransactionTraits<model::EmbeddedTokenSupplyChangeTransaction>;

		using TransactionType = model::TokenSupplyChangeTransaction;
		using TransactionPtr = std::unique_ptr<TransactionType>;

		struct TransactionProperties {
		public:
			explicit TransactionProperties(UnresolvedTokenId tokenId)
					: TokenId(tokenId)
					, Action(model::TokenSupplyChangeAction::Decrease)
			{}

		public:
			UnresolvedTokenId TokenId;
			model::TokenSupplyChangeAction Action;
			Amount Delta;
		};

		template<typename TTransaction>
		void AssertTransactionProperties(const TransactionProperties& expectedProperties, const TTransaction& transaction) {
			EXPECT_EQ(expectedProperties.TokenId, transaction.TokenId);
			EXPECT_EQ(expectedProperties.Action, transaction.Action);
			EXPECT_EQ(expectedProperties.Delta, transaction.Delta);
		}

		template<typename TTraits>
		void AssertCanBuildTransaction(
				const TransactionProperties& expectedProperties,
				const consumer<TokenSupplyChangeBuilder&>& buildTransaction) {
			// Arrange:
			auto networkIdentifier = static_cast<model::NetworkIdentifier>(0x62);
			auto signer = test::GenerateRandomByteArray<Key>();

			// Act:
			auto builder = TokenSupplyChangeBuilder(networkIdentifier, signer);
			buildTransaction(builder);
			auto pTransaction = TTraits::InvokeBuilder(builder);

			// Assert:
			TTraits::CheckBuilderSize(0, builder);
			TTraits::CheckFields(0, *pTransaction);
			EXPECT_EQ(signer, pTransaction->SignerPublicKey);
			EXPECT_EQ(1u, pTransaction->Version);
			EXPECT_EQ(static_cast<model::NetworkIdentifier>(0x62), pTransaction->Network);
			EXPECT_EQ(model::Entity_Type_Token_Supply_Change, pTransaction->Type);

			AssertTransactionProperties(expectedProperties, *pTransaction);
		}
	}

#define TRAITS_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_Regular) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<RegularTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Embedded) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<EmbeddedTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	// region constructor

	TRAITS_BASED_TEST(CanCreateTransaction) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<UnresolvedTokenId>();

		auto expectedProperties = TransactionProperties(tokenId);

		// Assert:
		AssertCanBuildTransaction<TTraits>(expectedProperties, [tokenId](auto& builder) {
			builder.setTokenId(tokenId);
		});
	}

	// endregion

	// region settings

	TRAITS_BASED_TEST(CanSetAction) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<UnresolvedTokenId>();

		auto expectedProperties = TransactionProperties(tokenId);
		expectedProperties.Action = model::TokenSupplyChangeAction::Increase;

		// Assert:
		AssertCanBuildTransaction<TTraits>(expectedProperties, [tokenId](auto& builder) {
			builder.setTokenId(tokenId);
			builder.setAction(model::TokenSupplyChangeAction::Increase);
		});
	}

	TRAITS_BASED_TEST(CanSetDelta) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<UnresolvedTokenId>();

		auto expectedProperties = TransactionProperties(tokenId);
		expectedProperties.Delta = Amount(12345678);

		// Assert:
		AssertCanBuildTransaction<TTraits>(expectedProperties, [tokenId](auto& builder) {
			builder.setTokenId(tokenId);
			builder.setDelta(Amount(12345678));
		});
	}

	TRAITS_BASED_TEST(CanSetActionAndDelta) {
		// Arrange:
		auto tokenId = test::GenerateRandomValue<UnresolvedTokenId>();

		auto expectedProperties = TransactionProperties(tokenId);
		expectedProperties.Action = model::TokenSupplyChangeAction::Increase;
		expectedProperties.Delta = Amount(12345678);

		// Assert:
		AssertCanBuildTransaction<TTraits>(expectedProperties, [tokenId](auto& builder) {
			builder.setTokenId(tokenId);
			builder.setAction(model::TokenSupplyChangeAction::Increase);
			builder.setDelta(Amount(12345678));
		});
	}

	// endregion
}}
