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

#include "BlockGenerator.h"
#include "GenesisConfiguration.h"
#include "GenesisExecutionHasher.h"
#include "bitxorcore/builders/TokenAliasBuilder.h"
#include "bitxorcore/builders/TokenDefinitionBuilder.h"
#include "bitxorcore/builders/TokenSupplyChangeBuilder.h"
#include "bitxorcore/builders/NamespaceRegistrationBuilder.h"
#include "bitxorcore/builders/TransferBuilder.h"
#include "bitxorcore/extensions/BlockExtensions.h"
#include "bitxorcore/extensions/ConversionExtensions.h"
#include "bitxorcore/extensions/IdGenerator.h"
#include "bitxorcore/extensions/TransactionExtensions.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/BlockUtils.h"
#include "bitxorcore/model/EntityHasher.h"

namespace bitxorcore { namespace tools { namespace bxorgen {

	namespace {
		std::string FixName(const std::string& tokenName) {
			auto name = tokenName;
			for (auto& ch : name) {
				if (':' == ch)
					ch = '.';
			}

			return name;
		}

		std::string GetChildName(const std::string& namespaceName) {
			return namespaceName.substr(namespaceName.rfind('.') + 1);
		}

		class GenesisTransactions {
		public:
			GenesisTransactions(
					model::NetworkIdentifier networkIdentifier,
					const GenerationHashSeed& generationHashSeed,
					const crypto::KeyPair& signer)
					: m_networkIdentifier(networkIdentifier)
					, m_generationHashSeed(generationHashSeed)
					, m_signer(signer)
			{}

		public:
			void addNamespaceRegistration(const std::string& namespaceName, BlockDuration duration) {
				builders::NamespaceRegistrationBuilder builder(m_networkIdentifier, m_signer.publicKey());
				builder.setName({ reinterpret_cast<const uint8_t*>(namespaceName.data()), namespaceName.size() });
				builder.setDuration(duration);
				signAndAdd(builder.build());
			}

			void addNamespaceRegistration(const std::string& namespaceName, NamespaceId parentId) {
				builders::NamespaceRegistrationBuilder builder(m_networkIdentifier, m_signer.publicKey());
				builder.setName({ reinterpret_cast<const uint8_t*>(namespaceName.data()), namespaceName.size() });
				builder.setParentId(parentId);
				signAndAdd(builder.build());
			}

			TokenId addTokenDefinition(TokenNonce nonce, const model::TokenProperties& properties) {
				builders::TokenDefinitionBuilder builder(m_networkIdentifier, m_signer.publicKey());
				builder.setNonce(nonce);
				builder.setFlags(properties.flags());
				builder.setDivisibility(properties.divisibility());
				builder.setDuration(properties.duration());

				auto pTransaction = builder.build();
				auto id = pTransaction->Id;
				signAndAdd(std::move(pTransaction));
				return id;
			}

			UnresolvedTokenId addTokenAlias(const std::string& tokenName, TokenId tokenId) {
				auto namespaceName = FixName(tokenName);
				auto namespacePath = extensions::GenerateNamespacePath(namespaceName);
				auto namespaceId = namespacePath[namespacePath.size() - 1];
				builders::TokenAliasBuilder builder(m_networkIdentifier, m_signer.publicKey());
				builder.setNamespaceId(namespaceId);
				builder.setTokenId(tokenId);
				builder.setAliasAction(model::AliasAction::Link);
				auto pTransaction = builder.build();
				signAndAdd(std::move(pTransaction));

				BITXORCORE_LOG(debug)
						<< "added alias from ns " << utils::HexFormat(namespaceId) << " (" << namespaceName
						<< ") -> token " << utils::HexFormat(tokenId);
				return UnresolvedTokenId(namespaceId.unwrap());
			}

			void addTokenSupplyChange(UnresolvedTokenId tokenId, Amount delta) {
				builders::TokenSupplyChangeBuilder builder(m_networkIdentifier, m_signer.publicKey());
				builder.setTokenId(tokenId);
				builder.setAction(model::TokenSupplyChangeAction::Increase);
				builder.setDelta(delta);
				auto pTransaction = builder.build();
				signAndAdd(std::move(pTransaction));
			}

			void addTransfer(
					const std::map<std::string, UnresolvedTokenId>& tokenNameToTokenIdMap,
					const Address& recipientAddress,
					const std::vector<TokenSeed>& seeds) {
				auto recipientUnresolvedAddress = extensions::CopyToUnresolvedAddress(recipientAddress);
				builders::TransferBuilder builder(m_networkIdentifier, m_signer.publicKey());
				builder.setRecipientAddress(recipientUnresolvedAddress);
				for (const auto& seed : seeds) {
					auto tokenId = tokenNameToTokenIdMap.at(seed.Name);
					builder.addToken({ tokenId, seed.Amount });
				}

				signAndAdd(builder.build());
			}

			void addTransactions(model::Transactions&& transactions) {
				for (auto&& pTransaction : transactions)
					m_transactions.push_back(std::move(pTransaction));
			}

		public:
			const model::Transactions& transactions() const {
				return m_transactions;
			}

		private:
			void signAndAdd(std::unique_ptr<model::Transaction>&& pTransaction) {
				pTransaction->Deadline = Timestamp(1);
				extensions::TransactionExtensions(m_generationHashSeed).sign(m_signer, *pTransaction);
				m_transactions.push_back(std::move(pTransaction));
			}

