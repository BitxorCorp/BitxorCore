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
#include "MockTransaction.h"
#include "bitxorcore/model/TransactionPlugin.h"

namespace bitxorcore { namespace mocks {

	/// Unsupported mock transaction plugin.
	class MockTransactionPluginUnsupported : public model::TransactionPlugin {
	public:
		model::EntityType type() const override {
			return mocks::MockTransaction::Entity_Type;
		}

		model::TransactionAttributes attributes() const override {
			BITXORCORE_THROW_RUNTIME_ERROR("attributes - not implemented in mock");
		}

		bool isSizeValid(const model::Transaction&) const override {
			BITXORCORE_THROW_RUNTIME_ERROR("isSizeValid - not implemented in mock");
		}

		void publish(
				const model::WeakEntityInfoT<model::Transaction>&,
				const model::PublishContext&,
				model::NotificationSubscriber&) const override {
			BITXORCORE_THROW_RUNTIME_ERROR("publish - not implemented in mock");
		}

		uint32_t embeddedCount(const model::Transaction&) const override {
			BITXORCORE_THROW_RUNTIME_ERROR("embeddedCount - not implemented in mock");
		}

		RawBuffer dataBuffer(const model::Transaction&) const override {
			BITXORCORE_THROW_RUNTIME_ERROR("dataBuffer - not implemented in mock");
		}

		std::vector<RawBuffer> merkleSupplementaryBuffers(const model::Transaction&) const override {
			BITXORCORE_THROW_RUNTIME_ERROR("merkleSupplementaryBuffers - not implemented in mock");
		}

		bool supportsTopLevel() const override {
			return true;
		}

		bool supportsEmbedding() const override {
			return false;
		}

		const model::EmbeddedTransactionPlugin& embeddedPlugin() const override {
			BITXORCORE_THROW_RUNTIME_ERROR("embeddedPlugin - not implemented in mock");
		}
	};
}}
