#!/usr/bin/env bash
set -ev
export WORKDIR=`pwd`;
export INSTALLDIR=$WORKDIR/installme
mkdir -p $WORKDIR/llvm-build
cd $WORKDIR/llvm-build
cmake -G "Unix Makefiles" \
$WORKDIR/llvm \
-DCMAKE_INSTALL_PREFIX=$INSTALLDIR \
-DCMAKE_BUILD_TYPE=Release


make -j2 install
cd ..
tar -zcvf myllvm.tar.gz $INSTALLDIR
if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then mv myllvm.tar.gz myllvm-osx.tar.gz ; fi
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then mv myllvm.tar.gz myllvm-linux.tar.gz ;fi
rm -rf llvm-build
