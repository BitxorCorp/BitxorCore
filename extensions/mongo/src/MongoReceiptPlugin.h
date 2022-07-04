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
#include "bitxorcore/model/PluginRegistry.h"
#include "bitxorcore/model/ReceiptType.h"
#include "bitxorcore/plugins.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>

namespace bitxorcore { namespace model { struct Receipt; } }

namespace bitxorcore { namespace mongo {

	/// Mongo receipt plugin.
	class MongoReceiptPlugin {
	public:
		virtual ~MongoReceiptPlugin() = default;

	public:
		/// Gets the receipt type.
		virtual model::ReceiptType type() const = 0;

		/// Streams \a receipt to \a builder.
		virtual void streamReceipt(bsoncxx::builder::stream::document& builder, const model::Receipt& receipt) const = 0;
	};

	/// Registry of mongo receipt plugins.
	class MongoReceiptRegistry : public model::PluginRegistry<MongoReceiptPlugin, model::ReceiptType> {};
}}
