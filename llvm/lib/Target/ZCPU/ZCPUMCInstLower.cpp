// ZCPUMCInstLower.cpp - Convert ZCPU MachineInstr to an MCInst //
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains code to lower ZCPU MachineInstrs to their
/// corresponding MCInst records.
///
//===----------------------------------------------------------------------===//

#include "ZCPUMCInstLower.h"
#include "ZCPUMachineFunctionInfo.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/IR/Constants.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

MCSymbol *
ZCPUMCInstLower::GetGlobalAddressSymbol(const MachineOperand &MO) const {
  return Printer.getSymbol(MO.getGlobal());
}

MCSymbol *ZCPUMCInstLower::GetExternalSymbolSymbol(
    const MachineOperand &MO) const {
  return Printer.GetExternalSymbolSymbol(MO.getSymbolName());
}

MCOperand ZCPUMCInstLower::LowerSymbolOperand(MCSymbol *Sym,
                                                     int64_t Offset,
                                                     bool IsFunc) const {
  MCSymbolRefExpr::VariantKind VK = MCSymbolRefExpr::VK_None;
  const MCExpr *Expr = MCSymbolRefExpr::create(Sym, VK, Ctx);

  if (Offset != 0) {
    if (IsFunc)
      report_fatal_error("Function addresses with offsets not supported");
    Expr =
        MCBinaryExpr::createAdd(Expr, MCConstantExpr::create(Offset, Ctx), Ctx);
  }

  return MCOperand::createExpr(Expr);
}

void ZCPUMCInstLower::Lower(const MachineInstr *MI,
                                   MCInst &OutMI) const {
  OutMI.setOpcode(MI->getOpcode());

  for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
    const MachineOperand &MO = MI->getOperand(i);

    MCOperand MCOp;
    switch (MO.getType()) {
    default:
      MI->dump();
      llvm_unreachable("unknown operand type");
    case MachineOperand::MO_MachineBasicBlock:
      MI->dump();
      llvm_unreachable("MachineBasicBlock operand should have been rewritten");
    case MachineOperand::MO_Register: {
      // Ignore all implicit register operands.
      if (MO.isImplicit())
        continue;
      const ZCPUFunctionInfo &MFI =
          *MI->getParent()->getParent()->getInfo<ZCPUFunctionInfo>();
      unsigned WAReg = MFI.getWAReg(MO.getReg());
      MCOp = MCOperand::createReg(WAReg);
      break;
    }
    case MachineOperand::MO_Immediate:
      MCOp = MCOperand::createImm(MO.getImm());
      break;
    case MachineOperand::MO_FPImmediate: {
      // TODO: MC converts all floating point immediate operands to double.
      // This is fine for numeric values, but may cause NaNs to change bits.
      const ConstantFP *Imm = MO.getFPImm();
      if (Imm->getType()->isFloatTy())
        MCOp = MCOperand::createFPImm(Imm->getValueAPF().convertToFloat());
      else if (Imm->getType()->isDoubleTy())
        MCOp = MCOperand::createFPImm(Imm->getValueAPF().convertToDouble());
      else
        llvm_unreachable("unknown floating point immediate type");
      break;
    }
    case MachineOperand::MO_GlobalAddress:
      assert(MO.getTargetFlags() == 0 &&
             "ZCPU does not use target flags on GlobalAddresses");
      MCOp = LowerSymbolOperand(GetGlobalAddressSymbol(MO), MO.getOffset(),
                                MO.getGlobal()->getValueType()->isFunctionTy());
      break;
    case MachineOperand::MO_ExternalSymbol:
      // The target flag indicates whether this is a symbol for a
      // variable or a function.
      assert((MO.getTargetFlags() & -2) == 0 &&
             "ZCPU uses only one target flag bit on ExternalSymbols");
      MCOp = LowerSymbolOperand(GetExternalSymbolSymbol(MO), /*Offset=*/0,
                                MO.getTargetFlags() & 1);
      break;
    }

    OutMI.addOperand(MCOp);
  }
}
