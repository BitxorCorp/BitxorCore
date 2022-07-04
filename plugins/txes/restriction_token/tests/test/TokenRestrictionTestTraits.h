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
#include "src/state/TokenRestrictionEntry.h"
#include "tests/test/core/ResolverTestUtils.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace test {

	// region TokenGlobalRestrictionTestTraits

	/// Basic token global restriction test traits.
	template<typename TNotification>
	class TokenGlobalRestrictionTestTraits {
	public:
		using NotificationType = TNotification;

	public:
		/// Creates default traits.
		TokenGlobalRestrictionTestTraits()
				: m_tokenId(GenerateRandomValue<TokenId>())
				, m_referenceTokenId(GenerateRandomValue<TokenId>())
		{}

	public:
		/// Gets the random (unresolved) reference token id.
		UnresolvedTokenId referenceTokenId() const {
			return UnresolveXor(m_referenceTokenId);
		}

	public:
		/// Gets the unique entry key.
		Hash256 uniqueKey() {
			return state::CreateTokenRestrictionEntryKey(m_tokenId);
		}

		/// Creates an add notification with \a key and \a value.
		NotificationType createAddNotification(uint64_t key, uint64_t value) {
			auto restrictionType = model::TokenRestrictionType::EQ;
			return NotificationType(UnresolveXor(m_tokenId), UnresolveXor(m_referenceTokenId), key, value, restrictionType);
		}

		/// Creates a delete notification with \a key.
		NotificationType createDeleteNotification(uint64_t key) {
			auto restrictionType = model::TokenRestrictionType::NONE;
			return NotificationType(UnresolveXor(m_tokenId), UnresolveXor(TokenId()), key, 0, restrictionType);
		}

	public:
		/// Adds restriction with multiple values to cache as specified in \a keyValuePairs.
		void addRestrictionWithValuesToCache(
				cache::TokenRestrictionCacheDelta& restrictionCache,
				const std::vector<std::pair<uint64_t, uint64_t>>& keyValuePairs) {
			state::TokenGlobalRestriction restriction(m_tokenId);
			for (const auto& pair : keyValuePairs)
				restriction.set(pair.first, { m_referenceTokenId, pair.second, model::TokenRestrictionType::EQ });

			restrictionCache.insert(state::TokenRestrictionEntry(restriction));
		}

	public:
		/// Asserts \a entry is equal to the expected entry based on these traits with values specified in \a keyValuePairs.
		void assertEqual(const state::TokenRestrictionEntry& entry, const std::vector<std::pair<uint64_t, uint64_t>>& keyValuePairs) {
			ASSERT_EQ(state::TokenRestrictionEntry::EntryType::Global, entry.entryType());

			const auto& restriction = entry.asGlobalRestriction();
			EXPECT_EQ(m_tokenId, restriction.tokenId());

			EXPECT_EQ(keyValuePairs.size(), restriction.size());
			for (const auto& pair : keyValuePairs) {
				auto message = "key " + std::to_string(pair.second);

				state::TokenGlobalRestriction::RestrictionRule rule;
				restriction.tryGet(pair.first, rule);

				EXPECT_EQ(m_referenceTokenId, rule.ReferenceTokenId) << message;
				EXPECT_EQ(pair.second, rule.RestrictionValue) << message;
				EXPECT_EQ(model::TokenRestrictionType::EQ, rule.RestrictionType) << message;
			}
		}

	private:
		TokenId m_tokenId;
		TokenId m_referenceTokenId;
	};

	// endregion

	// region TokenAddressRestrictionTestTraits

	/// Basic token address restriction test traits.
	template<typename TNotification>
	class TokenAddressRestrictionTestTraits {
	public:
		using NotificationType = TNotification;

	public:
		/// Creates default traits.
		TokenAddressRestrictionTestTraits()
				: m_tokenId(GenerateRandomValue<TokenId>())
				, m_address(GenerateRandomByteArray<Address>())
				, m_unresolvedAddress(UnresolveXor(m_address))
		{}

	public:
		/// Gets the unique entry key.
		Hash256 uniqueKey() {
			return state::CreateTokenRestrictionEntryKey(m_tokenId, m_address);
		}

		/// Creates an add notification with \a key and \a value.
		NotificationType createAddNotification(uint64_t key, uint64_t value) {
			return NotificationType(UnresolveXor(m_tokenId), key, m_unresolvedAddress, value);
		}

		/// Creates a delete notification with \a key.
		NotificationType createDeleteNotification(uint64_t key) {
			return createAddNotification(key, state::TokenAddressRestriction::Sentinel_Removal_Value);
		}

	public:
		/// Adds restriction with multiple values to cache as specified in \a keyValuePairs.
		void addRestrictionWithValuesToCache(
				cache::TokenRestrictionCacheDelta& restrictionCache,
				const std::vector<std::pair<uint64_t, uint64_t>>& keyValuePairs) {
			state::TokenAddressRestriction restriction(m_tokenId, m_address);
			for (const auto& pair : keyValuePairs)
				restriction.set(pair.first, pair.second);

			restrictionCache.insert(state::TokenRestrictionEntry(restriction));
		}

	public:
		/// Asserts \a entry is equal to the expected entry based on these traits with values specified in \a keyValuePairs.
		void assertEqual(const state::TokenRestrictionEntry& entry, const std::vector<std::pair<uint64_t, uint64_t>>& keyValuePairs) {
			ASSERT_EQ(state::TokenRestrictionEntry::EntryType::Address, entry.entryType());

			const auto& restriction = entry.asAddressRestriction();
			EXPECT_EQ(m_tokenId, restriction.tokenId());
			EXPECT_EQ(m_address, restriction.address());

			EXPECT_EQ(keyValuePairs.size(), restriction.size());
			for (const auto& pair : keyValuePairs) {
				auto value = restriction.get(pair.first);

				EXPECT_EQ(pair.second, value) << "key " << pair.second;
			}
		}

	private:
		TokenId m_tokenId;
		Address m_address;
		UnresolvedAddress m_unresolvedAddress;
	};

	// endregion
}}
