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

#include "TokenAddressRestriction.h"

namespace bitxorcore { namespace state {

	TokenAddressRestriction::TokenAddressRestriction(TokenId tokenId, Address address)
			: m_tokenId(tokenId)
			, m_address(address)
	{}

	TokenId TokenAddressRestriction::tokenId() const {
		return m_tokenId;
	}

	Address TokenAddressRestriction::address() const {
		return m_address;
	}

	size_t TokenAddressRestriction::size() const {
		return m_keyValuePairs.size();
	}

	std::set<uint64_t> TokenAddressRestriction::keys() const {
		return m_keyValuePairs.keys();
	}

	uint64_t TokenAddressRestriction::get(uint64_t key) const {
		uint64_t value;
		return m_keyValuePairs.tryGet(key, value) ? value : Sentinel_Removal_Value;
	}

	void TokenAddressRestriction::set(uint64_t key, uint64_t value) {
		if (Sentinel_Removal_Value == value)
			m_keyValuePairs.remove(key);
		else
			m_keyValuePairs.set(key, value);
	}
}}
