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

#include "bitxorcore/state/CompactTokenMap.h"
#include "bitxorcore/utils/Casting.h"
#include "tests/test/nodeps/IteratorTestTraits.h"
#include "tests/TestHarness.h"
#include <unordered_map>

namespace bitxorcore { namespace state {

#define TEST_CLASS CompactTokenMapTests

	namespace {
		using TokenMap = std::unordered_map<TokenId, Amount, utils::BaseValueHasher<TokenId>>;

		// region utils

		TokenMap GetTenExpectedTokens() {
			return {
				{ TokenId(1), Amount(1) },
				{ TokenId(102), Amount(4) },
				{ TokenId(3), Amount(9) },
				{ TokenId(104), Amount(16) },
				{ TokenId(5), Amount(25) },
				{ TokenId(106), Amount(36) },
				{ TokenId(7), Amount(49) },
				{ TokenId(108), Amount(64) },
				{ TokenId(9), Amount(81) },
				{ TokenId(110), Amount(100) }
			};
		}

		TokenId GetTokenId(size_t index) {
			return TokenId(index + (0 == index % 2 ? 100 : 0));
		}

		void InsertMany(CompactTokenMap& map, size_t count) {
			for (auto i = 1u; i <= count; ++i)
				map.insert(std::make_pair(GetTokenId(i), Amount(i * i)));
		}

		template<typename TMap>
		bool Contains(TMap& map, TokenId tokenId) {
			return map.end() != map.find(tokenId);
		}

		// endregion

		// region asserts

		template<typename TMap>
		void AssertEmpty(TMap& map, const std::string& description) {
			// Assert:
			EXPECT_EQ(map.begin(), map.end()) << description;

			// - find should return end for tokens
			EXPECT_FALSE(Contains(map, TokenId(1))) << description;
			EXPECT_FALSE(Contains(map, TokenId(2))) << description;
			EXPECT_FALSE(Contains(map, TokenId(100))) << description;
		}

		void AssertEmpty(CompactTokenMap& map) {
			// Assert:
			EXPECT_TRUE(map.empty());
			EXPECT_EQ(0u, map.size());

			AssertEmpty(utils::as_const(map), "const");
			AssertEmpty(map, "non-const");
		}

		template<typename TActual>
		void AssertContents(const TokenMap& expectedTokens, TActual& actualTokens, const std::string& description) {
			// Assert:
			EXPECT_EQ(expectedTokens.size(), actualTokens.size()) << description;

			for (const auto& pair : expectedTokens) {
				std::ostringstream message;
				message << "token " << pair.first << " " << description;

				auto iter = actualTokens.find(pair.first);
				ASSERT_NE(actualTokens.end(), iter) << message.str();
				EXPECT_EQ(pair.first, iter->first) << message.str();
				EXPECT_EQ(pair.second, iter->second) << message.str();
			}
		}

		template<typename TActual>
		void AssertIteratedContents(
				TokenId optimizedTokenId,
				const TokenMap& expectedTokens,
				TActual& actualTokens,
				const std::string& description) {
			// Assert:
			EXPECT_NE(actualTokens.begin(), actualTokens.end()) << description;

			auto numIteratedTokens = 0u;
			auto lastTokenId = TokenId();
			TokenMap iteratedTokens;
			for (const auto& pair : actualTokens) {
				EXPECT_LT(lastTokenId, pair.first) << "expected ordering at " << numIteratedTokens;

				// compact map uses a custom sort that treats optimizedTokenId as smallest value; all other tokens are sorted normally
				if (0 != numIteratedTokens || optimizedTokenId != pair.first)
					lastTokenId = pair.first;

				if (0 != numIteratedTokens)
					EXPECT_NE(optimizedTokenId, pair.first) << "unexpected optimizedTokenId at " << numIteratedTokens;

				iteratedTokens.insert(pair);
				++numIteratedTokens;
			}

			EXPECT_EQ(expectedTokens.size(), numIteratedTokens) << description;
			AssertContents(expectedTokens, iteratedTokens, description);
		}

