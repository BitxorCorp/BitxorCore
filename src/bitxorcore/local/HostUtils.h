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
#include "bitxorcore/plugins/PluginModule.h"
#include "bitxorcore/utils/ExceptionLogging.h"
#include "bitxorcore/exceptions.h"
#include <memory>
#include <vector>

namespace bitxorcore { namespace extensions { class ProcessBootstrapper; } }

namespace bitxorcore { namespace local {

	/// Creates and boots a host with \a args.
	template<typename THost, typename... TArgs>
	std::unique_ptr<THost> CreateAndBootHost(TArgs&&... args) {
		// create and boot the host
		auto pHost = std::make_unique<THost>(std::forward<TArgs>(args)...);
		try {
			pHost->boot();
		} catch (const std::exception& ex) {
			// log the exception and rethrow as a new exception in case the exception source is a dynamic plugin
			// (this avoids a potential crash in the calling code, which would occur if the host destructor unloads the source plugin)
			BITXORCORE_LOG(fatal) << UNHANDLED_EXCEPTION_MESSAGE("boot");
			BITXORCORE_THROW_RUNTIME_ERROR(ex.what());
		}

		return pHost;
	}

	/// Loads all plugins using \a bootstrapper.
	std::vector<plugins::PluginModule> LoadAllPlugins(extensions::ProcessBootstrapper& bootstrapper);
}}
