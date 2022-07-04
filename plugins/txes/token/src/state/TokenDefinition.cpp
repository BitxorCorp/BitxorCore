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

#include "TokenDefinition.h"
#include "src/model/TokenConstants.h"
#include "bitxorcore/constants.h"

namespace bitxorcore { namespace state {

	Height TokenDefinition::startHeight() const {
		return m_startHeight;
	}

	const Address& TokenDefinition::ownerAddress() const {
		return m_ownerAddress;
	}

	uint32_t TokenDefinition::revision() const {
		return m_revision;
	}

	const model::TokenProperties& TokenDefinition::properties() const {
		return m_properties;
	}

	bool TokenDefinition::isEternal() const {
		return Eternal_Artifact_Duration == m_properties.duration();
	}

	bool TokenDefinition::isActive(Height height) const {
		return isEternal() || (height < Height(m_startHeight.unwrap() + m_properties.duration().unwrap()) && height >= m_startHeight);
	}

	bool TokenDefinition::isExpired(Height height) const {
		return !isEternal() && m_startHeight + Height(m_properties.duration().unwrap()) <= height;
	}
}}
