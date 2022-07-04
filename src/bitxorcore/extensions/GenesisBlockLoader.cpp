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

#include "GenesisBlockLoader.h"
#include "ExecutionConfigurationFactory.h"
#include "LocalNodeStateRef.h"
#include "GenesisFundingObserver.h"
#include "PluginUtils.h"
#include "bitxorcore/chain/BatchEntityProcessor.h"
#include "bitxorcore/config/BitxorCoreConfiguration.h"
#include "bitxorcore/crypto/Vrf.h"
#include "bitxorcore/io/BlockStorageCache.h"
#include "bitxorcore/model/BlockUtils.h"
#include "bitxorcore/model/GenesisNotificationPublisher.h"
#include "bitxorcore/observers/NotificationObserverAdapter.h"
#include "bitxorcore/plugins/PluginManager.h"
#include "bitxorcore/utils/IntegerMath.h"

namespace bitxorcore { namespace extensions {

	namespace {
		std::unique_ptr<const observers::AggregateNotificationObserver> PrependGenesisObservers(
				const Address& genesisAddress,
				GenesisFundingState& genesisFundingState,
				std::unique_ptr<const observers::NotificationObserver>&& pObserver) {
			observers::DemuxObserverBuilder builder;
			builder.add(CreateGenesisFundingObserver(genesisAddress, genesisFundingState));
			builder.add(std::move(pObserver));
			return builder.build();
		}

		void LogGenesisBlockInfo(const model::BlockElement& blockElement) {
			auto networkIdentifier = blockElement.Block.Network;
			const auto& publicKey = blockElement.Block.SignerPublicKey;
			const auto& generationHash = blockElement.GenerationHash;
			BITXORCORE_LOG(important)
					<< std::endl << "      genesis network id: " << networkIdentifier
					<< std::endl << "      genesis public key: " << publicKey
					<< std::endl << " genesis generation hash: " << generationHash;
		}

		void OutputGenesisBalance(std::ostream& out, TokenId tokenId, Amount amount, char special = ' ') {
			out << std::endl << "      " << special << ' ' << utils::HexFormat(tokenId) << ": " << amount;
		}

		void LogGenesisBalances(TokenId currencyTokenId, TokenId harvestingTokenId, const state::AccountBalances& balances) {
			std::ostringstream out;
			out << "Genesis Tokens:";
			OutputGenesisBalance(out, currencyTokenId, balances.get(currencyTokenId), 'C');
			OutputGenesisBalance(out, harvestingTokenId, balances.get(harvestingTokenId), 'H');

			for (const auto& pair : balances) {
				if (currencyTokenId != pair.first && harvestingTokenId != pair.first)
					OutputGenesisBalance(out, pair.first, pair.second);
			}

			BITXORCORE_LOG(info) << out.str();
		}

		void CheckGenesisBlockInfo(const model::BlockElement& blockElement, const model::NetworkInfo& expectedNetwork) {
			auto networkIdentifier = blockElement.Block.Network;
			const auto& publicKey = blockElement.Block.SignerPublicKey;
			const auto& generationHash = blockElement.GenerationHash;
			const auto& generationHashProof = blockElement.Block.GenerationHashProof;

			if (expectedNetwork.Identifier != networkIdentifier)
				BITXORCORE_THROW_INVALID_ARGUMENT_1("genesis network id does not match network", networkIdentifier);

			if (expectedNetwork.GenesisSignerPublicKey != publicKey)
				BITXORCORE_THROW_INVALID_ARGUMENT_1("genesis public key does not match network", publicKey);

			crypto::VrfProof vrfProof{ generationHashProof.Gamma, generationHashProof.VerificationHash, generationHashProof.Scalar };
			auto proofHash = crypto::VerifyVrfProof(vrfProof, expectedNetwork.GenerationHashSeed, publicKey);
			if (Hash512() == proofHash)
				BITXORCORE_THROW_INVALID_ARGUMENT("genesis block has invalid generation hash proof");

			auto expectedGenerationHash = proofHash.copyTo<GenerationHash>();
			if (expectedGenerationHash != generationHash)
				BITXORCORE_THROW_INVALID_ARGUMENT_2("genesis block generation hash is invalid", generationHash, expectedGenerationHash);
		}

