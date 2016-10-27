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

#define DEBUG_TYPE "wasm-target-info"

Target llvm::TheZCPUTarget32;
Target llvm::TheZCPUTarget64;

extern "C" void LLVMInitializeZCPUTargetInfo() {
  RegisterTarget<Triple::wasm32> X(TheZCPUTarget32, "wasm32",
                                   "ZCPU 32-bit");
  RegisterTarget<Triple::wasm64> Y(TheZCPUTarget64, "wasm64",
                                   "ZCPU 64-bit");
}
