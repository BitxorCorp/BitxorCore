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

#include "FileFinalizationStorage.h"
#include "bitxorcore/io/PodIoUtils.h"

namespace bitxorcore { namespace filespooling {

	namespace {
		class FileFinalizationStorage final : public subscribers::FinalizationSubscriber {
		public:
			explicit FileFinalizationStorage(std::unique_ptr<io::OutputStream>&& pOutputStream) : m_pOutputStream(std::move(pOutputStream))
			{}

		public:
			void notifyFinalizedBlock(const model::FinalizationRound& round, Height height, const Hash256& hash) override {
				m_pOutputStream->write(hash);
				m_pOutputStream->write({ reinterpret_cast<const uint8_t*>(&round), sizeof(model::FinalizationRound) });
				io::Write(*m_pOutputStream, height);

				m_pOutputStream->flush();
			}

		private:
			std::unique_ptr<io::OutputStream> m_pOutputStream;
		};
	}

	std::unique_ptr<subscribers::FinalizationSubscriber> CreateFileFinalizationStorage(std::unique_ptr<io::OutputStream>&& pOutputStream) {
		return std::make_unique<FileFinalizationStorage>(std::move(pOutputStream));
	}
}}
