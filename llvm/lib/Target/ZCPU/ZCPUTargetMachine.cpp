//===-- ZCPUTargetMachine.cpp - Define TargetMachine for ZCPU -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about ZCPU target spec.
//
//===----------------------------------------------------------------------===//

#include "ZCPUTargetMachine.h"
#include "ZCPU.h"


#include "llvm/IR/LegacyPassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;



extern "C" void LLVMInitializeZCPUTarget() {
}
