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
#include "plugins/txes/metadata/src/model/MetadataTypes.h"
#include "plugins/txes/namespace/src/types.h"
#include <vector>

namespace bitxorcore { namespace model { class ResolverContext; } }

namespace bitxorcore { namespace state {

	/// Metadata key.
	class MetadataKey {
	public:
		/// Creates a metadata key from \a partialKey.
		explicit MetadataKey(const model::PartialMetadataKey& partialKey);

		/// Creates a metadata key from \a partialKey and \a tokenId target.
		MetadataKey(const model::PartialMetadataKey& partialKey, TokenId tokenId);

		/// Creates a metadata key from \a partialKey and \a namespaceId target.
		MetadataKey(const model::PartialMetadataKey& partialKey, NamespaceId namespaceId);

	public:
		/// Gets the unique (composite) key.
		const Hash256& uniqueKey() const;

	public:
		/// Gets the source address.
		const Address& sourceAddress() const;

		/// Gets the target address.
		const Address& targetAddress() const;

		/// Gets the scoped metadata key.
		uint64_t scopedMetadataKey() const;

		/// Gets the target id.
		uint64_t targetId() const;

		/// Gets the metadata type.
		model::MetadataType metadataType() const;

		/// Gets the token target.
		TokenId tokenTarget() const;

		/// Gets the namespace target.
		NamespaceId namespaceTarget() const;

	private:
		void require(model::MetadataType metadataType, const char* name) const;

		Hash256 generateUniqueKey() const;

	private:
		model::PartialMetadataKey m_partialKey;
		uint64_t m_targetId;
		model::MetadataType m_metadataType;
		Hash256 m_uniqueKey;
	};

	/// Merges \a partialKey and \a target into a (resolved) metadata key.
	/// \note \a target is expected to already be resolved.
	MetadataKey CreateMetadataKey(const model::PartialMetadataKey& partialKey, const model::MetadataTarget& target);

	/// Uses \a resolvers to merge \a partialKey and \a target into a (resolved) metadata key.
	MetadataKey ResolveMetadataKey(
			const model::UnresolvedPartialMetadataKey& partialKey,
			const model::MetadataTarget& target,
			const model::ResolverContext& resolvers);
}}
