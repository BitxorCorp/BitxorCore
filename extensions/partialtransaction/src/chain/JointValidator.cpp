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

#include "JointValidator.h"
#include "bitxorcore/cache/ReadOnlyBitxorCoreCache.h"
#include "bitxorcore/chain/ProcessContextsBuilder.h"
#include "bitxorcore/plugins/PluginManager.h"
#include "bitxorcore/validators/ValidatorContext.h"

using namespace bitxorcore::validators;

namespace bitxorcore { namespace chain {

	namespace {
		std::string CreateJointValidatorName(const std::string& statelessName, const std::string statefulName) {
			std::ostringstream out;
			out << "JointValidator (" << statelessName << ", " << statefulName << ")";
			return out.str();
		}

		class JointValidator : public stateless::NotificationValidator {
		public:
			JointValidator(
					const cache::BitxorCoreCache& cache,
					const TimeSupplier& timeSupplier,
					const plugins::PluginManager& pluginManager,
					const ValidationResultPredicate& isSuppressedFailure)
					: m_cache(cache)
					, m_timeSupplier(timeSupplier)
					, m_network(pluginManager.config().Network)
					, m_pStatelessValidator(pluginManager.createStatelessValidator(isSuppressedFailure))
					, m_pStatefulValidator(pluginManager.createStatefulValidator(isSuppressedFailure))
					, m_resolverContextFactory([&pluginManager](const auto& readOnlyCache) {
						return pluginManager.createResolverContext(readOnlyCache);
					})
					, m_name(CreateJointValidatorName(m_pStatelessValidator->name(), m_pStatefulValidator->name()))
			{}

		public:
			const std::string& name() const override {
				return m_name;
			}

			ValidationResult validate(const model::Notification& notification) const override {
				auto result = validateStateless(notification);
				if (IsValidationResultFailure(result))
					return result;

				auto statefulResult = validateStateful(notification);
				AggregateValidationResult(result, statefulResult);
				return result;
			}

		private:
			ValidationResult validateStateless(const model::Notification& notification) const {
				return m_pStatelessValidator->validate(notification);
			}

			ValidationResult validateStateful(const model::Notification& notification) const {
				auto cacheView = m_cache.createView();
				auto executionContextConfig = chain::ExecutionContextConfiguration{ m_network, m_resolverContextFactory };
				chain::ProcessContextsBuilder contextBuilder(cacheView.height(), m_timeSupplier(), executionContextConfig);
				contextBuilder.setCache(cacheView);

				auto validatorContext = contextBuilder.buildValidatorContext();
				return m_pStatefulValidator->validate(notification, validatorContext);
			}

		private:
			const cache::BitxorCoreCache& m_cache;
			TimeSupplier m_timeSupplier;
			model::NetworkInfo m_network;
			std::unique_ptr<const stateless::NotificationValidator> m_pStatelessValidator;
			std::unique_ptr<const stateful::NotificationValidator> m_pStatefulValidator;
			std::function<model::ResolverContext (const cache::ReadOnlyBitxorCoreCache&)> m_resolverContextFactory;
			std::string m_name;
		};
	}

	std::unique_ptr<const stateless::NotificationValidator> CreateJointValidator(
			const cache::BitxorCoreCache& cache,
			const TimeSupplier& timeSupplier,
			const plugins::PluginManager& pluginManager,
			const ValidationResultPredicate& isSuppressedFailure) {
		return std::make_unique<JointValidator>(cache, timeSupplier, pluginManager, isSuppressedFailure);
	}
}}
