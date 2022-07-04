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

#include "AccountState.h"
#include "bitxorcore/model/Address.h"

namespace bitxorcore { namespace state {

	AccountState::AccountState(const bitxorcore::Address& address, Height addressHeight)
			: Address(address)
			, AddressHeight(addressHeight)
			, PublicKey()
			, PublicKeyHeight(0)
			, AccountType(AccountType::Unlinked)
	{}

	namespace {
		void RequireAccountType(const AccountState& accountState, AccountType requiredAccountType, const char* requiredAccountTypeName) {
			if (requiredAccountType == accountState.AccountType)
				return;

			std::ostringstream out;
			out << "expected " << model::AddressToString(accountState.Address) << " to have account type " << requiredAccountTypeName;
			BITXORCORE_THROW_RUNTIME_ERROR(out.str().c_str());
		}

		bool AreLinked(const AccountState& lhs, const AccountState& rhs) {
			return GetLinkedPublicKey(lhs) == rhs.PublicKey && GetLinkedPublicKey(rhs) == lhs.PublicKey;
		}
	}

	bool IsRemote(AccountType accountType) {
		switch (accountType) {
		case AccountType::Remote:
		case AccountType::Remote_Unlinked:
			return true;

		default:
			return false;
		}
	}

	bool HasHistoricalInformation(const AccountState& accountState) {
		return !accountState.ImportanceSnapshots.empty() || !accountState.ActivityBuckets.empty();
	}

	void RequireLinkedRemoteAndMainAccounts(const AccountState& remoteAccountState, const AccountState& mainAccountState) {
		RequireAccountType(remoteAccountState, AccountType::Remote, "REMOTE");
		RequireAccountType(mainAccountState, AccountType::Main, "MAIN");

		if (AreLinked(mainAccountState, remoteAccountState))
			return;

		std::ostringstream out;
		out
				<< "remote " << model::AddressToString(remoteAccountState.Address) << " link to main "
				<< model::AddressToString(mainAccountState.Address) << " is improper";
		BITXORCORE_THROW_RUNTIME_ERROR(out.str().c_str());
	}

	void ApplyFeeSurplus(AccountState& accountState, const model::Token& fee, model::ImportanceHeight importanceHeight) {
		accountState.Balances.credit(fee.TokenId, fee.Amount);
		accountState.ActivityBuckets.tryUpdate(importanceHeight, [surplus = fee.Amount](auto& bucket) {
			bucket.TotalFeesPaid = bucket.TotalFeesPaid - surplus;
		});
	}

	Key GetLinkedPublicKey(const AccountState& accountState) {
		return accountState.SupplementalPublicKeys.linked().get();
	}

	Key GetNodePublicKey(const AccountState& accountState) {
		return accountState.SupplementalPublicKeys.node().get();
	}

	Key GetVrfPublicKey(const AccountState& accountState) {
		return accountState.SupplementalPublicKeys.vrf().get();
	}
}}
