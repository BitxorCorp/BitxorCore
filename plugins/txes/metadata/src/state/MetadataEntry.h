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
#include "MetadataKey.h"
#include "MetadataValue.h"
#include "bitxorcore/plugins.h"

namespace bitxorcore { namespace state {

	/// Metadata entry.
	class PLUGIN_API_DEPENDENCY MetadataEntry {
	public:
		/// Creates an entry around \a key.
		explicit MetadataEntry(const MetadataKey& key);

	public:
		/// Gets the metadata key.
		const MetadataKey& key() const;

		/// Gets the (const) metadata value.
		const MetadataValue& value() const;

		/// Gets the metadata value.
		MetadataValue& value();

	private:
		MetadataKey m_key;
		MetadataValue m_value;
	};
}}