		void AssertContents(CompactTokenMap& map, TokenId optimizedTokenId, const TokenMap& expectedTokens) {
			// Assert:
			EXPECT_FALSE(map.empty());

			// - check that all tokens are accessible via find
			AssertContents(expectedTokens, utils::as_const(map), "via find (const)");
			AssertContents(expectedTokens, map, "via find (non-const)");

			// - check that all tokens are accessible via iteration
			AssertIteratedContents(optimizedTokenId, expectedTokens, utils::as_const(map), "via iteration (const)");
			AssertIteratedContents(optimizedTokenId, expectedTokens, map, "via iteration (non-const)");
		}

		void AssertContents(CompactTokenMap& map, const TokenMap& expectedTokens) {
			AssertContents(map, TokenId(), expectedTokens);
		}

		// endregion
	}

	// region constructor

	TEST(TEST_CLASS, MapIsInitiallyEmpty) {
		// Act:
		CompactTokenMap map;

		// Assert:
		AssertEmpty(map);
	}

	// endregion

	// region insert

	TEST(TEST_CLASS, CanInsertToken) {
		// Arrange:
		CompactTokenMap map;

		// Act:
		map.insert(std::make_pair(TokenId(123), Amount(245)));

		// Assert:
		AssertContents(map, { { TokenId(123), Amount(245) } });
	}

	TEST(TEST_CLASS, CanInsertTokenWithSmallerValue) {
		// Arrange:
		CompactTokenMap map;

		// Act: add token ids of decreasing value
		map.insert(std::make_pair(TokenId(123), Amount(245)));
		map.insert(std::make_pair(TokenId(100), Amount(333)));

		// Assert:
		AssertContents(map, {
			{ TokenId(100), Amount(333) },
			{ TokenId(123), Amount(245) }
		});
	}

	TEST(TEST_CLASS, CanInsertMultipleTokens_ValueAndPartialArray) {
		// Arrange:
		CompactTokenMap map;

		// Act:
		InsertMany(map, 2);

		// Assert:
		AssertContents(map, {
			{ TokenId(1), Amount(1) },
			{ TokenId(102), Amount(4) }
		});
	}

	TEST(TEST_CLASS, CanInsertMultipleTokens_ValueAndFullArray) {
		// Arrange:
		CompactTokenMap map;

		// Act:
		InsertMany(map, 6);

		// Assert:
		AssertContents(map, {
			{ TokenId(1), Amount(1) },
			{ TokenId(102), Amount(4) },
			{ TokenId(3), Amount(9) },
			{ TokenId(104), Amount(16) },
			{ TokenId(5), Amount(25) },
			{ TokenId(106), Amount(36) }
		});
	}

	TEST(TEST_CLASS, CanInsertMultipleTokens_ValueAndFullArrayAndMap) {
		// Arrange:
		CompactTokenMap map;

		// Act:
		InsertMany(map, 7);

		// Assert:
		AssertContents(map, {
			{ TokenId(1), Amount(1) },
			{ TokenId(102), Amount(4) },
			{ TokenId(3), Amount(9) },
			{ TokenId(104), Amount(16) },
			{ TokenId(5), Amount(25) },
			{ TokenId(106), Amount(36) },
			{ TokenId(7), Amount(49) }
		});
	}

	TEST(TEST_CLASS, CanInsertMultipleTokens_Many) {
		// Arrange:
		CompactTokenMap map;

		// Act:
		InsertMany(map, 10);

		// Assert:
		AssertContents(map, GetTenExpectedTokens());
	}

	TEST(TEST_CLASS, CanInsertTokenWithSmallerValue_Many) {
		// Arrange:
		CompactTokenMap map;
		map.optimize(TokenId(2));
		InsertMany(map, 10);

		// Act:
		map.insert(std::make_pair(TokenId(2), Amount(333)));

		// Assert:
		auto expectedTokens = GetTenExpectedTokens();
		expectedTokens.emplace(TokenId(2), Amount(333));
		AssertContents(map, TokenId(2), expectedTokens);

		// Sanity:
		EXPECT_EQ(TokenId(2), map.begin()->first);
	}

	TEST(TEST_CLASS, CanInsertTokenWithZeroBalance) {
		// Arrange:
		CompactTokenMap map;

		// Act:
		map.insert(std::make_pair(TokenId(123), Amount(0)));

		// Assert:
		AssertContents(map, { { TokenId(123), Amount(0) } });
	}

