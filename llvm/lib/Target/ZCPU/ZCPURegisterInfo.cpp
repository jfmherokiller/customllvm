//===-- ZCPURegisterInfo.cpp - ZCPU Register Information ----===//
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
/// TargetRegisterInfo class.
///
//===----------------------------------------------------------------------===//

#include "ZCPURegisterInfo.h"
#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "ZCPUFrameLowering.h"
#include "ZCPUInstrInfo.h"
#include "ZCPUMachineFunctionInfo.h"
#include "ZCPUSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define DEBUG_TYPE "zcpu-reg-info"

#define GET_REGINFO_TARGET_DESC

#include "ZCPUGenRegisterInfo.inc"

ZCPURegisterInfo::ZCPURegisterInfo(const Triple &TT)
        : ZCPUGenRegisterInfo(0), TT(TT) {}

BitVector ZCPURegisterInfo::getReservedRegs(const MachineFunction & /*MF*/) const {

    BitVector Reserved(getNumRegs());
    static const uint16_t ReservedCPURegs[] = {
            ZCPU::SerialNo,ZCPU::XEIP,ZCPU::IP,ZCPU::ESP,ZCPU::EBP,
    };
    for (unsigned I = 0; I < array_lengthof(ReservedCPURegs); ++I)
        Reserved.set(ReservedCPURegs[I]);
    return Reserved;
}

void ZCPURegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, unsigned FIOperandNum, RegScavenger * RS) const {
    assert(SPAdj == 0);
    MachineInstr &MI = *II;

    MachineBasicBlock &MBB = *MI.getParent();
    MachineFunction &MF = *MBB.getParent();
    MachineRegisterInfo &MRI = MF.getRegInfo();
    int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
    const MachineFrameInfo &MFI = *MF.getFrameInfo();
    DebugLoc DL = MI.getDebugLoc();
    const TargetInstrInfo *TII = MF.getSubtarget().getInstrInfo();

    if (MI.getOpcode() == ZCPU::SSTACKTrun) {
        MI.getOperand(FIOperandNum).ChangeToImmediate(FrameIndex);
    }
}

unsigned ZCPURegisterInfo::getFrameRegister(const MachineFunction &MF) const {
    return ZCPU::EBP;
}

const TargetRegisterClass *
ZCPURegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                     unsigned Kind) const {
    return &ZCPU::SegmentRegsRegClass;
}

bool ZCPURegisterInfo::hasBasePointer(const MachineFunction &MF) const {
    const MachineFrameInfo *MFI = MF.getFrameInfo();
    // When we need stack realignment and there are dynamic allocas, we can't
    // reference off of the stack pointer, so we reserve a base pointer.
    if (needsStackRealignment(MF) && MFI->hasVarSizedObjects())
        return true;
    return false;
}
bool ZCPURegisterInfo::canRealignStack(const MachineFunction &MF) const {
    if (!TargetRegisterInfo::canRealignStack(MF))
        return false;
    return true;
}

const uint32_t *
ZCPURegisterInfo::getCallPreservedMask(const MachineFunction & /*MF*/,
                                        CallingConv::ID /*CC*/) const {
    return CSR_Saved_RegMask;
}
const MCPhysReg * ZCPURegisterInfo::getCalleeSavedRegs(const MachineFunction * /*MF*/) const {
    return CSR_Saved_SaveList;
}