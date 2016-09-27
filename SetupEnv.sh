#!/usr/bin/env bash
set -ev
export WORKDIR=`pwd`;
export INSTALLDIR=$WORKDIR/installme
mkdir $WORKDIR/llvm-build
cd $WORKDIR/llvm-build
cmake -GNinja -DCMAKE_INSTALL_PREFIX=$INSTALLDIR -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=WebAssembly -DCMAKE_BUILD_TYPE=Release $WORKDIR/llvm
ninja install
tar -zcvf myllvm.tar.gz $INSTALLDIR
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then mv myllvm.tar.gz myllvm-osx.tar.gz ; fi
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then mv myllvm.tar.gz myllvm-linux.tar.gz ;fi
