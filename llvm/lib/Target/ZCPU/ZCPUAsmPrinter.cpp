//===-- ZCPUAsmPrinter.cpp - ZCPU LLVM Assembly Printer ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to ZCPU machine code.
//
//===----------------------------------------------------------------------===//

#include "ZCPUAsmPrinter.h"
#include "ZCPU.h"
#include "ZCPUMCInstLower.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

void ZCPUAsmPrinter::EmitInstruction(const MachineInstr *MI)
{
  ZCPUMCInstLower MCInstLowering(Mang, *MF, *this);

  MCInst TmpInst;
  MCInstLowering.Lower(MI, TmpInst);
  OutStreamer.EmitInstruction(TmpInst);
}

//===----------------------------------------------------------------------===//
// Target Registry Stuff
//===----------------------------------------------------------------------===//

extern "C" void LLVMInitializeZCPUAsmPrinter()
{
  RegisterAsmPrinter<ZCPUAsmPrinter> X(TheZCPUTarget);
}
