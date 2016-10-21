//===-- ZCPUAsmBackend.h - ZCPU Asm Backend  ------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the ZCPUAsmBackend class.
//
//===----------------------------------------------------------------------===//
//

#ifndef LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUASMBACKEND_H
#define LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUASMBACKEND_H

#include "ZCPUConfig.h"
#if CH >= CH5_1

#include "MCTargetDesc/ZCPUFixupKinds.h"
#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCAsmBackend.h"

namespace llvm {

class MCAssembler;
struct MCFixupKindInfo;
class Target;
class MCObjectWriter;

class ZCPUAsmBackend : public MCAsmBackend {
  Triple::OSType OSType;
  bool IsLittle; // Big or little endian

public:
  ZCPUAsmBackend(const Target &T, Triple::OSType _OSType, bool IsLittle)
      : MCAsmBackend(), OSType(_OSType), IsLittle(IsLittle) {}

  MCObjectWriter *createObjectWriter(raw_pwrite_stream &OS) const override;

  void applyFixup(const MCFixup &Fixup, char *Data, unsigned DataSize,
                  uint64_t Value, bool IsPCRel) const override;

  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override;

  unsigned getNumFixupKinds() const override {
    return ZCPU::NumTargetFixupKinds;
  }

  /// @name Target Relaxation Interfaces
  /// @{

  /// MayNeedRelaxation - Check whether the given instruction may need
  /// relaxation.
  ///
  /// \param Inst - The instruction to test.
  bool mayNeedRelaxation(const MCInst &Inst) const override {
    return false;
  }

  /// fixupNeedsRelaxation - Target specific predicate for whether a given
  /// fixup requires the associated instruction to be relaxed.
   bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                             const MCRelaxableFragment *DF,
                             const MCAsmLayout &Layout) const override {
    // FIXME.
    llvm_unreachable("RelaxInstruction() unimplemented");
    return false;
  }

  /// RelaxInstruction - Relax the instruction in the given fragment
  /// to the next wider instruction.
  ///
  /// \param Inst - The instruction to relax, which may be the same
  /// as the output.
  /// \param [out] Res On return, the relaxed instruction.
  void relaxInstruction(const MCInst &Inst, MCInst &Res) const override {}

  /// @}

  bool writeNopData(uint64_t Count, MCObjectWriter *OW) const override;
}; // class ZCPUAsmBackend

} // namespace

#endif // #if CH >= CH5_1

#endif
