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

#include "AccountStateCacheView.h"
#include "bitxorcore/model/NetworkIdentifier.h"

namespace bitxorcore { namespace cache {

	BasicAccountStateCacheView::BasicAccountStateCacheView(
			const AccountStateCacheTypes::BaseSets& accountStateSets,
			const AccountStateCacheTypes::Options& options,
			const HighValueAccounts& highValueAccounts)
			: BasicAccountStateCacheView(
					accountStateSets,
					options,
					highValueAccounts,
					std::make_unique<AccountStateCacheViewMixins::KeyLookupAdapter>(
							accountStateSets.KeyLookupMap,
							accountStateSets.Primary))
	{}

	BasicAccountStateCacheView::BasicAccountStateCacheView(
			const AccountStateCacheTypes::BaseSets& accountStateSets,
			const AccountStateCacheTypes::Options& options,
			const HighValueAccounts& highValueAccounts,
			std::unique_ptr<AccountStateCacheViewMixins::KeyLookupAdapter>&& pKeyLookupAdapter)
			: AccountStateCacheViewMixins::Size(accountStateSets.Primary)
			, AccountStateCacheViewMixins::ContainsAddress(accountStateSets.Primary)
			, AccountStateCacheViewMixins::ContainsKey(accountStateSets.KeyLookupMap)
			, AccountStateCacheViewMixins::Iteration(accountStateSets.Primary)
			, AccountStateCacheViewMixins::ConstAccessorAddress(accountStateSets.Primary)
			, AccountStateCacheViewMixins::ConstAccessorKey(*pKeyLookupAdapter)
			, AccountStateCacheViewMixins::PatriciaTreeView(accountStateSets.PatriciaTree.get())
			, m_options(options)
			, m_highValueAccounts(highValueAccounts)
			, m_pKeyLookupAdapter(std::move(pKeyLookupAdapter))
	{}

	model::NetworkIdentifier BasicAccountStateCacheView::networkIdentifier() const {
		return m_options.NetworkIdentifier;
	}

	uint64_t BasicAccountStateCacheView::importanceGrouping() const {
		return m_options.ImportanceGrouping;
	}

	Amount BasicAccountStateCacheView::minHarvesterBalance() const {
		return m_options.MinHarvesterBalance;
	}

	Amount BasicAccountStateCacheView::maxHarvesterBalance() const {
		return m_options.MaxHarvesterBalance;
	}

	TokenId BasicAccountStateCacheView::harvestingTokenId() const {
		return m_options.HarvestingTokenId;
	}

	const HighValueAccounts& BasicAccountStateCacheView::highValueAccounts() const {
		return m_highValueAccounts;
	}
}}
