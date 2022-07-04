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
#include "BitxorCoreCacheDelta.h"
#include "bitxorcore/types.h"

namespace bitxorcore { namespace cache { class BitxorCoreCache; } }

namespace bitxorcore { namespace cache {

	/// Relockable detached bitxorcore cache.
	class RelockableDetachedBitxorCoreCache {
	public:
		/// Creates a relockable detached bitxorcore cache around \a bitxorcoreCache.
		explicit RelockableDetachedBitxorCoreCache(const BitxorCoreCache& bitxorcoreCache);

		/// Destroys the relockable detached bitxorcore cache.
		~RelockableDetachedBitxorCoreCache();

	public:
		/// Gets the current cache height.
		Height height() const;

		/// Gets the last (detached) bitxorcore cache delta and locks it.
		/// \note If locking fails, \c nullptr is returned.
		std::unique_ptr<BitxorCoreCacheDelta> getAndTryLock();

		/// Rebases and locks the (detached) bitxorcore cache delta.
		std::unique_ptr<BitxorCoreCacheDelta> rebaseAndLock();

	private:
		class Impl;
		std::unique_ptr<Impl> m_pImpl;
	};
}}
