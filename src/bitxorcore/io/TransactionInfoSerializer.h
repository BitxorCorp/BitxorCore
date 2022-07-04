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
#include "bitxorcore/model/EntityInfo.h"

namespace bitxorcore {
	namespace io {
		class InputStream;
		class OutputStream;
	}
}

namespace bitxorcore { namespace io {

	/// Writes \a transactionInfo into \a outputStream.
	void WriteTransactionInfo(const model::TransactionInfo& transactionInfo, OutputStream& outputStream);

	/// Reads transaction info from \a inputStream into \a transactionInfo.
	void ReadTransactionInfo(InputStream& inputStream, model::TransactionInfo& transactionInfo);

	/// Writes \a transactionInfos into \a outputStream.
	void WriteTransactionInfos(const model::TransactionInfosSet& transactionInfos, OutputStream& outputStream);

	/// Reads transaction infos from \a inputStream into \a transactionInfos.
	void ReadTransactionInfos(InputStream& inputStream, model::TransactionInfosSet& transactionInfos);
}}
