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
#include "CacheChanges.h"

namespace bitxorcore {
	namespace io {
		class InputStream;
		class OutputStream;
	}
}

namespace bitxorcore { namespace cache {

	/// Interface for loading and saving cache changes.
	class PLUGIN_API_DEPENDENCY CacheChangesStorage {
	public:
		virtual ~CacheChangesStorage() = default;

	public:
		/// Gets the cache id.
		virtual size_t id() const = 0;

	public:
		/// Saves cache \a changes to \a output.
		virtual void saveAll(const CacheChanges& changes, io::OutputStream& output) const = 0;

		/// Loads cache changes from \a input.
		virtual std::unique_ptr<const MemoryCacheChanges> loadAll(io::InputStream& input) const = 0;

		/// Applies cache \a changes to the underlying cache.
		virtual void apply(const CacheChanges& changes) const = 0;
	};
}}
