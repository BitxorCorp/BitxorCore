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

#include "SecureZero.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
#define BITXORCORE_ZEROMEM(PTR, SIZE) SecureZeroMemory(PTR, SIZE)
#elif defined(__APPLE__) || defined(__STDC_LIB_EXT1__)
#define BITXORCORE_ZEROMEM(PTR, SIZE) memset_s(PTR, SIZE, 0, SIZE)
#else
#define BITXORCORE_ZEROMEM(PTR, SIZE) \
	do { \
		volatile uint8_t* p = PTR; \
		size_t n = SIZE; \
		while (n--) *p++ = 0; \
	} while (false)
#endif

namespace bitxorcore { namespace crypto {

	void SecureZero(uint8_t* pData, size_t dataSize) {
		BITXORCORE_ZEROMEM(pData, dataSize);
	}
}}