	TEST(TEST_CLASS, CannotInsertReservedToken) {
		// Arrange:
		CompactTokenMap map;

		// Act:
		EXPECT_THROW(map.insert(std::make_pair(TokenId(0), Amount(245))), bitxorcore_invalid_argument);
	}

	TEST(TEST_CLASS, CannotInsertExistingToken) {
		// Arrange:
		CompactTokenMap map;
		map.insert(std::make_pair(TokenId(123), Amount(245)));

		// Act:
		EXPECT_THROW(map.insert(std::make_pair(TokenId(123), Amount(245))), bitxorcore_invalid_argument);
	}

	// endregion

	// region insert + optimize

	namespace {
		TokenMap GetSevenExpectedTokensForOptimizeTests() {
			return {
				{ TokenId(29), Amount(876) },
				{ TokenId(85), Amount(111) },
				{ TokenId(100), Amount(333) },
				{ TokenId(101), Amount(10) },
				{ TokenId(102), Amount(20) },
				{ TokenId(104), Amount(40) },
				{ TokenId(303), Amount(30) }
			};
		}

		void InsertSevenForOptimizeTests(CompactTokenMap& map) {
			map.insert(std::make_pair(TokenId(100), Amount(333)));
			map.insert(std::make_pair(TokenId(29), Amount(876)));
			map.insert(std::make_pair(TokenId(85), Amount(111)));
			map.insert(std::make_pair(TokenId(104), Amount(40)));
			map.insert(std::make_pair(TokenId(303), Amount(30)));
			map.insert(std::make_pair(TokenId(102), Amount(20)));
			map.insert(std::make_pair(TokenId(101), Amount(10)));
		}
	}

	TEST(TEST_CLASS, CanInsertOptimizedToken) {
		// Arrange:
		CompactTokenMap map;
		map.optimize(TokenId(85));

		// Act: insert after optimize
		InsertSevenForOptimizeTests(map);

		// Assert: optimized token id should be treated as smallest value
		AssertContents(map, TokenId(85), GetSevenExpectedTokensForOptimizeTests());
	}

	namespace {
		void AssertCanInsertAndThenOptimize(TokenId optimizedTokenId) {
			// Arrange:
			CompactTokenMap map;
			InsertSevenForOptimizeTests(map);

			// Sanity:
			EXPECT_EQ(TokenId(29), map.begin()->first);

			// Act: optimize after insert
			map.optimize(optimizedTokenId);

			// Assert: optimized token id should be treated as smallest value
			AssertContents(map, optimizedTokenId, GetSevenExpectedTokensForOptimizeTests());

			// Sanity:
			EXPECT_EQ(optimizedTokenId, map.begin()->first);
		}
	}

	TEST(TEST_CLASS, CanInsertAndThenOptimizeTokenFromArray) {
		AssertCanInsertAndThenOptimize(TokenId(101));
	}

	TEST(TEST_CLASS, CanInsertAndThenOptimizeTokenFromMap) {
		AssertCanInsertAndThenOptimize(TokenId(303));
	}

	TEST(TEST_CLASS, CanReoptimizeWithSameToken) {
		// Arrange:
		CompactTokenMap map;

		// Act: optimize, insert, optimize (note that optimize must be called with same id)
		map.optimize(TokenId(85));
		map.optimize(TokenId(85));
		InsertSevenForOptimizeTests(map);
		map.optimize(TokenId(85));
		map.optimize(TokenId(85));

		// Assert: optimized token id should be treated as smallest value
		AssertContents(map, TokenId(85), GetSevenExpectedTokensForOptimizeTests());
	}

	TEST(TEST_CLASS, CanDeoptimizeWhenUnoptimized) {
		// Arrange:
		CompactTokenMap map;

		// Act: first optimize is noop
		map.optimize(TokenId(0));
		map.optimize(TokenId(85));
		InsertSevenForOptimizeTests(map);

		// Assert: optimized token id should be treated as smallest value
		AssertContents(map, TokenId(85), GetSevenExpectedTokensForOptimizeTests());
	}

	TEST(TEST_CLASS, CanReoptimizeWithDifferentToken) {
		// Arrange: insert and optimize
		CompactTokenMap map;
		InsertSevenForOptimizeTests(map);
		map.optimize(TokenId(102));

		// Sanity:
		EXPECT_EQ(TokenId(102), map.begin()->first);

		// Act: change optimization
		map.optimize(TokenId(85));

		// Assert: optimized token id should be treated as smallest value
		AssertContents(map, TokenId(85), GetSevenExpectedTokensForOptimizeTests());

		// Sanity:
		EXPECT_EQ(TokenId(85), map.begin()->first);
	}

