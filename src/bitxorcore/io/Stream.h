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
#include "bitxorcore/plugins.h"
#include "bitxorcore/types.h"

namespace bitxorcore { namespace io {

	/// Reader interface.
	class PLUGIN_API_DEPENDENCY InputStream {
	public:
		virtual ~InputStream() = default;

	public:
		/// Returns \c true if no data is left in the stream.
		virtual bool eof() const = 0;

		/// Reads data from this stream into \a buffer.
		/// \throws bitxorcore_file_io_error if requested amount of data could not be read.
		virtual void read(const MutableRawBuffer& buffer) = 0;
	};

	/// Writer interface.
	class PLUGIN_API_DEPENDENCY OutputStream {
	public:
		virtual ~OutputStream() = default;

	public:
		/// Writes data pointed to by \a buffer to this stream.
		/// \throws bitxorcore_file_io_error if proper amount of data could not be written.
		virtual void write(const RawBuffer& buffer) = 0;

		/// Commits all pending data.
		/// \throws bitxorcore_file_io_error if flush failed.
		virtual void flush() = 0;
	};
}}
