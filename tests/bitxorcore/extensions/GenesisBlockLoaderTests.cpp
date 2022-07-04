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

#include "bitxorcore/extensions/GenesisBlockLoader.h"
#include "sdk/src/extensions/BlockExtensions.h"
#include "plugins/coresystem/src/observers/Observers.h"
#include "plugins/coresystem/src/validators/Validators.h"
#include "bitxorcore/cache_core/AccountStateCache.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/BlockUtils.h"
#include "bitxorcore/observers/NotificationObserverAdapter.h"
#include "tests/test/cache/BalanceTransfers.h"
#include "tests/test/cache/CacheTestUtils.h"
#include "tests/test/core/BlockTestUtils.h"
#include "tests/test/core/EntityTestUtils.h"
#include "tests/test/core/ResolverTestUtils.h"
#include "tests/test/core/mocks/MockMemoryBlockStorage.h"
#include "tests/test/core/mocks/MockTransaction.h"
#include "tests/test/local/LocalNodeTestState.h"
#include "tests/test/local/LocalTestUtils.h"
#include "tests/test/nodeps/Filesystem.h"
#include "tests/test/nodeps/KeyTestUtils.h"
#include "tests/test/plugins/PluginManagerFactory.h"

namespace bitxorcore { namespace extensions {

#define TEST_CLASS GenesisBlockLoaderTests

	namespace {
		constexpr auto Network_Identifier = model::NetworkIdentifier::Testnet;

		// currency and harvesting token transfers need to be tested for correct behavior
		constexpr auto Currency_Token_Id = TokenId(3456);
		constexpr auto Harvesting_Token_Id = TokenId(9876);

		// region create/set genesis block

		struct BlockSignerPair {
			std::unique_ptr<crypto::KeyPair> pSigner;
			std::unique_ptr<model::Block> pBlock;
		};

		struct GenesisOptions {
			Importance TotalChainImportance;
			Amount InitialCurrencyAtomicUnits;
			extensions::StateHashVerification StateHashVerification = StateHashVerification::Enabled;
		};

		BlockSignerPair CreateGenesisBlock(const std::vector<test::BalanceTransfers>& transferGroups) {
			BlockSignerPair blockSignerPair;
			blockSignerPair.pSigner = std::make_unique<crypto::KeyPair>(test::GenerateKeyPair());
			auto genesisPublicKey = blockSignerPair.pSigner->publicKey();
			model::Transactions transactions;
			for (const auto& transfers : transferGroups) {
				// since XOR token resolver is used in tests, unresolve all transfers so that they can be roundtripped
				// via resolvers during processing
				std::vector<model::UnresolvedToken> unresolvedTransfers;
				for (const auto& transfer : transfers)
					unresolvedTransfers.push_back({ test::UnresolveXor(transfer.TokenId), transfer.Amount });

				auto pTransaction = mocks::CreateTransactionWithFeeAndTransfers(Amount(), unresolvedTransfers);
				pTransaction->SignerPublicKey = genesisPublicKey;
				pTransaction->Version = mocks::MockTransaction::Current_Version;
				pTransaction->Network = model::NetworkIdentifier::Testnet;
				transactions.push_back(std::move(pTransaction));
			}

			blockSignerPair.pBlock = model::CreateBlock(
					model::Entity_Type_Block_Genesis,
					model::PreviousBlockContext(),
					Network_Identifier,
					genesisPublicKey,
					transactions);
			return blockSignerPair;
		}

		enum class GenesisBlockModification { None, Public_Key, Generation_Hash_Proof, Generation_Hash, Stateless, Stateful };

		void SetGenesisBlock(
				io::BlockStorageCache& storage,
				const BlockSignerPair& blockSignerPair,
				const model::NetworkInfo& network,
				GenesisBlockModification modification) {
			auto pGenesisBlockElement = storage.view().loadBlockElement(Height(1));
			auto storageModifier = storage.modifier();
			storageModifier.dropBlocksAfter(Height(0));

			// copy and prepare the block
			auto pModifiedBlock = test::CopyEntity(*blockSignerPair.pBlock);

			// 1. modify the block signer if requested
			if (GenesisBlockModification::Public_Key == modification)
				test::FillWithRandomData(pModifiedBlock->SignerPublicKey);
			else
				pModifiedBlock->SignerPublicKey = network.GenesisSignerPublicKey;

			// 2. modify the generation hash proof if requested
			if (GenesisBlockModification::Generation_Hash_Proof == modification) {
				test::FillWithRandomData(pModifiedBlock->GenerationHashProof);
			} else {
				auto vrfProof = crypto::GenerateVrfProof(network.GenerationHashSeed, *blockSignerPair.pSigner);
				pModifiedBlock->GenerationHashProof = { vrfProof.Gamma, vrfProof.VerificationHash, vrfProof.Scalar };
			}

			// 3. modify the generation hash if requested
			auto modifiedGenesisBlockElement = test::BlockToBlockElement(*pModifiedBlock);
			if (GenesisBlockModification::Generation_Hash == modification) {
				test::FillWithRandomData(modifiedGenesisBlockElement.GenerationHash);
			} else {
				auto proofHash = crypto::GenerateVrfProofHash(pModifiedBlock->GenerationHashProof.Gamma);
				modifiedGenesisBlockElement.GenerationHash = proofHash.copyTo<GenerationHash>();
			}

			// 4. modify the block to fail stateless validation
			if (GenesisBlockModification::Stateless == modification)
				++pModifiedBlock->Version;

			// 5. modify the block to fail stateful validation
			if (GenesisBlockModification::Stateful == modification)
				test::FillWithRandomData(pModifiedBlock->BeneficiaryAddress);

			storageModifier.saveBlock(modifiedGenesisBlockElement);
			storageModifier.commit();
		}

