//===-- ZCPUSubtarget.cpp - ZCPU Subtarget Information ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file implements the ZCPU-specific subclass of
/// TargetSubtarget.
///
//===----------------------------------------------------------------------===//

#include "ZCPUSubtarget.h"
#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "ZCPUInstrInfo.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "zcpu-subtarget"

#define GET_SUBTARGETINFO_CTOR
#define GET_SUBTARGETINFO_TARGET_DESC

#include "ZCPUGenSubtargetInfo.inc"

ZCPUSubtarget &
ZCPUSubtarget::initializeSubtargetDependencies(StringRef FS) {
    // Determine default and user-specified characteristics

    if (CPUString.empty())
        CPUString = "generic";

    ParseSubtargetFeatures(CPUString, FS);
    return *this;
}

ZCPUSubtarget::ZCPUSubtarget(const Triple &TT,
                             const std::string &CPU,
                             const std::string &FS,
                             const TargetMachine &TM)
        : ZCPUGenSubtargetInfo(TT, CPU, FS),
          CPUString(CPU), TargetTriple(TT), FrameLowering(),
          InstrInfo(initializeSubtargetDependencies(FS)), TSInfo(),
          TLInfo(TM, *this) {}



bool ZCPUSubtarget::useAA() const { return true; }
