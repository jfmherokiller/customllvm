//===-- ZCPUMCInstLower.cpp - Convert ZCPU MachineInstr to an MCInst --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower ZCPU MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "ZCPUMCInstLower.h"
#include "ZCPUAsmPrinter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/Target/Mangler.h"
using namespace llvm;

ZCPUMCInstLower::ZCPUMCInstLower(Mangler *mang, const MachineFunction &mf,
  ZCPUAsmPrinter &asmprinter)
  : Ctx(mf.getContext()), Mang(mang), MF(mf), TM(mf.getTarget()),
  MAI(*TM.getMCAsmInfo()), AsmPrinter(asmprinter)
{}

void ZCPUMCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const
{
  OutMI.setOpcode(MI->getOpcode());

  for (unsigned i = 0, e = MI->getNumOperands(); i != e; i++)
  {
    const MachineOperand &MO = MI->getOperand(i);

    MCOperand MCOp;
    switch (MO.getType())
    {
    default:
      MI->dump();
      llvm_unreachable("unknown operand type");
    case MachineOperand::MO_Register:
      // Ignore all implicit register operands
      if (MO.isImplicit()) continue;
      MCOp = MCOperand::CreateReg(MO.getReg());
      break;
    case MachineOperand::MO_Immediate:
      MCOp = MCOperand::CreateImm(MO.getImm());
      break;
    case MachineOperand::MO_MachineBasicBlock:
      MCOp = MCOperand::CreateExpr(MCSymbolRefExpr::Create(
        MO.getMBB()->getSymbol(), Ctx));
      break;
    case MachineOperand::MO_GlobalAddress:
      MCOp = LowerSymbolOperand(MO, GetGlobalAddressSymbol(MO));
      break;
    case MachineOperand::MO_ExternalSymbol:
      MCOp = LowerSymbolOperand(MO, GetExternalSymbolSymbol(MO));
      break;
    case MachineOperand::MO_RegisterMask:
      // Ignore call clobbers.
      continue;
    }
    OutMI.addOperand(MCOp);
  }
}

MCOperand ZCPUMCInstLower::LowerSymbolOperand(const MachineOperand &MO,
  MCSymbol *Sym) const
{
  if (MO.getTargetFlags() != 0)
    llvm_unreachable("Unknown target flag on GV operand");

  const MCExpr *Expr = MCSymbolRefExpr::Create(Sym, Ctx);
  return MCOperand::CreateExpr(Expr);
}

MCSymbol *ZCPUMCInstLower::GetGlobalAddressSymbol(
  const MachineOperand &MO) const
{
  if (MO.getTargetFlags() != 0)
    llvm_unreachable("Unknown target flag on GV operand");
  return AsmPrinter.getSymbol(MO.getGlobal());
}

MCSymbol *ZCPUMCInstLower::GetExternalSymbolSymbol(
  const MachineOperand &MO) const
{
  if (MO.getTargetFlags() != 0)
    llvm_unreachable("Unknown target flag on GV operand");
  return AsmPrinter.GetExternalSymbolSymbol(MO.getSymbolName());
}
