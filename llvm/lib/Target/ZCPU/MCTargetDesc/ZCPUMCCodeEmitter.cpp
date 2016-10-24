//===-- ZCPUMCCodeEmitter.cpp - Convert ZCPU Code to Machine Code ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the ZCPUMCCodeEmitter class.
//
//===----------------------------------------------------------------------===//
//

#include "ZCPUMCCodeEmitter.h"
#if CH >= CH5_1

#include "MCTargetDesc/ZCPUBaseInfo.h"
#include "MCTargetDesc/ZCPUFixupKinds.h"
#include "MCTargetDesc/ZCPUMCExpr.h"
#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/raw_ostream.h"

#define DEBUG_TYPE "mccodeemitter"

#define GET_INSTRMAP_INFO
#include "ZCPUGenInstrInfo.inc"
#undef GET_INSTRMAP_INFO

namespace llvm {
MCCodeEmitter * createZCPUMCCodeEmitterEB(const MCInstrInfo &MCII,
                                               const MCRegisterInfo &MRI,
                                               MCContext &Ctx) {
  return new ZCPUMCCodeEmitter(MCII, Ctx, false);
}

MCCodeEmitter * createZCPUMCCodeEmitterEL(const MCInstrInfo &MCII,
                                               const MCRegisterInfo &MRI,
                                               MCContext &Ctx) {
  return new ZCPUMCCodeEmitter(MCII, Ctx, true);
}
} // End of namespace llvm

void ZCPUMCCodeEmitter::EmitByte(unsigned char C, raw_ostream &OS) const {
  OS << (char)C;
}

void ZCPUMCCodeEmitter::EmitInstruction(uint64_t Val, unsigned Size, raw_ostream &OS) const {
  // Output the instruction encoding in little endian byte order.
  for (unsigned i = 0; i < Size; ++i) {
    unsigned Shift = IsLittleEndian ? i * 8 : (Size - 1 - i) * 8;
    EmitByte((Val >> Shift) & 0xff, OS);
  }
}

/// encodeInstruction - Emit the instruction.
/// Size the instruction (currently only 4 bytes)
void ZCPUMCCodeEmitter::
encodeInstruction(const MCInst &MI, raw_ostream &OS,
                  SmallVectorImpl<MCFixup> &Fixups,
                  const MCSubtargetInfo &STI) const
{
  uint32_t Binary = getBinaryCodeForInstr(MI, Fixups, STI);

  // Check for unimplemented opcodes.
  // Unfortunately in ZCPU both NOT and SLL will come in with Binary == 0
  // so we have to special check for them.
  unsigned Opcode = MI.getOpcode();
  if ((Opcode != ZCPU::NOP) && (Opcode != ZCPU::SHL) && !Binary)
    llvm_unreachable("unimplemented opcode in encodeInstruction()");

  const MCInstrDesc &Desc = MCII.get(MI.getOpcode());
  uint64_t TSFlags = Desc.TSFlags;

  // Pseudo instructions don't get encoded and shouldn't be here
  // in the first place!
  if ((TSFlags & ZCPUII::FormMask) == ZCPUII::Pseudo)
    llvm_unreachable("Pseudo opcode found in encodeInstruction()");

  // For now all instructions are 4 bytes
  int Size = 4; // FIXME: Have Desc.getSize() return the correct value!

  EmitInstruction(Binary, Size, OS);
}

//@CH8_1 {
/// getBranch16TargetOpValue - Return binary encoding of the branch
/// target operand. If the machine operand requires relocation,
/// record the relocation and return zero.
unsigned ZCPUMCCodeEmitter::
getBranch16TargetOpValue(const MCInst &MI, unsigned OpNo,
                         SmallVectorImpl<MCFixup> &Fixups,
                         const MCSubtargetInfo &STI) const {
#if CH >= CH8_1 //1
  const MCOperand &MO = MI.getOperand(OpNo);

  // If the destination is an immediate, we have nothing to do.
  if (MO.isImm()) return MO.getImm();
  assert(MO.isExpr() && "getBranch16TargetOpValue expects only expressions");

  const MCExpr *Expr = MO.getExpr();
  Fixups.push_back(MCFixup::create(0, Expr,
                                   MCFixupKind(ZCPU::fixup_ZCPU_PC16)));
#endif
  return 0;
}

