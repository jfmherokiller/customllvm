//===-- ZCPUMCInstLower.h - Lower MachineInstr to MCInst ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef ZCPUMCINSTLOWER_H
#define ZCPUMCINSTLOWER_H

#include "llvm/Support/Compiler.h"

namespace llvm {
  class MCAsmInfo;
  class MCContext;
  class MCInst;
  class MCOperand;
  class MCSymbol;
  class MachineFunction;
  class MachineInstr;
  class MachineOperand;
  class Mangler;
  class TargetMachine;
  class ZCPUAsmPrinter;

  class ZCPUMCInstLower {
    MCContext &Ctx;
    Mangler *Mang;
    const MachineFunction &MF;
    const TargetMachine &TM;
    const MCAsmInfo &MAI;
    ZCPUAsmPrinter &AsmPrinter;
  public:
    ZCPUMCInstLower(Mangler *mang, const MachineFunction &mf,
      ZCPUAsmPrinter &asmprinter);
    void Lower(const MachineInstr *MI, MCInst &OutMI) const;

    MCOperand LowerSymbolOperand(const MachineOperand &MO, MCSymbol *Sym) const;

    MCSymbol *GetGlobalAddressSymbol(const MachineOperand &MO) const;
    MCSymbol *GetExternalSymbolSymbol(const MachineOperand &MO) const;
  }; // end class ZCPUMCInstLower
} // end namespace llvm

#endif
