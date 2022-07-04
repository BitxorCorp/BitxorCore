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

#include "src/state/SecretLockInfoHistory.h"
#include "plugins/txes/lock_shared/tests/state/LockInfoHistoryTests.h"
#include "tests/test/SecretLockInfoCacheTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS SecretLockInfoHistoryTests

	namespace {
		struct SecretLockInfoHistoryTraits {
			static auto CreateLockInfo(Height endHeight, const Hash256& hash) {
				auto lockInfo = test::BasicSecretLockInfoTestTraits::CreateLockInfo();
				lockInfo.EndHeight = endHeight;
				lockInfo.CompositeHash = hash;
				return lockInfo;
			}
		};
	}

	DEFINE_LOCK_INFO_HISTORY_TESTS(SecretLockInfoHistory)
}}
