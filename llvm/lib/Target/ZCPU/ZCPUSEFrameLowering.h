//===-- ZCPUSEFrameLowering.h - ZCPU32/64 frame lowering --------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUSEFRAMELOWERING_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUSEFRAMELOWERING_H

#include "ZCPUConfig.h"
#if CH >= CH3_1

#include "ZCPUFrameLowering.h"

namespace llvm {

class ZCPUSEFrameLowering : public ZCPUFrameLowering {
public:
  explicit ZCPUSEFrameLowering(const ZCPUSubtarget &STI);

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

#if CH >= CH9_1
  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 const std::vector<CalleeSavedInfo> &CSI,
                                 const TargetRegisterInfo *TRI) const override;
#endif

#if CH >= CH3_5
  bool hasReservedCallFrame(const MachineFunction &MF) const override;

  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS) const override;
#endif
};

} // End llvm namespace

#endif // #if CH >= CH3_1

#endif
