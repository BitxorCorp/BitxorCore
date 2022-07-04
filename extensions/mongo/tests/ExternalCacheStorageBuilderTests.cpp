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

#include "mongo/src/ExternalCacheStorageBuilder.h"
#include "mongo/tests/test/mocks/MockExternalCacheStorage.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace mongo {

#define TEST_CLASS ExternalCacheStorageBuilderTests

	namespace {
		template<size_t CacheId>
		void AddSubStorageWithId(ExternalCacheStorageBuilder& builder) {
			builder.add(std::make_unique<mocks::MockExternalCacheStorage<CacheId>>());
		}
	}

	TEST(TEST_CLASS, CanCreateEmptyStorage) {
		// Arrange:
		ExternalCacheStorageBuilder builder;

		// Act:
		auto pStorage = builder.build();

		// Assert:
		EXPECT_EQ("{}", pStorage->name());
	}

	TEST(TEST_CLASS, CanCreateStorageWithSingleSubStorage) {
		// Arrange:
		ExternalCacheStorageBuilder builder;
		AddSubStorageWithId<5>(builder);

		// Act:
		auto pStorage = builder.build();

		// Assert:
		EXPECT_EQ("{ SimpleCache }", pStorage->name());
	}

	TEST(TEST_CLASS, CanCreateStorageWithMultipleSubStorage) {
		// Arrange:
		ExternalCacheStorageBuilder builder;
		AddSubStorageWithId<5>(builder);
		AddSubStorageWithId<2>(builder);
		AddSubStorageWithId<6>(builder);

		// Act:
		auto pStorage = builder.build();

		// Assert:
		EXPECT_EQ("{ SimpleCache, SimpleCache, SimpleCache }", pStorage->name());
	}

	TEST(TEST_CLASS, CannotAddMultipleStoragesWithSameId) {
		// Arrange:
		ExternalCacheStorageBuilder builder;
		AddSubStorageWithId<5>(builder);
		AddSubStorageWithId<2>(builder);
		AddSubStorageWithId<6>(builder);

		// Act + Assert:
		EXPECT_THROW(AddSubStorageWithId<5>(builder), bitxorcore_invalid_argument);
	}
}}
