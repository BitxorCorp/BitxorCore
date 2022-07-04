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

#include "ChainedSocketReader.h"
#include "bitxorcore/ionet/BufferedPacketIo.h"
#include "bitxorcore/ionet/PacketSocket.h"
#include "bitxorcore/ionet/SocketReader.h"
#include "bitxorcore/utils/Logging.h"

namespace bitxorcore { namespace net {

	namespace {
		constexpr utils::LogLevel MapToLogLevel(ionet::SocketOperationCode code) {
			return ionet::SocketOperationCode::Closed == code ? utils::LogLevel::debug : utils::LogLevel::warning;
		}

		class DefaultChainedSocketReader
				: public ChainedSocketReader
				, public std::enable_shared_from_this<DefaultChainedSocketReader> {
		public:
			DefaultChainedSocketReader(
					const std::shared_ptr<ionet::PacketSocket>& pPacketSocket,
					const ionet::ServerPacketHandlers& serverHandlers,
					const model::NodeIdentity& identity,
					const ChainedSocketReader::CompletionHandler& completionHandler)
					: m_pPacketSocket(pPacketSocket)
					, m_identity(identity)
					, m_completionHandler(completionHandler)
					, m_pReader(CreateSocketReader(m_pPacketSocket, m_pPacketSocket->buffered(), serverHandlers, identity))
			{}

		public:
			void start() override {
				m_pReader->read([pThis = shared_from_this()](auto code) { pThis->read(code); });
			}

			void stop() override {
				m_pPacketSocket->close();
			}

		private:
			void read(ionet::SocketOperationCode code) {
				switch (code) {
				case ionet::SocketOperationCode::Success:
					return;

				case ionet::SocketOperationCode::Insufficient_Data:
					// Insufficient_Data signals the definitive end of a (successful) batch operation,
					// whereas Success can be returned multiple times
					return start();

				default:
					BITXORCORE_LOG_LEVEL(MapToLogLevel(code)) << m_identity << " read completed with error: " << code;
					return m_completionHandler(code);
				}
			}

		private:
			std::shared_ptr<ionet::PacketSocket> m_pPacketSocket;
			model::NodeIdentity m_identity;
			ChainedSocketReader::CompletionHandler m_completionHandler;
			std::unique_ptr<ionet::SocketReader> m_pReader;
		};
	}

	std::shared_ptr<ChainedSocketReader> CreateChainedSocketReader(
			const std::shared_ptr<ionet::PacketSocket>& pPacketSocket,
			const ionet::ServerPacketHandlers& serverHandlers,
			const model::NodeIdentity& identity) {
		return CreateChainedSocketReader(pPacketSocket, serverHandlers, identity, [](auto) {});
	}

	std::shared_ptr<ChainedSocketReader> CreateChainedSocketReader(
			const std::shared_ptr<ionet::PacketSocket>& pPacketSocket,
			const ionet::ServerPacketHandlers& serverHandlers,
			const model::NodeIdentity& identity,
			const ChainedSocketReader::CompletionHandler& completionHandler) {
		return std::make_shared<DefaultChainedSocketReader>(pPacketSocket, serverHandlers, identity, completionHandler);
	}
}}
