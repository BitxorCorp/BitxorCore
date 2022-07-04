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

#include "FinalizationBootstrapperServiceTestUtils.h"
#include "finalization/src/FinalizationConfiguration.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "finalization/tests/test/mocks/MockProofStorage.h"
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/test/nodeps/TestConstants.h"

namespace bitxorcore { namespace test {

	cache::BitxorCoreCache FinalizationBootstrapperServiceTestUtils::CreateCache() {
		return CoreSystemCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
	}

	cache::BitxorCoreCache FinalizationBootstrapperServiceTestUtils::CreateCache(std::vector<AccountKeyPairDescriptor>& keyPairDescriptors) {
		auto config = model::BlockchainConfiguration::Uninitialized();
		config.HarvestingTokenId = Default_Harvesting_Token_Id;
		config.MinVoterBalance = Amount(2'000'000);

		auto cache = CoreSystemCacheFactory::Create(config);
		auto cacheDelta = cache.createDelta();
		auto& accountStateCacheDelta = cacheDelta.sub<cache::AccountStateCache>();
		keyPairDescriptors = AddAccountsWithBalances(accountStateCacheDelta, Height(1), Default_Harvesting_Token_Id, {
			Amount(2'000'000), Amount(4'000'000'000'000), Amount(1'000'000), Amount(6'000'000'000'000)
		});
		cache.commit(Height());
		return cache;
	}

	void FinalizationBootstrapperServiceTestUtils::Register(extensions::ServiceLocator& locator, extensions::ServiceState& state) {
		Register(locator, state, std::make_unique<mocks::MockProofStorage>());
	}

	void FinalizationBootstrapperServiceTestUtils::Register(
			extensions::ServiceLocator& locator,
			extensions::ServiceState& state,
			std::unique_ptr<io::ProofStorage>&& pProofStorage) {
		auto config = finalization::FinalizationConfiguration::Uninitialized();
		config.Size = 3000;
		config.Threshold = 2000;
		config.MaxHashesPerPoint = 64;
		config.VotingSetGrouping = 500;

		auto pBootstrapperRegistrar = CreateFinalizationBootstrapperServiceRegistrar(config, std::move(pProofStorage));
		pBootstrapperRegistrar->registerServices(locator, state);
	}
}}