/// getBranch24TargetOpValue - Return binary encoding of the branch
/// target operand. If the machine operand requires relocation,
/// record the relocation and return zero.
unsigned ZCPUMCCodeEmitter::
getBranch24TargetOpValue(const MCInst &MI, unsigned OpNo,
                       SmallVectorImpl<MCFixup> &Fixups,
                       const MCSubtargetInfo &STI) const {
#if CH >= CH8_1 //2
  const MCOperand &MO = MI.getOperand(OpNo);

  // If the destination is an immediate, we have nothing to do.
  if (MO.isImm()) return MO.getImm();
  assert(MO.isExpr() && "getBranch24TargetOpValue expects only expressions");

  const MCExpr *Expr = MO.getExpr();
  Fixups.push_back(MCFixup::create(0, Expr,
                                   MCFixupKind(ZCPU::fixup_ZCPU_PC24)));
#endif
  return 0;
}

/// getJumpTargetOpValue - Return binary encoding of the jump
/// target operand, such as JSUB. 
/// If the machine operand requires relocation,
/// record the relocation and return zero.
//@getJumpTargetOpValue {
unsigned ZCPUMCCodeEmitter::
getJumpTargetOpValue(const MCInst &MI, unsigned OpNo,
                     SmallVectorImpl<MCFixup> &Fixups,
                     const MCSubtargetInfo &STI) const {
#if CH >= CH8_1 //3
  unsigned Opcode = MI.getOpcode();
  const MCOperand &MO = MI.getOperand(OpNo);
  // If the destination is an immediate, we have nothing to do.
  if (MO.isImm()) return MO.getImm();
  assert(MO.isExpr() && "getJumpTargetOpValue expects only expressions");

  const MCExpr *Expr = MO.getExpr();
#if CH >= CH9_1 //1
  if (Opcode == ZCPU::JSUB || Opcode == ZCPU::JMP || Opcode == ZCPU::BAL)
#elif CH >= CH8_2 //1
  if (Opcode == ZCPU::JMP || Opcode == ZCPU::BAL)
#else
  if (Opcode == ZCPU::JMP)
#endif //#if CH >= CH9_1 //1
    Fixups.push_back(MCFixup::create(0, Expr,
                                     MCFixupKind(ZCPU::fixup_ZCPU_PC24)));
  else
    llvm_unreachable("unexpect opcode in getJumpAbsoluteTargetOpValue()");
#endif
  return 0;
}
//@CH8_1 }

