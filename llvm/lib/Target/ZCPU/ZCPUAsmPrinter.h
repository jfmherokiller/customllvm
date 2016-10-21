//===-- ZCPUAsmPrinter.h - ZCPU LLVM Assembly Printer ----------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// ZCPU Assembly printer class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUASMPRINTER_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUASMPRINTER_H

#include "ZCPUConfig.h"
#if CH >= CH3_2

#include "ZCPUMachineFunction.h"
#include "ZCPUMCInstLower.h"
#include "ZCPUSubtarget.h"
#include "ZCPUTargetMachine.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class MCStreamer;
class MachineInstr;
class MachineBasicBlock;
class Module;
class raw_ostream;

class LLVM_LIBRARY_VISIBILITY ZCPUAsmPrinter : public AsmPrinter {

  void EmitInstrWithMacroNoAT(const MachineInstr *MI);

private:
#if CH >= CH9_1
  // tblgen'erated function.
  bool emitPseudoExpansionLowering(MCStreamer &OutStreamer,
                                   const MachineInstr *MI);
#endif

#if CH >= CH9_3 //1
#ifdef ENABLE_GPRESTORE
  void emitPseudoCPRestore(MCStreamer &OutStreamer,
                           const MachineInstr *MI);
#endif
#endif //#if CH >= CH9_3 //1

  // lowerOperand - Convert a MachineOperand into the equivalent MCOperand.
  bool lowerOperand(const MachineOperand &MO, MCOperand &MCOp);

#if CH >= CH8_2 //1
  bool isLongBranchPseudo(int Opcode) const;
#endif

public:

  const ZCPUSubtarget *Subtarget;
  const ZCPUFunctionInfo *ZCPUFI;
  ZCPUMCInstLower MCInstLowering;

  explicit ZCPUAsmPrinter(TargetMachine &TM,
                          std::unique_ptr<MCStreamer> Streamer)
    : AsmPrinter(TM, std::move(Streamer)), 
      MCInstLowering(*this) {
    Subtarget = static_cast<ZCPUTargetMachine &>(TM).getSubtargetImpl();
  }

  virtual const char *getPassName() const override {
    return "ZCPU Assembly Printer";
  }

  virtual bool runOnMachineFunction(MachineFunction &MF) override;

//- EmitInstruction() must exists or will have run time error.
  void EmitInstruction(const MachineInstr *MI) override;
  void printSavedRegsBitmask(raw_ostream &O);
  void printHex32(unsigned int Value, raw_ostream &O);
  void emitFrameDirective();
  const char *getCurrentABIString() const;
  void EmitFunctionEntryLabel() override;
  void EmitFunctionBodyStart() override;
  void EmitFunctionBodyEnd() override;
#if CH >= CH11_2
  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                       unsigned AsmVariant, const char *ExtraCode,
                       raw_ostream &O) override;
  bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNum,
                             unsigned AsmVariant, const char *ExtraCode,
                             raw_ostream &O) override;
  void printOperand(const MachineInstr *MI, int opNum, raw_ostream &O);
#endif
  void EmitStartOfAsmFile(Module &M) override;
  void PrintDebugValueComment(const MachineInstr *MI, raw_ostream &OS);
};
}

#endif // #if CH >= CH3_2

#endif

