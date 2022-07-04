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
#include "RestrictionValueMap.h"
#include "bitxorcore/types.h"
#include <limits>

namespace bitxorcore { namespace state {

	/// Token restrictions scoped to an address.
	class TokenAddressRestriction {
	public:
		/// Sentinel value that triggers restriction removal.
		static constexpr uint64_t Sentinel_Removal_Value = std::numeric_limits<uint64_t>::max();

	public:
		/// Creates a restriction around \a tokenId and \a address.
		TokenAddressRestriction(TokenId tokenId, Address address);

	public:
		/// Gets the token id.
		TokenId tokenId() const;

		/// Gets the address.
		Address address() const;

		/// Gets the number of restriction rules.
		size_t size() const;

		/// Gets all restriction keys.
		std::set<uint64_t> keys() const;

	public:
		/// Gets the value associated with \a key.
		uint64_t get(uint64_t key) const;

		/// Sets the \a value associated with \a key.
		void set(uint64_t key, uint64_t value);

	public:
		TokenId m_tokenId;
		Address m_address;
		RestrictionValueMap<uint64_t> m_keyValuePairs;
	};
}}
