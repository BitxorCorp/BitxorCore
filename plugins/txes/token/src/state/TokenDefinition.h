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
#include "src/model/TokenProperties.h"
#include "bitxorcore/types.h"

namespace bitxorcore { namespace state {

	/// Represents a token definition.
	class TokenDefinition {
	public:
		/// Creates a token definition around \a startHeight, \a ownerAddress, token \a revision and token \a properties.
		TokenDefinition(Height startHeight, const Address& ownerAddress, uint32_t revision, const model::TokenProperties& properties)
				: m_startHeight(startHeight)
				, m_ownerAddress(ownerAddress)
				, m_revision(revision)
				, m_properties(properties)
		{}

	public:
		/// Gets the start height.
		Height startHeight() const;

		/// Gets the owner's address.
		const Address& ownerAddress() const;

		/// Gets the revision.
		uint32_t revision() const;

		/// Gets the token properties.
		const model::TokenProperties& properties() const;

		/// Returns \c true if the token definition has eternal duration.
		bool isEternal() const;

		/// Returns \c true if the token definition is active at \a height.
		bool isActive(Height height) const;

		/// Returns \c true if the token definition is expired at \a height.
		bool isExpired(Height height) const;

	private:
		Height m_startHeight;
		Address m_ownerAddress;
		uint32_t m_revision;
		model::TokenProperties m_properties;
	};
}}
