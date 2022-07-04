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

#include "NamespaceAlias.h"
#include "bitxorcore/exceptions.h"

namespace bitxorcore { namespace state {

	NamespaceAlias::NamespaceAlias() : m_type(AliasType::None)
	{}

	NamespaceAlias::NamespaceAlias(TokenId tokenId)
			: m_type(AliasType::Token)
			, m_tokenId(tokenId)
	{}

	NamespaceAlias::NamespaceAlias(const Address& address)
			: m_type(AliasType::Address)
			, m_address(address)
	{}

	NamespaceAlias::NamespaceAlias(const NamespaceAlias& alias)
			: m_type(alias.m_type)
			, m_address(alias.m_address)
	{}

	NamespaceAlias& NamespaceAlias::operator=(const NamespaceAlias& alias) {
		m_type = alias.m_type;
		m_address = alias.m_address;
		return *this;
	}

	AliasType NamespaceAlias::type() const {
		return m_type;
	}

	TokenId NamespaceAlias::tokenId() const {
		if (AliasType::Token != m_type)
			BITXORCORE_THROW_RUNTIME_ERROR("NamespaceAlias::tokenId called with invalid type");

		return m_tokenId;
	}

	const Address& NamespaceAlias::address() const {
		if (AliasType::Address != m_type)
			BITXORCORE_THROW_RUNTIME_ERROR("NamespaceAlias::address called with invalid type");

		return m_address;
	}
}}
