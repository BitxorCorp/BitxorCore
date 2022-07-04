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
#include "bitxorcore/disruptor/DisruptorConsumer.h"
#include "bitxorcore/disruptor/DisruptorElement.h"

namespace bitxorcore { namespace test {

	/// Creates a consumer input with \a numBlocks top-level blocks with \a source.
	disruptor::ConsumerInput CreateConsumerInputWithBlocks(size_t numBlocks, disruptor::InputSource source);

	/// Creates a consumer input with \a numTransactions top-level transactions with \a source.
	disruptor::ConsumerInput CreateConsumerInputWithTransactions(size_t numTransactions, disruptor::InputSource source);

	/// Creates a consumer input composed of the specified \a blocks.
	disruptor::ConsumerInput CreateConsumerInputFromBlocks(const std::vector<const model::Block*>& blocks);

	/// Creates a consumer input composed of the specified \a transactions.
	disruptor::ConsumerInput CreateConsumerInputFromTransactions(const std::vector<const model::Transaction*>& transactions);
}}
