#!/bin/bash
dir=$(cd -P -- "$(dirname -- "$0")" && pwd -P)
cd $dir

cd ./openthread/third_party/nlbuild-autotools/repo/
make tools
cd ../../../../../../
cd ./openthread/third_party/mbedtls/repo/
make
cd ../../../../
