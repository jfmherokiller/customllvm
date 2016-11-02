//=- ZCPUInstPrinter.cpp - ZCPU assembly instruction printing -=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Print MCInst instructions to wasm format.
///
//===----------------------------------------------------------------------===//

#include "InstPrinter/ZCPUInstPrinter.h"
#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "ZCPU.h"
#include "ZCPUMachineFunctionInfo.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Target/TargetRegisterInfo.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"
#define GET_INSTRUCTION_NAME
#include "ZCPUGenAsmWriter.inc"

ZCPUInstPrinter::ZCPUInstPrinter(const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI)
    : MCInstPrinter(MAI, MII, MRI), ControlFlowCounter(0) {}

void ZCPUInstPrinter::printRegName(raw_ostream &OS,
                                          unsigned RegNo) const {
  OS << StringRef(getRegisterName(RegNo)).upper();

}

void ZCPUInstPrinter::printInst(const MCInst *MI, raw_ostream &OS,
                                       StringRef Annot,
                                       const MCSubtargetInfo & /*STI*/) {
  // Print the instruction (this uses the AsmStrings from the .td files).
  printInstruction(MI, OS);

  // Print any additional variadic operands.
  const MCInstrDesc &Desc = MII.get(MI->getOpcode());
  if (Desc.isVariadic())
    for (auto i = Desc.getNumOperands(), e = MI->getNumOperands(); i < e; ++i) {
      if (i != 0)
        OS << ", ";
      printOperand(MI, i, OS);
    }

  // Print any added annotation.
  printAnnotation(OS, Annot);

  if (CommentStream) {
    // Observe any effects on the control flow stack, for use in annotating
    // control flow label references.
    switch (MI->getOpcode()) {
      default:
        break;
    }
  }
}

void ZCPUInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                   raw_ostream &O) {
  const MCOperand &MO = MI->getOperand (OpNo);

  if (MO.isReg()) {
    printRegName(O, MO.getReg());
    return ;
  }

  if (MO.isImm()) {
    O << (int)MO.getImm();
    return;
  }
  assert(MO.isExpr() && "Unknown operand kind in printOperand");
  MO.getExpr()->print(O, &MAI);
}


const char *llvm::ZCPU::TypeToString(MVT Ty) {
  switch (Ty.SimpleTy) {
    case MVT::i32:
      return "i32";
    case MVT::i64:
      return "i64";
    case MVT::f32:
      return "f32";
    case MVT::f64:
      return "f64";
    default:
      llvm_unreachable("unsupported type");
  }
}
