//===-- ZCPUTargetMachine.cpp - Define TargetMachine for the ZCPU -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the ZCPU specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#include "ZCPUTargetMachine.h"
#include "ZCPU.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
using namespace llvm;
extern "C" void LLVMInitializeZCPUTarget() {
    RegisterTargetMachine<ZCPUTargetMachine> X(TheZCPUTarget);
}
