//===-- ZCPUFixupKinds.h - ZCPU Specific Fixup Entries ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUFIXUPKINDS_H
#define LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUFIXUPKINDS_H

#include "ZCPUConfig.h"
#if CH >= CH5_1

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace ZCPU {
  // Although most of the current fixup types reflect a unique relocation
  // one can have multiple fixup types for a given relocation and thus need
  // to be uniquely named.
  //
  // This table *must* be in the save order of
  // MCFixupKindInfo Infos[ZCPU::NumTargetFixupKinds]
  // in ZCPUAsmBackend.cpp.
  //@Fixups {
  enum Fixups {
    //@ Pure upper 32 bit fixup resulting in - R_ZCPU_32.
    fixup_ZCPU_32 = FirstTargetFixupKind,

    // Pure upper 16 bit fixup resulting in - R_ZCPU_HI16.
    fixup_ZCPU_HI16,

    // Pure lower 16 bit fixup resulting in - R_ZCPU_LO16.
    fixup_ZCPU_LO16,

    // 16 bit fixup for GP offest resulting in - R_ZCPU_GPREL16.
    fixup_ZCPU_GPREL16,

    // Global symbol fixup resulting in - R_ZCPU_GOT16.
    fixup_ZCPU_GOT_Global,

    // Local symbol fixup resulting in - R_ZCPU_GOT16.
    fixup_ZCPU_GOT_Local,

#if CH >= CH8_1
    // PC relative branch fixup resulting in - R_ZCPU_PC16.
    // cpu0 PC16, e.g. beq
    fixup_ZCPU_PC16,

    // PC relative branch fixup resulting in - R_ZCPU_PC24.
    // cpu0 PC24, e.g. jeq, jmp
    fixup_ZCPU_PC24,
#endif
    
#if CH >= CH9_1
    // resulting in - R_ZCPU_CALL16.
    fixup_ZCPU_CALL16,
#endif

#if CH >= CH12_1
    // resulting in - R_ZCPU_TLS_GD.
    fixup_ZCPU_TLSGD,

    // resulting in - R_ZCPU_TLS_GOTTPREL.
    fixup_ZCPU_GOTTPREL,

    // resulting in - R_ZCPU_TLS_TPREL_HI16.
    fixup_ZCPU_TP_HI,

    // resulting in - R_ZCPU_TLS_TPREL_LO16.
    fixup_ZCPU_TP_LO,

    // resulting in - R_ZCPU_TLS_LDM.
    fixup_ZCPU_TLSLDM,

    // resulting in - R_ZCPU_TLS_DTP_HI16.
    fixup_ZCPU_DTP_HI,

    // resulting in - R_ZCPU_TLS_DTP_LO16.
    fixup_ZCPU_DTP_LO,
#endif

    // resulting in - R_ZCPU_GOT_HI16
    fixup_ZCPU_GOT_HI16,

    // resulting in - R_ZCPU_GOT_LO16
    fixup_ZCPU_GOT_LO16,

    // Marker
    LastTargetFixupKind,
    NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
  };
  //@Fixups }
} // namespace ZCPU
} // namespace llvm

#endif // #if CH >= CH5_1

#endif // LLVM_ZCPU_ZCPUFIXUPKINDS_H
