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

#include "CompactTokenMap.h"
#include "bitxorcore/utils/Casting.h"

namespace bitxorcore { namespace state {

	// region FirstLevelStorage

	bool CompactTokenMap::FirstLevelStorage::hasValue() const {
		return TokenId() != Value.ConstToken.first;
	}

	bool CompactTokenMap::FirstLevelStorage::hasArray() const {
		return !!pNextStorage;
	}

	bool CompactTokenMap::FirstLevelStorage::hasMap() const {
		return !!pNextStorage && !!pNextStorage->pMapStorage;
	}

	uint8_t& CompactTokenMap::FirstLevelStorage::arraySize() const {
		return pNextStorage->ArraySize;
	}

	CompactTokenMap::TokenArray& CompactTokenMap::FirstLevelStorage::array() const {
		return pNextStorage->ArrayStorage;
	}

	CompactTokenMap::TokenMap& CompactTokenMap::FirstLevelStorage::map() const {
		return *pNextStorage->pMapStorage;
	}

	// endregion

	// region basic_iterator

	CompactTokenMap::basic_iterator::basic_iterator(FirstLevelStorage& storage, Stage stage)
			: m_storage(storage)
			, m_stage(stage)
			, m_pCurrent(nullptr) {
		advance();
	}

	CompactTokenMap::basic_iterator::basic_iterator(FirstLevelStorage& storage, const TokenLocation& location)
			: m_storage(storage)
			, m_stage(static_cast<Stage>(utils::to_underlying_type(location.Source) + 1))
			, m_arrayIndex(location.ArrayIndex)
			, m_mapIterator(location.MapIterator) {
		switch (location.Source) {
		case TokenSource::Value:
			setValueToken();
			break;

		case TokenSource::Array:
			setArrayToken();
			break;

		case TokenSource::Map:
			setMapToken();
			break;
		}
	}

	bool CompactTokenMap::basic_iterator::operator==(const basic_iterator& rhs) const {
		return &m_storage == &rhs.m_storage && m_pCurrent == rhs.m_pCurrent;
	}

	bool CompactTokenMap::basic_iterator::operator!=(const basic_iterator& rhs) const {
		return !(*this == rhs);
	}

	CompactTokenMap::basic_iterator& CompactTokenMap::basic_iterator::operator++() {
		if (isEnd())
			BITXORCORE_THROW_OUT_OF_RANGE("cannot advance iterator beyond end");

		advance();
		return *this;
	}

	CompactTokenMap::basic_iterator CompactTokenMap::basic_iterator::operator++(int) {
		auto copy = *this;
		++*this;
		return copy;
	}

	CompactTokenMap::Token& CompactTokenMap::basic_iterator::current() const {
		if (isEnd())
			BITXORCORE_THROW_OUT_OF_RANGE("cannot dereference at end");

		return *m_pCurrent;
	}

	void CompactTokenMap::basic_iterator::advance() {
		switch (m_stage) {
		case Stage::Start:
			return m_storage.hasValue() ? setValueToken() : setEnd();

		case Stage::Value:
			m_arrayIndex = 0;
			return m_storage.hasArray() ? setArrayToken() : setEnd();

		case Stage::Array:
			if (++m_arrayIndex != m_storage.arraySize())
				return setArrayToken();

			if (!m_storage.hasMap())
				return setEnd();

			m_mapIterator = m_storage.map().begin();
			return setMapToken();

		case Stage::Map:
			return m_storage.map().cend() != ++m_mapIterator ? setMapToken() : setEnd();

		case Stage::End:
			return;
		}
	}

	void CompactTokenMap::basic_iterator::setValueToken() {
		m_stage = Stage::Value;
		m_pCurrent = &m_storage.Value.ConstToken;
	}

	void CompactTokenMap::basic_iterator::setArrayToken() {
		m_stage = Stage::Array;
		m_pCurrent = &m_storage.array()[m_arrayIndex].ConstToken;
	}

	void CompactTokenMap::basic_iterator::setMapToken() {
		m_stage = Stage::Map;
		m_pCurrent = &*m_mapIterator;
	}

	void CompactTokenMap::basic_iterator::setEnd() {
		m_stage = Stage::End;
		m_pCurrent = nullptr;
	}

	bool CompactTokenMap::basic_iterator::isEnd() const {
		return Stage::End == m_stage;
	}

	// endregion

	CompactTokenMap::const_iterator CompactTokenMap::begin() const {
		return const_iterator(const_cast<FirstLevelStorage&>(m_storage), const_iterator::Stage::Start);
	}

	CompactTokenMap::const_iterator CompactTokenMap::end() const {
		return const_iterator(const_cast<FirstLevelStorage&>(m_storage), const_iterator::Stage::End);
	}

	CompactTokenMap::iterator CompactTokenMap::begin() {
		return iterator(m_storage, const_iterator::Stage::Start);
	}

	CompactTokenMap::iterator CompactTokenMap::end() {
		return iterator(m_storage, const_iterator::Stage::End);
	}

	bool CompactTokenMap::empty() const {
		return !m_storage.hasValue();
	}

	size_t CompactTokenMap::size() const {
		if (empty())
			return 0;

		size_t count = 1;
		if (m_storage.hasArray()) {
			count += m_storage.arraySize();
			if (m_storage.hasMap())
				count += m_storage.map().size();
		}

		return count;
	}

	CompactTokenMap::const_iterator CompactTokenMap::find(TokenId id) const {
		TokenLocation location;
		return find(id, location) ? const_iterator(const_cast<FirstLevelStorage&>(m_storage), location) : end();
	}

