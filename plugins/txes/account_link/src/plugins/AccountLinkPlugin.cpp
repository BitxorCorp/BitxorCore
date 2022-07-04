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

#include "AccountLinkPlugin.h"
#include "AccountKeyLinkTransactionPlugin.h"
#include "NodeKeyLinkTransactionPlugin.h"
#include "src/observers/Observers.h"
#include "src/validators/Validators.h"
#include "bitxorcore/keylink/KeyLinkObserver.h"
#include "bitxorcore/keylink/KeyLinkValidator.h"
#include "bitxorcore/plugins/PluginManager.h"

namespace bitxorcore { namespace plugins {

	namespace {
		struct NodeKeyAccessor {
			static constexpr auto Failure_Link_Already_Exists = validators::Failure_AccountLink_Link_Already_Exists;
			static constexpr auto Failure_Inconsistent_Unlink_Data = validators::Failure_AccountLink_Inconsistent_Unlink_Data;

			template<typename TAccountState>
			static auto& Get(TAccountState& accountState) {
				return accountState.SupplementalPublicKeys.node();
			}
		};
	}

	void RegisterAccountLinkSubsystem(PluginManager& manager) {
		manager.addTransactionSupport(CreateAccountKeyLinkTransactionPlugin());
		manager.addTransactionSupport(CreateNodeKeyLinkTransactionPlugin());

		manager.addStatefulValidatorHook([](auto& builder) {
			builder
				.add(validators::CreateAccountKeyLinkValidator())
				.add(validators::CreateNewRemoteAccountAvailabilityValidator())
				.add(validators::CreateRemoteSenderValidator())
				.add(validators::CreateRemoteInteractionValidator())
				.add(keylink::CreateKeyLinkValidator<model::NodeKeyLinkNotification, NodeKeyAccessor>("Node"));
		});

		manager.addObserverHook([](auto& builder) {
			builder
				.add(observers::CreateAccountKeyLinkObserver())
				.add(keylink::CreateKeyLinkObserver<model::NodeKeyLinkNotification, NodeKeyAccessor>("Node"));
		});
	}
}}

extern "C" PLUGIN_API
void RegisterSubsystem(bitxorcore::plugins::PluginManager& manager) {
	bitxorcore::plugins::RegisterAccountLinkSubsystem(manager);
}
