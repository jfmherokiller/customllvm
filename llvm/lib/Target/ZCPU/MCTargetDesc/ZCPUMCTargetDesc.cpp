//===-- ZCPUMCTargetDesc.cpp - ZCPU Target Descriptions -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides ZCPU specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "ZCPUMCTargetDesc.h"
#include "ZCPUMCAsmInfo.h"
#include "InstPrinter/ZCPUInstPrinter.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "ZCPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "ZCPUGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "ZCPUGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createZCPUMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitZCPUMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createZCPUMCRegisterInfo(StringRef TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitZCPUMCRegisterInfo(X, ZCPU::IP);
  return X;
}

static MCSubtargetInfo *createZCPUMCSubtargetInfo(StringRef TT, StringRef CPU,
  StringRef FS) {
    MCSubtargetInfo *X = new MCSubtargetInfo();
    InitZCPUMCSubtargetInfo(X, TT, CPU, FS);
    return X;
}

static MCCodeGenInfo *createZCPUMCCodeGenInfo(StringRef TT, Reloc::Model RM,
  CodeModel::Model CM, CodeGenOpt::Level OL) {
    MCCodeGenInfo *X = new MCCodeGenInfo();
    X->InitMCCodeGenInfo(RM, CM, OL);
    return X;
}

static MCInstPrinter *createZCPUMCInstPrinter(const Target &T,
  unsigned SyntaxVariant, const MCAsmInfo &MAI, const MCInstrInfo &MII,
  const MCRegisterInfo &MRI, const MCSubtargetInfo &STI)
{
  return new ZCPUInstPrinter(MAI, MII, MRI);
}

extern "C" void LLVMInitializeZCPUTargetMC() {
  // Register the MC asm info
  RegisterMCAsmInfo<ZCPUMCAsmInfo> X(TheZCPUTarget);

  // Register the MC codegen info
  TargetRegistry::RegisterMCCodeGenInfo(TheZCPUTarget,
    createZCPUMCCodeGenInfo);

  // Register the MC instruction info
  TargetRegistry::RegisterMCInstrInfo(TheZCPUTarget,
    createZCPUMCInstrInfo);

  // Register the MC register info
  TargetRegistry::RegisterMCRegInfo(TheZCPUTarget,
    createZCPUMCRegisterInfo);

  // Register the MC subtarget info
  TargetRegistry::RegisterMCSubtargetInfo(TheZCPUTarget,
    createZCPUMCSubtargetInfo);

  // Register the MCInstPrinter
  TargetRegistry::RegisterMCInstPrinter(TheZCPUTarget,
    createZCPUMCInstPrinter);

  // Register the MCCodeEmitter
  TargetRegistry::RegisterMCCodeEmitter(TheZCPUTarget,
    createZCPUMCCodeEmitter);

  // Register the MCAsmBackend
  TargetRegistry::RegisterMCAsmBackend(TheZCPUTarget,
    createZCPUAsmBackend);
}
