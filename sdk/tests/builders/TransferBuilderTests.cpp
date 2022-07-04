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

#include "src/builders/TransferBuilder.h"
#include "src/extensions/ConversionExtensions.h"
#include "src/extensions/IdGenerator.h"
#include "bitxorcore/crypto/Hashes.h"
#include "sdk/tests/builders/test/BuilderTestUtils.h"
#include <map>

namespace bitxorcore { namespace builders {

#define TEST_CLASS TransferBuilderTests

	namespace {
		using RegularTraits = test::RegularTransactionTraits<model::TransferTransaction>;
		using EmbeddedTraits = test::EmbeddedTransactionTraits<model::EmbeddedTransferTransaction>;

		using TransactionType = model::TransferTransaction;
		using TransactionPtr = std::unique_ptr<TransactionType>;

		struct TransactionProperties {
		public:
			TransactionProperties() : Recipient(extensions::CopyToUnresolvedAddress(test::GenerateRandomByteArray<Address>()))
			{}

		public:
			UnresolvedAddress Recipient;
			std::vector<uint8_t> Message;
			std::vector<model::UnresolvedToken> Tokens;
		};

		template<typename TTransaction>
		void AssertTransactionProperties(const TransactionProperties& expectedProperties, const TTransaction& transaction) {
			EXPECT_EQ(expectedProperties.Recipient, transaction.RecipientAddress);

			ASSERT_EQ(expectedProperties.Message.size(), transaction.MessageSize);
			EXPECT_EQ_MEMORY(expectedProperties.Message.data(), transaction.MessagePtr(), expectedProperties.Message.size());

			// - note token comparison preserves order
			ASSERT_EQ(expectedProperties.Tokens.size(), transaction.TokensCount);
			const auto* pActualToken = transaction.TokensPtr();
			for (auto i = 0u; i < transaction.TokensCount; ++i, ++pActualToken) {
				EXPECT_EQ(expectedProperties.Tokens[i].TokenId, pActualToken->TokenId) << "token at " << i;
				EXPECT_EQ(expectedProperties.Tokens[i].Amount, pActualToken->Amount) << "token at " << i;
			}
		}

		template<typename TTraits>
		void AssertCanBuildTransfer(
				size_t additionalSize,
				const TransactionProperties& expectedProperties,
				const consumer<TransferBuilder&>& buildTransaction) {
			// Arrange:
			auto networkIdentifier = static_cast<model::NetworkIdentifier>(0x62);
			auto signer = test::GenerateRandomByteArray<Key>();

			// Act:
			TransferBuilder builder(networkIdentifier, signer);
			builder.setRecipientAddress(expectedProperties.Recipient);
			buildTransaction(builder);
			auto pTransaction = TTraits::InvokeBuilder(builder);

			// Assert:
			TTraits::CheckBuilderSize(additionalSize, builder);
			TTraits::CheckFields(additionalSize, *pTransaction);
			EXPECT_EQ(signer, pTransaction->SignerPublicKey);
			EXPECT_EQ(1u, pTransaction->Version);
			EXPECT_EQ(static_cast<model::NetworkIdentifier>(0x62), pTransaction->Network);
			EXPECT_EQ(model::Entity_Type_Transfer, pTransaction->Type);

			AssertTransactionProperties(expectedProperties, *pTransaction);
		}

		void RunBuilderTest(const consumer<TransferBuilder&>& buildTransaction) {
			// Arrange:
			TransferBuilder builder(static_cast<model::NetworkIdentifier>(0x62), test::GenerateRandomByteArray<Key>());
			builder.setRecipientAddress(extensions::CopyToUnresolvedAddress(test::GenerateRandomByteArray<Address>()));

			// Act:
			buildTransaction(builder);
		}
	}

#define TRAITS_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_Regular) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<RegularTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Embedded) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<EmbeddedTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	// region basic

	TRAITS_BASED_TEST(CanCreateTransferWithoutMessageOrTokens) {
		AssertCanBuildTransfer<TTraits>(0, TransactionProperties(), [](const auto&) {});
	}

	// endregion

	// region message

	namespace {
		struct BinaryMessageTraits {
			static constexpr auto GenerateRandomMessage = test::GenerateRandomVector;

			static void SetMessage(TransferBuilder& builder, const std::vector<uint8_t>& message) {
				builder.setMessage(message);
			}
		};
	}

#define TRAITS_BASED_MESSAGE_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_Regular_Binary) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<RegularTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Embedded_Binary) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<EmbeddedTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	TRAITS_BASED_MESSAGE_TEST(CanCreateTransferWithMessage) {
		// Arrange:
		auto expectedProperties = TransactionProperties();
		expectedProperties.Message = BinaryMessageTraits::GenerateRandomMessage(212);

		// Act:
		auto additionalSize = expectedProperties.Message.size();
		AssertCanBuildTransfer<TTraits>(additionalSize, expectedProperties, [&message = expectedProperties.Message](auto& builder) {
			BinaryMessageTraits::SetMessage(builder, message);
		});
	}

	TEST(TEST_CLASS, CannotSetEmptyMessage) {
		// Arrange:
		RunBuilderTest([](auto& builder) {
			// Act + Assert:
			EXPECT_THROW(BinaryMessageTraits::SetMessage(builder, {}), bitxorcore_invalid_argument);
		});
	}

