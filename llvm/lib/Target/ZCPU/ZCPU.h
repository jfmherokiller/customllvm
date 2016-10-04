//===- ZCPU.h - Define TargetMachine for ZCPU -------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// ZCPU back-end.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_ZCPU_H
#define TARGET_ZCPU_H

#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class ZCPUTargetMachine;
  class FunctionPass;

  FunctionPass *createZCPUISelDAG(ZCPUTargetMachine &TM, CodeGenOpt::Level OptLevel);
} // end namespace llvm

#endif
