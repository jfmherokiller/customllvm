//===-- ZCPUAsmBackend.cpp - ZCPU Assembler Backend -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "MCTargetDesc/ZCPUFixupKinds.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCMachObjectWriter.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

namespace {
  class ZCPUAsmBackend : public MCAsmBackend {
    StringRef CPU;
  public:
    ZCPUAsmBackend(const Target &T, StringRef _CPU)
      : MCAsmBackend(), CPU(_CPU) {}

    unsigned getNumFixupKinds() const {
      return ZCPU::NumTargetFixupKinds;
    }

    const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const {
      if (Kind >= FirstTargetFixupKind) assert("Invalid kind!");

      return MCAsmBackend::getFixupKindInfo(Kind);
    }

    void applyFixup(const MCFixup &Fixup, char *Data, unsigned DataSize,
      uint64_t Value) const {
    }

    bool mayNeedRelaxation(const MCInst &Inst) const {
      // FIXME
      llvm_unreachable("mayNeedRelaxation() unimplemented");
    }

    bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
      const MCRelaxableFragment *DF, const MCAsmLayout &Layout) const {
      // FIXME
      llvm_unreachable("fixupNeedsRelaxation() unimplemeted");
    }

    void relaxInstruction(const MCInst &Inst, MCInst &Res) const {
      // FIXME
      llvm_unreachable("relaxInstruction() unimplemented");
    }

    bool writeNopData(uint64_t Count, MCObjectWriter *OW) const {
      // FIXME: Zero fill for now. That's not right, but at least will get the
      // section size right.
      for (uint64_t i = 0; i < Count; i++)
        OW->Write8(0);
      return true;
    }
    MCObjectWriter *createObjectWriter(raw_ostream &OS) const {
      // FIXME
      llvm_unreachable("createObjectWriter() unimplemented");
    }
  }; // end class ZCPUAsmBackend
} // end namespace

MCAsmBackend *llvm::createZCPUAsmBackend(const Target &T,
                                        const MCRegisterInfo &MRI,
                                        StringRef TT,
                                        StringRef CPU)
{
  return new ZCPUAsmBackend(T, CPU);
}
