//===-- ZCPUAsmPrinter.h - ZCPU LLVM Assembly Printer -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// ZCPU assembly code printer class.
//
//===----------------------------------------------------------------------===//

#ifndef ZCPUASMPRINTER_H
#define ZCPUASMPRINTER_H

#include "ZCPUTargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"

namespace llvm {
  class ZCPUAsmPrinter : public AsmPrinter {
  public:
    explicit ZCPUAsmPrinter(TargetMachine &TM, MCStreamer &Streamer)
      : AsmPrinter(TM, Streamer) {}
    virtual const char *getPassName() const {
      return "ZCPU Assembly Printer";
    }
    virtual void EmitInstruction(const MachineInstr *MI);
  }; // end class ZCPUAsmPrinter
} // end namespace llvm

#endif
