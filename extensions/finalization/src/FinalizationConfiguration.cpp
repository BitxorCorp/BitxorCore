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

#include "FinalizationConfiguration.h"
#include "bitxorcore/config/ConfigurationFileLoader.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include "bitxorcore/utils/ConfigurationUtils.h"

namespace bitxorcore { namespace finalization {

#define LOAD_PROPERTY(NAME) utils::LoadIniProperty(bag, "finalization", #NAME, config.NAME)

	namespace {
		size_t ParseAddressesSection(const utils::ConfigurationBag& bag, model::AddressSet& addresses) {
			auto sectionName = "treasury_reissuance_epoch_ineligible_voter_addresses";
			auto addressesPair = utils::ExtractSectionKeysAsTypedVector<Address>(bag, sectionName, [](const auto& str, auto& address) {
				return model::TryParseValue(str, address);
			});
			addresses.insert(addressesPair.first.cbegin(), addressesPair.first.cend());
			return addressesPair.second;
		}
	}

	FinalizationConfiguration FinalizationConfiguration::Uninitialized() {
		return FinalizationConfiguration();
	}

	FinalizationConfiguration FinalizationConfiguration::LoadFromBag(const utils::ConfigurationBag& bag) {
		FinalizationConfiguration config;
		config.VotingSetGrouping = 0;

		LOAD_PROPERTY(EnableVoting);
		LOAD_PROPERTY(EnableRevoteOnBoot);

		LOAD_PROPERTY(Size);
		LOAD_PROPERTY(Threshold);
		LOAD_PROPERTY(StepDuration);

		LOAD_PROPERTY(ShortLivedCacheMessageDuration);
		LOAD_PROPERTY(MessageSynchronizationMaxResponseSize);

		LOAD_PROPERTY(MaxHashesPerPoint);
		LOAD_PROPERTY(PrevoteBlocksMultiple);

		LOAD_PROPERTY(UnfinalizedBlocksDuration);

		LOAD_PROPERTY(TreasuryReissuanceEpoch);

		auto numAddresses = ParseAddressesSection(bag, config.TreasuryReissuanceEpochIneligibleVoterAddresses);

		utils::VerifyBagSizeExact(bag, 11 + numAddresses);
		return config;
	}

#undef LOAD_PROPERTY

	FinalizationConfiguration FinalizationConfiguration::LoadFromPath(const std::filesystem::path& resourcesPath) {
		return config::LoadIniConfiguration<FinalizationConfiguration>(resourcesPath / "config-finalization.properties");
	}
}}
