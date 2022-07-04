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

#include "bitxorcore/extensions/ProcessBootstrapper.h"
#include "bitxorcore/local/server/LocalNode.h"
#include "bitxorcore/process/ProcessMain.h"

namespace {
	constexpr auto Process_Name = "server";
}

int main(int argc, const char** argv) {
	using namespace bitxorcore;
	return process::ProcessMain(argc, argv, Process_Name, [argc, argv](auto&& config, const auto& keys) {
		// create bootstrapper
		auto resourcesPath = process::GetResourcesPath(argc, argv).generic_string();
		auto disposition = extensions::ProcessDisposition::Production;
		auto pBootstrapper = std::make_unique<extensions::ProcessBootstrapper>(config, resourcesPath, disposition, Process_Name);
		AddStaticNodesFromPath(*pBootstrapper, (std::filesystem::path(resourcesPath) / "peers-p2p.json").generic_string());

		// register extension(s)
		pBootstrapper->loadExtensions();

		// create the local node
		return local::CreateLocalNode(keys, std::move(pBootstrapper));
	});
}