	TEST(TEST_CLASS, CanDeoptimizeWhenOptimized) {
		// Arrange: insert and optimize
		CompactTokenMap map;
		InsertSevenForOptimizeTests(map);
		map.optimize(TokenId(102));

		// Sanity:
		EXPECT_EQ(TokenId(102), map.begin()->first);

		// Act: deoptimize
		map.optimize(TokenId(0));

		// Assert:
		AssertContents(map, GetSevenExpectedTokensForOptimizeTests());

		// Sanity:
		EXPECT_EQ(TokenId(29), map.begin()->first);
	}

	// endregion

	// region erase

	TEST(TEST_CLASS, CanEraseToken_Value) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 5);

		// Act:
		map.erase(TokenId(1));

		// Assert:
		AssertContents(map, {
			{ TokenId(102), Amount(4) },
			{ TokenId(3), Amount(9) },
			{ TokenId(104), Amount(16) },
			{ TokenId(5), Amount(25) }
		});
	}

	TEST(TEST_CLASS, CanEraseToken_Array) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 5);

		// Act:
		map.erase(TokenId(3));

		// Assert:
		AssertContents(map, {
			{ TokenId(1), Amount(1) },
			{ TokenId(102), Amount(4) },
			{ TokenId(104), Amount(16) },
			{ TokenId(5), Amount(25) }
		});
	}

	TEST(TEST_CLASS, CanEraseMultipleTokens_Odd) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		for (auto i = 1u; i <= 10; i += 2)
			map.erase(GetTokenId(i));

		// Assert:
		AssertContents(map, {
			{ TokenId(102), Amount(4) },
			{ TokenId(104), Amount(16) },
			{ TokenId(106), Amount(36) },
			{ TokenId(108), Amount(64) },
			{ TokenId(110), Amount(100) }
		});
	}

	TEST(TEST_CLASS, CanEraseMultipleTokens_Even) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		for (auto i = 2u; i <= 10; i += 2)
			map.erase(GetTokenId(i));

		// Assert:
		AssertContents(map, {
			{ TokenId(1), Amount(1) },
			{ TokenId(3), Amount(9) },
			{ TokenId(5), Amount(25) },
			{ TokenId(7), Amount(49) },
			{ TokenId(9), Amount(81) }
		});
	}

	TEST(TEST_CLASS, CanEraseAllTokens_Forward) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		for (auto i = 1u; i <= 10; ++i)
			map.erase(GetTokenId(i));

		// Assert:
		AssertEmpty(map);
	}

	TEST(TEST_CLASS, CanEraseAllTokens_Reverse) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		for (auto i = 10u; i >= 1; --i)
			map.erase(GetTokenId(i));

		// Assert:
		AssertEmpty(map);
	}

	TEST(TEST_CLASS, CanEraseTokenNotInMap) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 5);

		// Act: erasing a non-existent token has no effect
		map.erase(TokenId(123));

		// Assert:
		AssertContents(map, {
			{ TokenId(1), Amount(1) },
			{ TokenId(102), Amount(4) },
			{ TokenId(3), Amount(9) },
			{ TokenId(104), Amount(16) },
			{ TokenId(5), Amount(25) }
		});
	}

	// endregion

	// region insert after erase

	TEST(TEST_CLASS, CanInsertAfterEraseMultipleTokens_Odd) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		for (auto i = 1u; i <= 10; i += 2)
			map.erase(GetTokenId(i));

		for (auto i = 1u; i <= 3; ++i)
			map.insert(std::make_pair(TokenId(1000 + i), Amount(10 - i)));

		// Assert:
		AssertContents(map, {
			{ TokenId(102), Amount(4) },
			{ TokenId(104), Amount(16) },
			{ TokenId(106), Amount(36) },
			{ TokenId(108), Amount(64) },
			{ TokenId(110), Amount(100) },
			{ TokenId(1001), Amount(9) },
			{ TokenId(1002), Amount(8) },
			{ TokenId(1003), Amount(7) }
		});
	}

	TEST(TEST_CLASS, CanInsertAfterEraseMultipleTokens_Even) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		for (auto i = 2u; i <= 10; i += 2)
			map.erase(GetTokenId(i));

		for (auto i = 1u; i <= 3; ++i)
			map.insert(std::make_pair(TokenId(1000 + i), Amount(10 - i)));

		// Assert:
		AssertContents(map, {
			{ TokenId(1), Amount(1) },
			{ TokenId(3), Amount(9) },
			{ TokenId(5), Amount(25) },
			{ TokenId(7), Amount(49) },
			{ TokenId(9), Amount(81) },
			{ TokenId(1001), Amount(9) },
			{ TokenId(1002), Amount(8) },
			{ TokenId(1003), Amount(7) }
		});
	}

	TEST(TEST_CLASS, CanInsertAfterEraseAllTokens) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		for (auto i = 1u; i <= 10; ++i)
			map.erase(GetTokenId(i));

		for (auto i = 1u; i <= 3; ++i)
			map.insert(std::make_pair(TokenId(1000 + i), Amount(10 - i)));

		// Assert:
		AssertContents(map, {
			{ TokenId(1001), Amount(9) },
			{ TokenId(1002), Amount(8) },
			{ TokenId(1003), Amount(7) }
		});
	}

	// endregion

	// region iteration

