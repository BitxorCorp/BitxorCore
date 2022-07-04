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
#include "TokenFlags.h"
#include "bitxorcore/types.h"

namespace bitxorcore { namespace model {

	/// Container for token properties.
	class TokenProperties {
	public:
		/// Creates zeroed token properties.
		TokenProperties() : TokenProperties(TokenFlags::None, 0, BlockDuration())
		{}

		/// Creates token properties around \a flags, \a divisibility and \a duration.
		TokenProperties(TokenFlags flags, uint8_t divisibility, BlockDuration duration)
				: m_flags(flags)
				, m_divisibility(divisibility)
				, m_duration(duration)
		{}

	public:
		/// Gets the token flags.
		TokenFlags flags() const {
			return m_flags;
		}

		/// Gets the token divisibility.
		uint8_t divisibility() const {
			return m_divisibility;
		}

		/// Gets the token duration.
		BlockDuration duration() const {
			return m_duration;
		}

		/// Returns \c true if token flags contain \a testedFlag.
		bool is(TokenFlags testedFlag) const {
			return HasFlag(testedFlag, m_flags);
		}

	public:
		/// Returns \c true if this properties bag is equal to \a rhs.
		bool operator==(const TokenProperties& rhs) const {
			return m_flags == rhs.m_flags
					&& m_divisibility == rhs.m_divisibility
					&& m_duration == rhs.m_duration;
		}

		/// Returns \c true if this properties bag is not equal to \a rhs.
		bool operator!=(const TokenProperties& rhs) const {
			return !(*this == rhs);
		}

	private:
		TokenFlags m_flags;
		uint8_t m_divisibility;
		BlockDuration m_duration;
	};
}}
