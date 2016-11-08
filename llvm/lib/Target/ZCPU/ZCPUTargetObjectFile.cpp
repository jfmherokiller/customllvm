//===-- ZCPUTargetObjectFile.cpp - ZCPU Object Info ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file defines the functions of the ZCPU-specific subclass
/// of TargetLoweringObjectFile.
///
//===----------------------------------------------------------------------===//

#include "ZCPUTargetObjectFile.h"
#include "ZCPUTargetMachine.h"

using namespace llvm;

void ZCPUTargetObjectFile::Initialize(MCContext &Ctx,
                                      const TargetMachine &TM) {
    TargetLoweringObjectFileELF::Initialize(Ctx, TM);
    InitializeELF(TM.Options.UseInitArray);
}
