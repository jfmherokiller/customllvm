//===-- Z80MCCodeEmitter.cpp - Convert Z80 code to machine code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the Z80MCCodeEmitter class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Z80MCTargetDesc.h"
#include "MCTargetDesc/Z80BaseInfo.h"
#include "MCTargetDesc/Z80FixupKinds.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
using namespace llvm;

namespace llvm {
  class Z80MCCodeEmitter : public MCCodeEmitter {
    const MCInstrInfo &MCII;
    const MCSubtargetInfo &STI;
    MCContext &Ctx;
    mutable uint64_t TSFlags;
  public:
    Z80MCCodeEmitter(const MCInstrInfo &mcii, const MCSubtargetInfo &sti,
      MCContext &ctx)
      : MCII(mcii), STI(sti), Ctx(ctx) {}

    ~Z80MCCodeEmitter() {}

    void EncodeInstruction(const MCInst &MI, raw_ostream &OS,
      SmallVectorImpl<MCFixup> &Fixups) const;
    void EmitByte(unsigned char Byte, raw_ostream &OS) const {
      OS << Byte;
    }
    void EmitInstruction(uint64_t Code, unsigned Size, raw_ostream &OS) const {
      for (unsigned i = 0; i < Size; i++) {
        EmitByte(Code, OS);
        Code >>= 8;
      }
    }
    void EmitPrefix(raw_ostream &OS) const {
      unsigned Prefix = Z80II::getPrefix(TSFlags);
      while (Prefix) {
        EmitByte(Prefix, OS);
        Prefix = Prefix>>8;
      }
    }
    bool hasRegPrefix() const {
      return TSFlags & Z80II::RegPrefixMask;
    }
    Z80II::Prefixes getRegPrefix(const MCInst &MI) const;

    // getBinaryCodeForInstr - tblgen generated function for getting the
    // binary encoding for an instruction.
    uint64_t getBinaryCodeForInstr(const MCInst &MI,
      SmallVectorImpl<MCFixup> &Fixups) const;

    // getMachineOpValue - Return binary encoding of operand.
    unsigned getMachineOpValue(const MCInst &MI, const MCOperand &MO,
      SmallVectorImpl<MCFixup> &Fixups) const;
    // getBREncoding
    unsigned getBREncoding(const MCInst &MI, unsigned OpNo,
      SmallVectorImpl<MCFixup> &Fixups) const;
    // getXMemOpValue
    unsigned getXMemOpValue(const MCInst &MI, unsigned OpNo,
      SmallVectorImpl<MCFixup> &Fixups) const;
  }; // end class Z80MCCodeEmitter
} // end namespace llvm

MCCodeEmitter *llvm::createZ80MCCodeEmitter(const MCInstrInfo &MCII,
  const MCRegisterInfo &MRI, const MCSubtargetInfo &STI, MCContext &Ctx)
{
  return new Z80MCCodeEmitter(MCII, STI, Ctx);
}

void Z80MCCodeEmitter::EncodeInstruction(const MCInst &MI, raw_ostream &OS,
  SmallVectorImpl<MCFixup> &Fixups) const
{
  unsigned Opcode = MI.getOpcode();
  const MCInstrDesc &Desc = MCII.get(Opcode);
  unsigned Size = Desc.getSize();
  TSFlags = Desc.TSFlags;

  // Move reg prefix from MCInst to TSFlags
  Z80II::setRegPrefix(TSFlags, getRegPrefix(MI));

  EmitPrefix(OS);
  uint64_t Bits = getBinaryCodeForInstr(MI, Fixups);

  switch (TSFlags & Z80II::PrefixMask)
  {
  default: break;
  case Z80II::DDCB:
  case Z80II::FDCB:
    Bits = ByteSwap_16(Bits);
    break;
  }

  EmitInstruction(Bits, Size, OS);
}

Z80II::Prefixes Z80MCCodeEmitter::getRegPrefix(const MCInst &MI) const
{
 for (unsigned i = 0, e = MI.getNumOperands(); i != e; i++)
  {
    MCOperand MCOp = MI.getOperand(i);
    if (MCOp.isReg())
    {
      switch (MCOp.getReg())
      {
      default: continue;
      case Z80::IX:
      case Z80::XH:
      case Z80::XL:
        return Z80II::DD;
      case Z80::IY:
      case Z80::YH:
      case Z80::YL:
        return Z80II::FD;
      }
    }
  }
  return Z80II::NoPrefix;
}

unsigned Z80MCCodeEmitter::getMachineOpValue(const MCInst &MI,
  const MCOperand &MO, SmallVectorImpl<MCFixup> &Fixups) const
{
  if (MO.isReg())
  {
    unsigned Reg = MO.getReg();
    unsigned RegNo = Ctx.getRegisterInfo()->getEncodingValue(Reg);
    return RegNo;
  }
  else if (MO.isImm())
  {
    return static_cast<unsigned>(MO.getImm());
  }
  else if (MO.isExpr())
  {
    if (hasRegPrefix())
      Fixups.push_back(MCFixup::Create(2, MO.getExpr(), FK_Data_2));
    else
      Fixups.push_back(MCFixup::Create(1, MO.getExpr(), FK_Data_2));
  }
  return 0;
}

unsigned Z80MCCodeEmitter::getBREncoding(const MCInst &MI, unsigned OpNo,
  SmallVectorImpl<MCFixup> &Fixups) const
{
  const MCOperand &MO = MI.getOperand(OpNo);
  if (MO.isReg() || MO.isImm()) return getMachineOpValue(MI, MO, Fixups);

  // Add a fixup for the branch target.
  Fixups.push_back(MCFixup::Create(1, MO.getExpr(), FK_PCRel_2));

  return 0;
}

unsigned Z80MCCodeEmitter::getXMemOpValue(const MCInst &MI, unsigned OpNo,
  SmallVectorImpl<MCFixup> &Fixups) const
{
  const MCOperand &MOReg = MI.getOperand(OpNo);
  const MCOperand &MOImm = MI.getOperand(OpNo+1);
  if (MOReg.isReg() && MOImm.isImm())
      return static_cast<unsigned>(MOImm.getImm());
  return 0;
}

#include "Z80GenMCCodeEmitter.inc"
