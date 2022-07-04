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

#include "TokenEntrySerializer.h"
#include "bitxorcore/io/PodIoUtils.h"

namespace bitxorcore { namespace state {

	namespace {
		void SaveProperties(io::OutputStream& output, const model::TokenProperties& properties) {
			io::Write8(output, utils::to_underlying_type(properties.flags()));
			io::Write8(output, properties.divisibility());
			io::Write(output, properties.duration());
		}

		void SaveDefinition(io::OutputStream& output, const TokenDefinition& definition) {
			io::Write(output, definition.startHeight());
			output.write(definition.ownerAddress());
			io::Write32(output, definition.revision());

			SaveProperties(output, definition.properties());
		}
	}

	void TokenEntrySerializer::Save(const TokenEntry& entry, io::OutputStream& output) {
		io::Write(output, entry.tokenId());
		io::Write(output, entry.supply());
		SaveDefinition(output, entry.definition());
	}

	namespace {
		model::TokenProperties LoadProperties(io::InputStream& input) {
			auto flags = static_cast<model::TokenFlags>(io::Read8(input));
			auto divisibility = io::Read8(input);
			auto duration = io::Read<BlockDuration>(input);
			return model::TokenProperties(flags, divisibility, duration);
		}

		TokenDefinition LoadDefinition(io::InputStream& input) {
			Address owner;
			auto height = io::Read<Height>(input);
			input.read(owner);
			auto revision = io::Read32(input);

			auto properties = LoadProperties(input);
			return TokenDefinition(height, owner, revision, properties);
		}
	}

	TokenEntry TokenEntrySerializer::Load(io::InputStream& input) {
		auto tokenId = io::Read<TokenId>(input);
		auto supply = io::Read<Amount>(input);
		auto definition = LoadDefinition(input);

		auto entry = TokenEntry(tokenId, definition);
		entry.increaseSupply(supply);
		return entry;
	}
}}
