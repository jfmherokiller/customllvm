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
ZCPUFrameLowering::ZCPUFrameLowering(const ZCPUSubtarget &STI,
                                   unsigned StackAlignOverride)
        : TargetFrameLowering(StackGrowsDown, StackAlignOverride, -8),
          STI(STI), TII(*STI.getInstrInfo()), TRI(STI.getRegisterInfo()) {
    // Cache a bunch of frame-related predicates for this subtarget.
    //SlotSize = TRI->getSlotSize();
    // standard x86_64 and NaCl use 64-bit frame/stack pointers, x32 - 32-bit.
    //Uses64BitFramePtr = STI.isTarget64BitLP64() || STI.isTargetNaCl64();
    //StackPtr = TRI->getStackRegister();
}
// Determines the size of the frame and maximum call frame size.
void ZCPUFrameLowering::determineFrameLayout(MachineFunction &MF) const {
    MachineFrameInfo *MFI = MF.getFrameInfo();
    const ZCPURegisterInfo *LRI = STI.getRegisterInfo();

    // Get the number of bytes to allocate from the FrameInfo.
    unsigned FrameSize = MFI->getStackSize();

    // Get the alignment.
    unsigned StackAlign = LRI->needsStackRealignment(MF) ? MFI->getMaxAlignment()
                                                         : getStackAlignment();

    // Get the maximum call frame size of all the calls.
    unsigned MaxCallFrameSize = MFI->getMaxCallFrameSize();

    // If we have dynamic alloca then MaxCallFrameSize needs to be aligned so
    // that allocations will be aligned.
    if (MFI->hasVarSizedObjects())
        MaxCallFrameSize = alignTo(MaxCallFrameSize, StackAlign);

    // Update maximum call frame size.
    MFI->setMaxCallFrameSize(MaxCallFrameSize);

    // Include call frame size in total.
    if (!(hasReservedCallFrame(MF) && MFI->adjustsStack()))
        FrameSize += MaxCallFrameSize;

    // Make sure the frame is aligned.
    FrameSize = alignTo(FrameSize, StackAlign);

    // Update frame info.
    MFI->setStackSize(FrameSize);
}
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
    assert(&MF.front() == &MBB && "Shrink-wrapping not yet supported");

    auto *MFI = MF.getFrameInfo();
    if (!needsSP(MF, *MFI) || !needsSPWriteback(MF, *MFI)) return;

    MachineBasicBlock::iterator MBBI = MBB.begin();
    // Determine the correct frame layout
    determineFrameLayout(MF);
    // Debug location must be unknown since the first debug location is used
    // to determine the end of the prologue.
    DebugLoc DL;

    unsigned StackSize = MFI->getStackSize();

    DEBUG(errs() << "ENTER\n");
    BuildMI(MBB, MBBI, DL, TII.get(ZCPU::ENTER))
            .addImm(StackSize)
            .setMIFlag(MachineInstr::FrameSetup);
    // Replace ADJDYNANALLOC
    if (MFI->hasVarSizedObjects())
        replaceAdjDynAllocPseudo(MF);
    //llvm_unreachable("failed in enter");

//return TargetFrameLowering::emitPrologue(MF,MBB);
}

void ZCPUFrameLowering::emitEpilogue(MachineFunction &MF,
                                     MachineBasicBlock &MBB) const {
    auto *MFI = MF.getFrameInfo();
    uint64_t StackSize = MFI->getStackSize();
    if (!needsSP(MF, *MFI) || !needsSPWriteback(MF, *MFI)) return;
    const auto *TII = MF.getSubtarget<ZCPUSubtarget>().getInstrInfo();
    auto &MRI = MF.getRegInfo();
    auto InsertPt = MBB.getFirstTerminator();
    DebugLoc DL;
    if (InsertPt != MBB.end())
        DL = InsertPt->getDebugLoc();

    DEBUG(errs() << "LEAVE\n");
    BuildMI(MBB, InsertPt, DL, TII->get(ZCPU::LEAVE))
            .setMIFlag(MachineInstr::FrameSetup);
    //llvm_unreachable("failed in leave");
    //return TargetFrameLowering::emitEpilogue(MF,MBB);
}
// Iterates through each basic block in a machine function and replaces
// ADJDYNALLOC pseudo instructions with a ZCPU:ADDI with the
// maximum call frame size as the immediate.
void ZCPUFrameLowering::replaceAdjDynAllocPseudo(MachineFunction &MF) const {
    const ZCPUInstrInfo &LII = *STI.getInstrInfo();
    unsigned MaxCallFrameSize = MF.getFrameInfo()->getMaxCallFrameSize();

    for (MachineFunction::iterator MBB = MF.begin(), E = MF.end(); MBB != E;
         ++MBB) {
        MachineBasicBlock::iterator MBBI = MBB->begin();
        while (MBBI != MBB->end()) {
            MachineInstr &MI = *MBBI++;
            if (MI.getOpcode() == ZCPU::ADJDYNALLOC) {
                DebugLoc DL = MI.getDebugLoc();
                unsigned Dst = MI.getOperand(0).getReg();
                unsigned Src = MI.getOperand(1).getReg();

                BuildMI(*MBB, MI, DL, LII.get(ZCPU::ADDregimmint), Dst).addReg(Src).addImm(MaxCallFrameSize);
                MI.eraseFromParent();
            }
        }
    }
}
void ZCPUFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                              BitVector &SavedRegs,
                                              RegScavenger *RS) const {
    TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);

    MachineFrameInfo *MFI = MF.getFrameInfo();
    const ZCPURegisterInfo *LRI = STI.getRegisterInfo();
    int Offset = -4;

    // Reserve 4 bytes for the saved RCA
    MFI->CreateFixedObject(4, Offset, true);
    Offset -= 4;

    // Reserve 4 bytes for the saved FP
    MFI->CreateFixedObject(4, Offset, true);
    Offset -= 4;

    if (LRI->hasBasePointer(MF)) {
        MFI->CreateFixedObject(4, Offset, true);
        SavedRegs.reset(ZCPU::EBP);
    }
}