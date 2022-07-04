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
#include "ApiTypes.h"
#include "bitxorcore/ionet/PacketIo.h"
#include "bitxorcore/thread/Future.h"

namespace bitxorcore { namespace api {

	/// Dispatches requests to a remote node
	class RemoteRequestDispatcher {
	public:
		/// Creates a remote request dispatcher around \a io.
		explicit RemoteRequestDispatcher(ionet::PacketIo& io) : m_io(io)
		{}

	public:
		/// Dispatches \a args to the underlying io.
		template<typename TFuncTraits, typename... TArgs>
		thread::future<typename TFuncTraits::ResultType> dispatch(const TFuncTraits& traits, TArgs&&... args) {
			auto pPromise = std::make_shared<thread::promise<typename TFuncTraits::ResultType>>();
			auto future = pPromise->get_future();
			auto packetPayload = TFuncTraits::CreateRequestPacketPayload(std::forward<TArgs>(args)...);
			send(traits, packetPayload, [pPromise](auto result, auto&& value) {
				if (RemoteChainResult::Success == result) {
					pPromise->set_value(std::move(value));
					return;
				}

				auto message = GetErrorMessage(result);
				BITXORCORE_LOG(error) << message << " for " << TFuncTraits::Friendly_Name << " request";
				pPromise->set_exception(std::make_exception_ptr(bitxorcore_api_error(message)));
			});

			return future;
		}

	private:
		template<typename TFuncTraits, typename TCallback>
		void send(const TFuncTraits& traits, const ionet::PacketPayload& packetPayload, const TCallback& callback) {
			using ResultType = typename TFuncTraits::ResultType;
			m_io.write(packetPayload, [traits, callback, &io = m_io](auto code) {
				if (ionet::SocketOperationCode::Success != code)
					return callback(RemoteChainResult::Write_Error, ResultType());

				io.read([traits, callback](auto readCode, const auto* pResponsePacket) {
					if (ionet::SocketOperationCode::Success != readCode)
						return callback(RemoteChainResult::Read_Error, ResultType());

					if (TFuncTraits::Packet_Type != pResponsePacket->Type) {
						BITXORCORE_LOG(warning)
								<< "received packet of type " << pResponsePacket->Type
								<< " but expected type " << TFuncTraits::Packet_Type;
						return callback(RemoteChainResult::Malformed_Packet, ResultType());
					}

					ResultType result;
					if (!traits.tryParseResult(*pResponsePacket, result)) {
						BITXORCORE_LOG(warning)
								<< "unable to parse " << pResponsePacket->Type
								<< " packet (size = " << pResponsePacket->Size << ")";
						return callback(RemoteChainResult::Malformed_Packet, ResultType());
					}

					return callback(RemoteChainResult::Success, std::move(result));
				});
			});
		}

	private:
		enum class RemoteChainResult {
			Success,
			Write_Error,
			Read_Error,
			Malformed_Packet
		};

		static constexpr const char* GetErrorMessage(RemoteChainResult result) {
			switch (result) {
			case RemoteChainResult::Write_Error:
				return "write to remote node failed";
			case RemoteChainResult::Read_Error:
				return "read from remote node failed";
			default:
				return "remote node returned malformed packet";
			}
		}

	private:
		ionet::PacketIo& m_io;
	};
}}
