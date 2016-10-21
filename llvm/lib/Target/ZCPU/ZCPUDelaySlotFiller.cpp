//===-- ZCPUDelaySlotFiller.cpp - ZCPU Delay Slot Filler ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Simple pass to fill delay slots with useful instructions.
//
//===----------------------------------------------------------------------===//

#include "ZCPU.h"
#if CH >= CH8_2

#include "ZCPUInstrInfo.h"
#include "ZCPUTargetMachine.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/CodeGen/MachineBranchProbabilityInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/PseudoSourceValue.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegisterInfo.h"

using namespace llvm;

#define DEBUG_TYPE "delay-slot-filler"

STATISTIC(FilledSlots, "Number of delay slots filled");

namespace {
  typedef MachineBasicBlock::iterator Iter;
  typedef MachineBasicBlock::reverse_iterator ReverseIter;

  class Filler : public MachineFunctionPass {
  public:
    Filler(TargetMachine &tm)
      : MachineFunctionPass(ID) { }

    const char *getPassName() const override {
      return "ZCPU Delay Slot Filler";
    }

    bool runOnMachineFunction(MachineFunction &F) override {
      bool Changed = false;
      for (MachineFunction::iterator FI = F.begin(), FE = F.end();
           FI != FE; ++FI)
        Changed |= runOnMachineBasicBlock(*FI);
      return Changed;
    }
  private:
    bool runOnMachineBasicBlock(MachineBasicBlock &MBB);

    static char ID;
  };
  char Filler::ID = 0;
} // end of anonymous namespace

static bool hasUnoccupiedSlot(const MachineInstr *MI) {
  return MI->hasDelaySlot() && !MI->isBundledWithSucc();
}

/// runOnMachineBasicBlock - Fill in delay slots for the given basic block.
/// We assume there is only one delay slot per delayed instruction.
bool Filler::runOnMachineBasicBlock(MachineBasicBlock &MBB) {
  bool Changed = false;
  const ZCPUSubtarget &STI = MBB.getParent()->getSubtarget<ZCPUSubtarget>();
  const ZCPUInstrInfo *TII = STI.getInstrInfo();

  for (Iter I = MBB.begin(); I != MBB.end(); ++I) {
    if (!hasUnoccupiedSlot(&*I))
      continue;

    ++FilledSlots;
    Changed = true;

    // Bundle the NOP to the instruction with the delay slot.
    BuildMI(MBB, std::next(I), I->getDebugLoc(), TII->get(ZCPU::NOP));
    MIBundleBuilder(MBB, I, std::next(I, 2));
  }

  return Changed;
}

/// createZCPUDelaySlotFillerPass - Returns a pass that fills in delay
/// slots in ZCPU MachineFunctions
FunctionPass *llvm::createZCPUDelaySlotFillerPass(ZCPUTargetMachine &tm) {
  return new Filler(tm);
}

#endif
