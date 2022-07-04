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
#include "bitxorcore/observers/NotificationObserver.h"
#include "bitxorcore/observers/ObserverTypes.h"
#include "tests/test/core/TaggedNotification.h"
#include <vector>

namespace bitxorcore { namespace mocks {

	/// Creates a (type 1) tagged breadcrumb observer with \a tag and \a breadcrumbs.
	observers::NotificationObserverPointerT<test::TaggedNotification> CreateTaggedBreadcrumbObserver(
			uint8_t tag,
			std::vector<uint16_t>& breadcrumbs);

	/// Creates a (type 2) tagged breadcrumb observer with \a tag and \a breadcrumbs.
	observers::NotificationObserverPointerT<test::TaggedNotification2> CreateTaggedBreadcrumbObserver2(
			uint8_t tag,
			std::vector<uint16_t>& breadcrumbs);
}}
