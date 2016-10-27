//=- ZCPUMachineFunctionInfo.cpp - ZCPU Machine Function Info -=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file implements ZCPU-specific per-machine-function
/// information.
///
//===----------------------------------------------------------------------===//

#include "ZCPUMachineFunctionInfo.h"
using namespace llvm;

ZCPUFunctionInfo::~ZCPUFunctionInfo() {}

void ZCPUFunctionInfo::initWARegs() {
  assert(WARegs.empty());
  unsigned Reg = UnusedReg;
  WARegs.resize(MF.getRegInfo().getNumVirtRegs(), Reg);
}
