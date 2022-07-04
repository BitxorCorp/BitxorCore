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

#include "Resolvable.h"
#include "ResolverContext.h"
#include "bitxorcore/exceptions.h"

namespace bitxorcore { namespace model {

	namespace {
		UnresolvedAddress Unresolve(const Address& address) {
			return address.copyTo<UnresolvedAddress>();
		}

		UnresolvedTokenId Unresolve(TokenId tokenId) {
			return UnresolvedTokenId(tokenId.unwrap());
		}
	}

	template<typename TUnresolved, typename TResolved>
	Resolvable<TUnresolved, TResolved>::Resolvable() : m_type(Type::Resolved)
	{}

	template<typename TUnresolved, typename TResolved>
	Resolvable<TUnresolved, TResolved>::Resolvable(const TUnresolved& unresolved)
			: m_unresolved(unresolved)
			, m_type(Type::Unresolved)
	{}

	template<typename TUnresolved, typename TResolved>
	Resolvable<TUnresolved, TResolved>::Resolvable(const TResolved& resolved)
			: m_resolved(resolved)
			, m_type(Type::Resolved)
	{}

	template<typename TUnresolved, typename TResolved>
	bool Resolvable<TUnresolved, TResolved>::isResolved() const {
		return Type::Resolved == m_type;
	}

	template<typename TUnresolved, typename TResolved>
	TUnresolved Resolvable<TUnresolved, TResolved>::unresolved() const {
		return isResolved() ? Unresolve(m_resolved) : m_unresolved;
	}

	template<typename TUnresolved, typename TResolved>
	TResolved Resolvable<TUnresolved, TResolved>::resolved() const {
		if (!isResolved())
			BITXORCORE_THROW_INVALID_ARGUMENT("underlying value is not resolved and requires ResolverContext");

		return m_resolved;
	}

	template<typename TUnresolved, typename TResolved>
	TResolved Resolvable<TUnresolved, TResolved>::resolved(const ResolverContext& resolvers) const {
		return isResolved() ? m_resolved : resolvers.resolve(m_unresolved);
	}

	template class Resolvable<UnresolvedAddress, Address>;
	template class Resolvable<UnresolvedTokenId, TokenId>;
}}
