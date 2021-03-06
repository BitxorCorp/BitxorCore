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

namespace bitxorcore { namespace crypto {

	/// Securely zeros out the memory backing the specified \a pData with size \a dataSize.
	void SecureZero(uint8_t* pData, size_t dataSize);

	/// Securely zeros out the memory backed by \a array.
	template<typename T, size_t N>
	void SecureZero(T (&array)[N]) {
		SecureZero(reinterpret_cast<uint8_t*>(array), sizeof(T) * N);
	}

	/// Securely zeros out the memory backed by \a byteArray.
	template<typename TByteArray>
	void SecureZero(TByteArray& byteArray) {
		SecureZero(byteArray.data(), byteArray.size());
	}
}}
