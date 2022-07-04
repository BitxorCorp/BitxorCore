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

#include "bitxorcore/tree/PatriciaTree.h"
#include "bitxorcore/tree/MemoryDataSource.h"
#include "tests/test/tree/PatriciaTreeTests.h"

namespace bitxorcore { namespace tree {

#define TEST_CLASS PatriciaTreeTests

	namespace {
		class MemoryTraits {
		public:
			using DataSourceType = MemoryDataSource;

		public:
			explicit MemoryTraits(DataSourceVerbosity verbosity) : m_dataSource(verbosity)
			{}

		public:
			DataSourceType& dataSource() {
				return m_dataSource;
			}

			void verifyDataSourceSize(size_t expectedSize) const {
				EXPECT_EQ(expectedSize, m_dataSource.size());
			}

		private:
			DataSourceType m_dataSource;
		};
	}

	DEFINE_PATRICIA_TREE_TESTS(MemoryTraits)
}}