		private:
			model::NetworkIdentifier m_networkIdentifier;
			const GenerationHashSeed& m_generationHashSeed;
			const crypto::KeyPair& m_signer;
			model::Transactions m_transactions;
		};

		void AddGenerationHashProof(model::Block& block, const GenerationHashSeed& generationHashSeed, const crypto::KeyPair& vrfKeyPair) {
			auto vrfProof = crypto::GenerateVrfProof(generationHashSeed, vrfKeyPair);
			block.GenerationHashProof = { vrfProof.Gamma, vrfProof.VerificationHash, vrfProof.Scalar };
		}
	}

	std::unique_ptr<model::Block> CreateGenesisBlock(const GenesisConfiguration& config, model::Transactions&& additionalTransactions) {
		auto signer = crypto::KeyPair::FromString(config.GenesisSignerPrivateKey);
		GenesisTransactions transactions(config.NetworkIdentifier, config.GenesisGenerationHashSeed, signer);

		// - namespace creation
		for (const auto& rootPair : config.RootNamespaces) {
			// - root
			const auto& root = rootPair.second;
			const auto& rootName = config.NamespaceNames.at(root.id());
			auto duration = std::numeric_limits<BlockDuration::ValueType>::max() == root.lifetime().End.unwrap()
					? Eternal_Artifact_Duration
					: BlockDuration((root.lifetime().End - root.lifetime().Start).unwrap());
			transactions.addNamespaceRegistration(rootName, duration);

			// - children
			std::map<size_t, std::vector<state::Namespace::Path>> paths;
			for (const auto& childPair : root.children()) {
				const auto& path = childPair.second.Path;
				paths[path.size()].push_back(path);
			}

			for (const auto& pair : paths) {
				for (const auto& path : pair.second) {
					const auto& child = state::Namespace(path);
					auto subName = GetChildName(config.NamespaceNames.at(child.id()));
					transactions.addNamespaceRegistration(subName, child.parentId());
				}
			}
		}

		// - token creation
		TokenNonce nonce;
		std::map<std::string, UnresolvedTokenId> nameToTokenIdMap;
		for (const auto& tokenPair : config.TokenEntries) {
			const auto& tokenEntry = tokenPair.second;

			// - definition
			auto tokenId = transactions.addTokenDefinition(nonce, tokenEntry.definition().properties());
			BITXORCORE_LOG(debug) << "mapping " << tokenPair.first << " to " << utils::HexFormat(tokenId) << " (nonce " << nonce << ")";
			nonce = nonce + TokenNonce(1);

			// - alias
			auto unresolvedTokenId = transactions.addTokenAlias(tokenPair.first, tokenId);
			nameToTokenIdMap.emplace(tokenPair.first, unresolvedTokenId);

			// - supply
			transactions.addTokenSupplyChange(unresolvedTokenId, tokenEntry.supply());
		}

		// - token distribution
		for (const auto& addressTokenSeedsPair : config.GenesisAddressToTokenSeeds) {
			auto recipient = model::StringToAddress(addressTokenSeedsPair.first);
			transactions.addTransfer(nameToTokenIdMap, recipient, addressTokenSeedsPair.second);
		}

		// - add additional transactions
		transactions.addTransactions(std::move(additionalTransactions));

		auto pBlock = model::CreateBlock(
				model::Entity_Type_Block_Genesis,
				model::PreviousBlockContext(),
				config.NetworkIdentifier,
				signer.publicKey(),
				transactions.transactions());

		// - add generation hash proof using signer as vrf key pair
		AddGenerationHashProof(*pBlock, config.GenesisGenerationHashSeed, signer);
		extensions::BlockExtensions(config.GenesisGenerationHashSeed).signFullBlock(signer, *pBlock);
		return pBlock;
	}

	Hash256 UpdateGenesisBlock(
			const GenesisConfiguration& config,
			model::Block& block,
			GenesisExecutionHashesDescriptor& executionHashesDescriptor) {
		auto& blockFooter = model::GetBlockFooter<model::ImportanceBlockFooter>(block);
		blockFooter.VotingEligibleAccountsCount = executionHashesDescriptor.VotingEligibleAccountsCount;
		blockFooter.HarvestingEligibleAccountsCount = executionHashesDescriptor.HarvestingEligibleAccountsCount;
		blockFooter.TotalVotingBalance = executionHashesDescriptor.TotalVotingBalance;

		block.ReceiptsHash = executionHashesDescriptor.ReceiptsHash;
		block.StateHash = executionHashesDescriptor.StateHash;

		auto signer = crypto::KeyPair::FromString(config.GenesisSignerPrivateKey);
		extensions::BlockExtensions(config.GenesisGenerationHashSeed).signFullBlock(signer, block);
		return model::CalculateHash(block);
	}

	model::BlockElement CreateGenesisBlockElement(
			const GenesisConfiguration& config,
			const model::TransactionRegistry& transactionRegistry,
			const model::Block& block) {
		auto proofHash = crypto::GenerateVrfProofHash(block.GenerationHashProof.Gamma);
		return extensions::BlockExtensions(config.GenesisGenerationHashSeed, transactionRegistry)
				.convertBlockToBlockElement(block, proofHash.copyTo<GenerationHash>());
	}
}}}