//@getExprOpValue {
unsigned ZCPUMCCodeEmitter::
getExprOpValue(const MCExpr *Expr,SmallVectorImpl<MCFixup> &Fixups,
               const MCSubtargetInfo &STI) const {
//@getExprOpValue body {
  MCExpr::ExprKind Kind = Expr->getKind();
  if (Kind == MCExpr::Constant) {
    return cast<MCConstantExpr>(Expr)->getValue();
  }

  if (Kind == MCExpr::Binary) {
    unsigned Res = getExprOpValue(cast<MCBinaryExpr>(Expr)->getLHS(), Fixups, STI);
    Res += getExprOpValue(cast<MCBinaryExpr>(Expr)->getRHS(), Fixups, STI);
    return Res;
  }

  if (Kind == MCExpr::Target) {
    const ZCPUMCExpr *ZCPUExpr = cast<ZCPUMCExpr>(Expr);

    ZCPU::Fixups FixupKind = ZCPU::Fixups(0);
    switch (ZCPUExpr->getKind()) {
    default: llvm_unreachable("Unsupported fixup kind for target expression!");
    case ZCPUMCExpr::VK_ZCPU_HI:
      FixupKind = ZCPU::fixup_ZCPU_HI16;
      break;
    case ZCPUMCExpr::VK_ZCPU_LO:
      FixupKind = ZCPU::fixup_ZCPU_LO16;
      break;
    }
    Fixups.push_back(MCFixup::create(0, ZCPUExpr, MCFixupKind(FixupKind)));
    return 0;
  }

#if CH >= CH6_1
  if (Kind == MCExpr::SymbolRef) {
    ZCPU::Fixups FixupKind = ZCPU::Fixups(0);

  //@switch {
    switch(cast<MCSymbolRefExpr>(Expr)->getKind()) {
  //@switch }
    case MCSymbolRefExpr::VK_ZCPU_GPREL:
      FixupKind = ZCPU::fixup_ZCPU_GPREL16;
      break;
#if CH >= CH9_1 //2
    case MCSymbolRefExpr::VK_ZCPU_GOT_CALL:
      FixupKind = ZCPU::fixup_ZCPU_CALL16;
      break;
#endif
    case MCSymbolRefExpr::VK_ZCPU_GOT16:
      FixupKind = ZCPU::fixup_ZCPU_GOT_Global;
      break;
    case MCSymbolRefExpr::VK_ZCPU_GOT:
      FixupKind = ZCPU::fixup_ZCPU_GOT_Local;
      break;
    case MCSymbolRefExpr::VK_ZCPU_ABS_HI:
      FixupKind = ZCPU::fixup_ZCPU_HI16;
      break;
    case MCSymbolRefExpr::VK_ZCPU_ABS_LO:
      FixupKind = ZCPU::fixup_ZCPU_LO16;
      break;
#if CH >= CH12_1
    case MCSymbolRefExpr::VK_ZCPU_TLSGD:
      FixupKind = ZCPU::fixup_ZCPU_TLSGD;
      break;
    case MCSymbolRefExpr::VK_ZCPU_TLSLDM:
      FixupKind = ZCPU::fixup_ZCPU_TLSLDM;
      break;
    case MCSymbolRefExpr::VK_ZCPU_DTP_HI:
      FixupKind = ZCPU::fixup_ZCPU_DTP_HI;
      break;
    case MCSymbolRefExpr::VK_ZCPU_DTP_LO:
      FixupKind = ZCPU::fixup_ZCPU_DTP_LO;
      break;
    case MCSymbolRefExpr::VK_ZCPU_GOTTPREL:
      FixupKind = ZCPU::fixup_ZCPU_GOTTPREL;
      break;
    case MCSymbolRefExpr::VK_ZCPU_TP_HI:
      FixupKind = ZCPU::fixup_ZCPU_TP_HI;
      break;
    case MCSymbolRefExpr::VK_ZCPU_TP_LO:
      FixupKind = ZCPU::fixup_ZCPU_TP_LO;
      break;
#endif
    case MCSymbolRefExpr::VK_ZCPU_GOT_HI16:
      FixupKind = ZCPU::fixup_ZCPU_GOT_HI16;
      break;
    case MCSymbolRefExpr::VK_ZCPU_GOT_LO16:
      FixupKind = ZCPU::fixup_ZCPU_GOT_LO16;
      break;
    default:
      break;
    } // switch

    Fixups.push_back(MCFixup::create(0, Expr, MCFixupKind(FixupKind)));
    return 0;
  }
#endif // #if CH >= CH6_1

  // All of the information is in the fixup.
  return 0;
}

/// getMachineOpValue - Return binary encoding of operand. If the machine
/// operand requires relocation, record the relocation and return zero.
unsigned ZCPUMCCodeEmitter::
getMachineOpValue(const MCInst &MI, const MCOperand &MO,
                  SmallVectorImpl<MCFixup> &Fixups,
                  const MCSubtargetInfo &STI) const {
  if (MO.isReg()) {
    unsigned Reg = MO.getReg();
    unsigned RegNo = Ctx.getRegisterInfo()->getEncodingValue(Reg);
    return RegNo;
  } else if (MO.isImm()) {
    return static_cast<unsigned>(MO.getImm());
  } else if (MO.isFPImm()) {
    return static_cast<unsigned>(APFloat(MO.getFPImm())
        .bitcastToAPInt().getHiBits(32).getLimitedValue());
  }
  // MO must be an Expr.
  assert(MO.isExpr());
  return getExprOpValue(MO.getExpr(),Fixups, STI);
}

/// getMemEncoding - Return binary encoding of memory related operand.
/// If the offset operand requires relocation, record the relocation.
unsigned
ZCPUMCCodeEmitter::getMemEncoding(const MCInst &MI, unsigned OpNo,
                                  SmallVectorImpl<MCFixup> &Fixups,
                                  const MCSubtargetInfo &STI) const {
  // Base register is encoded in bits 20-16, offset is encoded in bits 15-0.
  assert(MI.getOperand(OpNo).isReg());
  unsigned RegBits = getMachineOpValue(MI, MI.getOperand(OpNo), Fixups, STI) << 16;
  unsigned OffBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups, STI);

  return (OffBits & 0xFFFF) | RegBits;
}

#include "ZCPUGenMCCodeEmitter.inc"

#endif // #if CH >= CH5_1
