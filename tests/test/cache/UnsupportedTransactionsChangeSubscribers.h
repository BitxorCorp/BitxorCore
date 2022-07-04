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
#include "bitxorcore/cache_tx/PtChangeSubscriber.h"
#include "bitxorcore/cache_tx/UtChangeSubscriber.h"

namespace bitxorcore { namespace test {

	// region UnsupportedFlushBehavior / FlushInvoker

	/// Enumeration indicating behavior of unsupported flush operation.
	enum class UnsupportedFlushBehavior {
		/// Operation is ignored.
		Ignore,

		/// Operation throws an exception.
		Throw
	};

	/// Performs specified flush behavior.
	template<UnsupportedFlushBehavior FlushBehavior>
	struct FlushInvoker {
		static void Flush() {
			// do nothing;
		}
	};

	template<>
	struct FlushInvoker<UnsupportedFlushBehavior::Throw> {
		[[noreturn]]
		static void Flush() {
			BITXORCORE_THROW_RUNTIME_ERROR("flush - not supported in mock");
		}
	};

	// endregion

	/// Unsupported pt change subscriber.
	template<UnsupportedFlushBehavior FlushBehavior>
	class UnsupportedPtChangeSubscriber : public cache::PtChangeSubscriber {
	public:
		void notifyAddPartials(const TransactionInfos&) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyAddPartials - not supported in mock");
		}

		void notifyAddCosignature(const model::TransactionInfo&, const model::Cosignature&) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyAddCosignature - not supported in mock");
		}

		void notifyRemovePartials(const TransactionInfos&) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyRemovePartials - not supported in mock");
		}

		void flush() override {
			FlushInvoker<FlushBehavior>::Flush();
		}
	};

	/// Unsupported ut change subscriber
	template<UnsupportedFlushBehavior FlushBehavior>
	class UnsupportedUtChangeSubscriber : public cache::UtChangeSubscriber {
	public:
		void notifyAdds(const TransactionInfos&) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyAdds - not supported in mock");
		}

		void notifyRemoves(const TransactionInfos&) override {
			BITXORCORE_THROW_RUNTIME_ERROR("notifyRemoves - not supported in mock");
		}

		void flush() override {
			FlushInvoker<FlushBehavior>::Flush();
		}
	};
}}
