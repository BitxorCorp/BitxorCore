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

#include "src/validators/Validators.h"
#include "src/validators/ActiveTokenView.h"
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"
#include "bitxorcore/model/BlockchainConfiguration.h"
#include "tests/test/TokenCacheTestUtils.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace validators {

#define TEST_CLASS ActiveTokenViewTests

	namespace {
		struct TryGetTraits {
			static validators::ValidationResult TryGet(const ActiveTokenView& view, TokenId id, Height height) {
				ActiveTokenView::FindIterator tokenIter;
				return view.tryGet(id, height, tokenIter);
			}
		};

		struct TryGetWithOwnerTraits {
			static validators::ValidationResult TryGet(const ActiveTokenView& view, TokenId id, Height height) {
				ActiveTokenView::FindIterator tokenIter;
				return view.tryGet(id, height, Address(), tokenIter);
			}
		};
	}

#define TRY_GET_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<TryGetTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_WithOwner) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<TryGetWithOwnerTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	// region tryGet - active checks

	TRY_GET_BASED_TEST(CannotGetUnknownToken) {
		// Arrange:
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		auto delta = cache.createDelta();
		auto readOnlyCache = delta.toReadOnly();
		auto view = ActiveTokenView(readOnlyCache);

		// Act:
		auto result = TTraits::TryGet(view, TokenId(123), Height(100));

		// Assert:
		EXPECT_EQ(Failure_Token_Expired, result);
	}

	TRY_GET_BASED_TEST(CannotGetInactiveToken) {
		// Arrange:
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		auto delta = cache.createDelta();
		test::AddToken(delta, TokenId(123), Height(50), BlockDuration(100), Amount());
		cache.commit(Height());

		auto readOnlyCache = delta.toReadOnly();
		auto view = ActiveTokenView(readOnlyCache);

		// Act: token expires at 50 + 100
		auto result = TTraits::TryGet(view, TokenId(123), Height(200));

		// Assert:
		EXPECT_EQ(Failure_Token_Expired, result);
	}

	// endregion

	// region tryGet - owner checks

	template<typename TAction>
	void RunTestWithActiveToken(const Address& owner, TAction action) {
		// Arrange:
		auto cache = test::TokenCacheFactory::Create(model::BlockchainConfiguration::Uninitialized());
		auto delta = cache.createDelta();
		test::AddEternalToken(delta, TokenId(123), Height(50), owner);
		cache.commit(Height());

		auto readOnlyCache = delta.toReadOnly();
		auto view = ActiveTokenView(readOnlyCache);

		// Act + Assert:
		action(view);
	}

	TEST(TEST_CLASS, CanGetActiveTokenWithoutOwnerCheck) {
		// Arrange:
		RunTestWithActiveToken(test::CreateRandomOwner(), [](const auto& view) {
			// Act:
			ActiveTokenView::FindIterator tokenIter;
			auto result = view.tryGet(TokenId(123), Height(100), tokenIter);

			// Assert:
			EXPECT_EQ(ValidationResult::Success, result);
			EXPECT_EQ(TokenId(123), tokenIter.get().tokenId());
		});
	}

	TEST(TEST_CLASS, CannotGetActiveTokenWithWrongOwner) {
		// Arrange:
		RunTestWithActiveToken(test::CreateRandomOwner(), [](const auto& view) {
			// Act:
			ActiveTokenView::FindIterator tokenIter;
			auto result = view.tryGet(TokenId(123), Height(100), test::CreateRandomOwner(), tokenIter);

			// Assert:
			EXPECT_EQ(Failure_Token_Owner_Conflict, result);
		});
	}

	TEST(TEST_CLASS, CanGetActiveTokenWithCorrectOwner) {
		// Arrange:
		const auto& owner = test::CreateRandomOwner();
		RunTestWithActiveToken(owner, [&owner](const auto& view) {
			// Act:
			ActiveTokenView::FindIterator tokenIter;
			auto result = view.tryGet(TokenId(123), Height(100), owner, tokenIter);

			// Assert:
			EXPECT_EQ(ValidationResult::Success, result);
			EXPECT_EQ(TokenId(123), tokenIter.get().tokenId());
		});
	}

	// endregion
}}
