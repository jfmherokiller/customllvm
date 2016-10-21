//===-- ZCPUMCTargetDesc.h - ZCPU Target Descriptions -----------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUMCTARGETDESC_H
#define LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUMCTARGETDESC_H

#include "ZCPUConfig.h"
#include "llvm/Support/DataTypes.h"

namespace llvm {
#if CH >= CH3_2
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class StringRef;
#endif
class Target;
class Triple;
#if CH >= CH3_2
class raw_ostream;
class raw_pwrite_stream;
#endif

extern Target TheZCPUTarget;
extern Target TheZCPUelTarget;

#if CH >= CH5_1
MCCodeEmitter *createZCPUMCCodeEmitterEB(const MCInstrInfo &MCII,
                                         const MCRegisterInfo &MRI,
                                         MCContext &Ctx);
MCCodeEmitter *createZCPUMCCodeEmitterEL(const MCInstrInfo &MCII,
                                         const MCRegisterInfo &MRI,
                                         MCContext &Ctx);

MCAsmBackend *createZCPUAsmBackendEB32(const Target &T,
                                       const MCRegisterInfo &MRI,
                                       const Triple &TT, StringRef CPU);
MCAsmBackend *createZCPUAsmBackendEL32(const Target &T,
                                       const MCRegisterInfo &MRI,
                                       const Triple &TT, StringRef CPU);

MCObjectWriter *createZCPUELFObjectWriter(raw_pwrite_stream &OS,
                                          uint8_t OSABI,
                                          bool IsLittleEndian);
#endif
} // End llvm namespace

// Defines symbolic names for ZCPU registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "ZCPUGenRegisterInfo.inc"

// Defines symbolic names for the ZCPU instructions.
#define GET_INSTRINFO_ENUM
#include "ZCPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "ZCPUGenSubtargetInfo.inc"

#endif
