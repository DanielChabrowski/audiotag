#!/usr/bin/env bash

set -eux

readonly repo_root=$(git rev-parse --show-toplevel)

podman run -i --init --rm -v "$repo_root":/src mips-test /bin/bash -c <<EOF
cmake -S /src -B /src/build-mips -GNinja -DCMAKE_CXX_COMPILER=mips-linux-gnu-g++ -DBUILD_STATIC=ON
cd /src/build-mips
ninja
qemu-mips bin/unit_tests
EOF
