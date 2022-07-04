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
#include "bitxorcore/io/Stream.h"
#include "bitxorcore/subscribers/StateChangeSubscriber.h"
#include "bitxorcore/functions.h"
#include <memory>
#include <vector>

namespace bitxorcore { namespace cache { class CacheChangesStorage; } }

namespace bitxorcore { namespace local {

	/// Vector of cache changes storage unique pointers.
	using CacheChangesStorages = std::vector<std::unique_ptr<const cache::CacheChangesStorage>>;

	/// Creates a state change storage around \a pOutputStream using \a cacheChangesStoragesSupplier for creating storages
	/// used for serialization.
	/// \note Supplier is used because cache changes storages are not available when this storage is created.
	std::unique_ptr<subscribers::StateChangeSubscriber> CreateFileStateChangeStorage(
			std::unique_ptr<io::OutputStream>&& pOutputStream,
			const supplier<CacheChangesStorages>& cacheChangesStoragesSupplier);
}}
