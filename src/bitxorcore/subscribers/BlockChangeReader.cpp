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

#include "BlockChangeReader.h"
#include "SubscriberOperationTypes.h"
#include "bitxorcore/io/BlockChangeSubscriber.h"
#include "bitxorcore/io/BlockElementSerializer.h"
#include "bitxorcore/io/BlockStatementSerializer.h"
#include "bitxorcore/io/PodIoUtils.h"
#include "bitxorcore/io/Stream.h"
#include "bitxorcore/exceptions.h"

namespace bitxorcore { namespace subscribers {

	namespace {
		void ReadAndNotifyBlock(io::InputStream& inputStream, io::BlockChangeSubscriber& subscriber) {
			auto pBlockElement = io::ReadBlockElement(inputStream);
			if (0 != io::Read8(inputStream)) {
				auto pStatement = std::make_shared<model::BlockStatement>();
				io::ReadBlockStatement(inputStream, *pStatement);
				pBlockElement->OptionalStatement = pStatement;
			}

			subscriber.notifyBlock(*pBlockElement);
		}

		void ReadAndNotifyDropBlocksAfter(io::InputStream& inputStream, io::BlockChangeSubscriber& subscriber) {
			auto height = io::Read<Height>(inputStream);
			subscriber.notifyDropBlocksAfter(height);
		}
	}

	void ReadNextBlockChange(io::InputStream& inputStream, io::BlockChangeSubscriber& subscriber) {
		auto operationType = static_cast<BlockChangeOperationType>(io::Read8(inputStream));

		switch (operationType) {
		case BlockChangeOperationType::Block:
			return ReadAndNotifyBlock(inputStream, subscriber);
		case BlockChangeOperationType::Drop_Blocks_After:
			return ReadAndNotifyDropBlocksAfter(inputStream, subscriber);
		}

		BITXORCORE_THROW_INVALID_ARGUMENT_1("invalid block change operation type", static_cast<uint16_t>(operationType));
	}
}}
