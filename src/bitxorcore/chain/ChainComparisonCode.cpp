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

#include "ChainComparisonCode.h"
#include "bitxorcore/utils/MacroBasedEnumIncludes.h"

namespace bitxorcore { namespace chain {

#define DEFINE_ENUM ChainComparisonCode
#define EXPLICIT_VALUE_ENUM
#define ENUM_LIST CHAIN_COMPARISON_CODE_LIST
#include "bitxorcore/utils/MacroBasedEnum.h"
#undef ENUM_LIST
#undef EXPLICIT_VALUE_ENUM
#undef DEFINE_ENUM

	bool IsRemoteOutOfSync(ChainComparisonCode code) {
		return 0 != (Remote_Is_Out_Of_Sync_Flag & utils::to_underlying_type(code));
	}

	bool IsRemoteEvil(ChainComparisonCode code) {
		return 0 != (Remote_Is_Evil_Flag & utils::to_underlying_type(code));
	}
}}