		// endregion

		// region utils

		model::Token MakeCurrencyToken(Amount::ValueType amount) {
			return { Currency_Token_Id, Amount(amount) };
		}

		model::Token MakeHarvestingToken(Amount amount) {
			return { Harvesting_Token_Id, amount };
		}

		model::Token MakeHarvestingToken(Amount::ValueType amount) {
			return MakeHarvestingToken(Amount(amount));
		}

		model::BlockchainConfiguration CreateDefaultConfiguration(const model::Block& genesisBlock, const GenesisOptions& genesisOptions) {
			auto config = model::BlockchainConfiguration::Uninitialized();
			config.Network.Identifier = Network_Identifier;
			config.Network.GenesisSignerPublicKey = genesisBlock.SignerPublicKey;
			test::FillWithRandomData(config.Network.GenerationHashSeed);
			config.CurrencyTokenId = Currency_Token_Id;
			config.HarvestingTokenId = Harvesting_Token_Id;
			config.InitialCurrencyAtomicUnits = genesisOptions.InitialCurrencyAtomicUnits;
			config.MaxTokenAtomicUnits = Amount(15'000'000);
			config.TotalChainImportance = genesisOptions.TotalChainImportance;
			return config;
		}

		plugins::PluginManager CreatePluginManager() {
			// enable Publish_Transfers (MockTransaction Publish XORs recipient address, so XOR address resolver is required
			// for proper roundtripping or else test will fail)
			auto config = model::BlockchainConfiguration::Uninitialized();
			config.Network.Identifier = Network_Identifier;
			config.HarvestingTokenId = Harvesting_Token_Id;
			auto manager = test::CreatePluginManager(config);
			manager.addTransactionSupport(mocks::CreateMockTransactionPlugin(mocks::PluginOptionFlags::Publish_Transfers));
			manager.addTransactionSupport(mocks::CreateMockTransactionPlugin(
					static_cast<model::EntityType>(0xFFFE),
					mocks::PluginOptionFlags::Not_Top_Level));

			manager.addTokenResolver([](const auto&, const auto& unresolved, auto& resolved) {
				resolved = test::CreateResolverContextXor().resolve(unresolved);
				return true;
			});
			manager.addAddressResolver([](const auto&, const auto& unresolved, auto& resolved) {
				resolved = test::CreateResolverContextXor().resolve(unresolved);
				return true;
			});

			manager.addStatelessValidatorHook([](auto& builder) {
				builder.add(validators::CreateEntityVersionValidator());
			});

			manager.addStatefulValidatorHook([](auto& builder) {
				builder.add(validators::CreateAddressValidator());
			});
			return manager;
		}

		std::unique_ptr<const observers::NotificationObserver> CreateObserverWithHarvestNetworkFees(
				uint8_t harvestNetworkPercentage,
				const Address& harvestNetworkFeeSinkAddress) {
			// use real coresystem observers to create accounts, update balances and add harvest receipt
			observers::DemuxObserverBuilder builder;
			builder
				.add(observers::CreateAccountAddressObserver())
				.add(observers::CreateAccountPublicKeyObserver())
				.add(observers::CreateBalanceTransferObserver())
				.add(observers::CreateHarvestFeeObserver(
						{
							Harvesting_Token_Id,
							20,
							harvestNetworkPercentage,
							harvestNetworkPercentage,
							model::HeightDependentAddress(harvestNetworkFeeSinkAddress),
							model::HeightDependentAddress(harvestNetworkFeeSinkAddress)
						},
						model::InflationCalculator()));
			return builder.build();
		}

		std::unique_ptr<const observers::NotificationObserver> CreateObserver() {
			return CreateObserverWithHarvestNetworkFees(0, Address());
		}

		const Key& GetTransactionRecipient(const model::Block& block, size_t index) {
			auto transactions = block.Transactions();
			auto iter = transactions.cbegin();
			for (auto i = 0u; i < index; ++i, ++iter);
			return static_cast<const mocks::MockTransaction&>(*iter).RecipientPublicKey;
		}

		// endregion

		// region generic tests

