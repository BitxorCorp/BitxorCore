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
#include "MultisigCacheTypes.h"
#include "bitxorcore/model/ContainerTypes.h"

namespace bitxorcore { namespace cache {

	/// Finds all ancestors of \a address in \a cache, adds them to \a ancestors and returns the maximum distance between
	/// \a address and any ancestor.
	size_t FindAncestors(const MultisigCacheTypes::CacheReadOnlyType& cache, const Address& address, model::AddressSet& ancestors);

	/// Finds all descendants of \a address in \a cache, adds them to \a descendants and returns the maximum distance between
	/// \a address and any descendant.
	size_t FindDescendants(const MultisigCacheTypes::CacheReadOnlyType& cache, const Address& address, model::AddressSet& descendants);
}}
