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

#include "Observers.h"
#include "src/cache/NamespaceCache.h"

namespace bitxorcore { namespace observers {

	DEFINE_OBSERVER(IsoCodeNamespace, model::IsoCodeNamespaceNotification, [](
			const model::IsoCodeNamespaceNotification& notification,
			const ObserverContext& context) {
		auto& cache = context.Cache.sub<cache::NamespaceCache>();

		if (NotifyMode::Rollback == context.Mode) {
			cache.remove(notification.NamespaceId);
			return;
		}

		// make copy of parent path and append child id
		auto namespaceIter = cache.find(notification.ParentId);
		const auto& parentEntry = namespaceIter.get();
		auto childPath = parentEntry.ns().path();
		childPath.push_back(notification.NamespaceId);
		cache.insert(state::Namespace(childPath));
	})
}}