		template<typename TTraits, typename TAssertAccountStateCache>
		void RunLoadGenesisBlockTest(
				const BlockSignerPair& genesisBlockSignerPair,
				const GenesisOptions& genesisOptions,
				TAssertAccountStateCache assertAccountStateCache) {
			// Arrange: create the state
			auto config = CreateDefaultConfiguration(*genesisBlockSignerPair.pBlock, genesisOptions);
			test::LocalNodeTestState state(config);
			SetGenesisBlock(state.ref().Storage, genesisBlockSignerPair, config.Network, GenesisBlockModification::None);

			// - create the publisher, observer and loader
			auto pluginManager = CreatePluginManager();
			{
				auto cacheDelta = state.ref().Cache.createDelta();
				GenesisBlockLoader loader(cacheDelta, pluginManager, CreateObserver());

				// Act:
				TTraits::Execute(loader, state.ref(), genesisOptions.StateHashVerification);

				// Assert:
				TTraits::Assert(cacheDelta, assertAccountStateCache);
			}

			auto cacheView = state.ref().Cache.createView();
			TTraits::Assert(cacheView, assertAccountStateCache);
		}

		template<typename TTraits, typename TAssertAccountStateCache>
		void RunLoadGenesisBlockTest(
				const BlockSignerPair& genesisBlockSignerPair,
				Importance totalChainImportance,
				TAssertAccountStateCache assertAccountStateCache) {
			GenesisOptions genesisOptions{ totalChainImportance, Amount() };
			RunLoadGenesisBlockTest<TTraits>(genesisBlockSignerPair, genesisOptions, assertAccountStateCache);
		}

		enum class GenesisBlockVerifyOptions { None, State, Receipts };

		template<typename TTraits, typename TException = bitxorcore_invalid_argument>
		void AssertLoadGenesisBlockFailure(
				const BlockSignerPair& genesisBlockSignerPair,
				const GenesisOptions& genesisOptions,
				GenesisBlockModification modification = GenesisBlockModification::None,
				GenesisBlockVerifyOptions verifyOptions = GenesisBlockVerifyOptions::None) {
			// Arrange:
			bool enableVerifiableState = GenesisBlockVerifyOptions::State == verifyOptions;

			// - create the state
			auto config = CreateDefaultConfiguration(*genesisBlockSignerPair.pBlock, genesisOptions);
			config.EnableVerifiableReceipts = GenesisBlockVerifyOptions::Receipts == verifyOptions;

			auto cacheConfig = cache::CacheConfiguration();
			test::TempDirectoryGuard dbDirGuard;
			if (enableVerifiableState)
				cacheConfig = cache::CacheConfiguration(dbDirGuard.name(), cache::PatriciaTreeStorageMode::Enabled);

			auto cache = test::CreateEmptyBitxorCoreCache(config, cacheConfig);
			test::LocalNodeTestState state(config, "", std::move(cache));
			SetGenesisBlock(state.ref().Storage, genesisBlockSignerPair, config.Network, modification);

			{
				// Sanity: state hash calculation is configured properly on the cache
				auto cacheDelta = state.ref().Cache.createDelta();
				cacheDelta.sub<cache::AccountStateCache>().addAccount(test::GenerateRandomByteArray<Address>(), Height(1));

				auto cacheStateHash = cacheDelta.calculateStateHash(Height(1)).StateHash;
				if (enableVerifiableState)
					EXPECT_NE(Hash256(), cacheStateHash);
				else
					EXPECT_EQ(Hash256(), cacheStateHash);
			}

			// Arrange:  create the publisher, observer and loader
			auto pluginManager = CreatePluginManager();
			auto cacheDelta = state.ref().Cache.createDelta();
			GenesisBlockLoader loader(cacheDelta, pluginManager, CreateObserver());

			// Act + Assert:
			EXPECT_THROW(TTraits::Execute(loader, state.ref(), StateHashVerification::Enabled), TException)
					<< "total chain importance " << genesisOptions.TotalChainImportance
					<< ", initial currency atomic units " << genesisOptions.InitialCurrencyAtomicUnits;
		}

		template<typename TTraits, typename TException = bitxorcore_invalid_argument>
		void AssertLoadGenesisBlockFailure(
				const BlockSignerPair& genesisBlockSignerPair,
				Importance totalChainImportance,
				GenesisBlockModification modification = GenesisBlockModification::None,
				GenesisBlockVerifyOptions verifyOptions = GenesisBlockVerifyOptions::None) {
			GenesisOptions genesisOptions{ totalChainImportance, Amount() };
			AssertLoadGenesisBlockFailure<TTraits, TException>(genesisBlockSignerPair, genesisOptions, modification, verifyOptions);
		}

		// endregion
	}

	// region traits

	namespace {
		struct ExecuteTraits {
			static void Execute(
					GenesisBlockLoader& loader,
					const LocalNodeStateRef& stateRef,
					StateHashVerification stateHashVerification) {
				loader.execute(stateRef, stateHashVerification);
			}

			template<typename TAssertAccountStateCache>
			static void Assert(cache::BitxorCoreCacheDelta& cacheDelta, TAssertAccountStateCache assertAccountStateCache) {
				// Assert: changes should only be present in the delta
				const auto& accountStateCache = cacheDelta.sub<cache::AccountStateCache>();
				assertAccountStateCache(accountStateCache);
			}

