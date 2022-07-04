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
#include "version_inc.h"
#include <iosfwd>

#define STRINGIFY2(STR) #STR
#define STRINGIFY(STR) STRINGIFY2(STR)

#define BITXORCORE_BASE_VERSION \
	STRINGIFY(BITXORCORE_VERSION_MAJOR) "." \
	STRINGIFY(BITXORCORE_VERSION_MINOR) "." \
	STRINGIFY(BITXORCORE_VERSION_REVISION) "." \
	STRINGIFY(BITXORCORE_VERSION_BUILD)

#ifdef BITXORCORE_VERSION_DESCRIPTION
#define BITXORCORE_VERSION BITXORCORE_BASE_VERSION " " BITXORCORE_VERSION_DESCRIPTION
#else
#define BITXORCORE_VERSION BITXORCORE_BASE_VERSION
#endif

#define BITXORCORE_COPYRIGHT "Copyright (c) Bitxor Community, Kriptxor Corp, Microsula S.A."

namespace bitxorcore { namespace version {

	/// Writes custom version information to \a out.
	void WriteVersionInformation(std::ostream& out);
}}
