//===-- ZCPU.h - Top-level interface for ZCPU representation ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM ZCPU back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPU_H
#define LLVM_LIB_TARGET_ZCPU_ZCPU_H

#include "ZCPUConfig.h"
#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class ZCPUTargetMachine;
  class FunctionPass;

#if CH >= CH9_3
#ifdef ENABLE_GPRESTORE
  FunctionPass *createZCPUEmitGPRestorePass(ZCPUTargetMachine &TM);
#endif
#endif //#if CH >= CH9_3
#if CH >= CH8_2 //1
  FunctionPass *createZCPUDelaySlotFillerPass(ZCPUTargetMachine &TM);
#endif
#if CH >= CH8_2 //2
  FunctionPass *createZCPUDelJmpPass(ZCPUTargetMachine &TM);
#endif
#if CH >= CH8_2 //3
  FunctionPass *createZCPULongBranchPass(ZCPUTargetMachine &TM);
#endif

} // end namespace llvm;

#endif
