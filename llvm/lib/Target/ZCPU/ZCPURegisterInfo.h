// ZCPURegisterInfo.h - ZCPU Register Information Impl -*- C++ -*-
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
/// ZCPURegisterInfo class.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUREGISTERINFO_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUREGISTERINFO_H

#define GET_REGINFO_HEADER

#include "ZCPUGenRegisterInfo.inc"

namespace llvm {

    class MachineFunction;

    class RegScavenger;

    class TargetRegisterClass;

    class Triple;

    class ZCPURegisterInfo final : public ZCPUGenRegisterInfo {
        const Triple &TT;

    public:
        explicit ZCPURegisterInfo(const Triple &TT);

        // Code Generation virtual methods.
        const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

        BitVector getReservedRegs(const MachineFunction &MF) const override;

        void eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj, unsigned FIOperandNum, RegScavenger *RS = nullptr) const override;

        // Debug information queries.
        unsigned getFrameRegister(const MachineFunction &MF) const override;

        const TargetRegisterClass *getPointerRegClass(const MachineFunction &MF, unsigned Kind = 0) const override;

        bool hasBasePointer(const MachineFunction &MF) const;

        bool canRealignStack(const MachineFunction &MF) const override;

        const uint32_t *getCallPreservedMask(const MachineFunction &, CallingConv::ID) const override;
    };

} // end namespace llvm

#endif
