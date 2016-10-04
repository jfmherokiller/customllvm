//===-- ZCPURegisterInfo.h - ZCPU Register Information Impl -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the ZCPU implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef ZCPUREGISTERINFO_H
#define ZCPUREGISTERINFO_H

#define GET_REGINFO_HEADER
#include "ZCPUGenRegisterInfo.inc"

namespace llvm {
  class TargetInstrInfo;
  class ZCPUTargetMachine;

  class ZCPURegisterInfo : public ZCPUGenRegisterInfo {
    ZCPUTargetMachine &TM;
    const TargetInstrInfo &TII;
  public:
    ZCPURegisterInfo(ZCPUTargetMachine &tm, const TargetInstrInfo &tii);

    // Code Generation virtual methods...
    const uint16_t *getCalleeSavedRegs(const MachineFunction *MF = 0) const;
    const uint32_t *getCallPreservedMask(CallingConv::ID CallConv) const;

    BitVector getReservedRegs(const MachineFunction &MF) const;

    void eliminateFrameIndex(MachineBasicBlock::iterator I,
      int SPAdj, unsigned FIOperandNum, RegScavenger *RS = NULL) const;

    // Debug information queries
    unsigned getFrameRegister(const MachineFunction &MF) const;
  }; // end class ZCPURegisterInfo
} // end namespace llvm

#endif
