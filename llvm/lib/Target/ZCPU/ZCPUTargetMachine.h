//===-- ZCPUTargetMachine.h - Define TargetMachine for ZCPU -----*- C++ -*-===//
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

#include "ZCPUConfig.h"
#if CH >= CH3_1

#include "MCTargetDesc/ZCPUABIInfo.h"
#include "ZCPUSubtarget.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class formatted_raw_ostream;
class ZCPURegisterInfo;

class ZCPUTargetMachine : public LLVMTargetMachine {
  bool isLittle;
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  // Selected ABI
  ZCPUABIInfo ABI;
  ZCPUSubtarget DefaultSubtarget;

  mutable StringMap<std::unique_ptr<ZCPUSubtarget>> SubtargetMap;
public:
  ZCPUTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                    StringRef FS, const TargetOptions &Options, Reloc::Model RM,
                    CodeModel::Model CM, CodeGenOpt::Level OL, bool isLittle);
  ~ZCPUTargetMachine() override;

  const ZCPUSubtarget *getSubtargetImpl() const {
    return &DefaultSubtarget;
  }

  const ZCPUSubtarget *getSubtargetImpl(const Function &F) const override;

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }
  bool isLittleEndian() const { return isLittle; }
  const ZCPUABIInfo &getABI() const { return ABI; }
};

/// ZCPUebTargetMachine - ZCPU32 big endian target machine.
///
class ZCPUebTargetMachine : public ZCPUTargetMachine {
  virtual void anchor();
public:
  ZCPUebTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                      StringRef FS, const TargetOptions &Options,
                      Reloc::Model RM, CodeModel::Model CM,
                      CodeGenOpt::Level OL);
};

/// ZCPUelTargetMachine - ZCPU32 little endian target machine.
///
class ZCPUelTargetMachine : public ZCPUTargetMachine {
  virtual void anchor();
public:
  ZCPUelTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                      StringRef FS, const TargetOptions &Options,
                      Reloc::Model RM, CodeModel::Model CM,
                      CodeGenOpt::Level OL);
};
} // End llvm namespace

#endif // #if CH >= CH3_1

#endif
