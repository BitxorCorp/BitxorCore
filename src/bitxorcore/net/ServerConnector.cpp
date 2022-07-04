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

#include "ServerConnector.h"
#include "bitxorcore/ionet/Node.h"
#include "bitxorcore/ionet/PacketSocket.h"
#include "bitxorcore/thread/IoThreadPool.h"
#include "bitxorcore/thread/TimedCallback.h"
#include "bitxorcore/utils/Logging.h"
#include "bitxorcore/utils/WeakContainer.h"

namespace bitxorcore { namespace net {

	namespace {
		using PacketSocketPointer = std::shared_ptr<ionet::PacketSocket>;

		class DefaultServerConnector
				: public ServerConnector
				, public std::enable_shared_from_this<DefaultServerConnector> {
		public:
			DefaultServerConnector(
					thread::IoThreadPool& pool,
					const Key& serverPublicKey,
					const ConnectionSettings& settings,
					const std::string& name)
					: m_ioContext(pool.ioContext())
					, m_serverPublicKey(serverPublicKey)
					, m_settings(settings)
					, m_name(name)
					, m_tag(m_name.empty() ? std::string() : " (" + m_name + ")")
					, m_sockets([](auto& socket) { socket.close(); })
			{}

		public:
			size_t numActiveConnections() const override {
				return m_sockets.size();
			}

			const std::string& name() const override {
				return m_name;
			}

		public:
			void connect(const ionet::Node& node, const ConnectCallback& callback) override {
				const auto& identityKey = node.identity().PublicKey;
				if (!m_settings.AllowOutgoingSelfConnections && m_serverPublicKey == identityKey) {
					BITXORCORE_LOG(warning) << "self connection detected and aborted" << m_tag;
					return callback(PeerConnectCode::Self_Connection_Error, ionet::PacketSocketInfo());
				}

				auto pRequest = thread::MakeTimedCallback(m_ioContext, callback, PeerConnectCode::Timed_Out, ionet::PacketSocketInfo());
				pRequest->setTimeout(m_settings.Timeout);

				auto socketOptions = m_settings.toSocketOptions();
				const auto& endpoint = node.endpoint();
				auto cancel = ionet::Connect(m_ioContext, socketOptions, endpoint, [pThis = shared_from_this(), identityKey, pRequest](
						auto result,
						const auto& connectedSocketInfo) {
					if (ionet::ConnectResult::Connected != result)
						return pRequest->callback(PeerConnectCode::Socket_Error, ionet::PacketSocketInfo());

					pThis->verify(identityKey, connectedSocketInfo, pRequest);
				});

				pRequest->setTimeoutHandler([pThis = shared_from_this(), cancel]() {
					cancel();
					BITXORCORE_LOG(debug) << "connect failed due to timeout" << pThis->m_tag;
				});
			}

		private:
			template<typename TRequest>
			void verify(
					const Key& expectedIdentityKey,
					const ionet::PacketSocketInfo& connectedSocketInfo,
					const std::shared_ptr<TRequest>& pRequest) {
				if (expectedIdentityKey != connectedSocketInfo.publicKey()) {
					BITXORCORE_LOG(warning)
							<< "aborting connection with identity mismatch (expected " << expectedIdentityKey
							<< ", actual " << connectedSocketInfo.publicKey() << ")" << m_tag;
					return pRequest->callback(PeerConnectCode::Verify_Error, ionet::PacketSocketInfo());
				}

				m_sockets.insert(connectedSocketInfo.socket());
				pRequest->callback(PeerConnectCode::Accepted, connectedSocketInfo);
			}

		public:
			void shutdown() override {
				BITXORCORE_LOG(info) << "closing all connections in ServerConnector" << m_tag;
				m_sockets.clear();
			}

		private:
			boost::asio::io_context& m_ioContext;
			Key m_serverPublicKey;
			ConnectionSettings m_settings;

			std::string m_name;
			std::string m_tag;

			utils::WeakContainer<ionet::PacketSocket> m_sockets;
		};
	}

	std::shared_ptr<ServerConnector> CreateServerConnector(
			thread::IoThreadPool& pool,
			const Key& serverPublicKey,
			const ConnectionSettings& settings,
			const char* name) {
		return std::make_shared<DefaultServerConnector>(pool, serverPublicKey, settings, name ? std::string(name) : std::string());
	}
}}
