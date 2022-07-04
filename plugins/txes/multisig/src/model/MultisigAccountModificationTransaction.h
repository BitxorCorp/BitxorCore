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
#include "MultisigEntityType.h"
#include "bitxorcore/model/ContainerTypes.h"
#include "bitxorcore/model/Transaction.h"

namespace bitxorcore { namespace model {

#pragma pack(push, 1)

	/// Binary layout for a multisig account modification transaction body.
	template<typename THeader>
	struct MultisigAccountModificationTransactionBody : public THeader {
	private:
		using TransactionType = MultisigAccountModificationTransactionBody<THeader>;

	public:
		DEFINE_TRANSACTION_CONSTANTS(Entity_Type_Multisig_Account_Modification, 1)

	public:
		/// Relative change of the minimal number of cosignatories required when removing an account.
		int8_t MinRemovalDelta;

		/// Relative change of the minimal number of cosignatories required when approving a transaction.
		int8_t MinApprovalDelta;

		/// Number of cosignatory address additions.
		uint8_t AddressAdditionsCount;

		/// Number of cosignatory address deletions.
		uint8_t AddressDeletionsCount;

		/// Reserved padding to align AddressAdditions on 8-byte boundary.
		uint32_t MultisigAccountModificationTransactionBody_Reserved1;

		// followed by additions data if AddressAdditionsCount != 0
		DEFINE_TRANSACTION_VARIABLE_DATA_ACCESSORS(AddressAdditions, UnresolvedAddress)

		// followed by deletions data if AddressDeletionsCount != 0
		DEFINE_TRANSACTION_VARIABLE_DATA_ACCESSORS(AddressDeletions, UnresolvedAddress)

	private:
		template<typename T>
		static auto* AddressAdditionsPtrT(T& transaction) {
			return transaction.AddressAdditionsCount ? THeader::PayloadStart(transaction) : nullptr;
		}

		template<typename T>
		static auto* AddressDeletionsPtrT(T& transaction) {
			auto* pPayloadStart = THeader::PayloadStart(transaction);
			return transaction.AddressDeletionsCount && pPayloadStart
					? pPayloadStart + transaction.AddressAdditionsCount * Address::Size
					: nullptr;
		}

	public:
		/// Calculates the real size of a multisig account modification \a transaction.
		static constexpr uint64_t CalculateRealSize(const TransactionType& transaction) noexcept {
			return sizeof(TransactionType) + (transaction.AddressAdditionsCount + transaction.AddressDeletionsCount) * Address::Size;
		}
	};

	DEFINE_EMBEDDABLE_TRANSACTION(MultisigAccountModification)

#pragma pack(pop)

	/// Extracts addresses of additional accounts that must approve \a transaction.
	inline UnresolvedAddressSet ExtractAdditionalRequiredCosignatories(const EmbeddedMultisigAccountModificationTransaction& transaction) {
		UnresolvedAddressSet addedCosignatories;
		for (auto i = 0u; i < transaction.AddressAdditionsCount; ++i)
			addedCosignatories.insert(transaction.AddressAdditionsPtr()[i]);

		return addedCosignatories;
	}
}}
