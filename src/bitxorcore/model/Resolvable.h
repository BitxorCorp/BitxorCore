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

namespace bitxorcore { namespace model { class ResolverContext; } }

namespace bitxorcore { namespace model {

	/// Allows unified handling of unresolved and resolved types.
	/// \note Single parameter constructors are not explicit in order to allow this class to be a stand-in for those types.
	template<typename TUnresolved, typename TResolved>
	class Resolvable {
	private:
		enum class Type { Unresolved, Resolved };

	public:
		/// Creates a default resolvable.
		Resolvable();

		/// Creates a resolvable around \a unresolved value.
		Resolvable(const TUnresolved& unresolved);

		/// Creates a resolvable around \a resolved value.
		Resolvable(const TResolved& resolved);

	public:
		/// Returns \c true if the underlying value is resolved.
		bool isResolved() const;

		/// Gets an unresolved representation of the underlying value.
		TUnresolved unresolved() const;

		/// Gets a resolved representation of the underlying value if and only if isResolved returns \c true.
		TResolved resolved() const;

		/// Gets a resolved representation of the underlying value using \a resolvers.
		TResolved resolved(const ResolverContext& resolvers) const;

	private:
		TUnresolved m_unresolved;
		TResolved m_resolved;
		Type m_type;
	};

	/// Resolvable address.
	using ResolvableAddress = Resolvable<UnresolvedAddress, Address>;
	extern template class Resolvable<UnresolvedAddress, Address>;

	/// Resolvable token id.
	using ResolvableTokenId = Resolvable<UnresolvedTokenId, TokenId>;
	extern template class Resolvable<UnresolvedTokenId, TokenId>;
}}
