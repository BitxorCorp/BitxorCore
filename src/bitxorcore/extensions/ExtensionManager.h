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
#include "ServiceRegistrar.h"
#include "bitxorcore/functions.h"
#include "bitxorcore/types.h"
#include <memory>
#include <vector>

namespace bitxorcore {
	namespace cache { class BitxorCoreCache; }
	namespace utils { class TimeSpan; }
}

namespace bitxorcore { namespace extensions {

	/// Manager for registering extensions.
	class ExtensionManager {
	public:
		/// Supplier that returns the network time.
		using NetworkTimeSupplier = supplier<Timestamp>;

	public:
		/// Creates a manager.
		ExtensionManager();

	public:
		/// Registers a system plugin with \a name.
		void registerSystemPlugin(const std::string& name);

		/// Registers a network time \a supplier.
		void setNetworkTimeSupplier(const NetworkTimeSupplier& supplier);

		/// Adds a service registrar (\a pServiceRegistrar).
		void addServiceRegistrar(std::unique_ptr<ServiceRegistrar>&& pServiceRegistrar);

	public:
		/// Gets the system plugin names.
		const std::vector<std::string>& systemPluginNames() const;

		/// Gets the network time supplier given \a epochAdjustment.
		NetworkTimeSupplier networkTimeSupplier(const utils::TimeSpan& epochAdjustment) const;

		/// Registers all services by forwarding \a locator and \a state.
		void registerServices(ServiceLocator& locator, ServiceState& state);

	private:
		std::vector<std::string> m_systemPluginNames;
		NetworkTimeSupplier m_networkTimeSupplier;
		std::vector<std::unique_ptr<ServiceRegistrar>> m_serviceRegistrars;
	};
}}
