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

#include "Harvester.h"
#include "bitxorcore/cache_core/ImportanceView.h"
#include "bitxorcore/chain/BlockDifficultyScorer.h"
#include "bitxorcore/chain/BlockScorer.h"
#include "bitxorcore/model/BlockUtils.h"
#include "bitxorcore/utils/StackLogger.h"

namespace bitxorcore { namespace harvesting {

	namespace {
		struct NextBlockContext {
		public:
			NextBlockContext(const model::BlockElement& parentBlockElement, Timestamp nextTimestamp)
					: ParentBlock(parentBlockElement.Block)
					, ParentContext(parentBlockElement)
					, Timestamp(nextTimestamp)
					, Height(ParentBlock.Height + bitxorcore::Height(1))
					, BlockTime(utils::TimeSpan::FromDifference(Timestamp, ParentBlock.Timestamp))
			{}

		public:
			const model::Block& ParentBlock;
			model::PreviousBlockContext ParentContext;
			bitxorcore::Timestamp Timestamp;
			bitxorcore::Height Height;
			utils::TimeSpan BlockTime;
			bitxorcore::Difficulty Difficulty;

		public:
			bool tryCalculateDifficulty(const cache::BlockStatisticCache& cache, const model::BlockchainConfiguration& config) {
				return chain::TryCalculateDifficulty(cache, ParentBlock.Height, config, Difficulty);
			}
		};

		std::unique_ptr<model::Block> CreateUnsignedBlockHeader(
				const NextBlockContext& context,
				model::EntityType blockType,
				model::NetworkIdentifier networkIdentifier,
				const Key& signer,
				const Address& beneficiary) {
			auto pBlock = model::CreateBlock(blockType, context.ParentContext, networkIdentifier, signer, {});
			pBlock->Difficulty = context.Difficulty;
			pBlock->Timestamp = context.Timestamp;
			if (Address() != beneficiary)
				pBlock->BeneficiaryAddress = beneficiary;

			return pBlock;
		}

		void AddGenerationHashProof(model::Block& block, const crypto::VrfProof& vrfProof) {
			block.GenerationHashProof = { vrfProof.Gamma, vrfProof.VerificationHash, vrfProof.Scalar };
		}
	}

	Harvester::Harvester(
			const cache::BitxorCoreCache& cache,
			const model::BlockchainConfiguration& config,
			const Address& beneficiary,
			const UnlockedAccounts& unlockedAccounts,
			const BlockGenerator& blockGenerator)
			: m_cache(cache)
			, m_config(config)
			, m_beneficiary(beneficiary)
			, m_unlockedAccounts(unlockedAccounts)
			, m_blockGenerator(blockGenerator)
	{}

	std::unique_ptr<model::Block> Harvester::harvest(const model::BlockElement& lastBlockElement, Timestamp timestamp) {
		NextBlockContext context(lastBlockElement, timestamp);
		if (!context.tryCalculateDifficulty(m_cache.sub<cache::BlockStatisticCache>(), m_config)) {
			BITXORCORE_LOG(debug) << "skipping harvest attempt due to error calculating difficulty";
			return nullptr;
		}

		chain::BlockHitContext hitContext;
		hitContext.ElapsedTime = context.BlockTime;
		hitContext.Difficulty = context.Difficulty;
		hitContext.Height = context.Height;

		const auto& accountStateCache = m_cache.sub<cache::AccountStateCache>();
		chain::BlockHitPredicate hitPredicate(m_config, [&accountStateCache](const auto& key, auto height) {
			auto lockedCacheView = accountStateCache.createView();
			cache::ReadOnlyAccountStateCache readOnlyCache(*lockedCacheView);
			cache::ImportanceView view(readOnlyCache);
			return view.getAccountImportanceOrDefault(key, height);
		});

		auto unlockedAccountsView = m_unlockedAccounts.view();
		const crypto::KeyPair* pHarvesterKeyPair = nullptr;
		crypto::VrfProof vrfProof;

		unlockedAccountsView.forEach([&context, &hitContext, &hitPredicate, &pHarvesterKeyPair, &vrfProof](const auto& descriptor) {
			hitContext.Signer = descriptor.signingKeyPair().publicKey();

			vrfProof = crypto::GenerateVrfProof(context.ParentContext.GenerationHash, descriptor.vrfKeyPair());
			hitContext.GenerationHash = model::CalculateGenerationHash(vrfProof.Gamma);
			if (hitPredicate(hitContext)) {
				pHarvesterKeyPair = &descriptor.signingKeyPair();
				return false;
			}

			return true;
		});

		if (!pHarvesterKeyPair)
			return nullptr;

		utils::StackLogger stackLogger("generating candidate block", utils::LogLevel::debug);
		auto pBlockHeader = CreateUnsignedBlockHeader(
				context,
				model::CalculateBlockTypeFromHeight(context.Height, m_config.ImportanceGrouping),
				m_config.Network.Identifier,
				pHarvesterKeyPair->publicKey(),
				m_beneficiary);

		AddGenerationHashProof(*pBlockHeader, vrfProof);
		auto pBlock = m_blockGenerator(*pBlockHeader, m_config.MaxTransactionsPerBlock);
		if (pBlock)
			SignBlockHeader(*pHarvesterKeyPair, *pBlock);

		return pBlock;
	}
}}
