//===-- ZCPUMCTargetDesc.h - ZCPU Target Descriptions -------------*- C++ -*-===//
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

#ifndef ZCPUMCTARGETDESC_H
#define ZCPUMCTARGETDESC_H

namespace llvm {
  class MCAsmBackend;
  class MCCodeEmitter;
  class MCContext;
  class MCInstrInfo;
  class MCRegisterInfo;
  class MCSubtargetInfo;
  class Target;
  class StringRef;

  extern Target TheZCPUTarget;

  MCCodeEmitter *createZCPUMCCodeEmitter(const MCInstrInfo &MCII,
    const MCRegisterInfo &MRI, const MCSubtargetInfo &STI, MCContext &Ctx);
  MCAsmBackend *createZCPUAsmBackend(const Target &T, const MCRegisterInfo &MRI, StringRef TT, StringRef CPU);
} // end namespace llvm

#define GET_REGINFO_ENUM
#include "ZCPUGenRegisterInfo.inc"

#define GET_INSTRINFO_ENUM
#include "ZCPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "ZCPUGenSubtargetInfo.inc"

#endif
