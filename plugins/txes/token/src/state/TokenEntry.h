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
#include "TokenDefinition.h"

namespace bitxorcore { namespace state {

	// region TokenEntrySupplyMixin

	/// Mixin for storing and modifying a supply in TokenEntry.
	class TokenEntrySupplyMixin {
	public:
		/// Gets the token supply.
		Amount supply() const;

	public:
		/// Increases the supply by \a delta.
		void increaseSupply(Amount delta);

		/// Decreases the supply by \a delta.
		void decreaseSupply(Amount delta);

	private:
		Amount m_supply;
	};

	// endregion

	// region TokenEntry

	/// Tuple composed of a token definition and its current state.
	class PLUGIN_API_DEPENDENCY TokenEntry : public TokenEntrySupplyMixin {
	public:
		static constexpr auto Is_Deactivation_Destructive = false;

	public:
		/// Creates a token entry around token \a id and token \a definition.
		TokenEntry(TokenId id, const TokenDefinition& definition);

	public:
		/// Gets the token id.
		TokenId tokenId() const;

		/// Gets the token definition.
		const TokenDefinition& definition() const;

		/// Returns \c true if entry is active at \a height.
		bool isActive(Height height) const;

	private:
		TokenId m_id;
		TokenDefinition m_definition;
	};

	// endregion
}}
