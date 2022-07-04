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
#include "TokenAddressRestriction.h"
#include "TokenGlobalRestriction.h"
#include "bitxorcore/plugins.h"
#include <memory>

namespace bitxorcore { namespace state {

	// region TokenRestrictionEntry

	/// Token restriction entry.
	class PLUGIN_API_DEPENDENCY TokenRestrictionEntry {
	public:
		/// Type of entry.
		enum class EntryType : uint8_t {
			/// Address restriction.
			Address,

			/// Global (token) restriction.
			Global
		};

	public:
		/// Creates an entry around address \a restriction.
		explicit TokenRestrictionEntry(const TokenAddressRestriction& restriction);

		/// Creates an entry around global \a restriction.
		explicit TokenRestrictionEntry(const TokenGlobalRestriction& restriction);

		/// Copy constructor that makes a copy of \a entry.
		TokenRestrictionEntry(const TokenRestrictionEntry& entry);

	public:
		/// Assignment operator that makes a copy of \a entry.
		TokenRestrictionEntry& operator=(const TokenRestrictionEntry& entry);

	public:
		/// Gets the entry type.
		EntryType entryType() const;

		/// Gets the unique (composite) key.
		const Hash256& uniqueKey() const;

	public:
		/// Gets a const address restriction interface to this entry.
		const TokenAddressRestriction& asAddressRestriction() const;

		/// Gets an address restriction interface to this entry.
		TokenAddressRestriction& asAddressRestriction();

		/// Gets a const global restriction interface to this entry.
		const TokenGlobalRestriction& asGlobalRestriction() const;

		/// Gets a global restriction interface to this entry.
		TokenGlobalRestriction& asGlobalRestriction();

	private:
		Hash256 generateUniqueKey() const;

	private:
		std::shared_ptr<TokenAddressRestriction> m_pAddressRestriction;
		std::shared_ptr<TokenGlobalRestriction> m_pGlobalRestriction;
		Hash256 m_uniqueKey;
	};

	// endregion

	// region CreateTokenRestrictionEntryKey

	/// Creates a token restriction entry key from its component parts (\a tokenId).
	Hash256 CreateTokenRestrictionEntryKey(TokenId tokenId);

	/// Creates a token restriction entry key from its component parts (\a tokenId and \a address).
	Hash256 CreateTokenRestrictionEntryKey(TokenId tokenId, const Address& address);

	// endregion
}}
