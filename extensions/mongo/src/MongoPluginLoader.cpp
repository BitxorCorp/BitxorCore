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

#include "MongoPluginLoader.h"
#include "MongoPluginManager.h"
#include "bitxorcore/plugins/PluginExceptions.h"
#include "bitxorcore/utils/Logging.h"
#include "bitxorcore/preprocessor.h"
#include <boost/exception_ptr.hpp>

namespace bitxorcore { namespace mongo {

	namespace {
		plugins::PluginModule::Scope GetSymbolScope() {
#ifdef STRICT_SYMBOL_VISIBILITY
			// MemoryCacheChanges<X> typeinfos need to be merged between mongo and non-mongo plugins
			return plugins::PluginModule::Scope::Global;
#else
			return plugins::PluginModule::Scope::Local;
#endif
		}

		void LoadPlugin(MongoPluginManager& manager, const plugins::PluginModule& module, const char* symbolName) {
			auto registerSubsystem = module.symbol<decltype(::RegisterMongoSubsystem)*>(symbolName);

			try {
				registerSubsystem(manager);
			} catch (...) {
				// since the module will be unloaded after this function exits, throw a copy of the exception that
				// is not dependent on the (soon to be unloaded) module
				auto exInfo = boost::diagnostic_information(boost::current_exception());
				BITXORCORE_THROW_AND_LOG_0(plugins::plugin_load_error, exInfo.c_str());
			}
		}
	}

	void LoadPluginByName(MongoPluginManager& manager, PluginModules& modules, const std::string& directory, const std::string& name) {
		BITXORCORE_LOG(info) << "registering dynamic mongo plugin " << name;

		modules.emplace_back(directory, name, GetSymbolScope());
		LoadPlugin(manager, modules.back(), "RegisterMongoSubsystem");
	}
}}