			template<typename TAssertAccountStateCache>
			static void Assert(cache::BitxorCoreCacheView& cacheView, TAssertAccountStateCache) {
				// Sanity: the view is not modified
				EXPECT_EQ(0u, cacheView.sub<cache::AccountStateCache>().size());
			}
		};

		struct ExecuteAndCommitTraits {
			static void Execute(
					GenesisBlockLoader& loader,
					const LocalNodeStateRef& stateRef,
					StateHashVerification stateHashVerification) {
				loader.executeAndCommit(stateRef, stateHashVerification);
			}

			template<typename TAssertAccountStateCache>
			static void Assert(cache::BitxorCoreCacheDelta& cacheDelta, TAssertAccountStateCache assertAccountStateCache) {
				// Assert: changes should be present in the delta
				const auto& accountStateCache = cacheDelta.sub<cache::AccountStateCache>();
				assertAccountStateCache(accountStateCache);
			}

			template<typename TAssertAccountStateCache>
			static void Assert(cache::BitxorCoreCacheView& cacheView, TAssertAccountStateCache assertAccountStateCache) {
				// Assert: changes should be committed to the underlying cache, so check the view
				const auto& accountStateCache = cacheView.sub<cache::AccountStateCache>();
				assertAccountStateCache(accountStateCache);
			}
		};

		struct ExecuteDefaultStateTraits {
			static void Execute(GenesisBlockLoader& loader, const LocalNodeStateRef& stateRef, StateHashVerification) {
				auto pGenesisBlockElement = stateRef.Storage.view().loadBlockElement(Height(1));
				loader.execute(stateRef.Config.Blockchain, *pGenesisBlockElement);
			}

			template<typename TAssertAccountStateCache>
			static void Assert(cache::BitxorCoreCacheDelta& cacheDelta, TAssertAccountStateCache assertAccountStateCache) {
				// Assert: changes should only be present in the delta
				const auto& accountStateCache = cacheDelta.sub<cache::AccountStateCache>();
				assertAccountStateCache(accountStateCache);
			}

			template<typename TAssertAccountStateCache>
			static void Assert(cache::BitxorCoreCacheView& cacheView, TAssertAccountStateCache) {
				// Sanity: the view is not modified
				EXPECT_EQ(0u, cacheView.sub<cache::AccountStateCache>().size());
			}
		};
	}

#define TRAITS_BASED_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_ExecuteAndCommit) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<ExecuteAndCommitTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Execute) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<ExecuteTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_ExecuteDefaultState) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<ExecuteDefaultStateTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

#define TRAITS_BASED_DISABLED_VERIFICATION_TEST(TEST_NAME) \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)(); \
	TEST(TEST_CLASS, TEST_NAME##_ExecuteAndCommit) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<ExecuteAndCommitTraits>(); } \
	TEST(TEST_CLASS, TEST_NAME##_Execute) { TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)<ExecuteTraits>(); } \
	template<typename TTraits> void TRAITS_TEST_NAME(TEST_CLASS, TEST_NAME)()

	// endregion

	// region success - with transfers

	namespace {
		template<typename TTraits>
		void AssertCanLoadValidGenesisBlockWithSingleTokenTransfer(Importance totalChainImportance, Amount totalChainBalance) {
			// Arrange: create a valid genesis block with a single (token) transaction
			auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(totalChainBalance) } });
			const auto& genesisBlock = *genesisBlockSignerPair.pBlock;

			// Act:
			RunLoadGenesisBlockTest<TTraits>(genesisBlockSignerPair, totalChainImportance, [totalChainBalance, &genesisBlock](
					const auto& accountStateCache) {
				// Assert:
				EXPECT_EQ(2u, accountStateCache.size());
				const auto& recipient = GetTransactionRecipient(genesisBlock, 0);
				test::AssertBalances(accountStateCache, genesisBlock.SignerPublicKey, {});
				test::AssertBalances(accountStateCache, recipient, { MakeHarvestingToken(totalChainBalance) });
			});
		}
	}

	TRAITS_BASED_TEST(CanLoadValidGenesisBlock_SingleTokenTransfer) {
		AssertCanLoadValidGenesisBlockWithSingleTokenTransfer<TTraits>(Importance(1234), Amount(1234));
	}

	TRAITS_BASED_TEST(CanLoadValidGenesisBlock_MultipleTokenTransfers) {
		// Arrange: create a valid genesis block with multiple token transactions
		auto genesisBlockSignerPair = CreateGenesisBlock({
			{ MakeHarvestingToken(1234) },
			{ MakeHarvestingToken(123), MakeHarvestingToken(213) },
			{ MakeHarvestingToken(987) }
		});
		const auto& genesisBlock = *genesisBlockSignerPair.pBlock;

		// Act:
		auto totalChainImportance = Importance(1234 + 123 + 213 + 987);
		RunLoadGenesisBlockTest<TTraits>(genesisBlockSignerPair, totalChainImportance, [&genesisBlock](const auto& accountStateCache) {
			// Assert:
			EXPECT_EQ(4u, accountStateCache.size());
			test::AssertBalances(accountStateCache, genesisBlock.SignerPublicKey, {});
			test::AssertBalances(accountStateCache, GetTransactionRecipient(genesisBlock, 0), { MakeHarvestingToken(1234) });
			test::AssertBalances(accountStateCache, GetTransactionRecipient(genesisBlock, 1), { MakeHarvestingToken(123 + 213) });
			test::AssertBalances(accountStateCache, GetTransactionRecipient(genesisBlock, 2), { MakeHarvestingToken(987) });
		});
	}

