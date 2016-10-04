//===-- ZCPURegisterInfo.cpp - ZCPU Register Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the ZCPU implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "ZCPURegisterInfo.h"
#include "ZCPU.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Support/CommandLine.h"

#define GET_REGINFO_TARGET_DESC
#include "ZCPUGenRegisterInfo.inc"

using namespace llvm;

typedef enum {
  IX,
  IY
} FrameUseRegister;

static cl::opt<FrameUseRegister>
FrameRegister("z80-frame-register",
  cl::desc("Frame register (IX by default)"),
  cl::init(IX),
  cl::values(
    clEnumValN(IX, "ix", "IX register"),
    clEnumValN(IY, "iy", "IY register"),
    clEnumValEnd));

ZCPURegisterInfo::ZCPURegisterInfo(ZCPUTargetMachine &tm, const TargetInstrInfo &tii)
  : ZCPUGenRegisterInfo(ZCPU::PC), TM(tm), TII(tii)
{}

const uint16_t* ZCPURegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const
{
  static const uint16_t CalleeSavedRegs[] = {
    ZCPU::BC, ZCPU::DE, ZCPU::IX, ZCPU::IY,
    0
  };
  return CalleeSavedRegs;
}

const uint32_t* ZCPURegisterInfo::getCallPreservedMask(CallingConv::ID CallConv) const
{
  return CSR_16_RegMask;
}

BitVector ZCPURegisterInfo::getReservedRegs(const MachineFunction &MF) const
{
  BitVector Reserved(getNumRegs());

  Reserved.set(ZCPU::PC);
  Reserved.set(ZCPU::SP);
  Reserved.set(ZCPU::FLAGS);
  Reserved.set(getFrameRegister(MF));

  return Reserved;
}

void ZCPURegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator I,
  int SPAdj, unsigned FIOperandNum, RegScavenger *RS) const
{
  assert(SPAdj == 0 && "Not implemented yet!");

  unsigned i = 0;
  MachineInstr &MI = *I;
  MachineBasicBlock &MBB = *MI.getParent();
  MachineFunction &MF = *MBB.getParent();
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();

  while (!MI.getOperand(i).isFI())
  {
    i++;
    assert(i < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");
  }

  int FrameIndex = MI.getOperand(i).getIndex();
  uint64_t Offset = MF.getFrameInfo()->getObjectOffset(FrameIndex);
  Offset += 2; // Skip the saved PC
  Offset += MF.getFrameInfo()->getStackSize();
  Offset += MI.getOperand(i+1).getImm();

  if (TFI->hasFP(MF))
  {
    MI.getOperand(i).ChangeToRegister(getFrameRegister(MF), false);
    MI.getOperand(i+1).ChangeToImmediate(Offset);
  }
  else assert(0 && "Not implemented yet !");
}

unsigned ZCPURegisterInfo::getFrameRegister(const MachineFunction &MF) const
{
  return (FrameRegister == IX) ? ZCPU::IX : ZCPU::IY;
}
