//===-- ZCPUFrameLowering.cpp - ZCPU Frame Information ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the ZCPU implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "ZCPUFrameLowering.h"
#include "ZCPU.h"
#include "ZCPUInstrInfo.h"
#include "ZCPUMachineFunctionInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
using namespace llvm;

ZCPUFrameLowering::ZCPUFrameLowering(const ZCPUTargetMachine &tm)
  : TargetFrameLowering(TargetFrameLowering::StackGrowsDown, 1, -2), TM(tm)
{}

bool ZCPUFrameLowering::hasFP(const MachineFunction &MF) const
{
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  return (MFI->getMaxCallFrameSize() > 0 || (MFI->getNumObjects() > 0));
}

void ZCPUFrameLowering::emitPrologue(MachineFunction &MF) const
{
  MachineBasicBlock &MBB = MF.front();  // Prolog goes into entry BB
  MachineBasicBlock::iterator MBBI = MBB.begin();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  ZCPUMachineFunctionInfo *ZCPUFI = MF.getInfo<ZCPUMachineFunctionInfo>();
  const ZCPUInstrInfo &TII = 
    *static_cast<const ZCPUInstrInfo*>(MF.getTarget().getInstrInfo());
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

  uint64_t StackSize     = MFI->getStackSize();
  uint64_t CallFrameSize = MFI->getMaxCallFrameSize();
  uint64_t FrameSize     = ZCPUFI->getCalleeSavedFrameSize();

  uint64_t NumBytes = StackSize + CallFrameSize - FrameSize;

  // Skip the callee-saved push instructions.
//  while (MBBI != MBB.end() && (MBBI->getOpcode() == ZCPU::PUSH16r))
//    MBBI++;
//
//  if (NumBytes || ZCPUFI->isNeedFP())
//  {
//    unsigned FP = TII.getRegisterInfo().getFrameRegister(MF);
//
//    BuildMI(MBB, MBBI, dl, TII.get(ZCPU::LD16ri), FP).addImm(-NumBytes);
//    BuildMI(MBB, MBBI, dl, TII.get(ZCPU::ADD16rSP), FP);
//    BuildMI(MBB, MBBI, dl, TII.get(ZCPU::LD16SPr)).addReg(FP);
//  }
}

void ZCPUFrameLowering::emitEpilogue(MachineFunction &MF,
  MachineBasicBlock &MBB) const
{
  const MachineFrameInfo *MFI = MF.getFrameInfo();
  ZCPUMachineFunctionInfo *ZCPUFI = MF.getInfo<ZCPUMachineFunctionInfo>();
  const ZCPUInstrInfo &TII =
    *static_cast<const ZCPUInstrInfo*>(MF.getTarget().getInstrInfo());

  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  unsigned RetOpcode = MBBI->getOpcode();
  DebugLoc dl = MBBI->getDebugLoc();

  //if (RetOpcode != ZCPU::RET)
    //llvm_unreachable("Can only insert epilog into returning blocks");

  // Get the number of bytes to allocate from the FrameInfo
  uint64_t StackSize     = MFI->getStackSize();
  uint64_t CallFrameSize = MFI->getMaxCallFrameSize();
  uint64_t FrameSize     = ZCPUFI->getCalleeSavedFrameSize();

  uint64_t NumBytes = StackSize + CallFrameSize - FrameSize;

  // Skip the callee-saved pop instructions.
  while (MBBI != MBB.begin())
  {
    MachineBasicBlock::iterator I = prior(MBBI);
    unsigned Opc = I->getOpcode();
      if (!I->isTerminator())
      {
          break;
      }
      MBBI--;
          
    //    if (Opc != ZCPU::POP16r && !I->isTerminator())
//      break;
//    MBBI--;
  }

  if (NumBytes)
  {
    unsigned FP = TII.getRegisterInfo().getFrameRegister(MF);

    //BuildMI(MBB, MBBI, dl, TII.get(ZCPU::LD16ri), FP).addImm(NumBytes);
    //BuildMI(MBB, MBBI, dl, TII.get(ZCPU::ADD16rSP), FP);
    //BuildMI(MBB, MBBI, dl, TII.get(ZCPU::LD16SPr)).addReg(FP, RegState::Kill);
  }
}

bool ZCPUFrameLowering::spillCalleeSavedRegisters(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator MI,
  const std::vector<CalleeSavedInfo> &CSI,
  const TargetRegisterInfo *TRI) const
{
  if (CSI.empty())
    return false;

  DebugLoc dl;
  if (MI != MBB.end()) dl = MI->getDebugLoc();

  MachineFunction &MF = *MBB.getParent();
  const TargetInstrInfo &TII = *MF.getTarget().getInstrInfo();
  ZCPUMachineFunctionInfo *MFI = MF.getInfo<ZCPUMachineFunctionInfo>();
  MFI->setCalleeSavedFrameSize(CSI.size() * 2);

  for (unsigned i = CSI.size(); i != 0; i--)
  {
    unsigned Reg = CSI[i-1].getReg();

    // Add the callee-saved register as live-in. It's killed at the spill.
    MBB.addLiveIn(Reg);
    //BuildMI(MBB, MI, dl, TII.get(ZCPU::PUSH16r)).addReg(Reg, RegState::Kill);
  }
  return true;
}

bool ZCPUFrameLowering::restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator MI,
  const std::vector<CalleeSavedInfo> &CSI,
  const TargetRegisterInfo *TRI) const
{
  if (CSI.empty())
    return false;

  DebugLoc dl;
  if (MI != MBB.end()) dl = MI->getDebugLoc();

  MachineFunction &MF = *MBB.getParent();
  const TargetInstrInfo &TII = *MF.getTarget().getInstrInfo();

  for (unsigned i = 0, e = CSI.size(); i != e; i++)
    //BuildMI(MBB, MI, dl, TII.get(ZCPU::POP16r), CSI[i].getReg());

  return true;
}

void ZCPUFrameLowering::processFunctionBeforeCalleeSavedScan(
  MachineFunction &MF, RegScavenger *RS) const
{
  if (hasFP(MF))
  {
    unsigned FP = MF.getTarget().getRegisterInfo()->getFrameRegister(MF);
    MF.getRegInfo().setPhysRegUsed(FP);
  }
}

void ZCPUFrameLowering::eliminateCallFramePseudoInstr(MachineFunction &MF,
  MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const
{
  MachineInstr &MI = *I;

  switch (MI.getOpcode())
  {
  default: llvm_unreachable("Cannot handle this call frame pseudo instruction");
  case ZCPU::ADJCALLSTACKDOWN:
  case ZCPU::ADJCALLSTACKUP:
    break;
  }
  MBB.erase(I);
}
