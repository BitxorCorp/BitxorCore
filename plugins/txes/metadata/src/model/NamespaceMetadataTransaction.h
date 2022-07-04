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
#include "MetadataEntityType.h"
#include "MetadataSharedTransaction.h"
#include "plugins/txes/namespace/src/types.h"
#include "bitxorcore/model/ContainerTypes.h"
#include "bitxorcore/utils/ArraySet.h"

namespace bitxorcore { namespace model {

#pragma pack(push, 1)

	/// Metadata transaction header with namespace id target.
	template<typename THeader>
	struct NamespaceMetadataTransactionHeader : public MetadataTransactionHeader<THeader> {
		/// Target namespace identifier.
		NamespaceId TargetNamespaceId;
	};

	/// Binary layout for a namespace metadata transaction body.
	template<typename THeader>
	struct NamespaceMetadataTransactionBody
			: public BasicMetadataTransactionBody<NamespaceMetadataTransactionHeader<THeader>, Entity_Type_Namespace_Metadata>
	{};

	DEFINE_EMBEDDABLE_TRANSACTION(NamespaceMetadata)

#pragma pack(pop)

	/// Extracts addresses of additional accounts that must approve \a transaction.
	inline UnresolvedAddressSet ExtractAdditionalRequiredCosignatories(const EmbeddedNamespaceMetadataTransaction& transaction) {
		return { transaction.TargetAddress };
	}
}}

