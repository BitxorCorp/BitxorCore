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

#include "src/plugins/NamespacePlugin.h"
#include "src/cache/NamespaceCache.h"
#include "src/model/NamespaceEntityType.h"
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"
#include "tests/test/NamespaceTestUtils.h"
#include "tests/test/plugins/PluginManagerFactory.h"
#include "tests/test/plugins/PluginTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace plugins {

#define TEST_CLASS NamespacePluginTests

	// region basic

	namespace {
		struct NamespacePluginTraits {
		public:
			template<typename TAction>
			static void RunTestAfterRegistration(TAction action) {
				// Arrange:
				auto config = model::BlockchainConfiguration::Uninitialized();
				config.BlockGenerationTargetTime = utils::TimeSpan::FromSeconds(1);
				config.Plugins.emplace("bitxorcore.plugins.namespace", utils::ConfigurationBag({{
					"",
					{
						{ "maxNameSize", "0" },
						{ "maxChildNamespaces", "0" },
						{ "maxNamespaceDepth", "0" },

						{ "minNamespaceDuration", "0h" },
						{ "maxNamespaceDuration", "0h" },
						{ "namespaceGracePeriodDuration", "0h" },
						{ "reservedRootNamespaceNames", "reserved" },

						{ "namespaceRentalFeeSinkAddressPOS", "TESHM4G4RAQ2CKS6SLV4RSDQXF7Y3CAJLIOAOFA" },
						{ "namespaceRentalFeeSinkAddress", "TESHM4G4RAQ2CKS6SLV4RSDQXF7Y3CAJLIOAOFA" },
						{ "rootNamespaceRentalFeePerBlock", "0" },
						{ "childNamespaceRentalFee", "0" }
					}
				}}));

				auto manager = test::CreatePluginManager(config);
				RegisterNamespaceSubsystem(manager);

				// Act:
				action(manager);
			}

		public:
			static std::vector<model::EntityType> GetTransactionTypes() {
				return {
					model::Entity_Type_Namespace_Registration,
					model::Entity_Type_Alias_Address,
					model::Entity_Type_Alias_Token
				};
			}

			static std::vector<std::string> GetCacheNames() {
				return { "NamespaceCache" };
			}

			static std::vector<ionet::PacketType> GetNonDiagnosticPacketTypes() {
				return { ionet::PacketType::Namespace_State_Path };
			}

			static std::vector<ionet::PacketType> GetDiagnosticPacketTypes() {
				return { ionet::PacketType::Namespace_Infos };
			}

			static std::vector<std::string> GetDiagnosticCounterNames() {
				return { "NS C", "NS C AS", "NS C DS" };
			}

			static std::vector<std::string> GetStatelessValidatorNames() {
				return {
					"NamespaceRegistrationTypeValidator",
					"NamespaceNameValidator",
					"RootNamespaceValidator",
					"AliasActionValidator"
				};
			}

			static std::vector<std::string> GetStatefulValidatorNames() {
				return {
					"NamespaceReservedNameValidator",
					"RootNamespaceAvailabilityValidator",
					"NamespaceDurationOverflowValidator",
					"ChildNamespaceAvailabilityValidator",
					"RootNamespaceMaxChildrenValidator",
					"RequiredNamespaceValidator",
					"AliasAvailabilityValidator",
					"UnlinkAliasedAddressConsistencyValidator",
					"UnlinkAliasedTokenIdConsistencyValidator",
					"AddressAliasValidator"
				};
			}

			static std::vector<std::string> GetObserverNames() {
				return {
					"RootNamespaceObserver",
					"ChildNamespaceObserver",
					"NamespaceRentalFeeObserver",
					"NamespaceGracePeriodTouchObserver",
					"NamespaceTouchObserver",
					"AliasedAddressObserver",
					"AliasedTokenIdObserver"
				};
			}

			static std::vector<std::string> GetPermanentObserverNames() {
				return GetObserverNames();
			}
		};
	}

	DEFINE_PLUGIN_TESTS(NamespacePluginTests, NamespacePluginTraits)

	// endregion

	// region resolvers

	namespace {
		constexpr uint64_t Unresolved_Flag = 1ull << 63;
		constexpr UnresolvedAddress Unresolved_Address_With_Alias{ { 0x01, 0x33, 0x22, 0x11, 0x00, 0x00, 0x00, 0x00, 0x80 } };
		constexpr UnresolvedAddress Unresolved_Address_With_No_Alias{ { 0x01, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 } };
		constexpr Address Resolved_Address_With_No_Alias{ { 0x01, 0xDF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 } };

		template<typename TAction>
		void RunResolverTest(TAction action) {
			NamespacePluginTraits::RunTestAfterRegistration([action](auto& manager) {
				// Arrange: create a cache with three namespaces and two aliases (one root and one child)
				auto cache = manager.createCache();
				auto cacheDelta = cache.createDelta();
				auto readOnlyCache = cacheDelta.toReadOnly();
				auto& namespaceCacheDelta = cacheDelta.template sub<cache::NamespaceCache>();

				auto owner = test::CreateRandomOwner();
				namespaceCacheDelta.insert(state::RootNamespace(NamespaceId(Unresolved_Flag | 123), owner, test::CreateLifetime(10, 20)));
				namespaceCacheDelta.setAlias(NamespaceId(Unresolved_Flag | 123), state::NamespaceAlias(TokenId(456)));

				namespaceCacheDelta.insert(state::Namespace(test::CreatePath({ Unresolved_Flag | 123, Unresolved_Flag | 0x112233 })));
				namespaceCacheDelta.setAlias(NamespaceId(Unresolved_Flag | 0x112233), state::NamespaceAlias(Address{ { 25, 16 } }));

				namespaceCacheDelta.insert(state::RootNamespace(NamespaceId(Unresolved_Flag | 223), owner, test::CreateLifetime(10, 20)));

				auto resolverContext = manager.createResolverContext(readOnlyCache);

				// Act:
				action(resolverContext);
			});
		}
	}

	TEST(TEST_CLASS, TokenResolutionIsBypassedWhenValueIsAlreadyResolved) {
		// Act:
		RunResolverTest([](const auto& resolverContext) {
			// Act:
			auto tokenId = resolverContext.resolve(UnresolvedTokenId(124));

			// Assert:
			EXPECT_EQ(TokenId(124), tokenId);
		});
	}

	TEST(TEST_CLASS, TokenResolutionIsBypassedWhenNoNamespaceExists) {
		// Act:
		RunResolverTest([](const auto& resolverContext) {
			// Act:
			auto tokenId = resolverContext.resolve(UnresolvedTokenId(Unresolved_Flag | 124));

			// Assert:
			EXPECT_EQ(TokenId(Unresolved_Flag | 124), tokenId);
		});
	}

	TEST(TEST_CLASS, TokenResolutionIsBypassedWhenNamespaceExistsWithWrongAliasType) {
		// Act:
		RunResolverTest([](const auto& resolverContext) {
			// Act:
			auto tokenId = resolverContext.resolve(UnresolvedTokenId(Unresolved_Flag | 223));

			// Assert:
			EXPECT_EQ(TokenId(Unresolved_Flag | 223), tokenId);
		});
	}

	TEST(TEST_CLASS, TokenResolutionOccursWhenNamespaceExistsWithCorrectAliasType) {
		// Act:
		RunResolverTest([](const auto& resolverContext) {
			// Act:
			auto tokenId = resolverContext.resolve(UnresolvedTokenId(Unresolved_Flag | 123));

			// Assert:
			EXPECT_EQ(TokenId(456), tokenId);
		});
	}

	TEST(TEST_CLASS, AddressResolutionIsBypassedWhenValueIsAlreadyResolved) {
		// Act:
		RunResolverTest([](const auto& resolverContext) {
			// Act: unset bit 0 of first byte indicates a resolved address
			auto tokenId = resolverContext.resolve(UnresolvedAddress{ { 0x44, 0x38, 0x22, 0x11 } });

			// Assert:
			auto expectedAddress = Address{ { 0x44, 0x38, 0x22, 0x11 } };
			EXPECT_EQ(expectedAddress, tokenId);
		});
	}

	TEST(TEST_CLASS, AddressResolutionIsBypassedWhenNoNamespaceExists) {
		// Act:
		RunResolverTest([](const auto& resolverContext) {
			// Act:
			auto address = resolverContext.resolve(UnresolvedAddress{ { 0x43, 0x38, 0x22, 0x11 } });

			// Assert:
			auto expectedAddress = Address{ { 0x43, 0x38, 0x22, 0x11 } };
			EXPECT_EQ(expectedAddress, address);
		});
	}

	TEST(TEST_CLASS, AddressResolutionIsBypassedWhenNamespaceExistsWithWrongAliasType) {
		// Act:
		RunResolverTest([](const auto& resolverContext) {
			// Act:
			auto address = resolverContext.resolve(Unresolved_Address_With_No_Alias);

			// Assert:
			EXPECT_EQ(Resolved_Address_With_No_Alias, address);
		});
	}

	TEST(TEST_CLASS, AddressResolutionOccursWhenNamespaceExistsWithCorrectAliasType) {
		// Act:
		RunResolverTest([](const auto& resolverContext) {
			// Act:
			auto address = resolverContext.resolve(Unresolved_Address_With_Alias);

			// Assert:
			auto expectedAddress = Address{ { 25, 16 } };
			EXPECT_EQ(expectedAddress, address);
		});
	}

	// endregion
}}
