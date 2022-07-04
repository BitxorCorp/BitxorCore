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

#include "src/state/TokenDefinition.h"
#include "src/model/TokenProperties.h"
#include "bitxorcore/constants.h"
#include "tests/test/TokenTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace state {

#define TEST_CLASS TokenDefinitionTests

	namespace {
		constexpr Height Default_Height(345);

		void AssertDefaultRequiredProperties(const model::TokenProperties& properties) {
			EXPECT_FALSE(properties.is(model::TokenFlags::Supply_Mutable));
			EXPECT_FALSE(properties.is(model::TokenFlags::Transferable));
			EXPECT_FALSE(properties.is(model::TokenFlags::Restrictable));
			EXPECT_EQ(0u, properties.divisibility());
		}

		void AssertCustomOptionalProperties(const model::TokenProperties& expectedProperties, const model::TokenProperties& properties) {
			EXPECT_EQ(expectedProperties.duration(), properties.duration());
		}

		TokenDefinition CreateTokenDefinition(uint64_t duration) {
			auto owner = test::CreateRandomOwner();
			return TokenDefinition(Default_Height, owner, 3, test::CreateTokenPropertiesWithDuration(BlockDuration(duration)));
		}
	}

	// region ctor

	TEST(TEST_CLASS, CanCreateTokenDefinition_DefaultProperties) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = model::TokenProperties();

		// Act:
		TokenDefinition definition(Height(877), owner, 3, properties);

		// Assert:
		EXPECT_EQ(Height(877), definition.startHeight());
		EXPECT_EQ(owner, definition.ownerAddress());
		EXPECT_EQ(3u, definition.revision());
		AssertDefaultRequiredProperties(definition.properties());
		AssertCustomOptionalProperties(properties, definition.properties());
	}

	TEST(TEST_CLASS, CanCreateTokenDefinition_CustomProperties) {
		// Arrange:
		auto owner = test::CreateRandomOwner();
		auto properties = test::CreateTokenPropertiesWithDuration(BlockDuration(3));

		// Act:
		TokenDefinition definition(Height(877), owner, 3, properties);

		// Assert:
		EXPECT_EQ(Height(877), definition.startHeight());
		EXPECT_EQ(owner, definition.ownerAddress());
		EXPECT_EQ(3u, definition.revision());
		AssertDefaultRequiredProperties(definition.properties());
		AssertCustomOptionalProperties(properties, definition.properties());
	}

	// endregion

	// region isEternal

	TEST(TEST_CLASS, IsEternalReturnsTrueWhenTokenDefinitionHasEternalDuration) {
		// Arrange:
		auto definition = CreateTokenDefinition(Eternal_Artifact_Duration.unwrap());

		// Assert:
		EXPECT_TRUE(definition.isEternal());
	}

	TEST(TEST_CLASS, IsEternalReturnsFalseWhenTokenDefinitionDoesNotHaveEternalDuration) {
		// Arrange:
		for (auto duration : { 1u, 2u, 1000u, 10000u, 1'000'000'000u }) {
			auto definition = CreateTokenDefinition(duration);

			// Assert:
			EXPECT_FALSE(definition.isEternal()) << "duration " << duration;
		}
	}

	// endregion

	// region isActive

	namespace {
		void AssertActiveOrNot(BlockDuration::ValueType duration, const std::vector<Height::ValueType>& heights, bool expectedResult) {
			// Arrange: creation height is 345
			auto definition = CreateTokenDefinition(duration);

			// Assert:
			for (auto height : heights)
				EXPECT_EQ(expectedResult, definition.isActive(Height(height))) << "at height " << height;
		}
	}

	TEST(TEST_CLASS, IsActiveReturnsTrueWhenTokenDefinitionIsActive) {
		auto duration = 57u;
		auto height = Default_Height.unwrap();
		AssertActiveOrNot(duration, { height, height + 1, height + 22, height + duration - 2, height + duration - 1 }, true);
	}

	TEST(TEST_CLASS, IsActiveReturnsTrueWhenTokenDefinitionIsEternal) {
		auto duration = Eternal_Artifact_Duration.unwrap();
		auto height = Default_Height.unwrap();
		AssertActiveOrNot(duration, { height - 1, height, height + 1, 500u, 5000u, std::numeric_limits<Height::ValueType>::max() }, true);
	}

	TEST(TEST_CLASS, IsActiveReturnsFalseWhenTokenDefinitionIsNotActive) {
		auto duration = 57u;
		auto height = Default_Height.unwrap();
		AssertActiveOrNot(duration, { 1u, height - 2, height - 1, height + duration, height + duration + 1, height + 10'000 }, false);
	}

	// endregion

	// region isExpired

	namespace {
		void AssertExpiredOrNot(BlockDuration::ValueType duration, const std::vector<Height::ValueType>& heights, bool expectedResult) {
			// Arrange: creation height is 345
			auto definition = CreateTokenDefinition(duration);

			// Assert:
			for (auto height : heights)
				EXPECT_EQ(expectedResult, definition.isExpired(Height(height))) << "at height " << height;
		}
	}

	TEST(TEST_CLASS, IsExpiredReturnsTrueWhenTokenDefinitionIsExpired) {
		auto duration = 57u;
		auto height = Default_Height.unwrap();
		AssertExpiredOrNot(duration, { height + duration, height + duration + 1, height + 10'000 }, true);
	}

	TEST(TEST_CLASS, IsExpiredReturnsFalseWhenTokenDefinitionIsEternal) {
		auto duration = Eternal_Artifact_Duration.unwrap();
		auto height = Default_Height.unwrap();
		AssertExpiredOrNot(duration, { 1, height - 1, height, height + 1, 5000u, std::numeric_limits<Height::ValueType>::max() }, false);
	}

	TEST(TEST_CLASS, IsExpiredReturnsFalseWhenTokenDefinitionIsNotExpired) {
		auto duration = 57u;
		auto height = Default_Height.unwrap();
		AssertExpiredOrNot(duration, { 1, height - 1, height, height + 1, height + duration - 2, height + duration - 1 }, false);
	}

	// endregion
}}
