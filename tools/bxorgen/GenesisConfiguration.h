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

#pragma once
#include "bitxorcore/model/NetworkIdentifier.h"
#include "bitxorcore/state/TokenEntry.h"
#include "bitxorcore/state/RootNamespace.h"
#include "bitxorcore/utils/Hashers.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace bitxorcore { namespace utils { class ConfigurationBag; } }

namespace bitxorcore { namespace tools { namespace bxorgen {

	/// Token seed.
	struct TokenSeed {
		/// Token name.
		std::string Name;

		/// Token amount.
		bitxorcore::Amount Amount;
	};

	/// Mapping of namespace ids to human readable names.
	using NamespaceIdToNameMap = std::unordered_map<NamespaceId, std::string, utils::BaseValueHasher<NamespaceId>>;

	/// Mapping of namespace ids to root namespaces.
	using NamespaceIdToRootNamespaceMap = std::unordered_map<NamespaceId, state::RootNamespace, utils::BaseValueHasher<NamespaceId>>;

	/// Mapping of token names to token entries.
	using TokenNameToTokenEntryMap = std::vector<std::pair<std::string, state::TokenEntry>>;

	/// Mapping of addresses to token seeds.
	using AddressToTokenSeedsMap = std::vector<std::pair<std::string, std::vector<TokenSeed>>>;

	/// Genesis configuration.
	struct GenesisConfiguration {
	public:
		/// Blockchain network identifier.
		model::NetworkIdentifier NetworkIdentifier;

		/// Genesis generation hash seed.
		GenerationHashSeed GenesisGenerationHashSeed;

		/// Genesis signer private key.
		std::string GenesisSignerPrivateKey;

		/// Cpp file header.
		std::string CppFileHeader;

		/// Cpp file path.
		std::string CppFile;

		/// Binary destination directory.
		std::string BinDirectory;

		/// Map containing all namespace names.
		NamespaceIdToNameMap NamespaceNames;

		/// Map containing all root namespaces.
		NamespaceIdToRootNamespaceMap RootNamespaces;

		/// Map containing all token entries.
		TokenNameToTokenEntryMap TokenEntries;

		/// Map of genesis account addresses to token seeds.
		AddressToTokenSeedsMap GenesisAddressToTokenSeeds;

		/// Additional transactions directory.
		std::string TransactionsDirectory;

	public:
		/// Loads a genesis configuration from \a bag.
		static GenesisConfiguration LoadFromBag(const utils::ConfigurationBag& bag);
	};

	/// Gets the genesis signer address from \a config.
	Address GetGenesisSignerAddress(const GenesisConfiguration& config);
}}}
