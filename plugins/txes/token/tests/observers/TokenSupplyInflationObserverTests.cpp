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

#include "src/observers/Observers.h"
#include "bitxorcore/model/InflationCalculator.h"
#include "tests/test/TokenCacheTestUtils.h"
#include "tests/test/core/NotificationTestUtils.h"
#include "tests/test/plugins/ObserverTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace observers {

#define TEST_CLASS TokenSupplyInflationObserverTests

	using ObserverTestContext = test::ObserverTestContextT<test::TokenCacheFactory>;

	DEFINE_COMMON_OBSERVER_TESTS(TokenSupplyInflation, TokenId(), model::InflationCalculator())

	namespace {
		void AssertNoSupplyChangeWhenNoInflation(NotifyMode mode) {
			// Arrange:
			static constexpr auto Currency_Token_Id = TokenId(123);

			// - create observer
			model::InflationCalculator calculator;
			calculator.add(Height(100), Amount(200));
			calculator.add(Height(101), Amount(400));
			calculator.add(Height(102), Amount(0));
			auto pObserver = CreateTokenSupplyInflationObserver(Currency_Token_Id, calculator);

			// - create notification
			auto notification = test::CreateBlockNotification();

			// - create context
			ObserverTestContext context(mode, Height(888));

			// Act:
			ASSERT_NO_THROW(test::ObserveNotification(*pObserver, notification, context));

			// Assert: zero inflation at specified height, so token supply shouldn't have been modified
			const auto& tokenCacheDelta = context.cache().sub<cache::TokenCache>();
			EXPECT_FALSE(tokenCacheDelta.contains(Currency_Token_Id));
		}
	}

	TEST(TEST_CLASS, NoSupplyChangeWhenNoInflation_Commit) {
		AssertNoSupplyChangeWhenNoInflation(NotifyMode::Commit);
	}

	TEST(TEST_CLASS, NoSupplyChangeWhenNoInflation_Rollback) {
		AssertNoSupplyChangeWhenNoInflation(NotifyMode::Rollback);
	}

	namespace {
		void AssertSupplyChangeWhenInflation(NotifyMode mode, Amount expectedSupply) {
			// Arrange:
			static constexpr auto Currency_Token_Id = TokenId(123);

			// - create observer
			model::InflationCalculator calculator;
			calculator.add(Height(100), Amount(200));
			calculator.add(Height(101), Amount(400));
			calculator.add(Height(102), Amount(0));
			auto pObserver = CreateTokenSupplyInflationObserver(Currency_Token_Id, calculator);

			// - create notification
			auto notification = test::CreateBlockNotification();

			// - create context initialized with cache with a token supply
			ObserverTestContext context(mode, Height(101));
			test::AddToken(context.cache(), Currency_Token_Id, Height(1), Eternal_Artifact_Duration, Amount(1000));

			// Act:
			test::ObserveNotification(*pObserver, notification, context);

			// Assert: supply was adjusted for inflation
			const auto& tokenCacheDelta = context.cache().sub<cache::TokenCache>();
			EXPECT_EQ(expectedSupply, tokenCacheDelta.find(Currency_Token_Id).get().supply());
		}
	}

	TEST(TEST_CLASS, SupplyChangeWhenInflation_Commit) {
		AssertSupplyChangeWhenInflation(NotifyMode::Commit, Amount(1400));
	}

	TEST(TEST_CLASS, SupplyChangeWhenInflation_Rollback) {
		AssertSupplyChangeWhenInflation(NotifyMode::Rollback, Amount(600));
	}
}}
