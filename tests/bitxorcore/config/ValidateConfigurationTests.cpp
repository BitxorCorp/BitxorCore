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

#include "bitxorcore/config/ValidateConfiguration.h"
#include "bitxorcore/config/BitxorCoreConfiguration.h"
#include "bitxorcore/utils/ConfigurationBag.h"
#include "tests/test/net/NodeTestUtils.h"
#include "tests/test/other/MutableBitxorCoreConfiguration.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace config {

#define TEST_CLASS ValidateConfigurationTests

	namespace {
		auto CreateMutableBitxorCoreConfiguration() {
			test::MutableBitxorCoreConfiguration config;

			auto& blockchainConfig = config.Blockchain;
			blockchainConfig.ImportanceGrouping = 1;
			blockchainConfig.MaxTokenAtomicUnits = Amount(1000);

			auto& inflationConfig = config.Inflation;
			inflationConfig.InflationCalculator.add(Height(1), Amount(1));
			inflationConfig.InflationCalculator.add(Height(100), Amount());
			return config;
		}
	}

	// region importance grouping validation

	namespace {
		auto CreateBitxorCoreConfigurationWithImportanceGrouping(uint32_t importanceGrouping, uint32_t maxRollbackBlocks) {
			auto mutableConfig = CreateMutableBitxorCoreConfiguration();
			mutableConfig.Blockchain.ImportanceGrouping = importanceGrouping;
			mutableConfig.Blockchain.MaxRollbackBlocks = maxRollbackBlocks;
			return mutableConfig.ToConst();
		}
	}

	TEST(TEST_CLASS, ImportanceGroupingIsValidatedAgainstMaxRollbackBlocks) {
		// Arrange:
		auto assertNoThrow = [](uint32_t importanceGrouping, uint32_t maxRollbackBlocks) {
			auto config = CreateBitxorCoreConfigurationWithImportanceGrouping(importanceGrouping, maxRollbackBlocks);
			EXPECT_NO_THROW(ValidateConfiguration(config)) << "IG " << importanceGrouping << ", MRB " << maxRollbackBlocks;
		};

		auto assertThrow = [](uint32_t importanceGrouping, uint32_t maxRollbackBlocks) {
			auto config = CreateBitxorCoreConfigurationWithImportanceGrouping(importanceGrouping, maxRollbackBlocks);
			EXPECT_THROW(ValidateConfiguration(config), utils::property_malformed_error)
					<< "IG " << importanceGrouping << ", MRB " << maxRollbackBlocks;
		};

		// Act + Assert:
		// - no exceptions
		assertNoThrow(181, 360); // 2 * IG > MRB
		assertNoThrow(400, 360); // IG > MRB

		// - exceptions
		assertThrow(0, 360); // 0 IG
		assertThrow(180, 360); // 2 * IG == MRB
		assertThrow(179, 360); // 2 * IG < MRB
	}

	// endregion

	// region harvest beneficiary percentage validation

	namespace {
		auto CreateBitxorCoreConfigurationWithHarvestPercentages(uint8_t harvestBeneficiaryPercentage, uint8_t harvestNetworkPercentage) {
			auto mutableConfig = CreateMutableBitxorCoreConfiguration();
			mutableConfig.Blockchain.HarvestBeneficiaryPercentage = harvestBeneficiaryPercentage;
			mutableConfig.Blockchain.HarvestNetworkPercentage = harvestNetworkPercentage;
			return mutableConfig.ToConst();
		}
	}

	TEST(TEST_CLASS, HarvestPercentagesAreValidated) {
		// Arrange:
		auto assertNoThrow = [](uint8_t harvestBeneficiaryPercentage, uint8_t harvestNetworkPercentage) {
			auto config = CreateBitxorCoreConfigurationWithHarvestPercentages(harvestBeneficiaryPercentage, harvestNetworkPercentage);
			EXPECT_NO_THROW(ValidateConfiguration(config))
					<< "HBP " << static_cast<uint16_t>(harvestBeneficiaryPercentage)
					<< " HNP " << static_cast<uint16_t>(harvestNetworkPercentage);
		};

		auto assertThrow = [](uint8_t harvestBeneficiaryPercentage, uint8_t harvestNetworkPercentage) {
			auto config = CreateBitxorCoreConfigurationWithHarvestPercentages(harvestBeneficiaryPercentage, harvestNetworkPercentage);
			EXPECT_THROW(ValidateConfiguration(config), utils::property_malformed_error)
					<< "HBP " << static_cast<uint16_t>(harvestBeneficiaryPercentage)
					<< " HNP " << static_cast<uint16_t>(harvestNetworkPercentage);
		};

		auto dispatch = [](auto check, uint8_t percentage) {
			check(percentage, 0);
			check(0, percentage);
			check(static_cast<uint8_t>(percentage / 2), static_cast<uint8_t>(percentage / 2 + (0 == percentage % 2 ? 0 : 1)));
		};

		// Act + Assert:
		// - no exceptions
		for (auto percentage : std::initializer_list<uint8_t>{ 0, 1, 57, 99, 100 })
			dispatch(assertNoThrow, percentage);

		// - exceptions
		for (auto percentage : std::initializer_list<uint8_t>{ 101, 156, 255 })
			dispatch(assertThrow, percentage);
	}

	// endregion

	// region importance activity percentage validation

	namespace {
		auto CreateBitxorCoreConfigurationWithImportanceActivityPercentage(uint8_t importanceActivityPercentage) {
			auto mutableConfig = CreateMutableBitxorCoreConfiguration();
			mutableConfig.Blockchain.ImportanceActivityPercentage = importanceActivityPercentage;
			return mutableConfig.ToConst();
		}
	}

	TEST(TEST_CLASS, ImportanceActivityPercentageIsValidated) {
		// Arrange:
		auto assertNoThrow = [](uint8_t importanceActivityPercentage) {
			auto config = CreateBitxorCoreConfigurationWithImportanceActivityPercentage(importanceActivityPercentage);
			EXPECT_NO_THROW(ValidateConfiguration(config)) << "IAP " << importanceActivityPercentage;
		};

		auto assertThrow = [](uint8_t importanceActivityPercentage) {
			auto config = CreateBitxorCoreConfigurationWithImportanceActivityPercentage(importanceActivityPercentage);
			EXPECT_THROW(ValidateConfiguration(config), utils::property_malformed_error) << "IAP " << importanceActivityPercentage;
		};

		// Act + Assert:
		// - no exceptions
		for (auto percentage : std::initializer_list<uint8_t>{ 0, 1, 57, 99 })
			assertNoThrow(percentage);

		// - exceptions
		for (auto percentage : std::initializer_list<uint8_t>{ 100, 101, 156, 255 })
			assertThrow(percentage);
	}

	// endregion

	// region voting set grouping validation

	namespace {
		auto CreateBitxorCoreConfigurationWithGroupings(uint32_t importanceGrouping, uint32_t votingSetGrouping) {
			auto mutableConfig = CreateMutableBitxorCoreConfiguration();
			mutableConfig.Blockchain.ImportanceGrouping = importanceGrouping;
			mutableConfig.Blockchain.VotingSetGrouping = votingSetGrouping;
			return mutableConfig.ToConst();
		}
	}

	TEST(TEST_CLASS, VotingSetGroupingMustBeMultipleOfImportanceGrouping) {
		// Arrange:
		auto assertNoThrow = [](uint32_t importanceGrouping, uint32_t votingSetGrouping) {
			auto config = CreateBitxorCoreConfigurationWithGroupings(importanceGrouping, votingSetGrouping);
			EXPECT_NO_THROW(ValidateConfiguration(config)) << "IG " << importanceGrouping << ", VSG " << votingSetGrouping;
		};

		auto assertThrow = [](uint32_t importanceGrouping, uint32_t votingSetGrouping) {
			auto config = CreateBitxorCoreConfigurationWithGroupings(importanceGrouping, votingSetGrouping);
			EXPECT_THROW(ValidateConfiguration(config), utils::property_malformed_error)
					<< "IG " << importanceGrouping << ", VSG " << votingSetGrouping;
		};

		// Act + Assert:
		// - no exceptions
		assertNoThrow(200, 200); // IG == VSG
		assertNoThrow(200, 800); // IG < VSG
		assertNoThrow(1, 17);

		// - exceptions
		assertThrow(800, 200); // IG > VSG
		assertThrow(200, 700); // VSG not multiple of IG
		assertThrow(200, 799);
		assertThrow(200, 801);
		assertThrow(200, 900);
	}

	// endregion

	// region total chain currency validation

	namespace {
		struct InflationEntry {
			bitxorcore::Height Height;
			bitxorcore::Amount Amount;
		};

		namespace {
			auto CreateBitxorCoreConfiguration(uint64_t initialCurrencyAtomicUnits, const std::vector<InflationEntry>& inflationEntries) {
				auto mutableConfig = CreateMutableBitxorCoreConfiguration();
				mutableConfig.Blockchain.InitialCurrencyAtomicUnits = Amount(initialCurrencyAtomicUnits);

				model::InflationCalculator calculator;
				for (const auto& inflationEntry : inflationEntries)
					calculator.add(inflationEntry.Height, inflationEntry.Amount);

				// Sanity:
				EXPECT_EQ(inflationEntries.size(), calculator.size());

				mutableConfig.Inflation.InflationCalculator = std::move(calculator);
				return mutableConfig.ToConst();
			}
		}
	}

	TEST(TEST_CLASS, SuccessfulValidationWhenConfigIsValid) {
		// Arrange: MaxTokenAtomicUnits is 1000
		auto config = CreateBitxorCoreConfiguration(123, { { Height(1), Amount(1) }, { Height(234), Amount(0) } });

		// Act:
		EXPECT_NO_THROW(ValidateConfiguration(config));
	}

	TEST(TEST_CLASS, InitialTotalChainAmountMustNotExceedMaxTokenAtomicUnits) {
		// Act + Assert: MaxTokenAtomicUnits is 1000
		auto inflationEntries = std::vector<InflationEntry>();
		EXPECT_THROW(ValidateConfiguration(CreateBitxorCoreConfiguration(1001, inflationEntries)), utils::property_malformed_error);
		EXPECT_THROW(ValidateConfiguration(CreateBitxorCoreConfiguration(5000, inflationEntries)), utils::property_malformed_error);
		EXPECT_THROW(ValidateConfiguration(CreateBitxorCoreConfiguration(1234567890, inflationEntries)), utils::property_malformed_error);
	}

	TEST(TEST_CLASS, InitialTotalCurrenyPlusInflationMustNotExceedMaxTokenAtomicUnits) {
		// Arrange: MaxTokenAtomicUnits is 1000
		auto config = CreateBitxorCoreConfiguration(600, { { Height(1), Amount(1) }, { Height(500), Amount(0) } });

		// Act:
		EXPECT_THROW(ValidateConfiguration(config), utils::property_malformed_error);
	}

	TEST(TEST_CLASS, InitialTotalCurrenyPlusInflationMustNotOverflow) {
		// Arrange:
		auto numBlocks = std::numeric_limits<uint64_t>::max();
		auto config = CreateBitxorCoreConfiguration(2, { { Height(1), Amount(1) }, { Height(numBlocks), Amount(0) } });

		// Act:
		EXPECT_THROW(ValidateConfiguration(config), utils::property_malformed_error);
	}

	TEST(TEST_CLASS, TerminalInflationMustBeZero) {
		// Arrange:
		auto config = CreateBitxorCoreConfiguration(0, { { Height(3), Amount(5) }, { Height(10), Amount(2) } });

		// Act:
		EXPECT_THROW(ValidateConfiguration(config), utils::property_malformed_error);
	}

	// endregion

	// region cache database max write batch size validation

	namespace {
		auto CreateBitxorCoreConfigurationWithMaxWriteBatchSize(utils::FileSize maxWriteBatchSize) {
			auto mutableConfig = CreateMutableBitxorCoreConfiguration();
			mutableConfig.Node.CacheDatabase.MaxWriteBatchSize = maxWriteBatchSize;
			return mutableConfig.ToConst();
		}
	}

	TEST(TEST_CLASS, MaxWriteBatchSizeMustBeUnsetOrAtLeastMinValue) {
		// Arrange:
		auto assertNoThrow = [](uint32_t maxWriteBatchSizeKb) {
			auto config = CreateBitxorCoreConfigurationWithMaxWriteBatchSize(utils::FileSize::FromKilobytes(maxWriteBatchSizeKb));
			EXPECT_NO_THROW(ValidateConfiguration(config)) << "size " << maxWriteBatchSizeKb;
		};

		auto assertThrow = [](uint32_t maxWriteBatchSizeKb) {
			auto config = CreateBitxorCoreConfigurationWithMaxWriteBatchSize(utils::FileSize::FromKilobytes(maxWriteBatchSizeKb));
			EXPECT_THROW(ValidateConfiguration(config), utils::property_malformed_error) << "size " << maxWriteBatchSizeKb;
		};

		// Act + Assert:
		// - no exceptions
		assertNoThrow(0); // unset
		assertNoThrow(100); // min value
		assertNoThrow(101);
		assertNoThrow(9999);

		// - exceptions
		assertThrow(1);
		assertThrow(50);
		assertThrow(99);
	}

	// endregion
}}
