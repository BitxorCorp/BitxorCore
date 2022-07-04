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
#include "bitxorcore/types.h"
#include <string>

namespace bitxorcore { namespace model {

	// region NetworkIdentifier

/// \note The lower 3 bits must be cleared because they are used for different purposes, e.g. resolvers.
#define NETWORK_IDENTIFIER_LIST \
	/* Default (zero) identifier that does not identify any known network. */ \
	ENUM_VALUE(Zero, 0) \
	\
	/* Main network identifier. */ \
	ENUM_VALUE(Mainnet, 0x0de2) \
	\
	/* Test network identifier. */ \
	ENUM_VALUE(Testnet, 0x9924)


#define ENUM_VALUE(LABEL, VALUE) LABEL = VALUE,
	/// Possible network identifiers.
	enum class NetworkIdentifier : uint16_t {
		NETWORK_IDENTIFIER_LIST
	};
#undef ENUM_VALUE

	/// Insertion operator for outputting \a value to \a out.
	std::ostream& operator<<(std::ostream& out, NetworkIdentifier value);

	/// Tries to parse \a networkName into a network identifier (\a networkIdentifier).
	bool TryParseValue(const std::string& networkName, NetworkIdentifier& networkIdentifier);

	// endregion

	// region UniqueNetworkFingerprint

	/// Globally unique network fingerprint.
	struct UniqueNetworkFingerprint {
	public:
		/// Creates default fingerprint.
		UniqueNetworkFingerprint();

		/// Creates fingerprint around \a identifier.
		explicit UniqueNetworkFingerprint(NetworkIdentifier identifier);

		/// Creates fingerprint around \a identifier and \a generationHashSeed.
		UniqueNetworkFingerprint(NetworkIdentifier identifier, const bitxorcore::GenerationHashSeed& generationHashSeed);

	public:
		/// Network identifier.
		NetworkIdentifier Identifier;

		/// Genesis generation hash seed.
		bitxorcore::GenerationHashSeed GenerationHashSeed;

	public:
		/// Returns \c true if this fingerprint is equal to \a rhs.
		bool operator==(const UniqueNetworkFingerprint& rhs) const;

		/// Returns \c true if this fingerprint is not equal to \a rhs.
		bool operator!=(const UniqueNetworkFingerprint& rhs) const;
	};

	/// Insertion operator for outputting \a fingerprint to \a out.
	std::ostream& operator<<(std::ostream& out, const UniqueNetworkFingerprint& fingerprint);

	// endregion
}}
