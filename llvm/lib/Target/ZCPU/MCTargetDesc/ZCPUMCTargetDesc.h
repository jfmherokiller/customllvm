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


#include "llvm/Support/DataTypes.h"

namespace llvm {

class Target;
class Triple;

extern Target TheZCPUTarget;
extern Target TheZCPUelTarget;

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
