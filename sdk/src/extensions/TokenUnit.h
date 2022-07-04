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
#include "bitxorcore/utils/ImmutableValue.h"
#include "bitxorcore/utils/StreamFormatGuard.h"
#include "bitxorcore/types.h"

namespace bitxorcore { namespace extensions {

	struct BasicUnitAmount_tag {};

	/// Non-fractional amount of a token.
	using BasicUnitAmount = utils::BaseValue<uint64_t, BasicUnitAmount_tag>;

	/// Represents units of a token.
	template<size_t Divisibility>
	class TokenUnit {
	public:
		/// Raw value type.
		using ValueType = uint64_t;

	public:
		/// Creates a zeroed token unit.
		TokenUnit() : m_value(0)
		{}

		/// Creates a token unit from \a amount atomic units.
		explicit TokenUnit(Amount amount) : m_value(amount.unwrap())
		{}

		/// Creates a token unit from \a amount basic units.
		explicit TokenUnit(BasicUnitAmount amount) : m_value(amount.unwrap() * GetAtomicUnitMultiplier())
		{}

		/// Creates a copy of \a rhs.
		TokenUnit(const TokenUnit& rhs) : m_value(static_cast<ValueType>(rhs.m_value))
		{}

	private:
		static uint64_t GetAtomicUnitMultiplier() {
			uint64_t value = 1;
			for (auto i = 0u; i < Divisibility; ++i)
				value *= 10;

			return value;
		}

	public:
		/// Assigns \a rhs to this.
		TokenUnit& operator=(TokenUnit rhs) {
			m_value = std::move(rhs.m_value);
			return *this;
		}

		/// Assigns \a rhs to this.
		TokenUnit& operator=(Amount rhs) {
			m_value = TokenUnit(rhs).m_value;
			return *this;
		}

		/// Assigns \a rhs to this.
		TokenUnit& operator=(BasicUnitAmount rhs) {
			m_value = TokenUnit(rhs).m_value;
			return *this;
		}

	public:
		/// Gets the number of (basic unit) tokens.
		BasicUnitAmount basicUnit() const {
			return BasicUnitAmount(m_value / GetAtomicUnitMultiplier());
		}

		/// Gets the number of (atomic unit) tokens.
		Amount atomicUnit() const {
			return Amount(m_value);
		}

		/// Returns \c true if this unit includes fractional tokens.
		bool isFractional() const {
			return 0 != m_value % GetAtomicUnitMultiplier();
		}

	public:
		/// Returns \c true if this value is equal to \a rhs.
		bool operator==(TokenUnit rhs) const {
			return m_value == rhs.m_value;
		}

		/// Returns \c true if this value is not equal to \a rhs.
		bool operator!=(TokenUnit rhs) const {
			return !(*this == rhs);
		}

		/// Insertion operator for outputting unit to \a out.
		friend std::ostream& operator<<(std::ostream& out, TokenUnit unit) {
			utils::StreamFormatGuard guard(out, std::ios::dec, '0');
			out << unit.basicUnit();
			if (unit.isFractional())
				out << '.' << std::setw(Divisibility) << (unit.m_value % GetAtomicUnitMultiplier());

			return out;
		}

	private:
		utils::ImmutableValue<ValueType> m_value;
	};
}}
