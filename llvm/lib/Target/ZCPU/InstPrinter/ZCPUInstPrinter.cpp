//===-- ZCPUInstPrinter.cpp - Convert ZCPU MCInst to assembly syntax ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an ZCPU MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "ZCPUInstPrinter.h"

#if CH >= CH5_1 //1
#include "MCTargetDesc/ZCPUMCExpr.h"
#endif
#if CH >= CH3_2
#include "ZCPUInstrInfo.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

#define PRINT_ALIAS_INSTR
#include "ZCPUGenAsmWriter.inc"

void ZCPUInstPrinter::printRegName(raw_ostream &OS, unsigned RegNo) const {
//- getRegisterName(RegNo) defined in ZCPUGenAsmWriter.inc which indicate in 
//   ZCPU.td.
  OS << '$' << StringRef(getRegisterName(RegNo)).lower();
}

//@1 {
void ZCPUInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
                                StringRef Annot, const MCSubtargetInfo &STI) {
  // Try to print any aliases first.
  if (!printAliasInstr(MI, O))
//@1 }
    //- printInstruction(MI, O) defined in ZCPUGenAsmWriter.inc which came from 
    //   ZCPU.td indicate.
    printInstruction(MI, O);
  printAnnotation(O, Annot);
}

//@printExpr {
static void printExpr(const MCExpr *Expr, const MCAsmInfo *MAI,
                      raw_ostream &OS) {
//@printExpr body {
  int Offset = 0;
  const MCSymbolRefExpr *SRE;

  if (const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr)) {
    SRE = dyn_cast<MCSymbolRefExpr>(BE->getLHS());
    const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(BE->getRHS());
    assert(SRE && CE && "Binary expression must be sym+const.");
    Offset = CE->getValue();
#if CH >= CH5_1 //2
  } else if (const ZCPUMCExpr *ME = dyn_cast<ZCPUMCExpr>(Expr)) {
    ME->print(OS, MAI);
    return;
#endif
  } else
    SRE = cast<MCSymbolRefExpr>(Expr);

  MCSymbolRefExpr::VariantKind Kind = SRE->getKind();

  switch (Kind) {
  default:                                 llvm_unreachable("Invalid kind!");
  case MCSymbolRefExpr::VK_None:           break;
#if CH >= CH6_1 //VK_ZCPU_GPREL
// ZCPU_GPREL is for llc -march=cpu0 -relocation-model=static
  case MCSymbolRefExpr::VK_ZCPU_GPREL:     OS << "%gp_rel("; break;
#endif
#if CH >= CH9_1
  case MCSymbolRefExpr::VK_ZCPU_GOT_CALL:  OS << "%call16("; break;
#endif
#if CH >= CH6_1 //VK_ZCPU_GOT16
  case MCSymbolRefExpr::VK_ZCPU_GOT16:     OS << "%got(";    break;
  case MCSymbolRefExpr::VK_ZCPU_GOT:       OS << "%got(";    break;
  case MCSymbolRefExpr::VK_ZCPU_ABS_HI:    OS << "%hi(";     break;
  case MCSymbolRefExpr::VK_ZCPU_ABS_LO:    OS << "%lo(";     break;
#endif
#if CH >= CH12_1
  case MCSymbolRefExpr::VK_ZCPU_TLSGD:     OS << "%tlsgd(";  break;
  case MCSymbolRefExpr::VK_ZCPU_TLSLDM:    OS << "%tlsldm(";  break;
  case MCSymbolRefExpr::VK_ZCPU_DTP_HI:    OS << "%dtp_hi(";  break;
  case MCSymbolRefExpr::VK_ZCPU_DTP_LO:    OS << "%dtp_lo(";  break;
  case MCSymbolRefExpr::VK_ZCPU_GOTTPREL:  OS << "%gottprel("; break;
  case MCSymbolRefExpr::VK_ZCPU_TP_HI:     OS << "%tp_hi("; break;
  case MCSymbolRefExpr::VK_ZCPU_TP_LO:     OS << "%tp_lo("; break;
#endif
#if CH >= CH6_1
  case MCSymbolRefExpr::VK_ZCPU_GOT_HI16:  OS << "%got_hi("; break;
  case MCSymbolRefExpr::VK_ZCPU_GOT_LO16:  OS << "%got_lo("; break;
#endif
  }

  SRE->getSymbol().print(OS, MAI);

  if (Offset) {
    if (Offset > 0)
      OS << '+';
    OS << Offset;
  }

  if ((Kind == MCSymbolRefExpr::VK_ZCPU_GPOFF_HI) ||
      (Kind == MCSymbolRefExpr::VK_ZCPU_GPOFF_LO))
    OS << ")))";
  else if (Kind != MCSymbolRefExpr::VK_None)
    OS << ')';
}

void ZCPUInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
    return;
  }

  if (Op.isImm()) {
    O << Op.getImm();
    return;
  }

  assert(Op.isExpr() && "unknown operand kind in printOperand");
  printExpr(Op.getExpr(), &MAI, O);
}

void ZCPUInstPrinter::printUnsignedImm(const MCInst *MI, int opNum,
                                       raw_ostream &O) {
  const MCOperand &MO = MI->getOperand(opNum);
  if (MO.isImm())
    O << (unsigned short int)MO.getImm();
  else
    printOperand(MI, opNum, O);
}

void ZCPUInstPrinter::
printMemOperand(const MCInst *MI, int opNum, raw_ostream &O) {
  // Load/Store memory operands -- imm($reg)
  // If PIC target the target is loaded as the
  // pattern ld $t9,%call16($gp)
  printOperand(MI, opNum+1, O);
  O << "(";
  printOperand(MI, opNum, O);
  O << ")";
}

//#if CH >= CH7_1
// The DAG data node, mem_ea of ZCPUInstrInfo.td, cannot be disabled by
// ch7_1, only opcode node can be disabled.
void ZCPUInstPrinter::
printMemOperandEA(const MCInst *MI, int opNum, raw_ostream &O) {
  // when using stack locations for not load/store instructions
  // print the same way as all normal 3 operand instructions.
  printOperand(MI, opNum, O);
  O << ", ";
  printOperand(MI, opNum+1, O);
  return;
}
//#endif

#endif // #if CH >= CH3_2
