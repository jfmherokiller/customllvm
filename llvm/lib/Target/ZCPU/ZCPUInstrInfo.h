//===-- ZCPUInstrInfo.h - ZCPU Instruction Information ------------*- C++ -*-===//
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

#ifndef ZCPUINSTRINFO_H
#define ZCPUINSTRINFO_H

#include "ZCPU.h"
#include "ZCPURegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "ZCPUGenInstrInfo.inc"

namespace llvm {
  namespace ZCPU {
    enum CondCode {
      COND_NZ = 0,
      COND_Z  = 1,
      COND_NC = 2,
      COND_C  = 3,
      COND_PO = 4,
      COND_PE = 5,
      COND_P  = 6,
      COND_M  = 7,

      COND_INVALID
    };
  } // end namespace ZCPU
  class ZCPUInstrInfo : public ZCPUGenInstrInfo {
    const ZCPURegisterInfo RI;
    ZCPUTargetMachine &TM;
  public:
    explicit ZCPUInstrInfo(ZCPUTargetMachine &tm);

    virtual const ZCPURegisterInfo &getRegisterInfo() const { return RI; }

    virtual void copyPhysReg(MachineBasicBlock &MBB,
      MachineBasicBlock::iterator I, DebugLoc DL,
      unsigned DestReg, unsigned SrcReg, bool KillSrc) const;

    virtual void storeRegToStackSlot(MachineBasicBlock &MBB,
      MachineBasicBlock::iterator MI, unsigned SrcReg, bool isKill,
      int FrameIndex, const TargetRegisterClass *RC,
      const TargetRegisterInfo *TRI) const;
    virtual void loadRegFromStackSlot(MachineBasicBlock &MBB,
      MachineBasicBlock::iterator MI, unsigned DestReg,
      int FrameIndex, const TargetRegisterClass *RC,
      const TargetRegisterInfo *TRI) const;

    virtual bool expandPostRAPseudo(MachineBasicBlock::iterator MI) const;

    virtual MachineInstr* commuteInstruction(MachineInstr *MI,
      bool NewMI = false) const;

    virtual bool AnalyzeBranch(MachineBasicBlock &MBB,
      MachineBasicBlock *&TBB, MachineBasicBlock *&FBB,
      SmallVectorImpl<MachineOperand> &Cond,
      bool AllowModify) const;
    virtual unsigned RemoveBranch(MachineBasicBlock &MBB) const;
    virtual unsigned InsertBranch(MachineBasicBlock &MBB,
      MachineBasicBlock *TBB, MachineBasicBlock *FBB,
      const SmallVectorImpl<MachineOperand> &Cond,
      DebugLoc DL) const;
    virtual bool ReverseBranchCondition(
      SmallVectorImpl<MachineOperand> &Cond) const;
  }; // end class ZCPUInstrInfo
} // end namespace llvm

#endif
