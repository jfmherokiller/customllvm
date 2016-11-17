//===-- ZCPUFrameLowering.cpp - ZCPU Frame Lowering ----------==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the ZCPU implementation of
/// TargetFrameLowering class.
///
/// On ZCPU, there aren't a lot of things to do here. There are no
/// callee-saved registers to save, and no spill slots.
///
/// The stack grows downward.
///
//===----------------------------------------------------------------------===//

#include "ZCPUFrameLowering.h"
#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "ZCPUInstrInfo.h"
#include "ZCPUMachineFunctionInfo.h"
#include "ZCPUSubtarget.h"
#include "ZCPUTargetMachine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "zcpu-frame-info"

// TODO: wasm64
// TODO: Emit TargetOpcode::CFI_INSTRUCTION instructions

/// Return true if the specified function should have a dedicated frame pointer
/// register.
bool ZCPUFrameLowering::hasFP(const MachineFunction &MF) const {
    const MachineFrameInfo *MFI = MF.getFrameInfo();
    const auto *RegInfo =
            MF.getSubtarget<ZCPUSubtarget>().getRegisterInfo();
    return MFI->isFrameAddressTaken() || MFI->hasVarSizedObjects() ||
           MFI->hasStackMap() || MFI->hasPatchPoint() ||
           RegInfo->needsStackRealignment(MF);
}

/// Under normal circumstances, when a frame pointer is not required, we reserve
/// argument space for call sites in the function immediately on entry to the
/// current function. This eliminates the need for add/sub sp brackets around
/// call sites. Returns true if the call frame is included as part of the stack
/// frame.
bool ZCPUFrameLowering::hasReservedCallFrame(
        const MachineFunction &MF) const {
    return !MF.getFrameInfo()->hasVarSizedObjects();
}


/// Returns true if this function needs a local user-space stack pointer.
/// Unlike a machine stack pointer, the wasm user stack pointer is a global
/// variable, so it is loaded into a register in the prolog.
bool ZCPUFrameLowering::needsSP(const MachineFunction &MF,
                                const MachineFrameInfo &MFI) const {
    return MFI.getStackSize() || MFI.adjustsStack() || hasFP(MF);
}

/// Returns true if the local user-space stack pointer needs to be written back
/// to memory by this function (this is not meaningful if needsSP is false). If
/// false, the stack red zone can be used and only a local SP is needed.
bool ZCPUFrameLowering::needsSPWriteback(
        const MachineFunction &MF, const MachineFrameInfo &MFI) const {
    assert(needsSP(MF, MFI));
    return MFI.getStackSize() > RedZoneSize || MFI.hasCalls() ||
           MF.getFunction()->hasFnAttribute(Attribute::NoRedZone);
}

MachineBasicBlock::iterator
ZCPUFrameLowering::eliminateCallFramePseudoInstr(
        MachineFunction &MF, MachineBasicBlock &MBB,
        MachineBasicBlock::iterator I) const {
    return MBB.erase(I);
}

void ZCPUFrameLowering::emitPrologue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {

}

void ZCPUFrameLowering::emitEpilogue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {

}
