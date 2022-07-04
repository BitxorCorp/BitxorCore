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

#include "Namespace.h"
#include "bitxorcore/exceptions.h"

namespace bitxorcore { namespace state {

	Namespace::Namespace(const Path& path) : m_path(path) {
		if (m_path.empty())
			BITXORCORE_THROW_OUT_OF_RANGE("path cannot be empty");
	}

	NamespaceId Namespace::id() const {
		return m_path[m_path.size() - 1];
	}

	NamespaceId Namespace::parentId() const {
		return 1 == m_path.size() ? Namespace_Base_Id : m_path[m_path.size() - 2];
	}

	NamespaceId Namespace::rootId() const {
		return m_path[0];
	}

	bool Namespace::isRoot() const {
		return 1 == m_path.size();
	}

	const Namespace::Path& Namespace::path() const {
		return m_path;
	}

	Namespace Namespace::createChild(NamespaceId id) const {
		Path childPath = m_path;
		childPath.push_back(id);
		return Namespace(childPath);
	}

	bool Namespace::operator==(const Namespace& rhs) const {
		return m_path == rhs.m_path;
	}

	bool Namespace::operator!=(const Namespace& rhs) const {
		return !(*this == rhs);
	}
}}
