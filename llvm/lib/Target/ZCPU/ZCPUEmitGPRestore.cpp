//===-- ZCPUEmitGPRestore.cpp - Emit GP Restore Instruction ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass emits instructions that restore $gp right
// after jalr instructions.
//
//===----------------------------------------------------------------------===//

#include "ZCPU.h"
#if CH >= CH9_3
#ifdef ENABLE_GPRESTORE

#include "ZCPUTargetMachine.h"
#include "ZCPUMachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/ADT/Statistic.h"

using namespace llvm;

#define DEBUG_TYPE "emit-gp-restore"

namespace {
  struct Inserter : public MachineFunctionPass {

    TargetMachine &TM;

    static char ID;
    Inserter(TargetMachine &tm)
      : MachineFunctionPass(ID), TM(tm) { }

    virtual const char *getPassName() const {
      return "ZCPU Emit GP Restore";
    }

    bool runOnMachineFunction(MachineFunction &F);
  };
  char Inserter::ID = 0;
} // end of anonymous namespace

bool Inserter::runOnMachineFunction(MachineFunction &F) {
  ZCPUFunctionInfo *ZCPUFI = F.getInfo<ZCPUFunctionInfo>();
  const TargetSubtargetInfo *STI =  TM.getSubtargetImpl(*(F.getFunction()));
  const TargetInstrInfo *TII = STI->getInstrInfo();

  if ((TM.getRelocationModel() != Reloc::PIC_) ||
      (!ZCPUFI->globalBaseRegFixed()))
    return false;

  bool Changed = false;
  int FI = ZCPUFI->getGPFI();

  for (MachineFunction::iterator MFI = F.begin(), MFE = F.end();
       MFI != MFE; ++MFI) {
    MachineBasicBlock& MBB = *MFI;
    MachineBasicBlock::iterator I = MFI->begin();
    
    /// IsLandingPad - Indicate that this basic block is entered via an
    /// exception handler.
    // If MBB is a landing pad, insert instruction that restores $gp after
    // EH_LABEL.
    if (MBB.isLandingPad()) {
      // Find EH_LABEL first.
      for (; I->getOpcode() != TargetOpcode::EH_LABEL; ++I) ;

      // Insert ld.
      ++I;
      DebugLoc dl = I != MBB.end() ? I->getDebugLoc() : DebugLoc();
      BuildMI(MBB, I, dl, TII->get(ZCPU::LD), ZCPU::GP).addFrameIndex(FI)
                                                       .addImm(0);
      Changed = true;
    }

    while (I != MFI->end()) {
      if (I->getOpcode() != ZCPU::JALR) {
        ++I;
        continue;
      }

      DebugLoc dl = I->getDebugLoc();
      // emit ld $gp, ($gp save slot on stack) after jalr
      BuildMI(MBB, ++I, dl, TII->get(ZCPU::LD), ZCPU::GP).addFrameIndex(FI)
                                                         .addImm(0);
      Changed = true;
    }
  }

  return Changed;
}

/// createZCPUEmitGPRestorePass - Returns a pass that emits instructions that
/// restores $gp clobbered by jalr instructions.
FunctionPass *llvm::createZCPUEmitGPRestorePass(ZCPUTargetMachine &tm) {
  return new Inserter(tm);
}

#endif

#endif
