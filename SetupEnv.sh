#!/usr/bin/env bash
set -ev
export WORKDIR=`pwd`;
export INSTALLDIR=$WORKDIR
mkdir $WORKDIR/llvm-build
cd $WORKDIR/llvm-build
cmake -GNinja -DLLVM_ENABLE_LIBCXX=ON -DCMAKE_INSTALL_PREFIX=$INSTALLDIR -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=WebAssembly $WORKDIR/llvm 

