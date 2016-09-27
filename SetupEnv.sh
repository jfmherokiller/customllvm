#!/usr/bin/env bash
set -ev
export WORKDIR=`pwd`;
export INSTALLDIR=$WORKDIR\installme
mkdir $WORKDIR/llvm-build
cd $WORKDIR/llvm-build
cmake -GNinja -DCMAKE_INSTALL_PREFIX=$INSTALLDIR -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=WebAssembly $WORKDIR/llvm
ninja install
tar -zcvf myllvm.tar.gz $INSTALLDIR