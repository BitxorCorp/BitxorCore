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
#include <bsoncxx/builder/stream/document.hpp>

namespace bitxorcore {
	namespace state {
		class TokenAddressRestriction;
		class TokenGlobalRestriction;
		class TokenRestrictionEntry;
	}
}

namespace bitxorcore { namespace test {

	/// Token address restriction traits for tests.
	struct TokenAddressRestrictionTestTraits {
		/// Creates a token address restriction with \a numValues values.
		static state::TokenAddressRestriction CreateRestriction(size_t numValues);

		/// Asserts that model \a restrictionEntry and dbmodel \a dbRestrictionEntry are equal.
		static void AssertEqualRestriction(
				const state::TokenRestrictionEntry& restrictionEntry,
				const bsoncxx::document::view& dbRestrictionEntry);
	};

	/// Token global restriction traits for tests.
	struct TokenGlobalRestrictionTestTraits {
		/// Creates a token global restriction with \a numValues values.
		static state::TokenGlobalRestriction CreateRestriction(size_t numValues);

		/// Asserts that model \a restrictionEntry and dbmodel \a dbRestrictionEntry are equal.
		static void AssertEqualRestriction(
				const state::TokenRestrictionEntry& restrictionEntry,
				const bsoncxx::document::view& dbRestrictionEntry);
	};
}}
