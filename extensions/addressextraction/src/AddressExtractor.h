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
#include "bitxorcore/model/ContainerTypes.h"
#include "bitxorcore/model/NotificationPublisher.h"

namespace bitxorcore {
	namespace model {
		struct BlockElement;
		struct TransactionElement;
	}
}

namespace bitxorcore { namespace addressextraction {

	/// Utility class for extracting addresses.
	class AddressExtractor {
	public:
		/// Creates an extractor around \a pPublisher.
		explicit AddressExtractor(std::unique_ptr<const model::NotificationPublisher>&& pPublisher);

	public:
		/// Extracts transaction addresses into \a transactionInfo.
		void extract(model::TransactionInfo& transactionInfo) const;

		/// Extracts transaction addresses into \a transactionInfos.
		void extract(model::TransactionInfosSet& transactionInfos) const;

		/// Extracts transaction addresses into \a transactionElement.
		void extract(model::TransactionElement& transactionElement) const;

		/// Extracts transaction addresses into \a blockElement.
		void extract(model::BlockElement& blockElement) const;

	private:
		std::unique_ptr<const model::NotificationPublisher> m_pPublisher;
	};
}}
