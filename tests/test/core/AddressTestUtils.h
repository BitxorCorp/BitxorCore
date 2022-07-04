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
#include "bitxorcore/model/ContainerTypes.h"
#include "bitxorcore/model/NetworkIdentifier.h"
#include <memory>

namespace bitxorcore { namespace test {

	/// Generates a random address.
	Address GenerateRandomAddress();

	/// Generates a random address for a given network with id (\a networkIdentifier).
	Address GenerateRandomAddress(model::NetworkIdentifier networkIdentifier);

	/// Generates a random (unresolved) address.
	UnresolvedAddress GenerateRandomUnresolvedAddress();

	/// Generates a random vector of \a count addresses.
	std::vector<Address> GenerateRandomAddresses(size_t count);

	/// Generates a random set of \a count (unresolved) addresses.
	std::shared_ptr<model::UnresolvedAddressSet> GenerateRandomUnresolvedAddressSetPointer(size_t count);
}}
