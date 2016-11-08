//=- ZCPUInstrInfo.h - ZCPU Instruction Information -*- C++ -*-=//
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
/// TargetInstrInfo class.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUINSTRINFO_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUINSTRINFO_H

#include "ZCPURegisterInfo.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER

#include "ZCPUGenInstrInfo.inc"

namespace llvm {

    class ZCPUSubtarget;

    class ZCPUInstrInfo final : public ZCPUGenInstrInfo {
        const ZCPURegisterInfo RI;

    public:
        explicit ZCPUInstrInfo(const ZCPUSubtarget &STI);

        const ZCPURegisterInfo &getRegisterInfo() const { return RI; }

        bool isReallyTriviallyReMaterializable(const MachineInstr &MI,
                                               AliasAnalysis *AA) const override;

        void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
                         const DebugLoc &DL, unsigned DestReg, unsigned SrcReg,
                         bool KillSrc) const override;

        MachineInstr *commuteInstructionImpl(MachineInstr &MI, bool NewMI,
                                             unsigned OpIdx1,
                                             unsigned OpIdx2) const override;

        bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                           MachineBasicBlock *&FBB,
                           SmallVectorImpl<MachineOperand> &Cond,
                           bool AllowModify = false) const override;

        unsigned RemoveBranch(MachineBasicBlock &MBB) const override;

        unsigned InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                              MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                              const DebugLoc &DL) const override;

        bool
        ReverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const override;
    };

} // end namespace llvm

#endif
