//===-- ZCPUInstrInfo.cpp - ZCPU Instruction Information ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the ZCPU implementation of the
/// TargetInstrInfo class.
///
//===----------------------------------------------------------------------===//

#include "ZCPUInstrInfo.h"
#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "ZCPUMachineFunctionInfo.h"
#include "ZCPUSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
using namespace llvm;

#define DEBUG_TYPE "zcpu-instr-info"

#define GET_INSTRINFO_CTOR_DTOR
#include "ZCPUGenInstrInfo.inc"

ZCPUInstrInfo::ZCPUInstrInfo(const ZCPUSubtarget &STI)
    : ZCPUGenInstrInfo(ZCPU::ADJCALLSTACKDOWN,
                              ZCPU::ADJCALLSTACKUP),
      RI(STI.getTargetTriple()) {

}

bool ZCPUInstrInfo::isReallyTriviallyReMaterializable(
    const MachineInstr &MI, AliasAnalysis *AA) const {
  switch (MI.getOpcode()) {
  default:
    return false;
  }
}

void ZCPUInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                       MachineBasicBlock::iterator I,
                                       const DebugLoc &DL, unsigned DestReg,
                                       unsigned SrcReg, bool KillSrc) const {
  // This method is called by post-RA expansion, which expects only pregs to
  // exist. However we need to handle both here.
  auto &MRI = MBB.getParent()->getRegInfo();
  const TargetRegisterClass *RC =
      TargetRegisterInfo::isVirtualRegister(DestReg)
          ? MRI.getRegClass(DestReg)
          : MRI.getTargetRegisterInfo()->getMinimalPhysRegClass(DestReg);

  unsigned CopyLocalOpcode;
    llvm_unreachable("Unexpected register class");

  BuildMI(MBB, I, DL, get(CopyLocalOpcode), DestReg)
      .addReg(SrcReg, KillSrc ? RegState::Kill : 0);
}

MachineInstr *
ZCPUInstrInfo::commuteInstructionImpl(MachineInstr &MI, bool NewMI,
                                             unsigned OpIdx1,
                                             unsigned OpIdx2) const {
  // If the operands are stackified, we can't reorder them.
  ZCPUFunctionInfo &MFI =
      *MI.getParent()->getParent()->getInfo<ZCPUFunctionInfo>();
  if (MFI.isVRegStackified(MI.getOperand(OpIdx1).getReg()) ||
      MFI.isVRegStackified(MI.getOperand(OpIdx2).getReg()))
    return nullptr;

  // Otherwise use the default implementation.
  //return TargetInstrInfo::commuteInstructionImpl(MI, NewMI, OpIdx1, OpIdx2);
    return nullptr;
}

// Branch analysis.
bool ZCPUInstrInfo::analyzeBranch(MachineBasicBlock &MBB,
                                         MachineBasicBlock *&TBB,
                                         MachineBasicBlock *&FBB,
                                         SmallVectorImpl<MachineOperand> &Cond,
                                         bool /*AllowModify*/) const {
  return false;
}

unsigned ZCPUInstrInfo::RemoveBranch(MachineBasicBlock &MBB) const {
  MachineBasicBlock::instr_iterator I = MBB.instr_end();
  unsigned Count = 0;

  while (I != MBB.instr_begin()) {
    --I;
    if (I->isDebugValue())
      continue;
    if (!I->isTerminator())
      break;
    // Remove the branch.
    I->eraseFromParent();
    I = MBB.instr_end();
    ++Count;
  }

  return Count;
}

unsigned ZCPUInstrInfo::InsertBranch(MachineBasicBlock &MBB,
                                            MachineBasicBlock *TBB,
                                            MachineBasicBlock *FBB,
                                            ArrayRef<MachineOperand> Cond,
                                            const DebugLoc &DL) const {
  return 0;
}

bool ZCPUInstrInfo::ReverseBranchCondition(
    SmallVectorImpl<MachineOperand> &Cond) const {
  assert(Cond.size() == 2 && "Expected a flag and a successor block");
  Cond.front() = MachineOperand::CreateImm(!Cond.front().getImm());
  return false;
}
