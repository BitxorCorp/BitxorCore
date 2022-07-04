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

#include "ConsumerInputFactory.h"
#include "ConsumerTestUtils.h"
#include "bitxorcore/consumers/BlockConsumers.h"
#include "bitxorcore/consumers/TransactionConsumers.h"
#include "tests/test/core/BlockTestUtils.h"
#include "tests/test/core/mocks/MockTransaction.h"

using bitxorcore::disruptor::ConsumerInput;

namespace bitxorcore { namespace test {

	namespace {
		ConsumerInput PrepareBlockInput(ConsumerInput&& input) {
			// 1. link all blocks
			LinkBlocks(Height(12), input.blocks());

			// 2. add all hashes
			auto transactionRegistry = mocks::CreateDefaultTransactionRegistry();
			auto consumer = consumers::CreateBlockHashCalculatorConsumer(GetDefaultGenerationHashSeed(), transactionRegistry);
			consumer(input.blocks());
			return std::move(input);
		}

		ConsumerInput PrepareTransactionInput(ConsumerInput&& input) {
			// 1. add all hashes
			auto transactionRegistry = mocks::CreateDefaultTransactionRegistry();
			auto consumer = consumers::CreateTransactionHashCalculatorConsumer(GetDefaultGenerationHashSeed(), transactionRegistry);
			consumer(input.transactions());
			return std::move(input);
		}
	}

	ConsumerInput CreateConsumerInputWithBlocks(size_t numBlocks, disruptor::InputSource source) {
		auto range = CreateBlockEntityRange(numBlocks);
		return PrepareBlockInput(ConsumerInput(std::move(range), source));
	}

	ConsumerInput CreateConsumerInputWithTransactions(size_t numTransactions, disruptor::InputSource source) {
		auto range = CreateTransactionEntityRange(numTransactions);
		return PrepareTransactionInput(ConsumerInput(std::move(range), source));
	}

	ConsumerInput CreateConsumerInputFromBlocks(const std::vector<const model::Block*>& blocks) {
		auto range = CreateEntityRange(blocks);
		return PrepareBlockInput(ConsumerInput(std::move(range)));
	}

	ConsumerInput CreateConsumerInputFromTransactions(const std::vector<const model::Transaction*>& transactions) {
		auto range = CreateEntityRange(transactions);
		return PrepareTransactionInput(ConsumerInput(std::move(range)));
	}
}}
