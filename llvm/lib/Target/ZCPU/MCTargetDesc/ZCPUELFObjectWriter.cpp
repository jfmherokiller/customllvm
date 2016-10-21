//===-- ZCPUELFObjectWriter.cpp - ZCPU ELF Writer -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ZCPUConfig.h"
#if CH >= CH5_1

#include "MCTargetDesc/ZCPUBaseInfo.h"
#include "MCTargetDesc/ZCPUFixupKinds.h"
#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include <list>

using namespace llvm;

namespace {
  class ZCPUELFObjectWriter : public MCELFObjectTargetWriter {
  public:
    ZCPUELFObjectWriter(uint8_t OSABI);

    ~ZCPUELFObjectWriter() override;

    unsigned GetRelocType(const MCValue &Target, const MCFixup &Fixup,
                          bool IsPCRel) const override;
    bool needsRelocateWithSymbol(const MCSymbol &Sym,
                                 unsigned Type) const override;
  };
}

ZCPUELFObjectWriter::ZCPUELFObjectWriter(uint8_t OSABI)
  : MCELFObjectTargetWriter(/*_is64Bit=false*/ false, OSABI, ELF::EM_ZCPU,
                            /*HasRelocationAddend*/ false) {}

ZCPUELFObjectWriter::~ZCPUELFObjectWriter() {}

//@GetRelocType {
unsigned ZCPUELFObjectWriter::GetRelocType(const MCValue &Target,
                                           const MCFixup &Fixup,
                                           bool IsPCRel) const {
  // determine the type of the relocation
  unsigned Type = (unsigned)ELF::R_ZCPU_NONE;
  unsigned Kind = (unsigned)Fixup.getKind();

  switch (Kind) {
  default:
    llvm_unreachable("invalid fixup kind!");
  case FK_Data_4:
    Type = ELF::R_ZCPU_32;
    break;
#if CH >= CH8_1 //1
  case FK_GPRel_4:
    Type = ELF::R_ZCPU_GPREL32;
    break;
#endif
  case ZCPU::fixup_ZCPU_32:
    Type = ELF::R_ZCPU_32;
    break;
  case ZCPU::fixup_ZCPU_GPREL16:
    Type = ELF::R_ZCPU_GPREL16;
    break;
#if CH >= CH9_1
  case ZCPU::fixup_ZCPU_CALL16:
    Type = ELF::R_ZCPU_CALL16;
    break;
#endif
  case ZCPU::fixup_ZCPU_GOT_Global:
  case ZCPU::fixup_ZCPU_GOT_Local:
    Type = ELF::R_ZCPU_GOT16;
    break;
  case ZCPU::fixup_ZCPU_HI16:
    Type = ELF::R_ZCPU_HI16;
    break;
  case ZCPU::fixup_ZCPU_LO16:
    Type = ELF::R_ZCPU_LO16;
    break;
#if CH >= CH12_1
  case ZCPU::fixup_ZCPU_TLSGD:
    Type = ELF::R_ZCPU_TLS_GD;
    break;
  case ZCPU::fixup_ZCPU_GOTTPREL:
    Type = ELF::R_ZCPU_TLS_GOTTPREL;
    break;
#endif
#if CH >= CH8_1 //2
  case ZCPU::fixup_ZCPU_PC16:
    Type = ELF::R_ZCPU_PC16;
    break;
  case ZCPU::fixup_ZCPU_PC24:
    Type = ELF::R_ZCPU_PC24;
    break;
#endif
#if CH >= CH12_1
  case ZCPU::fixup_ZCPU_TP_HI:
    Type = ELF::R_ZCPU_TLS_TP_HI16;
    break;
  case ZCPU::fixup_ZCPU_TP_LO:
    Type = ELF::R_ZCPU_TLS_TP_LO16;
    break;
  case ZCPU::fixup_ZCPU_TLSLDM:
    Type = ELF::R_ZCPU_TLS_LDM;
    break;
  case ZCPU::fixup_ZCPU_DTP_HI:
    Type = ELF::R_ZCPU_TLS_DTP_HI16;
    break;
  case ZCPU::fixup_ZCPU_DTP_LO:
    Type = ELF::R_ZCPU_TLS_DTP_LO16;
    break;
#endif
  case ZCPU::fixup_ZCPU_GOT_HI16:
    Type = ELF::R_ZCPU_GOT_HI16;
    break;
  case ZCPU::fixup_ZCPU_GOT_LO16:
    Type = ELF::R_ZCPU_GOT_LO16;
    break;
  }

  return Type;
}
//@GetRelocType }

bool
ZCPUELFObjectWriter::needsRelocateWithSymbol(const MCSymbol &Sym,
                                             unsigned Type) const {
  // FIXME: This is extremelly conservative. This really needs to use a
  // whitelist with a clear explanation for why each realocation needs to
  // point to the symbol, not to the section.
  switch (Type) {
  default:
    return true;

  case ELF::R_ZCPU_GOT16:
  // For ZCPU pic mode, I think it's OK to return true but I didn't confirm.
  //  llvm_unreachable("Should have been handled already");
    return true;

  // These relocations might be paired with another relocation. The pairing is
  // done by the static linker by matching the symbol. Since we only see one
  // relocation at a time, we have to force them to relocate with a symbol to
  // avoid ending up with a pair where one points to a section and another
  // points to a symbol.
  case ELF::R_ZCPU_HI16:
  case ELF::R_ZCPU_LO16:
  // R_ZCPU_32 should be a relocation record, I don't know why Mips set it to 
  // false.
  case ELF::R_ZCPU_32:
    return true;

  case ELF::R_ZCPU_GPREL16:
    return false;
  }
}

MCObjectWriter *llvm::createZCPUELFObjectWriter(raw_pwrite_stream &OS,
                                                uint8_t OSABI,
                                                bool IsLittleEndian) {
  MCELFObjectTargetWriter *MOTW = new ZCPUELFObjectWriter(OSABI);
  return createELFObjectWriter(MOTW, OS, IsLittleEndian);
}

#endif // #if CH >= CH5_1
