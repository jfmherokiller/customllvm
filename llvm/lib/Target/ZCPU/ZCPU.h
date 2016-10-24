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

#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class ZCPUTargetMachine;
  class FunctionPass;

} // end namespace llvm;

#endif
