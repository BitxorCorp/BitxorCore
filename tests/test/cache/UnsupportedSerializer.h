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
#include "bitxorcore/exceptions.h"

namespace bitxorcore { namespace test {

	/// Unsupported state change subscriber.
	template<typename TKey, typename TValue>
	class UnsupportedSerializer {
	public:
		/// Serializes \a element to string.
		[[noreturn]]
		static std::string SerializeValue(const TValue&) {
			BITXORCORE_THROW_RUNTIME_ERROR("unsupported method: SerializeValue");
		}

		/// Deserializes value from \a buffer.
#ifndef _MSC_VER
		[[noreturn]]
#endif
		static TValue DeserializeValue(const RawBuffer&) {
#ifdef _MSC_VER
			// force msvc to not inline
			volatile bool shouldThrow = true;
			if (!shouldThrow)
				return TValue();
#endif
			BITXORCORE_THROW_RUNTIME_ERROR("unsupported method: DeserializeValue");
		}
	};
}}
