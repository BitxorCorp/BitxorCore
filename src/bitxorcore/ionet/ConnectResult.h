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
#include <iosfwd>

namespace bitxorcore { namespace ionet {

#define CONNECT_RESULT_LIST \
	/* Client address could not be resolved. */ \
	ENUM_VALUE(Resolve_Error) \
	\
	/* Connection could not be established. */ \
	ENUM_VALUE(Connect_Error) \
	\
	/* Connection handshake failed. */ \
	ENUM_VALUE(Handshake_Error) \
	\
	/* Connection attempt was cancelled. */ \
	ENUM_VALUE(Connect_Cancelled) \
	\
	/* Connection was successfully established. */ \
	ENUM_VALUE(Connected)

#define ENUM_VALUE(LABEL) LABEL,
	/// Enumeration of possible connection results.
	enum class ConnectResult {
		CONNECT_RESULT_LIST
	};
#undef ENUM_VALUE

	/// Insertion operator for outputting \a value to \a out.
	std::ostream& operator<<(std::ostream& out, ConnectResult value);
}}