		void CheckGenesisBlockTransactionTypes(const model::Block& block, const model::TransactionRegistry& transactionRegistry) {
			for (const auto& transaction : block.Transactions()) {
				const auto* pPlugin = transactionRegistry.findPlugin(transaction.Type);
				if (!pPlugin || !pPlugin->supportsTopLevel())
					BITXORCORE_THROW_INVALID_ARGUMENT_1("genesis block contains unsupported transaction type", transaction.Type);
			}
		}

		void CheckGenesisBlockFeeMultiplier(const model::Block& block) {
			if (BlockFeeMultiplier() != block.FeeMultiplier)
				BITXORCORE_THROW_INVALID_ARGUMENT_1("genesis block contains non-zero fee multiplier", block.FeeMultiplier);
		}

		void CheckImportanceAndBalanceConsistency(Importance totalChainImportance, Amount totalChainBalance) {
			if (!utils::IsPowerMultiple<uint64_t>(totalChainImportance.unwrap(), totalChainBalance.unwrap(), 10)) {
				std::ostringstream out;
				out
						<< "harvesting outflows (" << totalChainBalance << ") do not add up to power ten multiple of "
						<< "expected importance (" << totalChainImportance << ")";
				BITXORCORE_THROW_INVALID_ARGUMENT(out.str().c_str());
			}
		}

		void CheckInitialCurrencyAtomicUnits(Amount expectedInitialCurrencyAtomicUnits, Amount initialCurrencyAtomicUnits) {
			if (expectedInitialCurrencyAtomicUnits != initialCurrencyAtomicUnits) {
				std::ostringstream out;
				out
						<< "currency outflows (" << initialCurrencyAtomicUnits << ") do not equal the "
						<< "expected initial currency atomic units (" << expectedInitialCurrencyAtomicUnits << ")";
				BITXORCORE_THROW_INVALID_ARGUMENT(out.str().c_str());
			}
		}

		void CheckMaxTokenAtomicUnits(const state::AccountBalances& totalFundedTokens, Amount maxTokenAtomicUnits) {
			for (const auto& pair : totalFundedTokens) {
				if (maxTokenAtomicUnits < pair.second) {
					std::ostringstream out;
					out
							<< "currency outflows (" << pair.second << ") for token id " << pair.first
							<< " exceed max allowed atomic units (" << maxTokenAtomicUnits << ")";
					BITXORCORE_THROW_INVALID_ARGUMENT(out.str().c_str());
				}
			}
		}
	}

	GenesisBlockLoader::GenesisBlockLoader(
			cache::BitxorCoreCacheDelta& cacheDelta,
			const plugins::PluginManager& pluginManager,
			std::unique_ptr<const observers::NotificationObserver>&& pObserver)
			: m_cacheDelta(cacheDelta)
			, m_pluginManager(pluginManager)
			, m_pNotificationObserver(PrependGenesisObservers(m_genesisAddress, m_genesisFundingState, std::move(pObserver)))
	{}

	void GenesisBlockLoader::execute(const LocalNodeStateRef& stateRef, StateHashVerification stateHashVerification) {
		// 1. load the genesis block
		auto storageView = stateRef.Storage.view();
		auto pGenesisBlockElement = storageView.loadBlockElement(Height(1));

		// 2. execute the genesis block
		execute(stateRef.Config.Blockchain, *pGenesisBlockElement, stateHashVerification, Verbosity::On);
	}

	void GenesisBlockLoader::executeAndCommit(const LocalNodeStateRef& stateRef, StateHashVerification stateHashVerification) {
		// 1. execute the genesis block
		execute(stateRef, stateHashVerification);

		// 2. commit changes
		stateRef.Cache.commit(Height(1));
	}

	void GenesisBlockLoader::execute(const model::BlockchainConfiguration& config, const model::BlockElement& genesisBlockElement) {
		execute(config, genesisBlockElement, StateHashVerification::Enabled, Verbosity::Off);
	}

	namespace {
		void CheckValidationResult(const char* description, validators::ValidationResult validationResult) {
			if (validators::ValidationResult::Success == validationResult)
				return;

			std::ostringstream out;
			out << "genesis block failed " << description << " validation with " << validationResult;
			BITXORCORE_THROW_RUNTIME_ERROR(out.str().c_str());
		}

