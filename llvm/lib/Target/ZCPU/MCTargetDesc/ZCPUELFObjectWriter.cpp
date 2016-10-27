//===-- ZCPUELFObjectWriter.cpp - ZCPU ELF Writer -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file handles ELF-specific object emission, converting LLVM's
/// internal fixups into the appropriate relocations.
///
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/Support/ErrorHandling.h"
using namespace llvm;

namespace {
class ZCPUELFObjectWriter final : public MCELFObjectTargetWriter {
public:
  ZCPUELFObjectWriter(bool Is64Bit, uint8_t OSABI);

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
} // end anonymous namespace

ZCPUELFObjectWriter::ZCPUELFObjectWriter(bool Is64Bit,
                                                       uint8_t OSABI)
    : MCELFObjectTargetWriter(Is64Bit, OSABI, ELF::EM_ZCPU,
                              /*HasRelocationAddend=*/false) {}

unsigned ZCPUELFObjectWriter::getRelocType(MCContext &Ctx,
                                                  const MCValue &Target,
                                                  const MCFixup &Fixup,
                                                  bool IsPCRel) const {
  // ZCPU functions are not allocated in the address space. To resolve a
  // pointer to a function, we must use a special relocation type.
  if (const MCSymbolRefExpr *SyExp =
          dyn_cast<MCSymbolRefExpr>(Fixup.getValue()))
  switch (Fixup.getKind()) {
  default:
    llvm_unreachable("unimplemented fixup kind");
  }
  return 0;
}

MCObjectWriter *llvm::createZCPUELFObjectWriter(raw_pwrite_stream &OS,
                                                       bool Is64Bit,
                                                       uint8_t OSABI) {
  MCELFObjectTargetWriter *MOTW =
      new ZCPUELFObjectWriter(Is64Bit, OSABI);
  return createELFObjectWriter(MOTW, OS, /*IsLittleEndian=*/true);
}
