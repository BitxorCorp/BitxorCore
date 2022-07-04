#!/bin/bash

set -ex

git_root_directory="$(git rev-parse --show-toplevel)"
scripts_directory="${git_root_directory}/jenkins/bitxorcore"
if [ -z "${JENKINS_HOME}" ]
then
	python3 "${scripts_directory}/runDockerTests.py" \
		--image bitxor/bitxor-server-test:gcc-10-local \
		--compiler-configuration "${scripts_directory}/configurations/gcc-10.yaml" \
		--user "$(id -u):$(id -g)" \
		--mode test \
		--verbosity suite \
		--source-path "${git_root_directory}"
else
	output_path="${git_root_directory}/output"
	data_path="${output_path}/bitxorcore-data"
	mkdir -p "${data_path}/workdir"
	cd "${data_path}/workdir"
	python3 "${scripts_directory}/runDockerTestsInnerTest.py" \
		--compiler-configuration "${scripts_directory}/configurations/gcc-11.yaml" \
		--exe-path "${output_path}/binaries/tests" \
		--out-dir "${data_path}" \
		--source-path "${git_root_directory}/client/bitxorcore" \
		--verbosity suite
fi
