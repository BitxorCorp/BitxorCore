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

#include "Platform.h"
#include "bitxorcore/utils/Logging.h"

#ifndef _WIN32
#include <sys/stat.h>
#include <sys/types.h>
#endif

namespace bitxorcore { namespace process {

	void PlatformSettings() {
#ifndef _WIN32
		constexpr auto Required_Umask = S_IRWXG | S_IRWXO;
		auto previousUmask = umask(Required_Umask);

		// check group and other mask
		if (Required_Umask != previousUmask) {
			BITXORCORE_LOG(warning)
					<< std::endl << "\tCurrent user umask settings are too wide '" << std::oct << previousUmask << "'."
					<< std::endl << "\tIt's strongly suggested to use umask value '077' when starting bitxor software.";
		}
#endif
	}
}}
