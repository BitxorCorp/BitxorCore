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

#include "bitxorcore/model/TrailingVariableDataLayout.h"
#include "bitxorcore/model/Token.h"
#include "bitxorcore/utils/MemoryUtils.h"
#include "tests/test/core/VariableSizedEntityTestUtils.h"
#include "tests/test/nodeps/Alignment.h"
#include "tests/TestHarness.h"

namespace bitxorcore { namespace model {

#define TEST_CLASS TrailingVariableDataLayoutTests

	// region TokenContainer

	namespace {
#pragma pack(push, 1)

		struct TokenContainer : public TrailingVariableDataLayout<TokenContainer, Token> {
		public:
			// fixed size header
			uint16_t TokensCount;

		public:
			const Token* TokensPtr() const {
				return TokensCount ? ToTypedPointer(PayloadStart(*this)) : nullptr;
			}

			Token* TokensPtr() {
				return TokensCount ? ToTypedPointer(PayloadStart(*this)) : nullptr;
			}

		public:
			static constexpr uint64_t CalculateRealSize(const TokenContainer& container) noexcept {
				return sizeof(TokenContainer) + container.TokensCount * sizeof(Token);
			}
		};

#pragma pack(pop)
	}

	// endregion

	// region size + alignment

#define LAYOUT_FIELDS FIELD(Size)

	TEST(TEST_CLASS, LayoutHasExpectedSize) {
		// Arrange:
		using Layout = TrailingVariableDataLayout<TokenContainer, Token>;
		auto expectedSize = 0u;

#define FIELD(X) expectedSize += SizeOf32<decltype(Layout::X)>();
		LAYOUT_FIELDS
#undef FIELD

		// Assert:
		EXPECT_EQ(expectedSize, sizeof(Layout));
		EXPECT_EQ(4u, sizeof(Layout));
	}

	TEST(TEST_CLASS, LayoutHasProperAlignment) {
		using Layout = TrailingVariableDataLayout<TokenContainer, Token>;

#define FIELD(X) EXPECT_ALIGNED(Layout, X);
		LAYOUT_FIELDS
#undef FIELD
	}

#undef LAYOUT_FIELDS

	// endregion

	// region attachment pointer tests

	namespace {
		struct TokenContainerTraits {
			static auto GenerateEntityWithAttachments(uint16_t count) {
				uint32_t entitySize = SizeOf32<TokenContainer>() + count * SizeOf32<Token>();
				auto pContainer = utils::MakeUniqueWithSize<TokenContainer>(entitySize);
				pContainer->Size = entitySize;
				pContainer->TokensCount = count;
				return pContainer;
			}

			template<typename TEntity>
			static auto GetAttachmentPointer(TEntity& entity) {
				return entity.TokensPtr();
			}
		};
	}

	DEFINE_ATTACHMENT_POINTER_TESTS(TEST_CLASS, TokenContainerTraits) // TokensPtr

	// endregion
}}
