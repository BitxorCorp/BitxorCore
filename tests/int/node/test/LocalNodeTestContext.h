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

#pragma once
#include "ConfigurationTestUtils.h"
#include "LocalNodeGenesisHashTestUtils.h"
#include "LocalNodeTestUtils.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/config/BitxorCoreKeys.h"
#include "bitxorcore/crypto/OpensslKeyUtils.h"
#include "bitxorcore/extensions/ProcessBootstrapper.h"
#include "bitxorcore/extensions/ServiceState.h"
#include "bitxorcore/local/server/LocalNode.h"
#include "tests/test/core/StorageTestUtils.h"
#include "tests/test/crypto/CertificateTestUtils.h"
#include "tests/test/genesis/GenesisCompatibleConfiguration.h"
#include "tests/test/net/CertificateLocator.h"
#include "tests/test/nodeps/Filesystem.h"

namespace bitxorcore { namespace test {

	/// Traits for a peer node.
	struct LocalNodePeerTraits {
		static constexpr auto CountersToLocalNodeStats = test::CountersToPeerLocalNodeStats;
		static constexpr auto AddPluginExtensions = test::AddPeerPluginExtensions;
	};

	/// Traits for an api node.
	struct LocalNodeApiTraits {
		static constexpr auto CountersToLocalNodeStats = test::CountersToBasicLocalNodeStats;
		static constexpr auto AddPluginExtensions = test::AddApiPluginExtensions;
	};

	/// Common test context for local node tests.
	template<typename TTraits>
	class LocalNodeTestContext {
	public:
		/// Creates a context around \a nodeFlag.
		explicit LocalNodeTestContext(NodeFlag nodeFlag) : LocalNodeTestContext(nodeFlag, {})
		{}

		/// Creates a context around \a nodeFlag with custom \a nodes.
		LocalNodeTestContext(NodeFlag nodeFlag, const std::vector<ionet::Node>& nodes)
				: LocalNodeTestContext(nodeFlag, nodes, [](const auto&) {}, "")
		{}

		/// Creates a context around \a nodeFlag with custom \a nodes, config transform (\a configTransform)
		/// and temp directory postfix (\a tempDirPostfix).
		LocalNodeTestContext(
				NodeFlag nodeFlag,
				const std::vector<ionet::Node>& nodes,
				const consumer<config::BitxorCoreConfiguration&>& configTransform,
				const std::string& tempDirPostfix)
				: m_nodeFlag(nodeFlag)
				, m_nodes(nodes)
				, m_configTransform(configTransform)
				, m_tempDir("lntc" + tempDirPostfix)
				, m_partnerTempDir("lntc_partner" + tempDirPostfix) {
			m_serverKeys = initializeDataDirectory(m_tempDir.name());
			BITXORCORE_LOG(debug) << "creating server with public key " << m_serverKeys.caPublicKey();

			if (HasFlag(NodeFlag::With_Partner, nodeFlag)) {
				m_partnerServerKeys = initializeDataDirectory(m_partnerTempDir.name());
				BITXORCORE_LOG(debug) << "creating partner with public key " << m_partnerServerKeys.caPublicKey();

				// need to call configTransform first so that partner node loads all required transaction plugins
				auto config = CreatePrototypicalBitxorCoreConfiguration(m_partnerTempDir.name());
				m_configTransform(config);
				m_pLocalPartnerNode = BootLocalPartnerNode(std::move(config), m_partnerServerKeys, nodeFlag);

				m_nodes.push_back(CreateLocalPartnerNode(m_partnerServerKeys.caPublicKey()));
			}

			if (!HasFlag(NodeFlag::Require_Explicit_Boot, nodeFlag))
				boot();
		}

