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

#include "SslClient.h"
#include "tools/ToolNetworkUtils.h"
#include "bitxorcore/api/RemoteChainApi.h"
#include "bitxorcore/thread/FutureUtils.h"

namespace bitxorcore { namespace tools { namespace ssl {

	SslClient::SslClient(
			thread::IoThreadPool& pool,
			crypto::KeyPair&& caKeyPair,
			const std::string& certificateDirectory,
			ScenarioId scenarioId)
			: m_pool(pool)
			, m_pSslContext(std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv13)) {
		GenerateCertificateDirectory(std::move(caKeyPair), certificateDirectory, scenarioId);

		m_pSslContext->set_options(
				boost::asio::ssl::context::no_sslv2
				| boost::asio::ssl::context::no_sslv3
				| boost::asio::ssl::context::no_tlsv1
				| boost::asio::ssl::context::no_tlsv1_1
				| boost::asio::ssl::context::no_tlsv1_2);

		m_pSslContext->use_certificate_chain_file(certificateDirectory + "/node.full.crt.pem");
		m_pSslContext->use_private_key_file(certificateDirectory + "/node.key.pem", boost::asio::ssl::context::pem);

		// pick curve supported by server
		std::array<int, 1> curves{ NID_X25519 };
		SSL_CTX_set1_groups(m_pSslContext->native_handle(), curves.data(), static_cast<long>(curves.size()));
	}

	api::ChainStatistics SslClient::connect(const ionet::NodeEndpoint& nodeEndpoint) {
		auto connectionSettings = net::ConnectionSettings();
		connectionSettings.AllowOutgoingSelfConnections = true;
		connectionSettings.SslOptions.ContextSupplier = [pSslContext = m_pSslContext]() -> boost::asio::ssl::context& {
			return *pSslContext;
		};
		connectionSettings.SslOptions.VerifyCallbackSupplier = []() {
			return [](const auto&) {
				return true;
			};
		};

		model::NodeIdentity nodeIdentity;
		ionet::NodeMetadata nodeMetadata;
		ionet::Node node(nodeIdentity, nodeEndpoint, nodeMetadata);
		auto connectFuture = ConnectToNode(connectionSettings, node, m_pool);
		auto chainStatisticsFuture = thread::compose(std::move(connectFuture), [node](auto&& ioFuture) {
			try {
				auto pIo = ioFuture.get();
				auto pApi = api::CreateRemoteChainApiWithoutRegistry(*pIo);
				return pApi->chainStatistics();
			} catch (...) {
				// suppress
				BITXORCORE_LOG(error) << node << " appears to be offline";
				return thread::make_ready_future(api::ChainStatistics());
			}
		});

		return chainStatisticsFuture.get();
	}
}}}
