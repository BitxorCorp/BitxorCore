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

#include "mongo/src/CoreMongo.h"
#include "mongo/tests/test/MongoPluginTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace mongo {

	namespace {
		struct CoreMongoTraits {
		public:
			static constexpr auto RegisterSubsystem = RegisterCoreMongoSystem;

			static std::vector<model::EntityType> GetTransactionTypes() {
				return { model::Entity_Type_Voting_Key_Link, model::Entity_Type_Vrf_Key_Link };
			}

			static std::vector<model::ReceiptType> GetReceiptTypes() {
				return { model::Receipt_Type_Harvest_Fee, model::Receipt_Type_Inflation };
			}

			static std::string GetStorageName() {
				return "{ AccountStateCache }";
			}
		};
	}

	DEFINE_MONGO_PLUGIN_TESTS(CoreMongoTests, CoreMongoTraits)
}}
