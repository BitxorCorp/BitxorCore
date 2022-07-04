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
#include "bitxorcore/cache_tx/ShortHashPair.h"
#include "bitxorcore/model/CosignedTransactionInfo.h"
#include "bitxorcore/functions.h"
#include <vector>

namespace bitxorcore { namespace partialtransaction {

	/// Vector of cosigned (partial) transaction infos.
	using CosignedTransactionInfos = std::vector<model::CosignedTransactionInfo>;

	/// Prototype for a function that retrieves partial transaction infos given a filter and a set of short hash pairs.
	using CosignedTransactionInfosRetriever = std::function<CosignedTransactionInfos (Timestamp, const cache::ShortHashPairMap&)>;

	/// Function signature for consuming a vector of cosigned transaction infos.
	using CosignedTransactionInfosConsumer = consumer<CosignedTransactionInfos&&>;

	/// Function signature for supplying a range of short hash pairs.
	using ShortHashPairsSupplier = supplier<cache::ShortHashPairRange>;
}}
