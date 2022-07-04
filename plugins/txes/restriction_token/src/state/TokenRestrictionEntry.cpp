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

#include "TokenRestrictionEntry.h"
#include "bitxorcore/crypto/Hashes.h"
#include "bitxorcore/exceptions.h"

namespace bitxorcore { namespace state {

	// region TokenRestrictionEntry

	TokenRestrictionEntry::TokenRestrictionEntry(const TokenAddressRestriction& restriction)
			: m_pAddressRestriction(std::make_shared<TokenAddressRestriction>(restriction))
			, m_uniqueKey(generateUniqueKey())
	{}

	TokenRestrictionEntry::TokenRestrictionEntry(const TokenGlobalRestriction& restriction)
			: m_pGlobalRestriction(std::make_shared<TokenGlobalRestriction>(restriction))
			, m_uniqueKey(generateUniqueKey())
	{}

	TokenRestrictionEntry::TokenRestrictionEntry(const TokenRestrictionEntry& entry) {
		*this = entry;
	}

	TokenRestrictionEntry& TokenRestrictionEntry::operator=(const TokenRestrictionEntry& entry) {
		m_pAddressRestriction = entry.m_pAddressRestriction
				? std::make_shared<TokenAddressRestriction>(*entry.m_pAddressRestriction)
				: nullptr;
		m_pGlobalRestriction = entry.m_pGlobalRestriction
				? std::make_shared<TokenGlobalRestriction>(*entry.m_pGlobalRestriction)
				: nullptr;
		m_uniqueKey = entry.m_uniqueKey;

		return *this;
	}

	TokenRestrictionEntry::EntryType TokenRestrictionEntry::entryType() const {
		return m_pAddressRestriction ? EntryType::Address : EntryType::Global;
	}

	const Hash256& TokenRestrictionEntry::uniqueKey() const {
		return m_uniqueKey;
	}

	const TokenAddressRestriction& TokenRestrictionEntry::asAddressRestriction() const {
		if (!m_pAddressRestriction)
			BITXORCORE_THROW_RUNTIME_ERROR("entry is not an address restriction");

		return *m_pAddressRestriction;
	}

	TokenAddressRestriction& TokenRestrictionEntry::asAddressRestriction() {
		if (!m_pAddressRestriction)
			BITXORCORE_THROW_RUNTIME_ERROR("entry is not an address restriction");

		return *m_pAddressRestriction;
	}

	const TokenGlobalRestriction& TokenRestrictionEntry::asGlobalRestriction() const {
		if (!m_pGlobalRestriction)
			BITXORCORE_THROW_RUNTIME_ERROR("entry is not an global restriction");

		return *m_pGlobalRestriction;
	}

	TokenGlobalRestriction& TokenRestrictionEntry::asGlobalRestriction() {
		if (!m_pGlobalRestriction)
			BITXORCORE_THROW_RUNTIME_ERROR("entry is not an global restriction");

		return *m_pGlobalRestriction;
	}

	Hash256 TokenRestrictionEntry::generateUniqueKey() const {
		return m_pAddressRestriction
				? CreateTokenRestrictionEntryKey(m_pAddressRestriction->tokenId(), m_pAddressRestriction->address())
				: CreateTokenRestrictionEntryKey(m_pGlobalRestriction->tokenId());
	}

	// endregion

	// region CreateTokenRestrictionEntryKey

	Hash256 CreateTokenRestrictionEntryKey(TokenId tokenId) {
		return CreateTokenRestrictionEntryKey(tokenId, Address());
	}

	Hash256 CreateTokenRestrictionEntryKey(TokenId tokenId, const Address& address) {
		crypto::Sha3_256_Builder builder;
		builder.update({ reinterpret_cast<const uint8_t*>(&tokenId), sizeof(TokenId) });
		builder.update(address);

		Hash256 uniqueKey;
		builder.final(uniqueKey);
		return uniqueKey;
	}

	// endregion
}}
