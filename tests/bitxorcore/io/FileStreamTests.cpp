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

#include "bitxorcore/io/FileStream.h"
#include "tests/bitxorcore/io/test/StreamTests.h"
#include "tests/test/nodeps/Filesystem.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace io {

#define TEST_CLASS FileStreamTests

	namespace {
		class FileStreamContext {
		public:
			explicit FileStreamContext(const char* name) : m_guard(name)
			{}

			auto outputStream() const {
				return MakeStream(OpenMode::Read_Write);
			}

			auto inputStream() const {
				return MakeStream(OpenMode::Read_Only);
			}

			std::string filename() const {
				return m_guard.name();
			}

		private:
			std::unique_ptr<FileStream> MakeStream(OpenMode mode) const {
				return std::make_unique<FileStream>(RawFile(filename(), mode));
			}

		private:
			test::TempFileGuard m_guard;
		};
	}

	DEFINE_SEEKABLE_STREAM_TESTS(FileStreamContext)
}}
