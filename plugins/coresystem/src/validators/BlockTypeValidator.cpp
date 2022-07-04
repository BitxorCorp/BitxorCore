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

#include "Validators.h"
#include "bitxorcore/model/BlockUtils.h"

namespace bitxorcore { namespace validators {

	using Notification = model::BlockTypeNotification;

	DECLARE_STATELESS_VALIDATOR(BlockType, Notification)(uint64_t importanceGrouping) {
		return MAKE_STATELESS_VALIDATOR(BlockType, [importanceGrouping](const Notification& notification) {
			auto expectedBlockType = model::CalculateBlockTypeFromHeight(notification.BlockHeight, importanceGrouping);
			return expectedBlockType == notification.BlockType ? ValidationResult::Success : Failure_Core_Unexpected_Block_Type;
		});
	}
}}
