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

#include "NetworkIdentifier.h"
#include "bitxorcore/utils/ConfigurationValueParsers.h"
#include "bitxorcore/utils/MacroBasedEnumIncludes.h"

namespace bitxorcore { namespace model {

	// region NetworkIdentifier

#define DEFINE_ENUM NetworkIdentifier
#define EXPLICIT_VALUE_ENUM
#define ENUM_LIST NETWORK_IDENTIFIER_LIST
#include "bitxorcore/utils/MacroBasedEnum.h"
#undef ENUM_LIST
#undef EXPLICIT_VALUE_ENUM
#undef DEFINE_ENUM

	namespace {
		const std::array<std::pair<const char*, NetworkIdentifier>, 2> String_To_Network_Identifier_Pairs{{
			{ "mainnet", NetworkIdentifier::Mainnet },
			{ "testnet", NetworkIdentifier::Testnet }
		}};
	}

	bool TryParseValue(const std::string& networkName, NetworkIdentifier& networkIdentifier) {
		if (utils::TryParseEnumValue(String_To_Network_Identifier_Pairs, networkName, networkIdentifier))
			return true;

		uint16_t rawNetworkIdentifier;
		if (!utils::TryParseValue(networkName, rawNetworkIdentifier))
			return false;

		networkIdentifier = static_cast<NetworkIdentifier>(rawNetworkIdentifier);
		return true;
	}

	// endregion

	// region UniqueNetworkFingerprint

	UniqueNetworkFingerprint::UniqueNetworkFingerprint() : UniqueNetworkFingerprint(NetworkIdentifier::Zero)
	{}

	UniqueNetworkFingerprint::UniqueNetworkFingerprint(NetworkIdentifier identifier)
			: UniqueNetworkFingerprint(identifier, bitxorcore::GenerationHashSeed())
	{}

	UniqueNetworkFingerprint::UniqueNetworkFingerprint(
			NetworkIdentifier identifier,
			const bitxorcore::GenerationHashSeed& generationHashSeed)
			: Identifier(identifier)
			, GenerationHashSeed(generationHashSeed)
	{}

	bool UniqueNetworkFingerprint::operator==(const UniqueNetworkFingerprint& rhs) const {
		return Identifier == rhs.Identifier && GenerationHashSeed == rhs.GenerationHashSeed;
	}

	bool UniqueNetworkFingerprint::operator!=(const UniqueNetworkFingerprint& rhs) const {
		return !(*this == rhs);
	}

	std::ostream& operator<<(std::ostream& out, const UniqueNetworkFingerprint& fingerprint) {
		out << fingerprint.Identifier << "::" << fingerprint.GenerationHashSeed;
		return out;
	}

	// endregion
}}
