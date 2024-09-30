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

#include "GenesisConfiguration.h"
#include "bitxorcore/crypto/KeyPair.h"
#include "bitxorcore/extensions/IdGenerator.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/TokenIdGenerator.h"
#include "bitxorcore/model/NamespaceIdGenerator.h"
#include "bitxorcore/state/Namespace.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include "bitxorcore/utils/ConfigurationUtils.h"

namespace bitxorcore { namespace tools { namespace bxorgen {

	namespace {
		constexpr auto Namespace_Section_Prefix = "namespace>";
		constexpr auto Token_Section_Prefix = "token>";
		constexpr auto Distribution_Section_Prefix = "distribution>";
		constexpr size_t Num_Namespace_Properties = 1; // duration
		constexpr size_t Num_Token_Properties = 6; // divisibility, duration, supply, 3 flags

		template<typename TContainer>
		auto FindByKey(TContainer& pairContainer, const typename TContainer::value_type::first_type& key) {
			return std::find_if(pairContainer.begin(), pairContainer.end(), [&key](const auto& pair) {
				return key == pair.first;
			});
		}

		void Merge(
				AddressToTokenSeedsMap& aggregateMap,
				const std::string& tokenName,
				const std::vector<std::pair<std::string, uint64_t>>& addressToAmountMap) {
			for (const auto& addressAmountPair : addressToAmountMap) {
				const auto& address = addressAmountPair.first;
				auto iter = FindByKey(aggregateMap, address);
				if (aggregateMap.end() == iter) {
					aggregateMap.emplace_back(address, std::vector<TokenSeed>());
					iter = aggregateMap.end() - 1;
				}

				iter->second.push_back({ tokenName, Amount(addressAmountPair.second) });
			}
		}

		auto IsRoot(const std::string& namespaceName) {
			return std::string::npos == namespaceName.find('.');
		}

		auto CreateRoot(const utils::ConfigurationBag& bag, const Address& owner, const std::string& namespaceName) {
			const std::string section = Namespace_Section_Prefix + namespaceName;
			auto duration = bag.get<uint64_t>(utils::ConfigurationKey(section.c_str(), "duration"));
			auto id = model::GenerateRootNamespaceId(namespaceName);
			auto endHeight = 0 == duration ? Height(std::numeric_limits<BlockDuration::ValueType>::max()) : Height(duration + 1);
			return state::RootNamespace(id, owner, state::NamespaceLifetime(Height(1), endHeight));
		}

		auto ToTokenEntry(const state::TokenDefinition& definition, TokenNonce tokenNonce, Amount supply) {
			auto entry = state::TokenEntry(model::GenerateTokenId(definition.ownerAddress(), tokenNonce), definition);
			entry.increaseSupply(supply);
			return entry;
		}

		auto CreateTokenEntry(
				const utils::ConfigurationBag& bag,
				const Address& owner,
				const std::string& tokenName,
				TokenNonce tokenNonce) {
			const std::string section = Token_Section_Prefix + tokenName;
			auto makeKey = [&section](const auto* name) {
				return utils::ConfigurationKey(section.c_str(), name);
			};

			auto supply = Amount(bag.get<uint64_t>(makeKey("supply")));
			auto divisibility = bag.get<uint8_t>(makeKey("divisibility"));
			auto duration = BlockDuration(bag.get<uint64_t>(makeKey("duration")));

			auto flags = model::TokenFlags::None;
			if (bag.get<bool>(makeKey("isTransferable")))
				flags |= model::TokenFlags::Transferable;

			if (bag.get<bool>(makeKey("isSupplyMutable")))
				flags |= model::TokenFlags::Supply_Mutable;

			if (bag.get<bool>(makeKey("isRestrictable")))
				flags |= model::TokenFlags::Restrictable;

			state::TokenDefinition definition(Height(1), owner, 1, model::TokenProperties(flags, divisibility, duration));
			return ToTokenEntry(definition, tokenNonce, supply);
		}

