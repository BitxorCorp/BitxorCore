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

#include "sdk/src/extensions/BlockExtensions.h"
#include "sdk/src/extensions/TransactionExtensions.h"
#include "bitxorcore/model/BlockUtils.h"
#include "bitxorcore/model/NetworkIdentifier.h"
#include "tests/int/stress/test/EntityDump.h"
#include "tests/test/core/BlockTestUtils.h"
#include "tests/test/local/RealTransactionFactory.h"
#include "tests/test/nodeps/Genesis.h"
#include "tests/test/nodeps/TestNetworkConstants.h"
#include "tests/TestHarness.h"

namespace bitxorcore {

#define TEST_CLASS GenesisBlockGeneratorIntegrityTests

	namespace {
		constexpr auto Network_Identifier = model::NetworkIdentifier::Testnet;
		constexpr Amount Genesis_Amount(9000000000ull / CountOf(test::Test_Network_Private_Keys) * 1000000ull);

		bool VerifyGenesisNetworkTransactionSignature(const model::Transaction& transaction) {
			return extensions::TransactionExtensions(test::GetGenesisGenerationHashSeed()).verify(transaction);
		}
	}

	TEST(TEST_CLASS, CreateTransaction) {
		// Arrange:
		auto signer = crypto::KeyPair::FromString(test::Test_Network_Genesis_Private_Key);
		auto recipient = crypto::KeyPair::FromString(test::Test_Network_Private_Keys[0]);

		// Act:
		auto pTransaction = test::CreateTransferTransaction(signer, recipient.publicKey(), Amount(1234));
		test::EntityDump(*pTransaction);

		// Assert:
		EXPECT_TRUE(VerifyGenesisNetworkTransactionSignature(*pTransaction));
	}

	TEST(TEST_CLASS, CreateGenesisBlockTransactions) {
		// Arrange:
		auto signer = crypto::KeyPair::FromString(test::Test_Network_Genesis_Private_Key);

		for (const auto* pRecipientPrivateKeyString : test::Test_Network_Private_Keys) {
			auto recipient = crypto::KeyPair::FromString(pRecipientPrivateKeyString);

			// Act:
			auto pTransaction = test::CreateTransferTransaction(signer, recipient.publicKey(), Genesis_Amount);
#ifdef _DEBUG
			test::EntityDump(*pTransaction);
#endif

			// Assert:
			EXPECT_TRUE(VerifyGenesisNetworkTransactionSignature(*pTransaction));
		}
	}

	namespace {
		auto CreateGenesisBlock() {
			auto signer = crypto::KeyPair::FromString(test::Test_Network_Genesis_Private_Key);
			auto generationHashSeed = test::GetGenesisGenerationHashSeed();

			model::Transactions transactions;
			for (const auto* pRecipientPrivateKeyString : test::Test_Network_Private_Keys) {
				auto recipient = crypto::KeyPair::FromString(pRecipientPrivateKeyString);
				auto pTransfer = test::CreateTransferTransaction(signer, recipient.publicKey(), Genesis_Amount);
				pTransfer->MaxFee = Amount(0);
				extensions::TransactionExtensions(generationHashSeed).sign(signer, *pTransfer);
				transactions.push_back(std::move(pTransfer));
			}

			auto entityType = model::Entity_Type_Block_Normal;
			model::PreviousBlockContext context;
			context.GenerationHash = generationHashSeed.copyTo<GenerationHash>();
			auto pBlock = model::CreateBlock(entityType, context, Network_Identifier, signer.publicKey(), transactions);
			extensions::BlockExtensions(generationHashSeed).signFullBlock(signer, *pBlock);
			return pBlock;
		}

		void VerifyGenesisBlock(const model::Block& block) {
			auto blockExtensions = extensions::BlockExtensions(test::GetGenesisGenerationHashSeed());
			auto verifyResult = blockExtensions.verifyFullBlock(block);
			EXPECT_EQ(extensions::VerifyFullBlockResult::Success, verifyResult);
		}
	}

	TEST(TEST_CLASS, CreateGenesisBlock) {
		// Act:
		auto pBlock = CreateGenesisBlock();
		test::EntityDump(*pBlock);

		// Assert:
		VerifyGenesisBlock(*pBlock);
	}
}
