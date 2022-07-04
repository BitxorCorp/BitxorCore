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
#include "bitxorcore/io/BlockChangeSubscriber.h"
#include "bitxorcore/subscribers/FinalizationSubscriber.h"
#include "bitxorcore/subscribers/NodeSubscriber.h"
#include "bitxorcore/subscribers/StateChangeSubscriber.h"
#include "bitxorcore/subscribers/TransactionStatusSubscriber.h"
#include "tests/test/cache/UnsupportedTransactionsChangeSubscribers.h"

namespace bitxorcore { namespace test {

	/// Unsupported block change subscriber.
	class UnsupportedBlockChangeSubscriber : public io::BlockChangeSubscriber {
	public:
		void notifyBlock(const model::BlockElement&) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyBlock - not supported in mock");
		}

		void notifyDropBlocksAfter(Height) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyDropBlocksAfter - not supported in mock");
		}
	};

	/// Unsupported finalization subscriber.
	class UnsupportedFinalizationSubscriber : public subscribers::FinalizationSubscriber {
	public:
		void notifyFinalizedBlock(const model::FinalizationRound&, Height, const Hash256&) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyFinalizedBlock - not supported in mock");
		}
	};

	/// Unsupported node subscriber.
	class UnsupportedNodeSubscriber : public subscribers::NodeSubscriber {
	public:
		void notifyNode(const ionet::Node&) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyNode - not supported in mock");
		}

		bool notifyIncomingNode(const model::NodeIdentity&, ionet::ServiceIdentifier) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyIncomingNode - not supported in mock");
		}

		void notifyBan(const model::NodeIdentity&, uint32_t) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyBan - not supported in mock");
		}
	};

	/// Unsupported state change subscriber.
	class UnsupportedStateChangeSubscriber : public subscribers::StateChangeSubscriber {
	public:
		void notifyScoreChange(const model::ChainScore&) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyScoreChange - not supported in mock");
		}

		void notifyStateChange(const subscribers::StateChangeInfo&) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyStateChange - not supported in mock");
		}
	};

	/// Unsupported transaction status subscriber.
	template<UnsupportedFlushBehavior FlushBehavior>
	class UnsupportedTransactionStatusSubscriber : public subscribers::TransactionStatusSubscriber {
	public:
		void notifyStatus(const model::Transaction&, const Hash256&, uint32_t) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyStatus - not supported in mock");
		}

		void flush() override {
			FlushInvoker<FlushBehavior>::Flush();
		}
	};
}}
