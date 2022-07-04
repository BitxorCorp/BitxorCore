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
#include "bitxorcore/model/Elements.h"
#include "bitxorcore/functions.h"
#include <vector>

namespace bitxorcore { namespace ionet { struct Packet; } }

namespace bitxorcore { namespace parsers {

	/// Tries to parse transaction elements out of \a packet and into \a elements with a validity check (\a isValid).
	bool TryParseTransactionElements(
			const ionet::Packet& packet,
			const predicate<const model::Transaction&>& isValid,
			std::vector<model::TransactionElement>& elements);
}}
