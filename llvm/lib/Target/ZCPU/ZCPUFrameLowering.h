//===-- ZCPUFrameLowering.h - Define frame lowering for ZCPU ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//
#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUFRAMELOWERING_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUFRAMELOWERING_H

#include "ZCPUConfig.h"
#if CH >= CH3_1

#include "ZCPU.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
  class ZCPUSubtarget;

class ZCPUFrameLowering : public TargetFrameLowering {
protected:
  const ZCPUSubtarget &STI;

public:
  explicit ZCPUFrameLowering(const ZCPUSubtarget &sti, unsigned Alignment)
    : TargetFrameLowering(StackGrowsDown, Alignment, 0, Alignment),
      STI(sti) {
  }

  static const ZCPUFrameLowering *create(const ZCPUSubtarget &ST);

  bool hasFP(const MachineFunction &MF) const override;

#if CH >= CH9_2
  void eliminateCallFramePseudoInstr(MachineFunction &MF,
                                  MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator I) const override;
#endif
};

/// Create ZCPUFrameLowering objects.
const ZCPUFrameLowering *createZCPUSEFrameLowering(const ZCPUSubtarget &ST);

} // End llvm namespace

#endif // #if CH >= CH3_1

#endif

