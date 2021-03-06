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

#include "LocalNodeRequestTestUtils.h"
#include "sdk/src/extensions/BlockExtensions.h"
#include "sdk/src/extensions/TransactionExtensions.h"
#include "bitxorcore/preprocessor.h"
#include "tests/test/core/TransactionTestUtils.h"
#include "tests/test/local/RealTransactionFactory.h"
#include "tests/test/nodeps/Genesis.h"

namespace bitxorcore { namespace test {

	// region push

	std::shared_ptr<ionet::PacketIo> PushPayload(ExternalSourceConnection& connection, const ionet::PacketPayload& payload) {
		BITXORCORE_LOG(debug) << " >>>> starting push";
		std::atomic_bool isWriteFinished(false);
		connection.connect([&isWriteFinished, &payload](const auto& pPacketSocket) {
			auto pBufferedIo = pPacketSocket->buffered();

			BITXORCORE_LOG(debug) << "writing entity";
			pBufferedIo->write(payload, [&isWriteFinished](auto code) {
				BITXORCORE_LOG(debug) << "write result: " << code;
				isWriteFinished = true;
			});

			// perform a chain statistics (request / response) to ensure socket is not closed until ssl handshake is completed
			api::CreateRemoteChainApiWithoutRegistry(*pBufferedIo)->chainStatistics().then([](auto&& chainStatisticsFuture) {
				try {
					BITXORCORE_LOG(debug) << "received height from remote after pushing payload: " << chainStatisticsFuture.get().Height;
				} catch (const api::bitxorcore_api_error& ex) {
					BITXORCORE_LOG(warning) << "could not receive height from remote after pushing payload: " << ex.what();
				}
			});
		});

		WAIT_FOR(isWriteFinished);

		BITXORCORE_LOG(debug) << " <<< push finished";
		return connection.io();
	}

	namespace {
		crypto::KeyPair GetGenesisAccountKeyPair() {
			return crypto::KeyPair::FromString(Test_Network_Private_Keys[4]); // use a genesis account
		}

		model::PreviousBlockContext LoadGenesisPreviousBlockContext() {
			mocks::MockMemoryBlockStorage storage;
			auto pGenesisBlockElement = storage.loadBlockElement(Height(1));
			return model::PreviousBlockContext(*pGenesisBlockElement);
		}

		std::shared_ptr<model::Block> CreateBlock() {
			constexpr auto Network_Identifier = model::NetworkIdentifier::Testnet;
			auto signer = GetGenesisAccountKeyPair();
			auto context = LoadGenesisPreviousBlockContext();

			// ImportanceGrouping is 1
			auto entityType = model::Entity_Type_Block_Importance;
			auto pBlock = model::CreateBlock(entityType, context, Network_Identifier, signer.publicKey(), model::Transactions());
			pBlock->Timestamp = context.Timestamp + Timestamp(60000);

			auto vrfKeyPair = LookupVrfKeyPair(signer.publicKey());
			auto vrfProof = crypto::GenerateVrfProof(context.GenerationHash, vrfKeyPair);
			pBlock->GenerationHashProof = { vrfProof.Gamma, vrfProof.VerificationHash, vrfProof.Scalar };

			auto& blockFooter = model::GetBlockFooter<model::ImportanceBlockFooter>(*pBlock);
			blockFooter.HarvestingEligibleAccountsCount = CountOf(Test_Network_Vrf_Private_Keys);
			blockFooter.PreviousImportanceBlockHash = context.BlockHash;

			extensions::BlockExtensions(GetDefaultGenerationHashSeed()).signFullBlock(signer, *pBlock);
			return PORTABLE_MOVE(pBlock);
		}
	}

	std::shared_ptr<ionet::PacketIo> PushValidBlock(ExternalSourceConnection& connection) {
		auto pBlock = CreateBlock();
		return PushEntity(connection, ionet::PacketType::Push_Block, pBlock);
	}

	std::shared_ptr<ionet::PacketIo> PushValidTransaction(ExternalSourceConnection& connection) {
		auto signer = GetGenesisAccountKeyPair();
		auto context = LoadGenesisPreviousBlockContext();

		auto recipient = test::GenerateRandomUnresolvedAddress();
		auto pTransaction = CreateUnsignedTransferTransaction(signer.publicKey(), recipient, Amount(10000));
		pTransaction->MaxFee = Amount(10 * pTransaction->Size);
		pTransaction->Deadline = context.Timestamp + Timestamp(120000);
		extensions::TransactionExtensions(GetGenesisGenerationHashSeed()).sign(signer, *pTransaction);

		return PushEntity(connection, ionet::PacketType::Push_Transactions, std::shared_ptr<model::Transaction>(std::move(pTransaction)));
	}

	// endregion

	// region height

	namespace {
		constexpr auto Long_Wait_Seconds = 15u;
	}

	Height GetLocalNodeHeightViaApi(ExternalSourceConnection& connection) {
		struct ChainStatisticsResult {
		public:
			ChainStatisticsResult() : IsHeightReceived(false)
			{}

		public:
			bitxorcore::Height Height;
			std::atomic_bool IsHeightReceived;
		};

		auto pChainStatisticsResult = std::make_shared<ChainStatisticsResult>();
		connection.apiCall([pChainStatisticsResult](const auto& pRemoteChainApi) {
			pRemoteChainApi->chainStatistics().then([pChainStatisticsResult](auto&& chainStatisticsFuture) {
				pChainStatisticsResult->Height = chainStatisticsFuture.get().Height;
				pChainStatisticsResult->IsHeightReceived = true;
			});
		});

		WAIT_FOR_VALUE_SECONDS(true, pChainStatisticsResult->IsHeightReceived, Long_Wait_Seconds);
		return Height(pChainStatisticsResult->Height);
	}

	void WaitForLocalNodeHeight(ExternalSourceConnection& connection, Height height) {
		// use exponential backoff to reduce log noise
		auto sleepMs = 100;
		supplier<Height> heightSupplierWithBackoff = [&connection, desiredHeight = height, &sleepMs]() {
			auto currentHeight = GetLocalNodeHeightViaApi(connection);
			if (desiredHeight != currentHeight) {
				Sleep(sleepMs);
				sleepMs *= 2;
			}

			return currentHeight;
		};

		WAIT_FOR_VALUE_EXPR_SECONDS(height, heightSupplierWithBackoff(), Long_Wait_Seconds);
	}

	// endregion
}}
