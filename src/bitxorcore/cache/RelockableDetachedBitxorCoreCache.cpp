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

#include "RelockableDetachedBitxorCoreCache.h"
#include "BitxorCoreCache.h"
#include "BitxorCoreCacheDetachedDelta.h"

namespace bitxorcore { namespace cache {

	namespace {
		// this wrapper is needed because BitxorCoreCacheDetachedDelta / LockableCacheDelta is not assignable
		// due to a const& member variable
		class DetachedDeltaWrapper {
		public:
			explicit DetachedDeltaWrapper(BitxorCoreCacheDetachedDelta&& detachedDelta)
					: m_detachedDelta(std::move(detachedDelta))
			{}

		public:
			auto tryLock() {
				return m_detachedDelta.tryLock();
			}

		private:
			BitxorCoreCacheDetachedDelta m_detachedDelta;
		};
	}

	class RelockableDetachedBitxorCoreCache::Impl final {
	public:
		explicit Impl(const BitxorCoreCache& bitxorcoreCache) : m_bitxorcoreCache(bitxorcoreCache) {
			rebaseAndLock();
		}

	public:
		Height height() const {
			return m_cacheHeight;
		}

		std::unique_ptr<BitxorCoreCacheDelta> getAndTryLock() {
			return m_pLockableUnconfirmedBitxorCoreCache->tryLock();
		}

		std::unique_ptr<BitxorCoreCacheDelta> rebaseAndLock() {
			auto detachableDelta = m_bitxorcoreCache.createDetachableDelta();
			m_cacheHeight = detachableDelta.height();
			m_pLockableUnconfirmedBitxorCoreCache = std::make_unique<DetachedDeltaWrapper>(detachableDelta.detach());
			return m_pLockableUnconfirmedBitxorCoreCache->tryLock();
		}

	private:
		const BitxorCoreCache& m_bitxorcoreCache;
		Height m_cacheHeight;
		std::unique_ptr<DetachedDeltaWrapper> m_pLockableUnconfirmedBitxorCoreCache;
	};

	RelockableDetachedBitxorCoreCache::RelockableDetachedBitxorCoreCache(const BitxorCoreCache& bitxorcoreCache)
			: m_pImpl(std::make_unique<Impl>(bitxorcoreCache))
	{}

	RelockableDetachedBitxorCoreCache::~RelockableDetachedBitxorCoreCache() = default;

	Height RelockableDetachedBitxorCoreCache::height() const {
		return m_pImpl->height();
	}

	std::unique_ptr<BitxorCoreCacheDelta> RelockableDetachedBitxorCoreCache::getAndTryLock() {
		return m_pImpl->getAndTryLock();
	}

	std::unique_ptr<BitxorCoreCacheDelta> RelockableDetachedBitxorCoreCache::rebaseAndLock() {
		return m_pImpl->rebaseAndLock();
	}
}}