	TRAITS_BASED_TEST(CanLoadValidGenesisBlock_MultipleHeterogeneousTokenTransfersIncludingCurrencyToken) {
		// Arrange: create a valid genesis block with multiple token transactions
		auto genesisBlockSignerPair = CreateGenesisBlock({
			{ MakeHarvestingToken(1234), { TokenId(123), Amount(111) }, { TokenId(444), Amount(222) } },
			{ MakeCurrencyToken(987) },
			{ { TokenId(333), Amount(213) }, { TokenId(444), Amount(123) }, { TokenId(333), Amount(217) } }
		});
		const auto& genesisBlock = *genesisBlockSignerPair.pBlock;

		// Act:
		GenesisOptions genesisOptions{ Importance(1234), Amount(987) };
		RunLoadGenesisBlockTest<TTraits>(genesisBlockSignerPair, genesisOptions, [&genesisBlock](const auto& accountStateCache) {
			// Assert:
			EXPECT_EQ(4u, accountStateCache.size());
			test::AssertBalances(accountStateCache, genesisBlock.SignerPublicKey, {});
			test::AssertBalances(accountStateCache, GetTransactionRecipient(genesisBlock, 0), {
				MakeHarvestingToken(1234),
				{ TokenId(123), Amount(111) },
				{ TokenId(444), Amount(222) }
			});
			test::AssertBalances(accountStateCache, GetTransactionRecipient(genesisBlock, 1), { MakeCurrencyToken(987) });
			test::AssertBalances(accountStateCache, GetTransactionRecipient(genesisBlock, 2), {
				{ TokenId(333), Amount(213 + 217) },
				{ TokenId(444), Amount(123) }
			});
		});
	}

	TRAITS_BASED_TEST(CanLoadValidGenesisBlock_BalanceMultipleOfImportance) {
		// Assert: (balance == 10000 * importance)
		AssertCanLoadValidGenesisBlockWithSingleTokenTransfer<TTraits>(Importance(1234), Amount(1234 * 10000));
	}

	// endregion

	// region success - state hash

