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

#include "UnlockedFileQueueConsumer.h"
#include "bitxorcore/config/BitxorCoreDataDirectory.h"
#include "bitxorcore/crypto/AesDecrypt.h"
#include "bitxorcore/io/FileQueue.h"
#include "bitxorcore/io/RawFile.h"
#include "bitxorcore/utils/Logging.h"

namespace bitxorcore { namespace harvesting {

	namespace {
		size_t ExpectedSerializedHarvestRequestSize() {
			return 1 + sizeof(Height) + Key::Size + HarvestRequest::EncryptedPayloadSize();
		}
	}

	std::pair<BlockGeneratorAccountDescriptor, bool> TryDecryptBlockGeneratorAccountDescriptor(
			const RawBuffer& publicKeyPrefixedEncryptedPayload,
			const crypto::KeyPair& encryptionKeyPair) {
		std::vector<uint8_t> decrypted;
		auto isDecryptSuccessful = crypto::TryDecryptEd25199BlockCipher(publicKeyPrefixedEncryptedPayload, encryptionKeyPair, decrypted);
		if (!isDecryptSuccessful || HarvestRequest::DecryptedPayloadSize() != decrypted.size())
			return std::make_pair(BlockGeneratorAccountDescriptor(), false);

		auto iter = decrypted.begin();
		auto extractKeyPair = [&iter]() {
			return crypto::KeyPair::FromPrivate(crypto::PrivateKey::Generate([&iter]() mutable { return *iter++; }));
		};

		auto signingKeyPair = extractKeyPair();
		auto vrfKeyPair = extractKeyPair();
		return std::make_pair(BlockGeneratorAccountDescriptor(std::move(signingKeyPair), std::move(vrfKeyPair)), true);
	}

	void UnlockedFileQueueConsumer(
			const config::BitxorCoreDirectory& directory,
			Height maxHeight,
			const crypto::KeyPair& encryptionKeyPair,
			const consumer<const HarvestRequest&, BlockGeneratorAccountDescriptor&&>& processDescriptor) {
		io::FileQueueReader reader(directory.str());
		auto appendMessage = [maxHeight, &encryptionKeyPair, &processDescriptor](const auto& buffer) {
			// filter out invalid requests
			if (ExpectedSerializedHarvestRequestSize() != buffer.size()) {
				BITXORCORE_LOG(warning) << "rejecting buffer with wrong size: " << buffer.size();
				return true;
			}

			auto harvestRequest = DeserializeHarvestRequest(buffer);
			if (maxHeight < harvestRequest.Height) {
				BITXORCORE_LOG(debug) << "skipping request with height " << harvestRequest.Height << " given max height " << maxHeight;
				return false; // not fully processed
			}

			auto decryptedPair = TryDecryptBlockGeneratorAccountDescriptor(harvestRequest.EncryptedPayload, encryptionKeyPair);
			if (!decryptedPair.second) {
				BITXORCORE_LOG(warning) << "rejecting request with encrypted payload that could not be decrypted";
				return true;
			}

			processDescriptor(harvestRequest, std::move(decryptedPair.first));
			return true;
		};

		while (reader.tryReadNextMessageConditional(appendMessage))
		{}
	}
}}
