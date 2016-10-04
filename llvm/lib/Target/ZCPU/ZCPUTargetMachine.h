//==-- ZCPUTargetMachine.h - Define TargetMachine for ZCPU ---------*- C++ -*-==//
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

#ifndef ZCPUTARGETMACHINE_H
#define ZCPUTARGETMACHINE_H

#include "ZCPU.h"
#include "ZCPUFrameLowering.h"
#include "ZCPUISelLowering.h"
#include "ZCPUInstrInfo.h"
#include "ZCPUSelectionDAGInfo.h"
#include "ZCPUSubtarget.h"
#include "llvm/IR/DataLayout.h"

namespace llvm {
  class ZCPUTargetMachine : public LLVMTargetMachine {
    const DataLayout DL;  // Calculates type size & alignment
    ZCPUFrameLowering FrameLowering;
    ZCPUInstrInfo InstrInfo;
    ZCPUSelectionDAGInfo TSInfo;
    ZCPUSubtarget Subtarget;
    ZCPUTargetLowering TLInfo;
  public:
    ZCPUTargetMachine(const Target &T, StringRef TT, StringRef CPU,
      StringRef FS, const TargetOptions &Options, Reloc::Model RM,
      CodeModel::Model CM, CodeGenOpt::Level OL);
    virtual const DataLayout *getDataLayout() const { return &DL; }
    virtual const ZCPUFrameLowering *getFrameLowering() const {
      return &FrameLowering;
    }
    virtual const ZCPUInstrInfo *getInstrInfo() const { return &InstrInfo; }
    virtual const ZCPURegisterInfo *getRegisterInfo() const {
      return &getInstrInfo()->getRegisterInfo();
    }
    virtual const ZCPUSelectionDAGInfo *getSelectionDAGInfo() const {
      return &TSInfo;
    }
    virtual const ZCPUSubtarget *getSubtargetImpl() const { return &Subtarget; }
    virtual const ZCPUTargetLowering *getTargetLowering() const {
      return &TLInfo;
    }
    virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);
  }; // end class ZCPUTargetMachine
} // end namespace llvm

#endif
