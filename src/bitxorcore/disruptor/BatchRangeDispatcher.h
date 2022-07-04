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
#include "ConsumerDispatcher.h"
#include "bitxorcore/utils/Casting.h"
#include "bitxorcore/utils/Hashers.h"
#include "bitxorcore/utils/SpinLock.h"
#include <unordered_map>
#include <vector>

namespace bitxorcore { namespace disruptor {

	/// Batches entity ranges for processing by a ConsumerDispatcher.
	template<typename TAnnotatedEntityRange>
	class BatchRangeDispatcher {
	private:
		using EntityRange = decltype(TAnnotatedEntityRange::Range);

		// region RangeGroupKey

		struct RangeGroupKey {
			model::NodeIdentity SourceIdentity;
			InputSource Source;
		};

		class RangeGroupKeyEquality {
		public:
			explicit RangeGroupKeyEquality(model::NodeIdentityEqualityStrategy strategy) : m_equality(strategy)
			{}

		public:
			bool operator()(const RangeGroupKey& lhs, const RangeGroupKey& rhs) const {
				return m_equality(lhs.SourceIdentity, rhs.SourceIdentity) && lhs.Source == rhs.Source;
			}

		private:
			model::NodeIdentityEquality m_equality;
		};

		class RangeGroupKeyHasher {
		public:
			explicit RangeGroupKeyHasher(model::NodeIdentityEqualityStrategy strategy) : m_hasher(strategy)
			{}

		public:
			size_t operator()(const RangeGroupKey& key) const {
				return m_hasher(key.SourceIdentity);
			}

		private:
			model::NodeIdentityHasher m_hasher;
		};

		// endregion

		using GroupedRangesMap = std::unordered_map<RangeGroupKey, std::vector<EntityRange>, RangeGroupKeyHasher, RangeGroupKeyEquality>;

	public:
		/// Creates a batch range dispatcher around \a dispatcher with specified \a equalityStrategy.
		BatchRangeDispatcher(ConsumerDispatcher& dispatcher, model::NodeIdentityEqualityStrategy equalityStrategy)
				: m_dispatcher(dispatcher)
				, m_equalityStrategy(equalityStrategy)
				, m_rangesMap(CreateGroupedRangesMap(m_equalityStrategy))
		{}

	public:
		/// Returns \c true if no ranges are currently queued.
		bool empty() const {
			utils::SpinLockGuard guard(m_lock);
			return m_rangesMap.empty();
		}

	public:
		/// Queues processing of \a range from \a source.
		void queue(TAnnotatedEntityRange&& range, InputSource source) {
			utils::SpinLockGuard guard(m_lock);
			m_rangesMap[{ range.SourceIdentity, source }].push_back(std::move(range.Range));
		}

		/// Dispatches all queued elements to the underlying dispatcher.
		void dispatch() {
			auto rangesMap = CreateGroupedRangesMap(m_equalityStrategy);

			{
				utils::SpinLockGuard guard(m_lock);
				rangesMap = std::move(m_rangesMap);
			}

			for (auto& pair : rangesMap) {
				auto mergedRange = EntityRange::MergeRanges(std::move(pair.second));
				m_dispatcher.processElement(ConsumerInput({ std::move(mergedRange), pair.first.SourceIdentity }, pair.first.Source));
			}
		}

	private:
		static GroupedRangesMap CreateGroupedRangesMap(model::NodeIdentityEqualityStrategy equalityStrategy) {
			return GroupedRangesMap(0, RangeGroupKeyHasher(equalityStrategy), RangeGroupKeyEquality(equalityStrategy));
		}

	private:
		ConsumerDispatcher& m_dispatcher;
		model::NodeIdentityEqualityStrategy m_equalityStrategy;
		GroupedRangesMap m_rangesMap;
		mutable utils::SpinLock m_lock;
	};
}}
