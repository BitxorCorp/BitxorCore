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
#include "ObserverTestContext.h"
#include "bitxorcore/cache_core/AccountStateCache.h"

namespace bitxorcore { namespace test {

	/// Observer test context that wraps an observer context and exposes functions for interacting with the account state cache.
	class AccountObserverTestContext : public test::ObserverTestContext {
	public:
		using ObserverTestContext::ObserverTestContext;

	public:
		/// Finds the account identified by \a address.
		const state::AccountState* find(const Address& address) const {
			return cache().sub<cache::AccountStateCache>().find(address).tryGet();
		}

		/// Finds the account identified by \a publicKey.
		const state::AccountState* find(const Key& publicKey) const {
			return cache().sub<cache::AccountStateCache>().find(publicKey).tryGet();
		}

	private:
		state::AccountState& addAccount(const Address& address) {
			auto& accountStateCache = cache().sub<cache::AccountStateCache>();
			accountStateCache.addAccount(address, Height(1234));
			return accountStateCache.find(address).get();
		}

		state::AccountState& addAccount(const Key& publicKey) {
			auto& accountStateCache = cache().sub<cache::AccountStateCache>();
			accountStateCache.addAccount(publicKey, Height(1));
			return accountStateCache.find(publicKey).get();
		}
	};
}}
