//===-- ZCPUInstPrinter.cpp - Convert ZCPU MCInst to assembly syntax --------===//
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
#include "ZCPU.h"
#include "ZCPUInstrInfo.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

// Include the auto-generated portion of the assembler writer
#include "ZCPUGenAsmWriter.inc"

void ZCPUInstPrinter::printInst(const MCInst *MI, raw_ostream &O,
  StringRef Annot)
{
  printInstruction(MI, O);
  printAnnotation(O, Annot);
}

void ZCPUInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
  raw_ostream &O)
{
  const MCOperand &Op = MI->getOperand(OpNo);

  if (Op.isReg())
    O << getRegisterName(Op.getReg());
  else if (Op.isImm())
    O << Op.getImm();
  else if (Op.isExpr())
    O << *Op.getExpr();
  else assert(0 && "unknown operand kind in printOperand");
}

void ZCPUInstPrinter::printCCOperand(const MCInst *MI, unsigned OpNo,
  raw_ostream &O)
{
  const MCOperand &Op = MI->getOperand(OpNo);
  assert(Op.isImm() && "Invalid CC operand");

  const char *cond;

  switch (Op.getImm())
  {
  default:
    llvm_unreachable("Invalid CC operand");
  case ZCPU::COND_NZ:
    cond = "nz";
    break;
  case ZCPU::COND_Z:
    cond = "z";
    break;
  case ZCPU::COND_NC:
    cond = "nc";
    break;
  case ZCPU::COND_C:
    cond = "c";
    break;
  case ZCPU::COND_PO:
    cond = "po";
    break;
  case ZCPU::COND_PE:
    cond = "pe";
    break;
  case ZCPU::COND_P:
    cond = "p";
    break;
  case ZCPU::COND_M:
    cond = "m";
    break;
  }
  O << cond;
}

void ZCPUInstPrinter::printXMemOperand(const MCInst *MI, unsigned OpNo,
  raw_ostream &O)
{
  const MCOperand &Base = MI->getOperand(OpNo);
  const MCOperand &Disp = MI->getOperand(OpNo+1);

  if (Base.isReg())
  {
    assert(Disp.isImm() && "Expected immediate in displacement field");
    int64_t Offset = Disp.getImm();
    O << getRegisterName(Base.getReg());
    if (Offset >= 0) O << '+';
    O << Offset;
  }
  else llvm_unreachable("Invalid operand");
}
