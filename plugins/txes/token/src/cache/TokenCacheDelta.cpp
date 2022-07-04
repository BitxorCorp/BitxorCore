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

#include "TokenCacheDelta.h"
#include "bitxorcore/cache/IdentifierGroupCacheUtils.h"
#include "bitxorcore/utils/Casting.h"
#include <unordered_set>

namespace bitxorcore { namespace cache {

	namespace {
		using TokenByIdMap = TokenCacheTypes::PrimaryTypes::BaseSetDeltaType;
		using HeightBasedTokenIdsMap = TokenCacheTypes::HeightGroupingTypes::BaseSetDeltaType;

		Height GetExpiryHeight(const state::TokenDefinition& definition) {
			return Height(definition.startHeight().unwrap() + definition.properties().duration().unwrap());
		}

		void UpdateExpiryMap(HeightBasedTokenIdsMap& tokenIdsByExpiryHeight, const state::TokenEntry& entry) {
			// in case the token is not eternal, update the expiry height based token ids map
			const auto& definition = entry.definition();
			if (definition.isEternal())
				return;

			AddIdentifierWithGroup(tokenIdsByExpiryHeight, GetExpiryHeight(definition), entry.tokenId());
		}
	}

	BasicTokenCacheDelta::BasicTokenCacheDelta(const TokenCacheTypes::BaseSetDeltaPointers& tokenSets)
			: TokenCacheDeltaMixins::Size(*tokenSets.pPrimary)
			, TokenCacheDeltaMixins::Contains(*tokenSets.pPrimary)
			, TokenCacheDeltaMixins::ConstAccessor(*tokenSets.pPrimary)
			, TokenCacheDeltaMixins::MutableAccessor(*tokenSets.pPrimary)
			, TokenCacheDeltaMixins::PatriciaTreeDelta(*tokenSets.pPrimary, tokenSets.pPatriciaTree)
			, TokenCacheDeltaMixins::ActivePredicate(*tokenSets.pPrimary)
			, TokenCacheDeltaMixins::BasicInsertRemove(*tokenSets.pPrimary)
			, TokenCacheDeltaMixins::Touch(*tokenSets.pPrimary, *tokenSets.pHeightGrouping)
			, TokenCacheDeltaMixins::DeltaElements(*tokenSets.pPrimary)
			, m_pEntryById(tokenSets.pPrimary)
			, m_pTokenIdsByExpiryHeight(tokenSets.pHeightGrouping)
	{}

	void BasicTokenCacheDelta::insert(const state::TokenEntry& entry) {
		TokenCacheDeltaMixins::BasicInsertRemove::insert(entry);
		UpdateExpiryMap(*m_pTokenIdsByExpiryHeight, entry);
	}

	void BasicTokenCacheDelta::remove(TokenId tokenId) {
		auto iter = m_pEntryById->find(tokenId);
		const auto* pEntry = iter.get();
		if (!!pEntry) {
			const auto& definition = pEntry->definition();
			if (!definition.isEternal())
				RemoveIdentifierWithGroup(*m_pTokenIdsByExpiryHeight, GetExpiryHeight(definition), tokenId);
		}

		TokenCacheDeltaMixins::BasicInsertRemove::remove(tokenId);
	}
}}
