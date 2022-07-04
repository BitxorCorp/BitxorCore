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

#include "EntityDump.h"
#include "sdk/src/extensions/ConversionExtensions.h"
#include "plugins/txes/transfer/src/model/TransferTransaction.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/Block.h"
#include "bitxorcore/utils/HexFormatter.h"
#include "bitxorcore/utils/Logging.h"
#include <iomanip>

using namespace bitxorcore::model;
using namespace bitxorcore::crypto;

namespace bitxorcore { namespace test {

#define VALANDHEX(x) std::setw(5 * sizeof(x) / 2 + 1) << x << " (" << utils::HexFormat(x) << ")"

	namespace {
		void EntityDump(const TransferTransaction& tx) {
			BITXORCORE_LOG(debug) << "  Recipient: " << AddressToString(extensions::CopyToAddress(tx.RecipientAddress));
			BITXORCORE_LOG(debug) << "    Message: " << "size:" << VALANDHEX(tx.MessageSize);
			if (tx.MessageSize)
				BITXORCORE_LOG(debug) << "  Message D: " << utils::HexFormat(tx.MessagePtr(), tx.MessagePtr() + tx.MessageSize);

			BITXORCORE_LOG(debug) << "Tokens: " << VALANDHEX(tx.TokensCount);
			auto pToken = tx.TokensPtr();
			for (auto i = 0u; i < tx.TokensCount; ++i) {
				BITXORCORE_LOG(debug) << ":    Id: " << VALANDHEX(pToken->TokenId);
				BITXORCORE_LOG(debug) << ":Amount: " << VALANDHEX(pToken->Amount);
				pToken++;
			}
		}
	}

	void EntityDump(const Transaction& tx) {
		BITXORCORE_LOG(debug) << "       Size: " << VALANDHEX(tx.Size);
		BITXORCORE_LOG(debug) << "    Version: " << VALANDHEX(tx.Version);
		BITXORCORE_LOG(debug) << "       Type: " << VALANDHEX(tx.Type);
		BITXORCORE_LOG(debug) << "     Signer: " << tx.SignerPublicKey;
		BITXORCORE_LOG(debug) << "  Signature:\n" << tx.Signature;
		BITXORCORE_LOG(debug) << "    Max Fee: " << VALANDHEX(tx.MaxFee);
		BITXORCORE_LOG(debug) << "   Deadline: " << VALANDHEX(tx.Deadline);

		if (Entity_Type_Transfer == tx.Type)
			EntityDump(static_cast<const TransferTransaction&>(tx));
	}

	void EntityDump(const Block& block) {
		BITXORCORE_LOG(debug) << "        Size: " << VALANDHEX(block.Size);
		BITXORCORE_LOG(debug) << "     Version: " << VALANDHEX(block.Version);
		BITXORCORE_LOG(debug) << "        Type: " << VALANDHEX(block.Type);
		BITXORCORE_LOG(debug) << "      Signer: " << block.SignerPublicKey;
		BITXORCORE_LOG(debug) << "   Timestamp: " << VALANDHEX(block.Timestamp);
		BITXORCORE_LOG(debug) << "   Signature:\n" << block.Signature;
		BITXORCORE_LOG(debug) << "      Height: " << VALANDHEX(block.Height);
		BITXORCORE_LOG(debug) << "  Difficulty: " << VALANDHEX(block.Difficulty);
		BITXORCORE_LOG(debug) << " PrevBlockHa: " << block.PreviousBlockHash;
	}

#undef VALANDHEX
}}
