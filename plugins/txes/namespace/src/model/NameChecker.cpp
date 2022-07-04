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

#include "NameChecker.h"
#include <algorithm>

namespace bitxorcore { namespace model {

	namespace {
		constexpr bool IsAlphaNumeric(uint8_t ch) {
			return (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9');
		}

		constexpr bool IsValidChar(uint8_t ch) {
		//	return IsAlphaNumeric(ch) || '-' == ch || '_' == ch;
		return IsAlphaNumeric(ch) || '-' == ch;
		}
		constexpr bool IsAlphaNumericIso(uint8_t ch) {
			return (ch >= 'A' && ch <= 'Z');
		}

		constexpr bool IsValidCharIso(uint8_t ch) {
			return IsAlphaNumericIso(ch) || '+' == ch;
		}
	}

	bool IsValidName(const uint8_t* pName, size_t nameSize) {
		if (0 == nameSize)
			return false;

		return IsAlphaNumeric(pName[0]) && std::all_of(pName + 1, pName + nameSize, IsValidChar);
	}
	bool IsValidIsoCode(const uint8_t* pName, size_t nameSize) {
		if (0 == nameSize || nameSize < 3)
			return false;

		if(nameSize > 5)
			return false;

		return IsAlphaNumericIso(pName[0]) && std::all_of(pName + 1, pName + nameSize, IsValidCharIso);
	}
}}
