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
#include "bitxorcore/utils/Hashers.h"
#include "bitxorcore/utils/NonCopyable.h"
#include "bitxorcore/exceptions.h"
#include "bitxorcore/types.h"
#include <map>

namespace bitxorcore { namespace state {

	/// Token (ordered) map that is optimized for storage of a small number of elements.
	/// \note This map assumes that TokenId(0) is not a valid token.
	///       This is acceptable for tokens stored in AccountBalances but not for a general purpose map.
	class CompactTokenMap : utils::MoveOnly {
	private:
		static constexpr auto Array_Size = 5;

		// in order for this map to behave like std::unordered_map, the element type needs to be a pair, not model::Token
		using Token = std::pair<const TokenId, Amount>;
		using MutableToken = std::pair<TokenId, Amount>;

		// gcc strict aliasing workaround because pair<X, Y> and pair<const X, Y> are two different types
		struct TokenUnion {
		public:
			TokenUnion() : Token()
			{}

			TokenUnion(TokenUnion&& rhs) : Token(std::move(rhs.Token))
			{}

		public:
			TokenUnion& operator=(TokenUnion&& rhs) {
				Token = std::move(rhs.Token);
				return *this;
			}

		public:
			union {
				MutableToken Token;
				CompactTokenMap::Token ConstToken;
			};
		};

		using TokenArray = std::array<TokenUnion, Array_Size>;
		using TokenMap = std::map<TokenId, Amount>;

	private:
		struct SecondLevelStorage {
			TokenArray ArrayStorage;
			uint8_t ArraySize;
			std::unique_ptr<TokenMap> pMapStorage;
		};

		struct FirstLevelStorage {
		public:
			TokenUnion Value;
			std::unique_ptr<SecondLevelStorage> pNextStorage;

		public:
			bool hasValue() const;

			bool hasArray() const;

			bool hasMap() const;

		public:
			uint8_t& arraySize() const;

			TokenArray& array() const;

			TokenMap& map() const;
		};

	private:
		enum class TokenSource { Value, Array, Map };

		struct TokenLocation {
		public:
			TokenLocation()
					: Source(static_cast<TokenSource>(-1))
					, ArrayIndex(0)
					, MapIterator()
			{}

		public:
			TokenSource Source;
			size_t ArrayIndex;
			TokenMap::iterator MapIterator;
		};

	private:
		/// Base of token iterators.
		class basic_iterator {
		public:
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::forward_iterator_tag;

		public:
			/// Iterator stage.
			enum class Stage { Start, Value, Array, Map, End };

		public:
			/// Creates an iterator around \a storage with initial \a stage.
			basic_iterator(FirstLevelStorage& storage, Stage stage);

			/// Creates an iterator around \a storage pointing to the token at \a location.
			basic_iterator(FirstLevelStorage& storage, const TokenLocation& location);

		public:
			/// Returns \c true if this iterator and \a rhs are equal.
			bool operator==(const basic_iterator& rhs) const;

			/// Returns \c true if this iterator and \a rhs are not equal.
			bool operator!=(const basic_iterator& rhs) const;

		public:
			/// Advances the iterator to the next position.
			basic_iterator& operator++();

			/// Advances the iterator to the next position.
			basic_iterator operator++(int);

		protected:
			/// Gets a reference to the current value.
			Token& current() const;

		private:
			void advance();

			void setValueToken();

			void setArrayToken();

			void setMapToken();

			void setEnd();

			bool isEnd() const;

		private:
			FirstLevelStorage& m_storage;
			Stage m_stage;
			Token* m_pCurrent;

			// indexing into sub containers
			size_t m_arrayIndex;
			TokenMap::iterator m_mapIterator;
		};

		/// Basic typed iterator that adds support for dereferencing.
		template<typename T>
		class basic_iterator_t : public basic_iterator {
		public:
			using value_type = T;
			using pointer = value_type*;
			using reference = value_type&;

		public:
			using basic_iterator::basic_iterator;

		public:
			/// Gets a reference to the current value.
			reference operator*() const {
				return current();
			}

			/// Gets a pointer to the current value.
			pointer operator->() const {
				return &current();
			}
		};

	public:
		/// Token const iterator.
		using const_iterator = basic_iterator_t<const Token>;

		/// Token non-const iterator.
		using iterator = basic_iterator_t<Token>;

	public:
		/// Gets a const iterator to the first element of the underlying container.
		const_iterator begin() const;

		/// Gets a const iterator to the element following the last element of the underlying container.
		const_iterator end() const;

		/// Gets an iterator to the first element of the underlying container.
		iterator begin();

		/// Gets an iterator to the element following the last element of the underlying container.
		iterator end();

	public:
		/// Returns \c true if the map is empty.
		bool empty() const;

		/// Gets the number of tokens in the map.
		size_t size() const;

		/// Finds the token with \a id.
		const_iterator find(TokenId id) const;

		/// Finds the token with \a id.
		iterator find(TokenId id);

		/// Inserts a token \a pair.
		void insert(const Token& pair);

		/// Erases the token with \a id.
		void erase(TokenId id);

		/// Optimizes access of the token with \a id.
		void optimize(TokenId id);

	private:
		bool find(TokenId id, TokenLocation& location) const;

		void insertIntoArray(size_t index, const Token& pair);

		void insertIntoMap(const Token& pair);

		void eraseFromArray(size_t index);

		void compact();

	private:
		FirstLevelStorage m_storage;
		TokenId m_optimizedTokenId;
	};
}}
