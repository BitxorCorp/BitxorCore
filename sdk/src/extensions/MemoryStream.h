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
#include "bitxorcore/io/SeekableStream.h"
#include <vector>

namespace bitxorcore { namespace extensions {

	/// Memory-based implementation of input and output stream.
	class MemoryStream : public io::SeekableStream {
	public:
		/// Creates a memory stream around \a buffer.
		explicit MemoryStream(std::vector<uint8_t>& buffer);

	public:
		void write(const RawBuffer& buffer) override;
		void flush() override;

	public:
		bool eof() const override;
		void read(const MutableRawBuffer& buffer) override;

	public:
		void seek(uint64_t position) override;
		uint64_t position() const override;

	private:
		std::vector<uint8_t>& m_buffer;
		size_t m_position;
	};
}}
