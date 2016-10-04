//===-- ZCPUInstrInfo.cpp - ZCPU Instruction Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the ZCPU implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "ZCPUInstrInfo.h"
#include "ZCPU.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#define GET_INSTRINFO_CTOR
#include "ZCPUGenInstrInfo.inc"

using namespace llvm;

ZCPUInstrInfo::ZCPUInstrInfo(ZCPUTargetMachine &tm)
  : ZCPUGenInstrInfo(ZCPU::ADJCALLSTACKDOWN, ZCPU::ADJCALLSTACKUP),
  RI(tm, *this), TM(tm)
{}

void ZCPUInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator I, DebugLoc DL,
  unsigned DestReg, unsigned SrcReg, bool KillSrc) const
{
  if (ZCPU::GR8RegClass.contains(DestReg, SrcReg))
  {
    // copy GR8 to GR8
    BuildMI(MBB, I, DL, get(ZCPU::LD8rr), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }
  else if (ZCPU::BR16RegClass.contains(DestReg, SrcReg))
  {
    // copy BR16 to BR16
    if (ZCPU::EXR16RegClass.contains(DestReg, SrcReg) && KillSrc)
    {
      BuildMI(MBB, I, DL, get(ZCPU::EX_DE_HL));
    }
    else
    {
      unsigned DestSubReg, SrcSubReg;

      DestSubReg = RI.getSubReg(DestReg, ZCPU::subreg_hi);
      SrcSubReg  = RI.getSubReg(SrcReg,  ZCPU::subreg_hi);
      BuildMI(MBB, I, DL, get(ZCPU::LD8rr), DestSubReg)
        .addReg(SrcSubReg);

      DestSubReg = RI.getSubReg(DestReg, ZCPU::subreg_lo);
      SrcSubReg  = RI.getSubReg(SrcReg,  ZCPU::subreg_lo);
      BuildMI(MBB, I, DL, get(ZCPU::LD8rr), DestSubReg)
        .addReg(SrcSubReg);
    }

    if (KillSrc)
      BuildMI(MBB, I, DL, get(ZCPU::KILL))
        .addReg(SrcReg);
    return;
  }
  else if (ZCPU::GR16RegClass.contains(DestReg) ||
           ZCPU::GR16RegClass.contains(SrcReg))
  {
    // copy GR16 to GR16
    BuildMI(MBB, I, DL, get(ZCPU::PUSH16r))
      .addReg(SrcReg, getKillRegState(KillSrc));
    BuildMI(MBB, I, DL, get(ZCPU::POP16r), DestReg);
    return;
  }
  llvm_unreachable("Imposible reg-to-reg copy");
}

MachineInstr *ZCPUInstrInfo::commuteInstruction(MachineInstr *MI,
  bool NewMI) const
{
  switch (MI->getOpcode())
  {
  default: return TargetInstrInfo::commuteInstruction(MI, NewMI);
  case ZCPU::ADD8r:
  case ZCPU::ADD16r:
  case ZCPU::ADC8r:
  case ZCPU::ADC16r:
  case ZCPU::AND8r:
  case ZCPU::XOR8r:
  case ZCPU::OR8r:
    break;
  case ZCPU::ADD8i:
  case ZCPU::ADC8i:
  case ZCPU::AND8i:
  case ZCPU::XOR8i:
  case ZCPU::OR8i:
  case ZCPU::ADD8xm:
  case ZCPU::ADC8xm:
  case ZCPU::AND8xm:
  case ZCPU::XOR8xm:
  case ZCPU::OR8xm:
    return NULL;
  }
  assert(!NewMI && "Not implemented yet!");

  MachineBasicBlock &MBB = *MI->getParent();
  MachineFunction &MF = *MBB.getParent();
  unsigned reg[2], arg[] = { 0, 0 };

  MachineInstr *MILoadReg = MI->getPrevNode();
  if (MILoadReg == NULL || MILoadReg->getOpcode() != ZCPU::COPY) return NULL;

  MachineOperand &MO0 = MI->getOperand(0);
  MachineOperand &MO1 = MILoadReg->getOperand(1);
  reg[0] = MO0.getReg();
  reg[1] = MO1.getReg();

  DEBUG(dbgs() << "COMMUTING:\n\t" << *MILoadReg << "\t" << *MI);
  DEBUG(dbgs() << "COMMUTING OPERANDS: " << MO0 << ", " << MO1 << "\n");
  unsigned PreferArg = -1;

  for (MachineFunction::iterator MFI = MF.begin(), MFE = MF.end(); MFI != MFE; MFI++)
  {
    MachineBasicBlock::iterator MBBI = MFI->begin();
    while (MBBI != MFI->end())
    {
      if (MBBI->getOpcode() == TargetOpcode::COPY)
      {
        if (MBBI->findRegisterDefOperand(reg[0])) {
          DEBUG(dbgs() << "DEFINE OPERAND " << MO0 << ":\n\t" << *MBBI);
          arg[0] = MBBI->getOperand(1).getReg();
          if (RI.isPhysicalRegister(arg[0])) PreferArg = 0;
        }
        if (MBBI->findRegisterDefOperand(reg[1])) {
          DEBUG(dbgs() << "DEFINE OPERAND " << MO1 << ":\n\t" << *MBBI);
          arg[1] = MBBI->getOperand(1).getReg();
          if (RI.isPhysicalRegister(arg[0])) PreferArg = 1;
        }
        if (arg[0] && arg[1]) break;
      }
      MBBI++;
    }
    if (arg[0] && arg[1]) break;
  }

  if (arg[0] == 0 || arg[1] == 0)
  {
    DEBUG(dbgs() << "COPY TO OPERANDS NOT FOUND\n");
    return NULL;
  }

  if (PreferArg == 0)
  {
    MO0.setReg(reg[1]);
    MO1.setReg(reg[0]);
    DEBUG(dbgs() << "COMMUTING TO:\n\t" << *MILoadReg << "\t" << *MI);
  }
  else DEBUG(dbgs() << "COMMUTING NOT NEEDED\n");
  return NULL;
}

bool ZCPUInstrInfo::AnalyzeBranch(MachineBasicBlock &MBB,
  MachineBasicBlock *&TBB, MachineBasicBlock *&FBB,
  SmallVectorImpl<MachineOperand> &Cond, bool AllowModify = false) const
{
  // Start from the bottom of the block and work up, examining the
  // terminator instructions.
  MachineBasicBlock::iterator I = MBB.end();

  while (I != MBB.begin())
  {
    I--;
    if (I->isDebugValue())
      continue;

    // Working from the bottom, when we see a non-terminator
    // instruction, we're done.
    if (!isUnpredicatedTerminator(I))
      break;

    // A terminator that isn't a branch can't easily be handled
    // by this analisys.
    if (!I->isBranch())
      return true;

    // Handle unconditional branches.
    if (I->getOpcode() == ZCPU::JP)
    {
      if (!AllowModify)
      {
        TBB = I->getOperand(0).getMBB();
        continue;
      }

      // If the block has any instructions after a JP, delete them.
      while (llvm::next(I) != MBB.end())
        llvm::next(I)->eraseFromParent();

      Cond.clear();
      FBB = 0;

      // Delete the JP if it's equivalent to a fall-through.
      if (MBB.isLayoutSuccessor(I->getOperand(0).getMBB()))
      {
        TBB = 0;
        I->eraseFromParent();
        I = MBB.end();
        continue;
      }
      // TBB is used to indicate the unconditional destination.
      TBB = I->getOperand(0).getMBB();
      continue;
    }

    // Handle conditional branches.
    assert(I->getOpcode() == ZCPU::JPCC && "Invalid conditional branch");
    ZCPU::CondCode ZCPUCC = static_cast<ZCPU::CondCode>(I->getOperand(1).getImm());
    if (ZCPUCC == ZCPU::COND_INVALID)
      return true;

    // Working from the bottom, handle the first conditional branch.
    if (Cond.empty())
    {
      FBB = TBB;
      TBB = I->getOperand(0).getMBB();
      Cond.push_back(MachineOperand::CreateImm(ZCPUCC));
      continue;
    }

    // Handle subsequent conditional branches.
    assert(0 && "Not implemented yet!");
  }
  return false;
}

unsigned ZCPUInstrInfo::RemoveBranch(MachineBasicBlock &MBB) const
{
  MachineBasicBlock::iterator I = MBB.end();
  unsigned Count = 0;

  while (I != MBB.begin())
  {
    I--;
    if (I->isDebugValue())
      continue;
    if (I->getOpcode() != ZCPU::JP &&
        I->getOpcode() != ZCPU::JPCC)
        break;
    // Remove branch.
    I->eraseFromParent();
    I = MBB.end();
    Count++;
  }
  return Count;
}

unsigned ZCPUInstrInfo::InsertBranch(MachineBasicBlock &MBB,
  MachineBasicBlock *TBB, MachineBasicBlock *FBB,
  const SmallVectorImpl<MachineOperand> &Cond,
  DebugLoc DL) const
{
  // Shouldn't be a fall through.
  assert(TBB && "InsertBranch must not be told to insert a fallthrough");
  assert((Cond.size() == 0 || Cond.size() == 1) &&
    "ZCPU branch conditions must have one component!");

  if (Cond.empty())
  {
    // Unconditional branch?
    assert(!FBB && "Unconditional branch with multiple successors!");
    BuildMI(&MBB, DL, get(ZCPU::JP)).addMBB(TBB);
    return 1;
  }

  // Conditional branch.
  unsigned Count = 0;
  BuildMI(&MBB, DL, get(ZCPU::JPCC))
    .addMBB(TBB)
    .addImm(Cond[0].getImm());
  Count++;

  if (FBB)
  {
    // Two-way conditional branch. Insert the second branch.
    BuildMI(&MBB, DL, get(ZCPU::JP)).addMBB(FBB);
    Count++;
  }
  return Count;
}

bool ZCPUInstrInfo::ReverseBranchCondition(
  SmallVectorImpl<MachineOperand> &Cond) const
{
  assert(Cond.size() == 1 && "Invalid branch condition!");

  ZCPU::CondCode CC = static_cast<ZCPU::CondCode>(Cond[0].getImm());

  switch (CC)
  {
  default: return true;
  case ZCPU::COND_Z:
    CC = ZCPU::COND_NZ;
    break;
  case ZCPU::COND_NZ:
    CC = ZCPU::COND_Z;
    break;
  case ZCPU::COND_C:
    CC = ZCPU::COND_NC;
    break;
  case ZCPU::COND_NC:
    CC = ZCPU::COND_C;
    break;
  }

  Cond[0].setImm(CC);
  return false;
}

void ZCPUInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator MI, unsigned SrcReg, bool isKill,
  int FrameIndex, const TargetRegisterClass *RC,
  const TargetRegisterInfo *TRI) const
{
  DebugLoc dl;
  if (MI != MBB.end()) dl = MI->getDebugLoc();

  if (RC == &ZCPU::GR8RegClass)
    BuildMI(MBB, MI, dl, get(ZCPU::LD8xmr))
      .addFrameIndex(FrameIndex).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else if (RC == &ZCPU::BR16RegClass ||
           ZCPU::BR16RegClass.contains(SrcReg)) {
    BuildMI(MBB, MI, dl, get(ZCPU::LD16xmr))
      .addFrameIndex(FrameIndex).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  }
  else
    llvm_unreachable("Can't store this register to stack slot");
}

void ZCPUInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
  MachineBasicBlock::iterator MI, unsigned DestReg,
  int FrameIndex, const TargetRegisterClass *RC,
  const TargetRegisterInfo *TRI) const
{
  DebugLoc dl;
  if (MI != MBB.end()) dl = MI->getDebugLoc();

  if (RC == &ZCPU::GR8RegClass)
    BuildMI(MBB, MI, dl, get(ZCPU::LD8rxm), DestReg)
      .addFrameIndex(FrameIndex).addImm(0);
  else if (RC == &ZCPU::BR16RegClass ||
           ZCPU::BR16RegClass.contains(DestReg)) {
    BuildMI(MBB, MI, dl, get(ZCPU::LD16rxm), DestReg)
      .addFrameIndex(FrameIndex).addImm(0);
  }
  else
    llvm_unreachable("Can't load this register from stack slot");
}

bool ZCPUInstrInfo::expandPostRAPseudo(MachineBasicBlock::iterator MI) const
{
  MachineBasicBlock &MBB = *MI->getParent();
  MachineFunction &MF = *MBB.getParent();
  const TargetRegisterInfo &RI = *MF.getTarget().getRegisterInfo();
  DebugLoc dl = MI->getDebugLoc();
  unsigned Opc, Reg, Imm, FPReg, Idx;

  switch (MI->getOpcode())
  {
  default:
    return false;
  case ZCPU::LD16xmr:
    Opc   = ZCPU::LD8xmr;
    Reg   = MI->getOperand(2).getReg();
    FPReg = MI->getOperand(0).getReg();
    Idx   = MI->getOperand(1).getImm();
    break;
    case ZCPU::LD16xmi:
    Opc   = ZCPU::LD8xmi;
    Imm   = MI->getOperand(2).getImm();
    FPReg = MI->getOperand(0).getReg();
    Idx   = MI->getOperand(1).getImm();
    break;
  case ZCPU::LD16rxm:
    Opc   = ZCPU::LD8rxm;
    Reg   = MI->getOperand(0).getReg();
    FPReg = MI->getOperand(1).getReg();
    Idx   = MI->getOperand(2).getImm();
    break;
  }
  unsigned Lo, Hi;

  if (Opc == ZCPU::LD8xmi) {
    Lo = Imm & 0xFF;
    Hi = (Imm>>8) & 0xFF;
  }
  else {
    Lo = RI.getSubReg(Reg, ZCPU::subreg_lo);
    Hi = RI.getSubReg(Reg, ZCPU::subreg_hi);
  }

  switch (Opc)
  {
  case ZCPU::LD8xmr:
    BuildMI(MBB, MI, dl, get(Opc))
      .addReg(FPReg).addImm(Idx)
      .addReg(Lo);
    BuildMI(MBB, MI, dl, get(Opc))
      .addReg(FPReg).addImm(Idx+1)
      .addReg(Hi);
    break;
  case ZCPU::LD8xmi:
    BuildMI(MBB, MI, dl, get(Opc))
      .addReg(FPReg).addImm(Idx)
      .addImm(Lo);
    BuildMI(MBB, MI, dl, get(Opc))
      .addReg(FPReg).addImm(Idx+1)
      .addImm(Hi);
    break;
  case ZCPU::LD8rxm:
    BuildMI(MBB, MI, dl, get(Opc), Lo)
      .addReg(FPReg).addImm(Idx);
    BuildMI(MBB, MI, dl, get(Opc), Hi)
      .addReg(FPReg).addImm(Idx+1);
    break;
  }

  DEBUG(dbgs() << "Pseudo: " << *MI);
  DEBUG(dbgs() << "Replaced by:\n" <<
    "\t" << *MI->getPrevNode()->getPrevNode() <<
    "\t" << *MI->getPrevNode());

  MI->eraseFromParent();
  return true;
}
