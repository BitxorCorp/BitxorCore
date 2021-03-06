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
#include "DiagnosticCounterId.h"
#include "bitxorcore/functions.h"

namespace bitxorcore { namespace utils {

	/// Diagnostic counter.
	class DiagnosticCounter {
	public:
		/// Creates a counter around \a id and \a supplier.
		DiagnosticCounter(const DiagnosticCounterId& id, const supplier<uint64_t>& supplier)
				: m_id(id)
				, m_supplier(supplier)
		{}

	public:
		/// Gets the id.
		const DiagnosticCounterId& id() const {
			return m_id;
		}

		/// Gets the current value.
		uint64_t value() const {
			return m_supplier();
		}

	private:
		DiagnosticCounterId m_id;
		supplier<uint64_t> m_supplier;
	};
}}
