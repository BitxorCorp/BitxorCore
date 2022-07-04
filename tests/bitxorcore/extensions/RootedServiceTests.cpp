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

#include "bitxorcore/extensions/RootedService.h"
#include "tests/test/local/ServiceLocatorTestContext.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace extensions {

#define TEST_CLASS RootedServiceTests

	namespace {
		struct RootedServiceTraits {
			static auto CreateRegistrar(const std::shared_ptr<void>& pService, const std::string& serviceName) {
				return CreateRootedServiceRegistrar(pService, serviceName, ServiceRegistrarPhase::Initial);
			}
		};

		using TestContext = test::ServiceLocatorTestContext<RootedServiceTraits>;
	}

	TEST(TEST_CLASS, ServiceRegistrarSelfReportsCorrectInformation) {
		// Arrange:
		auto pService = CreateRootedServiceRegistrar(std::make_shared<int>(3), "ALPHA", ServiceRegistrarPhase::Initial_With_Modules);

		// Act:
		auto info = pService->info();

		// Assert:
		EXPECT_EQ("Rooted - ALPHA", info.Name);
		EXPECT_EQ(ServiceRegistrarPhase::Initial_With_Modules, info.Phase);
	}

	TEST(TEST_CLASS, NoCountersAreRegistered) {
		// Arrange:
		TestContext context;

		// Act:
		context.boot(std::make_shared<int>(3), "ALPHA");

		// Assert:
		EXPECT_EQ(0u, context.locator().counters().size());
	}

	TEST(TEST_CLASS, SingleServiceIsRegistered) {
		// Arrange:
		TestContext context;
		auto pService = std::make_shared<int>(3);

		// Act:
		context.boot(pService, "ALPHA");

		// Assert:
		EXPECT_EQ(1u, context.locator().numServices());
		EXPECT_EQ(pService, context.locator().service<int>("ALPHA"));
	}
}}
