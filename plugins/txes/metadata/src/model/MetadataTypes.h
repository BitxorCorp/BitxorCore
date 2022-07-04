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
#include "plugins/txes/namespace/src/types.h"
#include <vector>

namespace bitxorcore { namespace model {

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow" // MetadataType::Token shadows model::Token
#endif

	/// Metadata type.
	enum class MetadataType : uint8_t {
		/// Account metadata.
		Account,

		/// Token metadata.
		Token,

		/// Namespace metadata.
		Namespace
	};

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

	/// Partial metadata key shared by all types of metadata.
	template<typename TTargetAddress>
	struct PartialMetadataKeyT {
		/// Address of the metadata source (provider).
		Address SourceAddress;

		/// Address of the metadata target.
		TTargetAddress TargetAddress;

		/// Metadata key scoped to source, target and type.
		uint64_t ScopedMetadataKey;
	};

	/// Partial metadata key shared by all types of metadata.
	using PartialMetadataKey = PartialMetadataKeyT<Address>;

	/// Unresolved partial metadata key shared by all types of metadata.
	using UnresolvedPartialMetadataKey = PartialMetadataKeyT<UnresolvedAddress>;

	/// Metadata target.
	struct MetadataTarget {
		/// Target type.
		MetadataType Type;

		/// Raw target identifier.
		uint64_t Id;
	};
}}
