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

#include "AccountBalances.h"

namespace bitxorcore { namespace state {

	namespace {
		static constexpr bool IsZero(Amount amount) {
			return Amount(0) == amount;
		}
	}

	AccountBalances::AccountBalances() = default;

	AccountBalances::AccountBalances(const AccountBalances& accountBalances) {
		*this = accountBalances;
	}

	AccountBalances::AccountBalances(AccountBalances&& accountBalances) = default;

	AccountBalances& AccountBalances::operator=(const AccountBalances& accountBalances) {
		m_optimizedTokenId = accountBalances.optimizedTokenId();
		m_balances.optimize(m_optimizedTokenId);
		for (const auto& pair : accountBalances)
			m_balances.insert(pair);

		return *this;
	}

	AccountBalances& AccountBalances::operator=(AccountBalances&& accountBalances) = default;

	size_t AccountBalances::size() const {
		return m_balances.size();
	}

	CompactTokenMap::const_iterator AccountBalances::begin() const {
		return m_balances.begin();
	}

	CompactTokenMap::const_iterator AccountBalances::end() const {
		return m_balances.end();
	}

	TokenId AccountBalances::optimizedTokenId() const {
		return m_optimizedTokenId;
	}

	Amount AccountBalances::get(TokenId tokenId) const {
		auto iter = m_balances.find(tokenId);
		return m_balances.end() == iter ? Amount(0) : iter->second;
	}

	AccountBalances& AccountBalances::credit(TokenId tokenId, Amount amount) {
		if (IsZero(amount))
			return *this;

		auto iter = m_balances.find(tokenId);
		if (m_balances.end() == iter)
			m_balances.insert(std::make_pair(tokenId, amount));
		else
			iter->second = iter->second + amount;

		return *this;
	}

	AccountBalances& AccountBalances::debit(TokenId tokenId, Amount amount) {
		if (IsZero(amount))
			return *this;

		auto iter = m_balances.find(tokenId);
		auto hasZeroBalance = m_balances.end() == iter;
		if (hasZeroBalance || amount > iter->second) {
			auto currentBalance = hasZeroBalance ? Amount(0) : iter->second;
			std::ostringstream out;
			out
					<< "debit amount (" << amount << ") is greater than current balance (" << currentBalance
					<< ") for token " << utils::HexFormat(tokenId);
			BITXORCORE_THROW_RUNTIME_ERROR(out.str().c_str());
		}

		iter->second = iter->second - amount;
		if (IsZero(iter->second))
			m_balances.erase(tokenId);

		return *this;
	}

	void AccountBalances::optimize(TokenId id) {
		m_balances.optimize(id);
		m_optimizedTokenId = id;
	}
}}