	private:
		config::BitxorCoreKeys initializeDataDirectory(
				const std::string& directory,
				GenesisStorageDisposition genesisStorageDisposition = GenesisStorageDisposition::Seed) const {
			auto genesisDirectory = directory;
			if (GenesisStorageDisposition::Seed == genesisStorageDisposition && !HasFlag(NodeFlag::Bypass_Seed, m_nodeFlag)) {
				genesisDirectory += "/seed";
				PrepareSeedStorage(genesisDirectory);
			} else {
				PrepareStorage(genesisDirectory);
			}

			PrepareConfiguration(directory, m_nodeFlag);
			config::BitxorCoreDataDirectoryPreparer::Prepare(directory);

			if (HasFlag(NodeFlag::Verify_Receipts, m_nodeFlag))
				SetGenesisReceiptsHash(genesisDirectory, genesisStorageDisposition);

			if (HasFlag(NodeFlag::Verify_State, m_nodeFlag)) {
				auto config = CreatePrototypicalBitxorCoreConfiguration(directory);
				prepareBitxorCoreConfiguration(config);

				SetGenesisStateHash(genesisDirectory, genesisStorageDisposition, config);
			}

			return config::BitxorCoreKeys(findCertificateSubdirectory(directory));
		}

	public:
		/// Gets the data directory.
		std::string dataDirectory() const {
			return m_tempDir.name();
		}

		/// Gets the resources directory.
		std::string resourcesDirectory() const {
			return m_tempDir.name() + "/resources";
		}

		/// Gets the public key of the (primary) local node.
		const Key& publicKey() const {
			return m_serverKeys.caPublicKey();
		}

		/// Gets the public key of the (partner) local node.
		const Key& partnerPublicKey() const {
			return m_partnerServerKeys.caPublicKey();
		}

		/// Gets the (primary) local node.
		local::LocalNode& localNode() const {
			return *m_pLocalNode;
		}

		/// Gets the node stats.
		auto stats() const {
			return TTraits::CountersToLocalNodeStats(m_pLocalNode->counters());
		}

		/// Loads saved height from persisted state.
		Height loadSavedStateChainHeight() const {
			auto path = std::filesystem::path(m_tempDir.name()) / "state" / "supplemental.dat";
			io::RawFile file(path.generic_string(), io::OpenMode::Read_Only);
			return io::Read<Height>(file);
		}

	public:
		/// Creates a copy of the default bitxorcore configuration.
		config::BitxorCoreConfiguration createConfig() const {
			auto config = CreatePrototypicalBitxorCoreConfiguration(m_tempDir.name());
			m_configTransform(config);
			return config;
		}

		/// Prepares a fresh data \a directory and returns corresponding configuration.
		config::BitxorCoreConfiguration prepareFreshDataDirectory(const std::string& directory) const {
			initializeDataDirectory(directory, GenesisStorageDisposition::Data);

			auto config = CreatePrototypicalBitxorCoreConfiguration(directory);
			prepareBitxorCoreConfiguration(config);
			return config;
		}

		/// Regenerates (primary) certificates with specified \a caKeyPair.
		/// \note This is required for happy blockchain tests, which require high-balance genesis accounts.
		void regenerateCertificates(const crypto::KeyPair& caKeyPair) {
			auto certificateDirectory = findCertificateSubdirectory(dataDirectory());
			GenerateCertificateDirectory(certificateDirectory, PemCertificate(caKeyPair, GenerateKeyPair()));

			m_serverKeys = config::BitxorCoreKeys(certificateDirectory);
		}

	public:
		/// Boots a new local node.
		/// \note This overload is intended to be called only for nodes that require explicit booting.
		void boot() {
			if (m_pLocalNode)
				BITXORCORE_THROW_RUNTIME_ERROR("cannot boot local node multiple times via same test context");

			m_pLocalNode = boot(createConfig());
		}

		/// Boots a new local node around \a config.
		std::unique_ptr<local::LocalNode> boot(config::BitxorCoreConfiguration&& config) {
			return boot(std::move(config), [](const auto&) {});
		}

		/// Boots a new local node allowing additional customization via \a configure.
		std::unique_ptr<local::LocalNode> boot(const consumer<extensions::ProcessBootstrapper&>& configure) {
			return boot(createConfig(), configure);
		}

