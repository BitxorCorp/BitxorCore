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
#include "bitxorcore/utils/BitwiseEnum.h"
#include "bitxorcore/functions.h"
#include "bitxorcore/types.h"

namespace bitxorcore { namespace extensions {

	/// Possible transaction events.
	enum class TransactionEvent {
		/// Transaction dependency was removed.
		Dependency_Removed = 1
	};

	MAKE_BITWISE_ENUM(TransactionEvent)

	/// Data associated with a transaction event.
	struct TransactionEventData {
	public:
		/// Creates transaction event data around \a transactionHash and \a event.
		TransactionEventData(const Hash256& transactionHash, TransactionEvent event)
				: TransactionHash(transactionHash)
				, Event(event)
		{}

	public:
		/// Transaction hash.
		const Hash256& TransactionHash;

		/// Transaction event.
		TransactionEvent Event;
	};

	/// Handler that is called when a transaction event is raised.
	using TransactionEventHandler = consumer<const TransactionEventData&>;
}}
