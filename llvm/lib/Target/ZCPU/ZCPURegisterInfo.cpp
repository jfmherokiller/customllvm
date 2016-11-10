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

const MCPhysReg *
ZCPURegisterInfo::getCalleeSavedRegs(const MachineFunction *) const {
    static const MCPhysReg CalleeSavedRegs[] = {0};
    return CalleeSavedRegs;
}

BitVector
ZCPURegisterInfo::getReservedRegs(const MachineFunction & /*MF*/) const {

    BitVector Reserved(getNumRegs());
    static const uint16_t ReservedCPURegs[] = {
            ZCPU::IP,ZCPU::SerialNo,ZCPU::XEIP,
    };
    for (unsigned I = 0; I < array_lengthof(ReservedCPURegs); ++I)
        Reserved.set(ReservedCPURegs[I]);
    return Reserved;
}

void ZCPURegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj, unsigned FIOperandNum,
                                           RegScavenger * /*RS*/) const {

}

unsigned ZCPURegisterInfo::getFrameRegister(const MachineFunction &MF) const {
    return ZCPU::ESP;
}

const TargetRegisterClass *
ZCPURegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                     unsigned Kind) const {
    return &ZCPU::SegmentRegsRegClass;
}
