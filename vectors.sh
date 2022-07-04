#!/bin/bash

set -ex

./_build/bin/bitxorcore.tools.testvectors --vectors "$(git rev-parse --show-toplevel)/tests/vectors/bitxor/crypto"
