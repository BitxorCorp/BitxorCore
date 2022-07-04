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

#include "TokenRestrictionEntrySerializer.h"
#include "bitxorcore/io/PodIoUtils.h"
#include "bitxorcore/utils/Casting.h"

namespace bitxorcore { namespace state {

	// region Save

	namespace {
		template<typename TSaveKeyPayload>
		void SaveKeyValuePairs(const std::set<uint64_t>& keys, io::OutputStream& output, TSaveKeyPayload saveKeyPayload) {
			io::Write8(output, static_cast<uint8_t>(keys.size()));

			for (auto key : keys) {
				io::Write64(output, key);
				saveKeyPayload(key);
			}
		}

		void SaveRestriction(const TokenAddressRestriction& restriction, io::OutputStream& output) {
			io::Write(output, restriction.tokenId());
			output.write(restriction.address());

			SaveKeyValuePairs(restriction.keys(), output, [&restriction, &output](auto key) {
				auto value = restriction.get(key);

				io::Write64(output, value);
			});
		}

		void SaveRestriction(const TokenGlobalRestriction& restriction, io::OutputStream& output) {
			io::Write(output, restriction.tokenId());

			SaveKeyValuePairs(restriction.keys(), output, [&restriction, &output](auto key) {
				TokenGlobalRestriction::RestrictionRule rule;
				restriction.tryGet(key, rule);

				io::Write(output, rule.ReferenceTokenId);
				io::Write64(output, rule.RestrictionValue);
				io::Write8(output, utils::to_underlying_type(rule.RestrictionType));
			});
		}
	}

	void TokenRestrictionEntrySerializer::Save(const TokenRestrictionEntry& entry, io::OutputStream& output) {
		io::Write8(output, utils::to_underlying_type(entry.entryType()));
		if (TokenRestrictionEntry::EntryType::Address == entry.entryType())
			SaveRestriction(entry.asAddressRestriction(), output);
		else
			SaveRestriction(entry.asGlobalRestriction(), output);
	}

	// endregion

	// region Load

	namespace {
		template<typename TLoadKeyPayload>
		void LoadKeyValuePairs(io::InputStream& input, TLoadKeyPayload loadKeyPayload) {
			auto numKeys = io::Read8(input);

			for (auto i = 0u; i < numKeys; ++i) {
				auto key = io::Read64(input);
				loadKeyPayload(key);
			}
		}

		TokenAddressRestriction LoadAddressRestriction(io::InputStream& input) {
			auto tokenId = io::Read<TokenId>(input);
			Address address;
			input.read(address);

			auto restriction = TokenAddressRestriction(tokenId, address);
			LoadKeyValuePairs(input, [&restriction, &input](auto key) {
				auto value = io::Read64(input);

				restriction.set(key, value);
			});

			return restriction;
		}

		TokenGlobalRestriction LoadGlobalRestriction(io::InputStream& input) {
			auto tokenId = io::Read<TokenId>(input);

			auto restriction = TokenGlobalRestriction(tokenId);
			LoadKeyValuePairs(input, [&restriction, &input](auto key) {
				TokenGlobalRestriction::RestrictionRule rule;
				rule.ReferenceTokenId = io::Read<TokenId>(input);
				rule.RestrictionValue = io::Read64(input);
				rule.RestrictionType = static_cast<model::TokenRestrictionType>(io::Read8(input));

				restriction.set(key, rule);
			});

			return restriction;
		}
	}

	TokenRestrictionEntry TokenRestrictionEntrySerializer::Load(io::InputStream& input) {
		auto entryType = static_cast<TokenRestrictionEntry::EntryType>(io::Read8(input));
		switch (entryType) {
		case TokenRestrictionEntry::EntryType::Address:
			return TokenRestrictionEntry(LoadAddressRestriction(input));

		case TokenRestrictionEntry::EntryType::Global:
			return TokenRestrictionEntry(LoadGlobalRestriction(input));
		}

		BITXORCORE_THROW_INVALID_ARGUMENT_1(
				"cannot load token restriction entry with unsupported entry type",
				static_cast<uint16_t>(entryType));
	}

	// endregion
}}