		size_t LoadNamespaces(const utils::ConfigurationBag& bag, GenesisConfiguration& config, const Address& owner) {
			auto namespaces = bag.getAllOrdered<bool>("namespaces");
			auto numNamespaceProperties = namespaces.size();
			for (const auto& optionalNs : namespaces) {
				const auto& namespaceName = optionalNs.first;
				if (IsRoot(namespaceName)) {
					auto root = CreateRoot(bag, owner, namespaceName);
					numNamespaceProperties += Num_Namespace_Properties;
					if (!optionalNs.second)
						continue;

					auto result = config.RootNamespaces.emplace(root.id(), root);
					if (!result.second)
						BITXORCORE_THROW_INVALID_ARGUMENT_1("duplicate root namespace", namespaceName);

					config.NamespaceNames.emplace(root.id(), namespaceName);
					continue;
				}

				if (!optionalNs.second)
					continue;

				auto child = state::Namespace(extensions::GenerateNamespacePath(namespaceName));
				auto rootIter = config.RootNamespaces.find(child.rootId());
				if (config.RootNamespaces.cend() == rootIter)
					BITXORCORE_THROW_INVALID_ARGUMENT_1("root namespace not found", namespaceName);

				// note that add will throw if the child is already known
				rootIter->second.add(child);
				config.NamespaceNames.emplace(child.id(), namespaceName);
			}

			return numNamespaceProperties;
		}

		size_t LoadTokens(const utils::ConfigurationBag& bag, GenesisConfiguration& config, const Address& owner) {
			auto tokens = bag.getAllOrdered<bool>("tokens");
			auto numTokenProperties = tokens.size();

			uint32_t tokenNonce = 0;
			for (const auto& optionalToken : tokens) {
				const auto& tokenName = optionalToken.first;

				// - token entry
				auto tokenEntry = CreateTokenEntry(bag, owner, tokenName, TokenNonce(tokenNonce));
				numTokenProperties += Num_Token_Properties;
				++tokenNonce;

				// - initial distribution
				const std::string section = Distribution_Section_Prefix + tokenName;
				auto addressToAmountMap = bag.getAllOrdered<uint64_t>(section.c_str());
				numTokenProperties += addressToAmountMap.size();
				if (!optionalToken.second)
					continue;

				// - add information
				if (config.TokenEntries.cend() != FindByKey(config.TokenEntries, tokenName))
					BITXORCORE_THROW_RUNTIME_ERROR_1("multiple entries for", tokenName);

				config.TokenEntries.emplace_back(tokenName, tokenEntry);
				Merge(config.GenesisAddressToTokenSeeds, tokenName, addressToAmountMap);
			}

			return numTokenProperties;
		}
	}

#define LOAD_PROPERTY(SECTION, NAME) utils::LoadIniProperty(bag, SECTION, #NAME, config.NAME)

	GenesisConfiguration GenesisConfiguration::LoadFromBag(const utils::ConfigurationBag& bag) {
		GenesisConfiguration config;

#define LOAD_GENESIS_PROPERTY(NAME) LOAD_PROPERTY("genesis", NAME)

		LOAD_GENESIS_PROPERTY(NetworkIdentifier);
		LOAD_GENESIS_PROPERTY(GenesisGenerationHashSeed);
		LOAD_GENESIS_PROPERTY(GenesisSignerPrivateKey);

#undef LOAD_GENESIS_PROPERTY

#define LOAD_CPP_PROPERTY(NAME) LOAD_PROPERTY("cpp", NAME)

		LOAD_CPP_PROPERTY(CppFileHeader);

#undef LOAD_CPP_PROPERTY

#define LOAD_OUTPUT_PROPERTY(NAME) LOAD_PROPERTY("output", NAME)

		LOAD_OUTPUT_PROPERTY(CppFile);
		LOAD_OUTPUT_PROPERTY(BinDirectory);

#undef LOAD_OUTPUT_PROPERTY

		// the genesis account owns all namespaces and token definitions in the configuration
		auto owner = GetGenesisSignerAddress(config);

		// load namespace information
		auto numNamespaceProperties = LoadNamespaces(bag, config, owner);

		// load tokens information
		auto numTokenProperties = LoadTokens(bag, config, owner);

		LOAD_PROPERTY("transactions", TransactionsDirectory);

		utils::VerifyBagSizeExact(bag, 7 + numNamespaceProperties + numTokenProperties);
		return config;
	}

	Address GetGenesisSignerAddress(const GenesisConfiguration& config) {
		auto publicKey = crypto::KeyPair::FromString(config.GenesisSignerPrivateKey).publicKey();
		return model::PublicKeyToAddress(publicKey, config.NetworkIdentifier);
	}
}}}
