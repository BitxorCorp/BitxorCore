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

#include "ImportanceCalculator.h"
#include "bitxorcore/cache_core/AccountStateCache.h"

namespace bitxorcore { namespace importance {

	namespace {
		class RestoreImportanceCalculator final : public ImportanceCalculator {
		public:
			void recalculate(
					ImportanceRollbackMode,
					model::ImportanceHeight importanceHeight,
					cache::AccountStateCacheDelta& cache) const override {
				const auto& highValueAccounts = cache.highValueAccounts();

				PopAll(cache, highValueAccounts.addresses());
				PopAll(cache, highValueAccounts.removedAddresses());

				BITXORCORE_LOG(debug) << "restored importances at height " << importanceHeight;
			}

		private:
			static void PopAll(cache::AccountStateCacheDelta& cache, const model::AddressSet& addresses) {
				for (const auto& address : addresses) {
					auto accountStateIter = cache.find(address);
					if (!accountStateIter.tryGet())
						continue;

					auto& accountState = accountStateIter.get();
					accountState.ImportanceSnapshots.pop();
					accountState.ActivityBuckets.pop();
				}
			}
		};
	}

	std::unique_ptr<ImportanceCalculator> CreateRestoreImportanceCalculator() {
		return std::make_unique<RestoreImportanceCalculator>();
	}
}}