	TEST(TEST_CLASS, CannotSetMultipleMessages) {
		// Arrange:
		RunBuilderTest([](auto& builder) {
			BinaryMessageTraits::SetMessage(builder, BinaryMessageTraits::GenerateRandomMessage(212));

			// Act + Assert:
			EXPECT_THROW(
					BinaryMessageTraits::SetMessage(builder, BinaryMessageTraits::GenerateRandomMessage(212)),
					bitxorcore_runtime_error);
		});
	}

	// endregion

	// region tokens

	namespace {
		struct TokenIdTraits {
			static std::vector<model::UnresolvedToken> GenerateTokens(size_t count) {
				std::vector<model::UnresolvedToken> tokens;
				for (auto i = 0u; i < count; ++i) {
					auto token = model::UnresolvedToken{
						test::GenerateRandomValue<UnresolvedTokenId>(),
						test::GenerateRandomValue<Amount>()
					};
					tokens.push_back(token);
				}

				std::sort(tokens.begin(), tokens.end(), [](const auto& lhs, const auto& rhs) {
					return lhs.TokenId < rhs.TokenId;
				});

				return tokens;
			}
		};

		void AddToken(TransferBuilder& builder, UnresolvedTokenId tokenId, Amount amount) {
			builder.addToken({ tokenId, amount });
		}

		template<typename TTraits>
		void AssertCanCreateTransferWithTokens(size_t numTokens) {
			// Arrange:
			auto expectedProperties = TransactionProperties();
			expectedProperties.Tokens = TokenIdTraits::GenerateTokens(numTokens);

			// Act:
			auto additionalSize = expectedProperties.Tokens.size() * sizeof(model::UnresolvedToken);
			AssertCanBuildTransfer<TTraits>(additionalSize, expectedProperties, [&tokens = expectedProperties.Tokens](auto& builder) {
				for (const auto& token : tokens)
					AddToken(builder, token.TokenId, token.Amount);
			});
		}
	}

#define TRAITS_BASED_TOKENS_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_Regular_Id) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<RegularTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Embedded_Id) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<EmbeddedTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	TRAITS_BASED_TOKENS_TEST(CanCreateTransferWithSingleToken) {
		AssertCanCreateTransferWithTokens<TTraits>(1);
	}

	TRAITS_BASED_TOKENS_TEST(CanCreateTransferWithMultipleTokens) {
		AssertCanCreateTransferWithTokens<TTraits>(3);
	}

	TRAITS_BASED_TEST(CannotAddSameTokenMultipleTimes) {
		// Arrange:
		RunBuilderTest([](auto& builder) {
			auto tokenPair = *TokenIdTraits::GenerateTokens(1).cbegin();

			// Act:
			AddToken(builder, tokenPair.TokenId, tokenPair.Amount);

			// Act + Assert:
			EXPECT_THROW(AddToken(builder, tokenPair.TokenId, tokenPair.Amount), bitxorcore_runtime_error);
		});
	}

	TRAITS_BASED_TEST(MultipleTokensAreSortedByTokenId) {
		// Arrange:
		auto expectedProperties = TransactionProperties();
		expectedProperties.Tokens = {
			{ UnresolvedTokenId(12), Amount(4'321) },
			{ UnresolvedTokenId(23), Amount(3'321) },
			{ UnresolvedTokenId(75), Amount(1'321) },
			{ UnresolvedTokenId(99), Amount(7'321) }
		};

		auto additionalSize = 4 * sizeof(model::UnresolvedToken);
		AssertCanBuildTransfer<TTraits>(additionalSize, expectedProperties, [](auto& builder) {
			// Act: add tokens out of order
			AddToken(builder, UnresolvedTokenId(12), Amount(4'321));
			AddToken(builder, UnresolvedTokenId(99), Amount(7'321));
			AddToken(builder, UnresolvedTokenId(75), Amount(1'321));
			AddToken(builder, UnresolvedTokenId(23), Amount(3'321));
		});
	}

	// endregion

	// region message and tokens

	namespace {
		static void SetMessage(TransferBuilder& builder, const std::string& message) {
			builder.setMessage({ reinterpret_cast<const uint8_t*>(message.data()), message.size() });
		}
	}

	TRAITS_BASED_TEST(CanCreateTransferWithMessageAndTokens) {
		// Arrange:
		auto message = std::string("this is a great transfer!");
		auto expectedProperties = TransactionProperties();
		expectedProperties.Message.resize(message.size());
		std::memcpy(&expectedProperties.Message[0], message.data(), message.size());
		expectedProperties.Tokens = {
			{ UnresolvedTokenId(0), Amount(4'321) },
			{ UnresolvedTokenId(1234), Amount(1'000'000) }
		};

		// Act:
		auto additionalSize = message.size() + 2 * sizeof(model::UnresolvedToken);
		AssertCanBuildTransfer<TTraits>(additionalSize, expectedProperties, [&message](auto& builder) {
			AddToken(builder, UnresolvedTokenId(0), Amount(4'321));
			SetMessage(builder, message);
			AddToken(builder, UnresolvedTokenId(1234), Amount(1'000'000));
		});
	}

	// endregion
}}
