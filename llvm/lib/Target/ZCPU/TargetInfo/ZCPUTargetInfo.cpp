//===-- ZCPUTargetInfo.cpp - ZCPU Target Implementation -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file registers the ZCPU target.
///
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "llvm/ADT/Triple.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

#define DEBUG_TYPE "zcpu-target-info"

Target llvm::TheZCPUTarget32;

extern "C" void LLVMInitializeZCPUTargetInfo() {
  RegisterTarget<Triple::zcpu> X(TheZCPUTarget32, "zcpu",
                                   "ZCPU");
}
