//===-- ZCPUTargetFrameLowering.h - Define frame lowering for ZCPU -*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class implements ZCPU-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef ZCPUFRAMELOWERING_H
#define ZCPUFRAMELOWERING_H

#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
  class ZCPUTargetMachine;

  class ZCPUFrameLowering : public TargetFrameLowering {
    const ZCPUTargetMachine &TM;
  public:
    explicit ZCPUFrameLowering(const ZCPUTargetMachine &tm);

    void emitPrologue(MachineFunction &MF) const;
    void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const;

    void processFunctionBeforeCalleeSavedScan(MachineFunction &MF,
      RegScavenger *RS = NULL) const;

    void eliminateCallFramePseudoInstr(MachineFunction &MF,
      MachineBasicBlock &MBB, MachineBasicBlock::iterator I) const;

    bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
      MachineBasicBlock::iterator MI,
      const std::vector<CalleeSavedInfo> &CSI,
      const TargetRegisterInfo *TRI) const;
    bool restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
      MachineBasicBlock::iterator MI,
      const std::vector<CalleeSavedInfo> &CSI,
      const TargetRegisterInfo *TRI) const;

    bool hasFP(const MachineFunction &MF) const;
  }; // end class ZCPUFrameLowering
} // end namespace llvm

#endif
