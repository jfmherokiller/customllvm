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

#include "ZCPUGenAsmWriter.inc"

ZCPUInstPrinter::ZCPUInstPrinter(const MCAsmInfo &MAI,
                                               const MCInstrInfo &MII,
                                               const MCRegisterInfo &MRI)
    : MCInstPrinter(MAI, MII, MRI), ControlFlowCounter(0) {}

void ZCPUInstPrinter::printRegName(raw_ostream &OS,
                                          unsigned RegNo) const {

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

static std::string toString(const APFloat &FP) {
  // Print NaNs with custom payloads specially.
  if (FP.isNaN() &&
      !FP.bitwiseIsEqual(APFloat::getQNaN(FP.getSemantics())) &&
      !FP.bitwiseIsEqual(APFloat::getQNaN(FP.getSemantics(), /*Negative=*/true))) {
    APInt AI = FP.bitcastToAPInt();
    return
        std::string(AI.isNegative() ? "-" : "") + "nan:0x" +
        utohexstr(AI.getZExtValue() &
                  (AI.getBitWidth() == 32 ? INT64_C(0x007fffff) :
                                            INT64_C(0x000fffffffffffff)),
                  /*LowerCase=*/true);
  }

  // Use C99's hexadecimal floating-point representation.
  static const size_t BufBytes = 128;
  char buf[BufBytes];
  auto Written = FP.convertToHexString(
      buf, /*hexDigits=*/0, /*upperCase=*/false, APFloat::rmNearestTiesToEven);
  (void)Written;
  assert(Written != 0);
  assert(Written < BufBytes);
  return buf;
}

void ZCPUInstPrinter::printOperand(const MCInst *MI, unsigned OpNo,
                                          raw_ostream &O) {
}


const char *llvm::ZCPU::TypeToString(MVT Ty) {
  switch (Ty.SimpleTy) {
  default:
    llvm_unreachable("unsupported type");
  }
}