		void RequireHashMatch(const char* hashDescription, const Hash256& blockHash, const Hash256& calculatedHash) {
			if (blockHash == calculatedHash)
				return;

			std::ostringstream out;
			out
					<< "genesis block " << hashDescription << " hash (" << blockHash << ") does not match "
					<< "calculated " << hashDescription << " hash (" << calculatedHash << ")";
			BITXORCORE_THROW_RUNTIME_ERROR(out.str().c_str());
		}
	}

	void GenesisBlockLoader::validateStateless(const model::WeakEntityInfos& entityInfos) const {
		auto pValidator = CreateStatelessEntityValidator(m_pluginManager);
		for (const auto& entityInfo : entityInfos) {
			auto validationResult = pValidator->validate(entityInfo);
			CheckValidationResult("stateless", validationResult);
		}
	}

	void GenesisBlockLoader::validateStatefulAndObserve(
			Timestamp timestamp,
			const model::WeakEntityInfos& entityInfos,
			observers::ObserverState& observerState) const {
		auto executionConfig = CreateExecutionConfiguration(m_pluginManager);
		executionConfig.pObserver = m_pNotificationObserver;
		executionConfig.pNotificationPublisher = model::CreateGenesisNotificationPublisher(
				m_pluginManager.createNotificationPublisher(),
				m_publisherOptions);
		auto batchEntityProcessor = chain::CreateBatchEntityProcessor(executionConfig);

		auto validationResult = batchEntityProcessor(Height(1), timestamp, entityInfos, observerState);
		CheckValidationResult("stateful", validationResult);
	}

	void GenesisBlockLoader::execute(
			const model::BlockchainConfiguration& config,
			const model::BlockElement& genesisBlockElement,
			StateHashVerification stateHashVerification,
			Verbosity verbosity) {
		// 1. check the genesis block
		if (Verbosity::On == verbosity)
			LogGenesisBlockInfo(genesisBlockElement);

		CheckGenesisBlockInfo(genesisBlockElement, config.Network);
		CheckGenesisBlockTransactionTypes(genesisBlockElement.Block, m_pluginManager.transactionRegistry());
		CheckGenesisBlockFeeMultiplier(genesisBlockElement.Block);

		// 2. reset genesis funding observer data and custom state
		m_genesisAddress = model::GetSignerAddress(genesisBlockElement.Block);
		m_genesisFundingState = GenesisFundingState();
		m_publisherOptions = model::ExtractGenesisNotificationPublisherOptions(config);

		// 3. stateless validation
		model::WeakEntityInfos entityInfos;
		model::ExtractEntityInfos(genesisBlockElement, entityInfos);
		validateStateless(entityInfos);

		// 4. stateful validation and observation
		auto blockStatementBuilder = model::BlockStatementBuilder();
		auto observerState = config.EnableVerifiableReceipts
				? observers::ObserverState(m_cacheDelta, blockStatementBuilder)
				: observers::ObserverState(m_cacheDelta);

		validateStatefulAndObserve(genesisBlockElement.Block.Timestamp, entityInfos, observerState);
		m_cacheDelta.dependentState().LastFinalizedHeight = Height(1);

		// 5. check the funded balances are reasonable
		if (Verbosity::On == verbosity)
			LogGenesisBalances(config.CurrencyTokenId, config.HarvestingTokenId, m_genesisFundingState.TotalFundedTokens);

		CheckImportanceAndBalanceConsistency(
				config.TotalChainImportance,
				m_genesisFundingState.TotalFundedTokens.get(config.HarvestingTokenId));

		CheckInitialCurrencyAtomicUnits(
				config.InitialCurrencyAtomicUnits,
				m_genesisFundingState.TotalFundedTokens.get(config.CurrencyTokenId));

		CheckMaxTokenAtomicUnits(m_genesisFundingState.TotalFundedTokens, config.MaxTokenAtomicUnits);

		// 6. check the hashes
		auto calculatedReceiptsHash = model::CalculateMerkleHash(*blockStatementBuilder.build());
		RequireHashMatch("receipts", genesisBlockElement.Block.ReceiptsHash, calculatedReceiptsHash);

		// important: do not remove calculateStateHash call because it has important side-effect of populating the patricia tree delta
		auto cacheStateHash = m_cacheDelta.calculateStateHash(Height(1)).StateHash;
		if (StateHashVerification::Enabled == stateHashVerification)
			RequireHashMatch("state", genesisBlockElement.Block.StateHash, cacheStateHash);
	}
}}
