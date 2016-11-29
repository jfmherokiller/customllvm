// ZCPUFrameLowering.h - TargetFrameLowering for ZCPU -*- C++ -*-/
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This class implements ZCPU-specific bits of
/// TargetFrameLowering class.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUFRAMELOWERING_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUFRAMELOWERING_H

#include <llvm/CodeGen/MachineFrameInfo.h>
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
    class MachineInstrBuilder;
    class MCCFIInstruction;
    class ZCPUSubtarget;
    class ZCPURegisterInfo;

    class ZCPUFrameLowering final : public TargetFrameLowering {
    public:
        ZCPUFrameLowering(const ZCPUSubtarget &STI, unsigned StackAlignOverride);
        const ZCPUSubtarget &STI;
        const TargetInstrInfo &TII;
        const ZCPURegisterInfo *TRI;
        /// Size of the red zone for the user stack (leaf functions can use this much
        /// space below the stack pointer without writing it back to memory).
        // TODO: (ABI) Revisit and decide how large it should be.
        static const size_t RedZoneSize = 128;

        MachineBasicBlock::iterator eliminateCallFramePseudoInstr(
                MachineFunction &MF, MachineBasicBlock &MBB,
                MachineBasicBlock::iterator I) const override;

        /// These methods insert prolog and epilog code into the function.
        void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

        void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

        bool hasFP(const MachineFunction &MF) const override;

        bool hasReservedCallFrame(const MachineFunction &MF) const override;

    private:
        bool needsSP(const MachineFunction &MF, const MachineFrameInfo &MFI) const;

        bool needsSPWriteback(const MachineFunction &MF,
                              const MachineFrameInfo &MFI) const;

        void determineFrameLayout(MachineFunction &MF) const;
    };

}  // end namespace llvm

#endif