	CompactTokenMap::iterator CompactTokenMap::find(TokenId id) {
		TokenLocation location;
		return find(id, location) ? iterator(m_storage, location) : end();
	}

	namespace {
		bool IsLessThan(TokenId optimizedTokenId, TokenId lhs, TokenId rhs) {
			// customize so that optimized token id is smallest
			if (lhs == rhs)
				return false;

			if (optimizedTokenId == lhs || optimizedTokenId == rhs)
				return optimizedTokenId == lhs;

			return lhs < rhs;
		}
	}

	void CompactTokenMap::insert(const Token& pair) {
		if (TokenId() == pair.first)
			BITXORCORE_THROW_INVALID_ARGUMENT_1("cannot insert reserved token", pair.first);

		if (end() != utils::as_const(*this).find(pair.first))
			BITXORCORE_THROW_INVALID_ARGUMENT_1("cannot insert token already in map", pair.first);

		if (empty()) {
			m_storage.Value.Token = pair;
			return;
		}

		// use insertion sort to insert into array
		if (!m_storage.hasArray())
			m_storage.pNextStorage = std::make_unique<SecondLevelStorage>();

		if (IsLessThan(m_optimizedTokenId, pair.first, m_storage.Value.ConstToken.first)) {
			insertIntoArray(0, m_storage.Value.ConstToken);
			m_storage.Value.Token = pair;
			return;
		}

		auto arrayIndex = 0u;
		for (; arrayIndex < m_storage.arraySize(); ++arrayIndex) {
			if (pair.first < m_storage.array()[arrayIndex].ConstToken.first)
				break;
		}

		if (arrayIndex < Array_Size) {
			insertIntoArray(arrayIndex, pair);
			return;
		}

		// if all array values are smaller, insert into map
		insertIntoMap(pair);
	}

	void CompactTokenMap::erase(TokenId id) {
		TokenLocation location;
		if (!find(id, location))
			return;

		switch (location.Source) {
		case TokenSource::Value:
			if (m_storage.hasArray()) {
				m_storage.Value.Token = m_storage.array()[0].ConstToken;
				eraseFromArray(0);
			} else {
				m_storage.Value.Token = MutableToken();
			}
			break;

		case TokenSource::Array:
			eraseFromArray(location.ArrayIndex);
			break;

		case TokenSource::Map:
			m_storage.map().erase(id);
			break;
		}

		compact();
	}

	namespace {
		void reinsert(CompactTokenMap& map, TokenId id) {
			auto iter = map.find(id);
			if (map.end() == iter)
				return;

			// copy and reinsert the matching token
			auto tokenCopy = *iter;
			map.erase(tokenCopy.first);
			map.insert(tokenCopy);
		}
	}

	void CompactTokenMap::optimize(TokenId id) {
		if (id == m_optimizedTokenId)
			return;

		auto previousOptimizedTokenId = m_optimizedTokenId;
		m_optimizedTokenId = id;

		reinsert(*this, previousOptimizedTokenId);
		reinsert(*this, m_optimizedTokenId);
	}

	bool CompactTokenMap::find(TokenId id, TokenLocation& location) const {
		if (empty())
			return false;

		if (id == m_storage.Value.ConstToken.first) {
			location.Source = TokenSource::Value;
			return true;
		}

		if (m_storage.hasArray()) {
			for (auto i = 0u; i < m_storage.arraySize(); ++i) {
				if (id == m_storage.array()[i].ConstToken.first) {
					location.Source = TokenSource::Array;
					location.ArrayIndex = i;
					return true;
				}
			}
		}

		if (m_storage.hasMap()) {
			auto iter = m_storage.map().find(id);
			if (m_storage.map().end() != iter) {
				location.Source = TokenSource::Map;
				location.MapIterator = iter;
				return true;
			}
		}

		return false;
	}

	void CompactTokenMap::insertIntoArray(size_t index, const Token& pair) {
		// move the last array value into the map
		if (Array_Size == m_storage.arraySize())
			insertIntoMap(m_storage.array().back().ConstToken);
		else
			++m_storage.arraySize();

		for (auto i = m_storage.arraySize(); i > index + 1; --i)
			m_storage.array()[i - 1].Token = m_storage.array()[i - 2].ConstToken;

		m_storage.array()[index].Token = pair;
	}

	void CompactTokenMap::insertIntoMap(const Token& pair) {
		if (!m_storage.hasMap())
			m_storage.pNextStorage->pMapStorage = std::make_unique<TokenMap>();

		m_storage.pNextStorage->pMapStorage->insert(pair);
	}

	void CompactTokenMap::eraseFromArray(size_t index) {
		for (auto i = index; i < m_storage.arraySize() - 1u; ++i)
			m_storage.array()[i].Token = m_storage.array()[i + 1].ConstToken;

		--m_storage.arraySize();
	}

	void CompactTokenMap::compact() {
		if (!m_storage.hasArray())
			return;

		if (m_storage.hasMap()) {
			// compact into array
			while (m_storage.arraySize() < Array_Size && !m_storage.map().empty()) {
				m_storage.array()[m_storage.arraySize()++].Token = *m_storage.map().cbegin();
				m_storage.map().erase(m_storage.map().cbegin());
			}

			if (m_storage.map().empty())
				m_storage.pNextStorage->pMapStorage.reset();
		}

		if (0 == m_storage.arraySize())
			m_storage.pNextStorage.reset();
	}
}}
