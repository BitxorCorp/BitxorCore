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
#include "plugins/txes/metadata/src/state/MetadataKey.h"
#include "plugins/txes/namespace/src/types.h"
#include "bitxorcore/ionet/PackedNodeInfo.h"
#include "bitxorcore/model/CacheEntryInfo.h"
#include "bitxorcore/model/DiagnosticCounterValue.h"
#include "bitxorcore/model/RangeTypes.h"
#include "bitxorcore/state/TimestampedHash.h"
#include "bitxorcore/thread/Future.h"

namespace bitxorcore { namespace ionet { class PacketIo; } }

namespace bitxorcore { namespace extensions {

	/// Api for retrieving diagnostic information from a remote node.
	class RemoteDiagnosticApi {
	private:
		template<typename T>
		using future = thread::future<T>;

	public:
		virtual ~RemoteDiagnosticApi() = default;

	public:
		/// Gets the timestamped hashes in \a timestampedHashes that are unconfirmed, i.e. not in the hash cache.
		virtual future<state::TimestampedHashRange> confirmTimestampedHashes(state::TimestampedHashRange&& timestampedHashes) const = 0;

		/// Gets the current diagnostic counter values.
		virtual future<model::EntityRange<model::DiagnosticCounterValue>> diagnosticCounters() const = 0;

		/// Gets the node infos for all active nodes
		virtual future<model::EntityRange<ionet::PackedNodeInfo>> activeNodeInfos() const = 0;

		/// Gets the current unlocked accounts.
		virtual future<model::EntityRange<Key>> unlockedAccounts() const = 0;

		/// Gets the account infos for all accounts with addresses in \a addresses.
		virtual future<model::EntityRange<model::CacheEntryInfo<Address>>> accountInfos(model::AddressRange&& addresses) const = 0;

		/// Gets the account restrictions infos for all accounts with addresses in \a addresses.
		virtual future<model::EntityRange<model::CacheEntryInfo<Address>>> accountRestrictionsInfos(
				model::AddressRange&& addresses) const = 0;

		/// Gets the namespace infos for all namespace ids in \a namespaceIds.
		virtual future<model::EntityRange<model::CacheEntryInfo<NamespaceId>>> namespaceInfos(
				model::EntityRange<NamespaceId>&& namespaceIds) const = 0;

		/// Gets the token infos for all token ids in \a tokenIds.
		virtual future<model::EntityRange<model::CacheEntryInfo<TokenId>>> tokenInfos(
				model::EntityRange<TokenId>&& tokenIds) const = 0;

		/// Gets the metadata infos for all unique keys in \a uniqueKeys.
		virtual future<model::EntityRange<model::CacheEntryInfo<Hash256>>> metadataInfos(
				model::EntityRange<Hash256>&& uniqueKeys) const = 0;
	};

	/// Creates a diagnostic api for interacting with a remote node with the specified \a io.
	std::unique_ptr<RemoteDiagnosticApi> CreateRemoteDiagnosticApi(ionet::PacketIo& io);
}}
