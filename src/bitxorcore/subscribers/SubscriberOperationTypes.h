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
#include <stdint.h>

namespace bitxorcore { namespace subscribers {

	/// Block change operation type.
	enum class BlockChangeOperationType : uint8_t {
		/// Block saved.
		Block,

		/// Blocks dropped.
		Drop_Blocks_After
	};

	/// Partial transactions change operation type.
	enum class PtChangeOperationType : uint8_t {
		/// Add partial transaction infos.
		Add_Partials,

		/// Remove partial transaction infos.
		Remove_Partials,

		/// Add cosignature.
		Add_Cosignature
	};

	/// State change operation type.
	enum class StateChangeOperationType : uint8_t {
		/// Score change.
		Score_Change,

		/// State change.
		State_Change
	};

	/// Unconfirmed transactions change operation type.
	enum class UtChangeOperationType : uint8_t {
		/// Add transaction infos.
		Add,

		/// Remove transaction infos.
		Remove
	};
}}
