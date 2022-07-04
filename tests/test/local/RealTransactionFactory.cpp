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

#include "RealTransactionFactory.h"
#include "sdk/src/builders/AddressAliasBuilder.h"
#include "sdk/src/builders/NamespaceRegistrationBuilder.h"
#include "sdk/src/builders/TransferBuilder.h"
#include "sdk/src/extensions/ConversionExtensions.h"
#include "sdk/src/extensions/TransactionExtensions.h"
#include "plugins/txes/namespace/src/model/NamespaceIdGenerator.h"
#include "bitxorcore/crypto/Signer.h"
#include "bitxorcore/model/Address.h"
#include "bitxorcore/model/NetworkIdentifier.h"
#include "bitxorcore/preprocessor.h"
#include "tests/test/core/AddressTestUtils.h"
#include "tests/test/core/BlockTestUtils.h"
#include "tests/test/nodeps/Genesis.h"
#include "tests/test/nodeps/Random.h"
#include "tests/test/nodeps/TestConstants.h"
#include <string.h>

namespace bitxorcore { namespace test {

	namespace {
		constexpr auto Network_Identifier = model::NetworkIdentifier::Testnet;
	}

	// region transfer transaction

	size_t GetTransferTransactionSize() {
		return sizeof(model::TransferTransaction);
	}

	namespace {
		std::unique_ptr<model::Transaction> CreateUnsignedTransferTransaction(
				const Key& signerPublicKey,
				const UnresolvedAddress& recipient,
				const std::vector<uint8_t>& message,
				const std::vector<model::UnresolvedToken>& tokens) {
			builders::TransferBuilder builder(Network_Identifier, signerPublicKey);
			builder.setRecipientAddress(recipient);

			if (!message.empty())
				builder.setMessage(message);

			for (const auto& token : tokens)
				builder.addToken(token);

			auto pTransfer = builder.build();
			pTransfer->MaxFee = GenerateRandomValue<Amount>();
			pTransfer->Deadline = GenerateRandomValue<Timestamp>();
			return PORTABLE_MOVE(pTransfer);
		}
	}

	std::unique_ptr<model::Transaction> CreateUnsignedTransferTransaction(
			const Key& signerPublicKey,
			const UnresolvedAddress& recipient,
			Amount amount) {
		auto tokenId = extensions::CastToUnresolvedTokenId(Default_Currency_Token_Id);
		return CreateUnsignedTransferTransaction(signerPublicKey, recipient, {}, { { tokenId, amount } });
	}

	std::unique_ptr<model::Transaction> CreateTransferTransaction(
			const crypto::KeyPair& signer,
			const UnresolvedAddress& recipient,
			Amount amount) {
		auto pTransaction = CreateUnsignedTransferTransaction(signer.publicKey(), recipient, amount);
		extensions::TransactionExtensions(GetGenesisGenerationHashSeed()).sign(signer, *pTransaction);
		return pTransaction;
	}

	std::unique_ptr<model::Transaction> CreateTransferTransaction(
			const crypto::KeyPair& signer,
			const Key& recipientPublicKey,
			Amount amount) {
		auto recipient = model::PublicKeyToAddress(recipientPublicKey, Network_Identifier);
		return CreateTransferTransaction(signer, extensions::CopyToUnresolvedAddress(recipient), amount);
	}

	// endregion

	// region namespace transactions

	std::unique_ptr<model::Transaction> CreateRootNamespaceRegistrationTransaction(
			const crypto::KeyPair& signer,
			const std::string& name,
			BlockDuration duration) {
		builders::NamespaceRegistrationBuilder builder(Network_Identifier, signer.publicKey());
		builder.setName({ reinterpret_cast<const uint8_t*>(name.data()), name.size() });
		builder.setDuration(duration);
		auto pTransaction = builder.build();

		extensions::TransactionExtensions(GetGenesisGenerationHashSeed()).sign(signer, *pTransaction);
		return PORTABLE_MOVE(pTransaction);
	}

	std::unique_ptr<model::Transaction> CreateRootAddressAliasTransaction(
			const crypto::KeyPair& signer,
			const std::string& name,
			const Address& address) {
		auto namespaceId = model::GenerateNamespaceId(NamespaceId(), name);
		builders::AddressAliasBuilder builder(Network_Identifier, signer.publicKey());
		builder.setNamespaceId(namespaceId);
		builder.setAddress(address);
		builder.setAliasAction(model::AliasAction::Link);
		auto pTransaction = builder.build();

		extensions::TransactionExtensions(GetGenesisGenerationHashSeed()).sign(signer, *pTransaction);
		return PORTABLE_MOVE(pTransaction);
	}

	// endregion
}}
