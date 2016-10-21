//===-- ZCPURegisterInfo.h - ZCPU Register Information Impl -----*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUREGISTERINFO_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUREGISTERINFO_H

#include "ZCPUConfig.h"
#if CH >= CH3_1

#include "ZCPU.h"
#include "llvm/Target/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "ZCPUGenRegisterInfo.inc"

namespace llvm {
class ZCPUSubtarget;
class TargetInstrInfo;
class Type;

class ZCPURegisterInfo : public ZCPUGenRegisterInfo {
protected:
  const ZCPUSubtarget &Subtarget;

public:
  ZCPURegisterInfo(const ZCPUSubtarget &Subtarget);

#if CH >= CH12_1 //1
  /// Code Generation virtual methods...
  const TargetRegisterClass *getPointerRegClass(const MachineFunction &MF,
                                                unsigned Kind) const override;
#endif

  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  bool requiresRegisterScavenging(const MachineFunction &MF) const override;

  bool trackLivenessAfterRegAlloc(const MachineFunction &MF) const override;

  /// Stack Frame Processing Methods
  void eliminateFrameIndex(MachineBasicBlock::iterator II,
                           int SPAdj, unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  /// Debug information queries.
  unsigned getFrameRegister(const MachineFunction &MF) const override;

  /// \brief Return GPR register class.
  virtual const TargetRegisterClass *intRegClass(unsigned Size) const = 0;
};

} // end namespace llvm

#endif // #if CH >= CH3_1

#endif
