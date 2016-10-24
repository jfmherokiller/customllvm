//===-- ZCPUTargetInfo.cpp - ZCPU Target Implementation -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ZCPU.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheZCPUTarget, llvm::TheZCPUelTarget;

extern "C" void LLVMInitializeZCPUTargetInfo() {
  RegisterTarget<Triple::zcpu,
        /*HasJIT=*/true> X(TheZCPUTarget, "zcpu", "ZCPU");
}
