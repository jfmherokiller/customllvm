//===-- ZCPU.h - Top-level interface for ZCPU  ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the entry points for global functions defined in
/// the LLVM ZCPU back-end.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPU_H
#define LLVM_LIB_TARGET_ZCPU_ZCPU_H

#include "llvm/Support/CodeGen.h"

namespace llvm {

    class ZCPUTargetMachine;

    class FunctionPass;

// LLVM IR passes.
    FunctionPass *createZCPUOptimizeReturned();

// ISel and immediate followup passes.
    FunctionPass *createZCPUISelDag(ZCPUTargetMachine &TM,
                                    CodeGenOpt::Level OptLevel);
} // end namespace llvm

#endif
