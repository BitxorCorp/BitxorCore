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
#include "CertificateDirectoryGenerator.h"
#include "tools/ToolThreadUtils.h"
#include "bitxorcore/api/ChainApi.h"
#include "bitxorcore/ionet/Node.h"
#include <boost/asio/ssl.hpp>

namespace bitxorcore { namespace tools { namespace ssl {

	/// Ssl client.
	class SslClient {
	public:
		/// Creates ssl client around \a pool, \a caKeyPair, \a certificateDirectory and \a scenarioId.
		SslClient(thread::IoThreadPool& pool, crypto::KeyPair&& caKeyPair, const std::string& certificateDirectory, ScenarioId scenarioId);

	public:
		/// Connects to \a nodeEndpoint and retrieves chain statistics.
		api::ChainStatistics connect(const ionet::NodeEndpoint& nodeEndpoint);

	private:
		thread::IoThreadPool& m_pool;
		std::shared_ptr<boost::asio::ssl::context> m_pSslContext;
	};
}}}
