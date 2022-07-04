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
#include "bitxorcore/types.h"
#include <vector>

namespace bitxorcore { namespace state {

	/// Metadata value.
	class MetadataValue {
	public:
		/// Returns \c true if the value empty.
		bool empty() const;

		/// Gets the size of the value.
		size_t size() const;

		/// Gets a pointer to the value data.
		const uint8_t* data() const;

	public:
		/// Determines if \a count bytes can be trimmed after applying \a buffer.
		bool canTrim(const RawBuffer& buffer, size_t count) const;

		/// Updates the value with \a buffer.
		void update(const RawBuffer& buffer);

	private:
		std::vector<uint8_t> m_buffer;
	};
}}