		/// Resets this context and shuts down the local node.
		void reset() {
			if (m_pLocalPartnerNode)
				BITXORCORE_THROW_INVALID_ARGUMENT("local node's partner node is expected to be uninitialized");

			m_pLocalNode->shutdown();
			m_pLocalNode.reset();
		}

	private:
		std::unique_ptr<local::LocalNode> boot(
				config::BitxorCoreConfiguration&& config,
				const consumer<extensions::ProcessBootstrapper&>& configure) {
			prepareBitxorCoreConfiguration(config);

			auto pBootstrapper = std::make_unique<extensions::ProcessBootstrapper>(
					std::move(config),
					resourcesDirectory(),
					extensions::ProcessDisposition::Production,
					"LocalNodeTests");
			pBootstrapper->addStaticNodes(m_nodes);

			auto& extensionManager = pBootstrapper->extensionManager();
			extensionManager.addServiceRegistrar(std::make_unique<CapturingServiceRegistrar>(m_capturedServiceState));
			pBootstrapper->loadExtensions();

			configure(*pBootstrapper);

			return local::CreateLocalNode(m_serverKeys, std::move(pBootstrapper));
		}

	private:
		void prepareBitxorCoreConfiguration(config::BitxorCoreConfiguration& config) const {
			PrepareBitxorCoreConfiguration(config, TTraits::AddPluginExtensions, m_nodeFlag);
			m_configTransform(config);
		}

		std::string findCertificateSubdirectory(const std::string& directory) const {
			auto config = CreatePrototypicalBitxorCoreConfiguration(directory);
			m_configTransform(config);
			return config.User.CertificateDirectory;
		}

	public:
		/// Waits for \a value active readers.
		void waitForNumActiveReaders(size_t value) const {
			WAIT_FOR_VALUE_EXPR(value, stats().NumActiveReaders);
		}

		/// Waits for \a value active writers.
		void waitForNumActiveWriters(size_t value) const {
			WAIT_FOR_VALUE_EXPR(value, stats().NumActiveWriters);
		}

		/// Waits for \a value scheduled tasks.
		void waitForNumScheduledTasks(size_t value) const {
			WAIT_FOR_VALUE_EXPR(value, stats().NumScheduledTasks);
		}

	public:
		/// Gets the captured node subscriber.
		subscribers::NodeSubscriber& nodeSubscriber() const {
			return *m_capturedServiceState.pNodeSubscriber;
		}

	private:
		struct CapturedServiceState {
			subscribers::NodeSubscriber* pNodeSubscriber;
		};

		// service registrar for capturing ServiceState values
		// \note only node subscriber is currently captured
		class CapturingServiceRegistrar : public extensions::ServiceRegistrar {
		public:
			explicit CapturingServiceRegistrar(CapturedServiceState& state) : m_state(state)
			{}

		public:
			extensions::ServiceRegistrarInfo info() const override {
				return { "Capturing", extensions::ServiceRegistrarPhase::Initial };
			}

			void registerServiceCounters(extensions::ServiceLocator&) override {
				// do nothing
			}

			void registerServices(extensions::ServiceLocator&, extensions::ServiceState& state) override {
				m_state.pNodeSubscriber = &state.nodeSubscriber();
			}

		private:
			CapturedServiceState& m_state;
		};

	private:
		NodeFlag m_nodeFlag;
		std::vector<ionet::Node> m_nodes;
		consumer<config::BitxorCoreConfiguration&> m_configTransform;
		config::BitxorCoreKeys m_serverKeys;
		config::BitxorCoreKeys m_partnerServerKeys;
		TempDirectoryGuard m_tempDir;
		TempDirectoryGuard m_partnerTempDir;

		std::unique_ptr<local::LocalNode> m_pLocalPartnerNode;
		std::unique_ptr<local::LocalNode> m_pLocalNode;
		mutable CapturedServiceState m_capturedServiceState;
	};
}}
