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
#include "finalization/src/chain/FinalizationOrchestrator.h"
#include "bitxorcore/io/RawFile.h"

namespace bitxorcore { namespace finalization {

	/// File containing finalization voting status.
	class VotingStatusFile final {
	public:
		/// Creates a file with name \a filename.
		explicit VotingStatusFile(const std::string& filename);

	public:
		/// Loads the current status.
		chain::VotingStatus load() const;

		/// Saves \a status.
		void save(const chain::VotingStatus& status);

	private:
		io::RawFile open(io::OpenMode mode) const;

	private:
		std::string m_filename;
	};
}}
