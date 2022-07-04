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
#include <functional>

namespace bitxorcore { namespace model {

	/// Context used to resolve unresolved types.
	class ResolverContext {
	private:
		template<typename TUnresolved, typename TResolved>
		using Resolver = std::function<TResolved (const TUnresolved&)>;
		using TokenResolver = Resolver<UnresolvedTokenId, TokenId>;
		using AddressResolver = Resolver<UnresolvedAddress, Address>;

	public:
		/// Creates a default context.
		ResolverContext();

		/// Creates a context around \a tokenResolver and \a addressResolver.
		ResolverContext(const TokenResolver& tokenResolver, const AddressResolver& addressResolver);

	public:
		/// Resolves token id (\a tokenId).
		TokenId resolve(UnresolvedTokenId tokenId) const;

		/// Resolves \a address.
		Address resolve(const UnresolvedAddress& address) const;

	private:
		TokenResolver m_tokenResolver;
		AddressResolver m_addressResolver;
	};
}}
