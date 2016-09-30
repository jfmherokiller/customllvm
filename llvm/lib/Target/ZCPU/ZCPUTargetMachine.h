//===-- ZCPUTargetMachine.h - Define TargetMachine for ZCPU ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the ZCPU specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUTARGETMACHINE_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUTARGETMACHINE_H
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
    class StringRef;
class ZCPUTargetMachine final : public LLVMTargetMachine {
public:
    ZCPUTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     Optional<Reloc::Model> RM, CodeModel::Model CM,
                     CodeGenOpt::Level OL);
    ~ZCPUTargetMachine() override;
};

} // end namespace llvm
#endif
