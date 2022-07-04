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

#include "TokenEntry.h"
#include "bitxorcore/utils/IntegerMath.h"

namespace bitxorcore { namespace state {

	// region TokenEntrySupplyMixin

	Amount TokenEntrySupplyMixin::supply() const {
		return m_supply;
	}

	void TokenEntrySupplyMixin::increaseSupply(Amount delta) {
		if (!utils::CheckedAdd(m_supply, delta))
			BITXORCORE_THROW_INVALID_ARGUMENT_2("cannot increase token supply above max (supply, delta)", m_supply, delta);
	}

	void TokenEntrySupplyMixin::decreaseSupply(Amount delta) {
		if (delta > m_supply)
			BITXORCORE_THROW_INVALID_ARGUMENT_2("cannot decrease token supply below zero (supply, delta)", m_supply, delta);

		m_supply = m_supply - delta;
	}

	// endregion

	// region TokenEntry

	TokenEntry::TokenEntry(TokenId id, const TokenDefinition& definition)
			: m_id(id)
			, m_definition(definition)
	{}

	TokenId TokenEntry::tokenId() const {
		return m_id;
	}

	const TokenDefinition& TokenEntry::definition() const {
		return m_definition;
	}

	bool TokenEntry::isActive(Height height) const {
		return m_definition.isActive(height);
	}

	// endregion
}}
