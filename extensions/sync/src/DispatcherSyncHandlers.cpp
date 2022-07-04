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

#include "DispatcherSyncHandlers.h"
#include "bitxorcore/cache/CacheStorage.h"
#include "bitxorcore/cache/BitxorCoreCache.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/extensions/LocalNodeChainScore.h"
#include "bitxorcore/extensions/LocalNodeStateFileStorage.h"

namespace bitxorcore { namespace sync {

	void AddSupplementalDataResiliency(
			consumers::BlockchainSyncHandlers& syncHandlers,
			const config::BitxorCoreDataDirectory& dataDirectory,
			const cache::BitxorCoreCache& cache,
			const extensions::LocalNodeChainScore& score) {
		auto preStateWrittenHandler = syncHandlers.PreStateWritten;

		// can't create any views (or storages) in PreStateWritten handler because cache lock is held by calling code
		auto pStorages = std::make_shared<decltype(cache.storages())>(cache.storages());
		syncHandlers.PreStateWritten = [preStateWrittenHandler, pStorages, dataDirectory, &score](const auto& cacheDelta, auto height) {
			extensions::LocalNodeStateSerializer serializer(dataDirectory.dir("state.tmp"));
			serializer.save(cacheDelta, *pStorages, score.get(), height);

			preStateWrittenHandler(cacheDelta, height);
		};

		auto commitStepHandler = syncHandlers.CommitStep;
		syncHandlers.CommitStep = [commitStepHandler, dataDirectory](auto step) {
			if (consumers::CommitOperationStep::All_Updated == step) {
				extensions::LocalNodeStateSerializer serializer(dataDirectory.dir("state.tmp"));
				serializer.moveTo(dataDirectory.dir("state"));
			}

			commitStepHandler(step);
		};
	}
}}
