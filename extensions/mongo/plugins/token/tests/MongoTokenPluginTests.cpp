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

#include "mongo/tests/test/MongoPluginTestUtils.h"
#include "plugins/txes/token/src/model/TokenEntityType.h"
#include "plugins/txes/token/src/model/TokenReceiptType.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace mongo { namespace plugins {

	namespace {
		struct MongoTokenPluginTraits {
		public:
			static constexpr auto RegisterSubsystem = RegisterMongoSubsystem;

			static std::vector<model::EntityType> GetTransactionTypes() {
				return {
					model::Entity_Type_Token_Definition,
					model::Entity_Type_Token_Supply_Change,
					model::Entity_Type_Token_Supply_Revocation
				};
			}

			static std::vector<model::ReceiptType> GetReceiptTypes() {
				return {
					model::Receipt_Type_Token_Expired,
					model::Receipt_Type_Token_Rental_Fee
				};
			}

			static std::string GetStorageName() {
				return "{ TokenCache }";
			}
		};
	}

	DEFINE_MONGO_PLUGIN_TESTS(MongoTokenPluginTests, MongoTokenPluginTraits)
}}}