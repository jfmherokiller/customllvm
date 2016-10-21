//===-- ZCPUInstrInfo.h - ZCPU Instruction Information ----------*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUINSTRINFO_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUINSTRINFO_H

#include "ZCPUConfig.h"
#if CH >= CH3_1

#include "ZCPU.h"
#if CH >= CH3_5 //1
#include "ZCPUAnalyzeImmediate.h"
#endif
#include "ZCPURegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "ZCPUGenInstrInfo.inc"

namespace llvm {

class ZCPUInstrInfo : public ZCPUGenInstrInfo {
  virtual void anchor();
protected:
  const ZCPUSubtarget &Subtarget;
public:
  explicit ZCPUInstrInfo(const ZCPUSubtarget &STI);

  static const ZCPUInstrInfo *create(ZCPUSubtarget &STI);

  /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  /// such, whenever a client has an instance of instruction info, it should
  /// always be able to get register info as well (through this method).
  ///
  virtual const ZCPURegisterInfo &getRegisterInfo() const = 0;

  /// Return the number of bytes of code the specified instruction may be.
  unsigned GetInstSizeInBytes(const MachineInstr *MI) const;

#if CH >= CH8_2 //1
  virtual unsigned getOppositeBranchOpc(unsigned Opc) const = 0;
#endif
  
#if CH >= CH9_1 //2
  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MBBI,
                           unsigned SrcReg, bool isKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI) const override {
    storeRegToStack(MBB, MBBI, SrcReg, isKill, FrameIndex, RC, TRI, 0);
  }

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI,
                            unsigned DestReg, int FrameIndex,
                            const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI) const override {
    loadRegFromStack(MBB, MBBI, DestReg, FrameIndex, RC, TRI, 0);
  }

  virtual void storeRegToStack(MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator MI,
                               unsigned SrcReg, bool isKill, int FrameIndex,
                               const TargetRegisterClass *RC,
                               const TargetRegisterInfo *TRI,
                               int64_t Offset) const = 0;

  virtual void loadRegFromStack(MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI,
                                unsigned DestReg, int FrameIndex,
                                const TargetRegisterClass *RC,
                                const TargetRegisterInfo *TRI,
                                int64_t Offset) const = 0;
#endif

#if CH >= CH3_5 //3
  virtual void adjustStackPtr(unsigned SP, int64_t Amount,
                              MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator I) const = 0;
#endif

#if CH >= CH3_3
  virtual MachineInstr* emitFrameIndexDebugValue(MachineFunction &MF,
                                                 int FrameIx, uint64_t Offset,
                                                 const MDNode *MDPtr,
                                                 DebugLoc DL) const;
#endif

protected:
#if CH >= CH9_1 //4
  MachineMemOperand *GetMemOperand(MachineBasicBlock &MBB, int FI,
                                   unsigned Flag) const;
#endif
};
const ZCPUInstrInfo *createZCPUSEInstrInfo(const ZCPUSubtarget &STI);
}

#endif // #if CH >= CH3_1

#endif
