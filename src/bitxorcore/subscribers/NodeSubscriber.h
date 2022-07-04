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
#include "bitxorcore/ionet/NodeInfo.h"
#include "bitxorcore/validators/ValidationResult.h"
#include "bitxorcore/plugins.h"
#include "bitxorcore/types.h"

namespace bitxorcore {
	namespace ionet { class Node; }
	namespace model { struct NodeIdentity; }
}

namespace bitxorcore { namespace subscribers {

	/// Node subscriber.
	class PLUGIN_API_DEPENDENCY NodeSubscriber {
	public:
		virtual ~NodeSubscriber() = default;

	public:
		/// Indicates a new \a node was found.
		virtual void notifyNode(const ionet::Node& node) = 0;

		/// Indicates a new incoming connection for node with \a identity connected to \a serviceId.
		virtual bool notifyIncomingNode(const model::NodeIdentity& identity, ionet::ServiceIdentifier serviceId) = 0;

		/// Indicates node with \a identity is banned due to \a reason.
		virtual void notifyBan(const model::NodeIdentity& identity, uint32_t reason) = 0;
	};
}}