	TRAITS_BASED_DISABLED_VERIFICATION_TEST(CanLoadGenesisBlockContainingWrongStateHash_VerifiableStateDisabled) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });
		auto& genesisBlock = *genesisBlockSignerPair.pBlock;

		// - use the wrong state hash
		test::FillWithRandomData(genesisBlock.StateHash);

		// Act:
		GenesisOptions genesisOptions{ Importance(1234), Amount(), StateHashVerification::Disabled };
		RunLoadGenesisBlockTest<TTraits>(genesisBlockSignerPair, genesisOptions, [&genesisBlock](const auto& accountStateCache) {
			// Assert:
			EXPECT_EQ(2u, accountStateCache.size());
			test::AssertBalances(accountStateCache, genesisBlock.SignerPublicKey, {});
			test::AssertBalances(accountStateCache, GetTransactionRecipient(genesisBlock, 0), { MakeHarvestingToken(1234) });
		});
	}

	TRAITS_BASED_TEST(CanLoadValidGenesisBlock_VerifiableStateEnabled) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });
		auto& genesisBlock = *genesisBlockSignerPair.pBlock;

		// - create the state (with verifiable state enabled)
		test::TempDirectoryGuard dbDirGuard;
		GenesisOptions genesisOptions{ Importance(1234), Amount() };
		auto config = CreateDefaultConfiguration(genesisBlock, genesisOptions);
		auto cacheConfig = cache::CacheConfiguration(dbDirGuard.name(), cache::PatriciaTreeStorageMode::Enabled);
		auto cache = test::CreateEmptyBitxorCoreCache(config, cacheConfig);
		{
			// - calculate the expected state hash after block execution
			auto cacheDelta = cache.createDelta();
			auto& accountStateCacheDelta = cacheDelta.sub<cache::AccountStateCache>();

			auto recipient = GetTransactionRecipient(genesisBlock, 0);
			accountStateCacheDelta.addAccount(genesisBlock.SignerPublicKey, Height(1));
			accountStateCacheDelta.addAccount(recipient, Height(1));
			accountStateCacheDelta.find(recipient).get().Balances.credit(Harvesting_Token_Id, Amount(1234));

			genesisBlock.StateHash = cacheDelta.calculateStateHash(Height(1)).StateHash;

			// Sanity:
			EXPECT_NE(Hash256(), genesisBlock.StateHash);
		}

		test::LocalNodeTestState state(config, "", std::move(cache));

		SetGenesisBlock(state.ref().Storage, genesisBlockSignerPair, config.Network, GenesisBlockModification::None);

		// - create the publisher, observer and loader
		auto pluginManager = CreatePluginManager();
		auto cacheDelta = state.ref().Cache.createDelta();
		GenesisBlockLoader loader(cacheDelta, pluginManager, CreateObserver());

		// Act:
		TTraits::Execute(loader, state.ref(), StateHashVerification::Enabled);

		// Assert:
		TTraits::Assert(cacheDelta, [&genesisBlock](const auto& accountStateCache) {
			// Assert:
			EXPECT_EQ(2u, accountStateCache.size());
			test::AssertBalances(accountStateCache, genesisBlock.SignerPublicKey, {});
			test::AssertBalances(accountStateCache, GetTransactionRecipient(genesisBlock, 0), { MakeHarvestingToken(1234) });
		});
	}

	// endregion

	// region success - receipts block hash

	TRAITS_BASED_TEST(CanLoadValidGenesisBlock_VerifiableReceiptsEnabled) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });
		auto& genesisBlock = *genesisBlockSignerPair.pBlock;

		// - create the state (with verifiable receipts enabled)
		GenesisOptions genesisOptions{ Importance(1234), Amount() };
		auto config = CreateDefaultConfiguration(genesisBlock, genesisOptions);
		config.EnableVerifiableReceipts = true;
		auto cache = test::CreateEmptyBitxorCoreCache(config);
		{
			// - calculate the expected receipts hash
			model::BlockStatementBuilder blockStatementBuilder;

			// - harvest receipt added by HarvestFeeObserver
			auto receiptType = model::Receipt_Type_Harvest_Fee;
			auto receiptTokenId = Harvesting_Token_Id;
			blockStatementBuilder.addReceipt(model::BalanceChangeReceipt(
					receiptType,
					model::GetSignerAddress(genesisBlock),
					receiptTokenId,
					Amount()));

			// - resolution receipts due to use of CreateResolverContextXor and interaction with MockTransaction
			auto recipient = model::PublicKeyToAddress(GetTransactionRecipient(genesisBlock, 0), model::NetworkIdentifier::Testnet);
			blockStatementBuilder.addResolution(test::UnresolveXor(recipient), recipient);
			blockStatementBuilder.addResolution(test::UnresolveXor(receiptTokenId), receiptTokenId);

			genesisBlock.ReceiptsHash = model::CalculateMerkleHash(*blockStatementBuilder.build());

			// Sanity:
			EXPECT_NE(Hash256(), genesisBlock.ReceiptsHash);
		}

		test::LocalNodeTestState state(config, "", std::move(cache));

		SetGenesisBlock(state.ref().Storage, genesisBlockSignerPair, config.Network, GenesisBlockModification::None);

		// - create the publisher, observer and loader
		auto pluginManager = CreatePluginManager();
		auto cacheDelta = state.ref().Cache.createDelta();
		GenesisBlockLoader loader(cacheDelta, pluginManager, CreateObserver());

		// Act:
		TTraits::Execute(loader, state.ref(), StateHashVerification::Enabled);

		// Assert:
		TTraits::Assert(cacheDelta, [&genesisBlock](const auto& accountStateCache) {
			// Assert:
			EXPECT_EQ(2u, accountStateCache.size());
			test::AssertBalances(accountStateCache, genesisBlock.SignerPublicKey, {});
			test::AssertBalances(accountStateCache, GetTransactionRecipient(genesisBlock, 0), { MakeHarvestingToken(1234) });
		});
	}

	// endregion

	// region account states

	TRAITS_BASED_TEST(CanLoadValidGenesisBlock_GenesisAccountStateIsCorrectAfterLoading) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });
		const auto& genesisBlock = *genesisBlockSignerPair.pBlock;

		// Act:
		RunLoadGenesisBlockTest<TTraits>(genesisBlockSignerPair, Importance(1234), [&genesisBlock](const auto& accountStateCache) {
			const auto& publicKey = genesisBlock.SignerPublicKey;
			auto address = model::PublicKeyToAddress(publicKey, Network_Identifier);
			const auto& accountState = accountStateCache.find(address).get();

			// Assert:
			EXPECT_EQ(Height(1), accountState.AddressHeight);
			EXPECT_EQ(address, accountState.Address);
			EXPECT_EQ(Height(1), accountState.PublicKeyHeight);
			EXPECT_EQ(publicKey, accountState.PublicKey);
		});
	}

	TRAITS_BASED_TEST(CanLoadValidGenesisBlock_OtherAccountStateIsCorrectAfterLoading) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });
		const auto& genesisBlock = *genesisBlockSignerPair.pBlock;

		// Act:
		RunLoadGenesisBlockTest<TTraits>(genesisBlockSignerPair, Importance(1234), [&genesisBlock](const auto& accountStateCache) {
			const auto& publicKey = GetTransactionRecipient(genesisBlock, 0);
			auto address = model::PublicKeyToAddress(publicKey, Network_Identifier);
			const auto& accountState = accountStateCache.find(address).get();

			// Assert:
			EXPECT_EQ(Height(1), accountState.AddressHeight);
			EXPECT_EQ(address, accountState.Address);
			EXPECT_EQ(Height(1), accountState.PublicKeyHeight);
			EXPECT_EQ(publicKey, accountState.PublicKey);
		});
	}

	namespace {
		template<typename TTraits, typename TAssertAccountStateCache>
		void RunGenesisBlockSpecialSinkAccountTest(
				uint8_t harvestNetworkPercentage,
				const Address& harvestNetworkFeeSinkAddress,
				TAssertAccountStateCache assertAccountStateCache) {
			// Arrange: create a valid genesis block with a single (token) transaction
			auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });
			auto genesisOptions = GenesisOptions{ Importance(1234), Amount() };

			// - create the state
			auto config = CreateDefaultConfiguration(*genesisBlockSignerPair.pBlock, genesisOptions);
			config.HarvestNetworkPercentage = harvestNetworkPercentage;
			config.ForkHeights.TreasuryReissuance = Height(100);
			test::FillWithRandomData(config.HarvestNetworkFeeSinkAddress);
			config.HarvestNetworkFeeSinkAddressPOS = harvestNetworkFeeSinkAddress;
			test::LocalNodeTestState state(config);
			SetGenesisBlock(state.ref().Storage, genesisBlockSignerPair, config.Network, GenesisBlockModification::None);

			// - create the publisher, observer and loader
			auto pluginManager = CreatePluginManager();
			{
				auto cacheDelta = state.ref().Cache.createDelta();
				GenesisBlockLoader loader(
						cacheDelta,
						pluginManager,
						CreateObserverWithHarvestNetworkFees(harvestNetworkPercentage, harvestNetworkFeeSinkAddress));

				// Act:
				TTraits::Execute(loader, state.ref(), genesisOptions.StateHashVerification);

				// Assert:
				TTraits::Assert(cacheDelta, assertAccountStateCache);
			}

			auto cacheView = state.ref().Cache.createView();
			TTraits::Assert(cacheView, assertAccountStateCache);
		}
	}

	TRAITS_BASED_TEST(CanLoadValidGenesisBlock_SpecialSinkAccountIsCreatedWhenEnabled) {
		// Arrange:
		auto sinkAddress = test::GenerateRandomAddress(Network_Identifier);

		// Act:
		RunGenesisBlockSpecialSinkAccountTest<TTraits>(10, sinkAddress, [&sinkAddress](const auto& accountStateCache) {
			auto accountStateIter = accountStateCache.find(sinkAddress);

			// Assert:
			EXPECT_TRUE(!!accountStateIter.tryGet());
			EXPECT_EQ(Height(1), accountStateIter.get().AddressHeight);
			EXPECT_EQ(sinkAddress, accountStateIter.get().Address);
		});
	}

	TRAITS_BASED_TEST(CanLoadValidGenesisBlock_SpecialSinkAccountIsNotCreatedWhenDisabled) {
		// Arrange:
		auto sinkAddress = test::GenerateRandomAddress(Network_Identifier);

		// Act:
		RunGenesisBlockSpecialSinkAccountTest<TTraits>(0, sinkAddress, [&sinkAddress](const auto& accountStateCache) {
			auto accountStateIter = accountStateCache.find(sinkAddress);

			// Assert:
			EXPECT_FALSE(!!accountStateIter.tryGet());
		});
	}

	// endregion

	// region failure - balance

	TRAITS_BASED_TEST(CannotLoadGenesisBlockWithTotalChainCurrencyTooSmall) {
		// Arrange: create a valid genesis block
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234), MakeCurrencyToken(1234) } });

		// Act: pass in an incompatible total chain currency
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, { Importance(1234), Amount(1233) });
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, { Importance(1234), Amount(617) });
	}

	TRAITS_BASED_TEST(CannotLoadGenesisBlockWithTotalChainCurrencyTooLarge) {
		// Arrange: create a valid genesis block
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234), MakeCurrencyToken(1234) } });

		// Act: pass in an incompatible total chain currency
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, { Importance(1234), Amount(1235) });
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, { Importance(1234), Amount(2468) });
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, { Importance(1234), Amount(12340) });
	}

	TRAITS_BASED_TEST(CannotLoadGenesisBlockWithTotalChainImportanceTooSmall) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// Act: pass in an incompatible chain importance
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, Importance(1233));
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, Importance(617));
	}

	TRAITS_BASED_TEST(CannotLoadGenesisBlockWithTotalChainImportanceTooLarge) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// Act: pass in an incompatible chain importance
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, Importance(1235));
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, Importance(2468));
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, Importance(12340));
	}

	TRAITS_BASED_TEST(CannotLoadGenesisBlockWithTotalTokenSupplyExceedingMaxAtomicUnits) {
		// Arrange: create an invalid genesis block, max atomic units is 15'000'000
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234), { TokenId(345), Amount(15'000'001) } } });

		// Act:
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, { Importance(1234), Amount(0) });
	}

	// endregion

	// region failure - info

	TRAITS_BASED_TEST(CannotLoadGenesisBlockWithWrongNetwork) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// - use the wrong network
		genesisBlockSignerPair.pBlock->Network = model::NetworkIdentifier::Zero;

		// Act:
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, Importance(1234));
	}

	TRAITS_BASED_TEST(CannotLoadGenesisBlockWithWrongPublicKey) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// Act: use the wrong public key
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, Importance(1234), GenesisBlockModification::Public_Key);
	}

	TRAITS_BASED_TEST(CannotLoadGenesisBlockWithWrongGenerationHashProof) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// Act: use the wrong generation hash proof
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, Importance(1234), GenesisBlockModification::Generation_Hash_Proof);
	}

	TRAITS_BASED_TEST(CannotLoadGenesisBlockWithWrongGenerationHash) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// Act: use the wrong generation hash
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, Importance(1234), GenesisBlockModification::Generation_Hash);
	}

	// endregion

	// region failure - transactions

	namespace {
		template<typename TTraits>
		void AssertInvalidTransactionType(model::EntityType invalidTransactionType) {
			// Arrange: create a valid genesis block with multiple token transactions but make the second transaction type invalid
			//          so that none of its transfers are processed
			auto genesisBlockSignerPair = CreateGenesisBlock({
				{ MakeHarvestingToken(1234) },
				{ MakeHarvestingToken(123), MakeHarvestingToken(213) },
				{ MakeHarvestingToken(987) }
			});

			(++genesisBlockSignerPair.pBlock->Transactions().begin())->Type = invalidTransactionType;

			// Act:
			AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, Importance(1234));
		}
	}

	TRAITS_BASED_TEST(CannotLoadGenesisBlockContainingUnknownTransactions) {
		// Assert: 0xFFFF is not registered
		AssertInvalidTransactionType<TTraits>(static_cast<model::EntityType>(0xFFFF));
	}

	TRAITS_BASED_TEST(CannotLoadGenesisBlockContainingNonTopLevelTransactions) {
		// Assert: 0xFFFE does not have top-level support
		AssertInvalidTransactionType<TTraits>(static_cast<model::EntityType>(0xFFFE));
	}

	// endregion

	// region failure - fee multiplier

	TRAITS_BASED_TEST(CannotLoadGenesisBlockWithNonzeroFeeMultiplier) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// - use non-zero block fee multiplier
		genesisBlockSignerPair.pBlock->FeeMultiplier = BlockFeeMultiplier(12);

		// Act:
		AssertLoadGenesisBlockFailure<TTraits>(genesisBlockSignerPair, Importance(1234));
	}

	// endregion

	// region failure - state hash

	TRAITS_BASED_TEST(CannotLoadGenesisBlockContainingWrongStateHash_VerifiableStateDisabled) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// - use the wrong state hash
		test::FillWithRandomData(genesisBlockSignerPair.pBlock->StateHash);

		// Act:
		AssertLoadGenesisBlockFailure<TTraits, bitxorcore_runtime_error>(genesisBlockSignerPair, Importance(1234));
	}

	TRAITS_BASED_TEST(CannotLoadGenesisBlockContainingWrongStateHash_VerifiableStateEnabled) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// - use the wrong state hash
		genesisBlockSignerPair.pBlock->StateHash = Hash256();

		// Act:
		auto modification = GenesisBlockModification::None;
		auto options = GenesisBlockVerifyOptions::State;
		AssertLoadGenesisBlockFailure<TTraits, bitxorcore_runtime_error>(genesisBlockSignerPair, Importance(1234), modification, options);
	}

	// endregion

	// region failure - receipts hash

	TRAITS_BASED_TEST(CannotLoadGenesisBlockContainingWrongReceiptsHash_VerifiableReceiptsDisabled) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// - use the wrong receipts hash
		test::FillWithRandomData(genesisBlockSignerPair.pBlock->ReceiptsHash);

		// Act:
		AssertLoadGenesisBlockFailure<TTraits, bitxorcore_runtime_error>(genesisBlockSignerPair, Importance(1234));
	}

	TRAITS_BASED_TEST(CannotLoadGenesisBlockContainingWrongReceiptsHash_VerifiableReceiptsEnabled) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// - use the wrong receipts hash
		genesisBlockSignerPair.pBlock->ReceiptsHash = Hash256();

		// Act:
		auto modification = GenesisBlockModification::None;
		auto options = GenesisBlockVerifyOptions::Receipts;
		AssertLoadGenesisBlockFailure<TTraits, bitxorcore_runtime_error>(genesisBlockSignerPair, Importance(1234), modification, options);
	}

	// endregion

	// region failure - validation

	TRAITS_BASED_TEST(CannotLoadGenesisBlockThatFailsStatelessValidation) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// Act:
		auto modification = GenesisBlockModification::Stateless;
		AssertLoadGenesisBlockFailure<TTraits, bitxorcore_runtime_error>(genesisBlockSignerPair, Importance(1234), modification);
	}

	TRAITS_BASED_TEST(CannotLoadGenesisBlockThatFailsStatefulValidation) {
		// Arrange: create a valid genesis block with a single (token) transaction
		auto genesisBlockSignerPair = CreateGenesisBlock({ { MakeHarvestingToken(1234) } });

		// Act:
		auto modification = GenesisBlockModification::Stateful;
		AssertLoadGenesisBlockFailure<TTraits, bitxorcore_runtime_error>(genesisBlockSignerPair, Importance(1234), modification);
	}

	// endregion
}}
