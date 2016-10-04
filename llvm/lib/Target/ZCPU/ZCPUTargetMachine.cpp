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
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeZCPUTarget() {
  RegisterTargetMachine<ZCPUTargetMachine> X(TheZCPUTarget);
}

ZCPUTargetMachine::ZCPUTargetMachine(const Target &T, StringRef TT, StringRef CPU,
  StringRef FS, const TargetOptions &Options, Reloc::Model RM,
  CodeModel::Model CM, CodeGenOpt::Level OL)
  : LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
  DL("e-p:16:8:8-i8:8:8-i16:8:8-n8:16"),
  FrameLowering(*this), InstrInfo(*this), TSInfo(*this),
  Subtarget(TT, CPU, FS), TLInfo(*this)
{
  initAsmInfo();
}

namespace {
  class ZCPUPassConfig : public TargetPassConfig {
  public:
    ZCPUPassConfig(ZCPUTargetMachine *TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}
    ZCPUTargetMachine &getZCPUTargetMachine() const {
      return getTM<ZCPUTargetMachine>();
    }
    virtual bool addInstSelector();
  }; // end class ZCPUPassConfig
} // end namespace

TargetPassConfig *ZCPUTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new ZCPUPassConfig(this, PM);
}

bool ZCPUPassConfig::addInstSelector() {
  addPass(createZCPUISelDAG(getZCPUTargetMachine(), getOptLevel()));
  return false;
}
