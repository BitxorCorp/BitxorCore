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
#include "StateVersion.h"
#include "bitxorcore/io/BufferInputStreamAdapter.h"
#include "bitxorcore/io/SizeCalculatingOutputStream.h"
#include "bitxorcore/io/StringOutputStream.h"

namespace bitxorcore { namespace cache {

	/// Cache serializer adapter.
	template<typename TSerializerTraits, typename TDescriptor = TSerializerTraits>
	class CacheSerializerAdapter {
	public:
		using KeyType = typename TDescriptor::KeyType;
		using ValueType = typename TDescriptor::ValueType;

	public:
		/// Serializes \a value to string.
		static std::string SerializeValue(const ValueType& value) {
			io::SizeCalculatingOutputStream calculator;
			StateVersion<TSerializerTraits>::Write(calculator);
			TSerializerTraits::Save(value, calculator);

			io::StringOutputStream output(calculator.size());
			StateVersion<TSerializerTraits>::Write(output);
			TSerializerTraits::Save(value, output);
			return output.str();
		}

		/// Deserializes value from \a buffer.
		static ValueType DeserializeValue(const RawBuffer& buffer) {
			io::BufferInputStreamAdapter<RawBuffer> input(buffer);
			StateVersion<TSerializerTraits>::ReadAndCheck(input);
			return TSerializerTraits::Load(input);
		}
	};
}}
