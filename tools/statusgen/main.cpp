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

#include "tools/ToolMain.h"
#include "bitxorcore/model/FacilityCode.h"
#include "bitxorcore/utils/HexFormatter.h"
#include "../../src/bitxorcore/validators/ValidationResult.h" /* notice that validators are not in sdk */
#include <iostream>

using namespace bitxorcore::validators;

#define DEFINE_WELL_KNOWN_RESULT(CODE) Output(ValidationResult::CODE, #CODE)

#define CUSTOM_RESULT_DEFINITION 1
#undef DEFINE_VALIDATION_RESULT

#define STR(SYMBOL) #SYMBOL
#define DEFINE_VALIDATION_RESULT(SEVERITY, FACILITY, DESCRIPTION, CODE, FLAGS) \
		Output( \
				MakeValidationResult((ResultSeverity::SEVERITY), (FacilityCode::FACILITY), CODE, (ResultFlags::FLAGS)), \
				STR(SEVERITY##_##FACILITY##_##DESCRIPTION))

namespace bitxorcore { namespace tools { namespace statusgen {

	namespace {
		void Output(ValidationResult result, const std::string& friendlyName) {
			std::cout << "case 0x" << utils::HexFormat(result) << ": return '" << friendlyName << "';" << std::endl;
		}

		class StatusTool : public Tool {
		public:
			std::string name() const override {
				return "Status Generator";
			}

			void prepareOptions(OptionsBuilder&, OptionsPositional&) override {
			}

			int run(const Options&) override {
				DEFINE_WELL_KNOWN_RESULT(Success);
				DEFINE_WELL_KNOWN_RESULT(Neutral);
				DEFINE_WELL_KNOWN_RESULT(Failure);

				// allow this tool to reach into src and plugins and not be limited by bitxorcore-sdk
				#include "../../plugins/coresystem/src/validators/Results.h"
				#include "../../plugins/services/hashcache/src/validators/Results.h"
				#include "../../plugins/services/signature/src/validators/Results.h"
				#include "../../plugins/txes/account_link/src/validators/Results.h"
				#include "../../plugins/txes/aggregate/src/validators/Results.h"
				#include "../../plugins/txes/lock_hash/src/validators/Results.h"
				#include "../../plugins/txes/lock_secret/src/validators/Results.h"
				#include "../../plugins/txes/metadata/src/validators/Results.h"
				#include "../../plugins/txes/token/src/validators/Results.h"
				#include "../../plugins/txes/multisig/src/validators/Results.h"
				#include "../../plugins/txes/namespace/src/validators/Results.h"
				#include "../../plugins/txes/restriction_account/src/validators/Results.h"
				#include "../../plugins/txes/restriction_token/src/validators/Results.h"
				#include "../../plugins/txes/transfer/src/validators/Results.h"
				#include "../../src/bitxorcore/chain/ChainResults.h"
				#include "../../src/bitxorcore/consumers/ConsumerResults.h"
				#include "../../src/bitxorcore/extensions/Results.h"
				return 0;
			}
		};
	}
}}}

int main(int argc, const char** argv) {
	bitxorcore::tools::statusgen::StatusTool tool;
	return bitxorcore::tools::ToolMain(argc, argv, tool);
}
