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

#include "Validators.h"
#include "src/cache/NamespaceCache.h"
#include "bitxorcore/validators/ValidatorContext.h"

namespace bitxorcore { namespace validators {

	namespace {
		template<typename TValidatorTraits>
		ValidationResult UnlinkAliasedDataConsistencyValidator(
				const typename TValidatorTraits::NotificationType& notification,
				const ValidatorContext& context) {
			if (model::AliasAction::Unlink != notification.AliasAction)
				return ValidationResult::Success;

			const auto& cache = context.Cache.sub<cache::NamespaceCache>();
			auto namespaceIter = cache.find(notification.NamespaceId);
			if (!namespaceIter.tryGet())
				return Failure_Namespace_Unknown;

			const auto& entry = namespaceIter.get();
			auto aliasType = entry.root().alias(notification.NamespaceId).type();
			if (TValidatorTraits::AliasedType != aliasType)
				return Failure_Namespace_Alias_Inconsistent_Unlink_Type;

			return TValidatorTraits::GetAliased(entry.root().alias(notification.NamespaceId)) != notification.AliasedData
					? Failure_Namespace_Alias_Inconsistent_Unlink_Data
					: ValidationResult::Success;
		}
	}

#define DEFINE_UNLINK_ALIASED_DATA_VALIDATOR(VALIDATOR_NAME, TRAITS_NAME) \
	DEFINE_STATEFUL_VALIDATOR_WITH_TYPE(VALIDATOR_NAME, TRAITS_NAME::NotificationType, &UnlinkAliasedDataConsistencyValidator<TRAITS_NAME>)

	namespace {
		struct AddressTraits {
		public:
			using NotificationType = model::AliasedAddressNotification;
			static constexpr auto AliasedType = state::AliasType::Address;

		public:
			static const auto& GetAliased(const state::NamespaceAlias& namespaceAlias) {
				return namespaceAlias.address();
			}
		};

		struct TokenIdTraits {
		public:
			using NotificationType = model::AliasedTokenIdNotification;
			static constexpr auto AliasedType = state::AliasType::Token;

		public:
			static auto GetAliased(const state::NamespaceAlias& namespaceAlias) {
				return namespaceAlias.tokenId();
			}
		};
	}

	DEFINE_UNLINK_ALIASED_DATA_VALIDATOR(UnlinkAliasedAddressConsistency, AddressTraits)
	DEFINE_UNLINK_ALIASED_DATA_VALIDATOR(UnlinkAliasedTokenIdConsistency, TokenIdTraits)
}}