#define ITERATOR_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_NonConst) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<test::BeginEndTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Const) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<test::BeginEndConstTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	ITERATOR_BASED_TEST(CanAdvanceIteratorsPostfixOperator) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		TokenMap allTokens;
		for (auto iter = TTraits::begin(map); TTraits::end(map) != iter; iter++)
			allTokens.insert(*iter);

		// Assert:
		AssertContents(GetTenExpectedTokens(), allTokens, "postfix");
	}

	ITERATOR_BASED_TEST(CanAdvanceIteratorsPrefixOperator) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		TokenMap allTokens;
		for (auto iter = TTraits::begin(map); TTraits::end(map) != iter; ++iter)
			allTokens.insert(*iter);

		// Assert:
		AssertContents(GetTenExpectedTokens(), allTokens, "prefix");
	}

	ITERATOR_BASED_TEST(CannotAdvanceIteratorsPastEnd) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		EXPECT_THROW(++TTraits::end(map), bitxorcore_out_of_range);
		EXPECT_THROW(TTraits::end(map)++, bitxorcore_out_of_range);
	}

	ITERATOR_BASED_TEST(CannotDereferenceIteratorsAtEnd) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		EXPECT_THROW(*TTraits::end(map), bitxorcore_out_of_range);
		EXPECT_THROW(TTraits::end(map).operator->(), bitxorcore_out_of_range);
	}

	ITERATOR_BASED_TEST(BeginEndIteratorsBasedOnDifferentMapsAreNotEqual) {
		// Arrange:
		CompactTokenMap map1;
		CompactTokenMap map2;

		// Act + Assert:
		EXPECT_NE(TTraits::begin(map1), TTraits::begin(map2));
		EXPECT_NE(TTraits::end(map1), TTraits::end(map2));
	}

	// endregion

	// region modification

	TEST(TEST_CLASS, CanModifyAmountViaFind) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 3);

		// Act:
		auto iter = map.find(TokenId(102));
		ASSERT_NE(map.end(), iter);

		iter->second = Amount(999);

		// Assert:
		AssertContents(map, {
			{ TokenId(1), Amount(1) },
			{ TokenId(102), Amount(999) },
			{ TokenId(3), Amount(9) }
		});
	}

	TEST(TEST_CLASS, CanZeroAndEraseAllTokens_Forward) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		for (auto i = 1u; i <= 10; ++i)
			map.find(GetTokenId(i))->second = Amount();

		for (auto i = 1u; i <= 10; ++i)
			map.erase(GetTokenId(i));

		// Assert:
		AssertEmpty(map);
	}

	TEST(TEST_CLASS, CanZeroAndEraseAllTokens_Reverse) {
		// Arrange:
		CompactTokenMap map;
		InsertMany(map, 10);

		// Act:
		for (auto i = 10u; i >= 1; --i)
			map.find(GetTokenId(i))->second = Amount();

		for (auto i = 10u; i >= 1; --i)
			map.erase(GetTokenId(i));

		// Assert:
		AssertEmpty(map);
	}

	// endregion
}}
