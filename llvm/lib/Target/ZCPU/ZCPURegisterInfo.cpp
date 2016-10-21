//===-- ZCPURegisterInfo.cpp - ZCPU Register Information -== --------------===//
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

#define DEBUG_TYPE "cpu0-reg-info"

#include "ZCPURegisterInfo.h"
#if CH >= CH3_1

#include "ZCPU.h"
#include "ZCPUSubtarget.h"
#include "ZCPUMachineFunction.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#define GET_REGINFO_TARGET_DESC
#include "ZCPUGenRegisterInfo.inc"

using namespace llvm;

ZCPURegisterInfo::ZCPURegisterInfo(const ZCPUSubtarget &ST)
  : ZCPUGenRegisterInfo(ZCPU::LR), Subtarget(ST) {}

#if CH >= CH12_1 //1
const TargetRegisterClass *
ZCPURegisterInfo::getPointerRegClass(const MachineFunction &MF,
                                     unsigned Kind) const {
  return &ZCPU::CPURegsRegClass;
}
#endif

//===----------------------------------------------------------------------===//
// Callee Saved Registers methods
//===----------------------------------------------------------------------===//
/// ZCPU Callee Saved Registers
// In ZCPUCallConv.td,
// def CSR_O32 : CalleeSavedRegs<(add LR, FP,
//                                   (sequence "S%u", 2, 0))>;
// llc create CSR_O32_SaveList and CSR_O32_RegMask from above defined.
const MCPhysReg *
ZCPURegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_O32_SaveList;
}

const uint32_t *
ZCPURegisterInfo::getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const {
  return CSR_O32_RegMask; 
}

// pure virtual method
//@getReservedRegs {
BitVector ZCPURegisterInfo::
getReservedRegs(const MachineFunction &MF) const {
//@getReservedRegs body {
  static const uint16_t ReservedCPURegs[] = {
    ZCPU::ZERO, ZCPU::AT, ZCPU::SP, ZCPU::LR, ZCPU::PC
  };
  BitVector Reserved(getNumRegs());

  for (unsigned I = 0; I < array_lengthof(ReservedCPURegs); ++I)
    Reserved.set(ReservedCPURegs[I]);

#if CH >= CH9_3 //2
  // Reserve FP if this function should have a dedicated frame pointer register.
  if (MF.getSubtarget().getFrameLowering()->hasFP(MF)) {
    Reserved.set(ZCPU::FP);
  }
#endif

#if CH >= CH6_1
#ifdef ENABLE_GPRESTORE //1
  const ZCPUFunctionInfo *ZCPUFI = MF.getInfo<ZCPUFunctionInfo>();
  // Reserve GP if globalBaseRegFixed()
  if (ZCPUFI->globalBaseRegFixed())
#endif
    Reserved.set(ZCPU::GP);
#endif //#if CH >= CH6_1

  return Reserved;
}

//@eliminateFrameIndex {
//- If no eliminateFrameIndex(), it will hang on run. 
// pure virtual method
// FrameIndex represent objects inside a abstract stack.
// We must replace FrameIndex with an stack/frame pointer
// direct reference.
void ZCPURegisterInfo::
eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                    unsigned FIOperandNum, RegScavenger *RS) const {
#if CH >= CH3_5
  MachineInstr &MI = *II;
  MachineFunction &MF = *MI.getParent()->getParent();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  ZCPUFunctionInfo *ZCPUFI = MF.getInfo<ZCPUFunctionInfo>();

  unsigned i = 0;
  while (!MI.getOperand(i).isFI()) {
    ++i;
    assert(i < MI.getNumOperands() &&
           "Instr doesn't have FrameIndex operand!");
  }

  DEBUG(errs() << "\nFunction : " << MF.getFunction()->getName() << "\n";
        errs() << "<--------->\n" << MI);

  int FrameIndex = MI.getOperand(i).getIndex();
  uint64_t stackSize = MF.getFrameInfo()->getStackSize();
  int64_t spOffset = MF.getFrameInfo()->getObjectOffset(FrameIndex);

  DEBUG(errs() << "FrameIndex : " << FrameIndex << "\n"
               << "spOffset   : " << spOffset << "\n"
               << "stackSize  : " << stackSize << "\n");

  const std::vector<CalleeSavedInfo> &CSI = MFI->getCalleeSavedInfo();
  int MinCSFI = 0;
  int MaxCSFI = -1;

  if (CSI.size()) {
    MinCSFI = CSI[0].getFrameIdx();
    MaxCSFI = CSI[CSI.size() - 1].getFrameIdx();
  }

  // The following stack frame objects are always referenced relative to $sp:
  //  1. Outgoing arguments.
  //  2. Pointer to dynamically allocated stack space.
  //  3. Locations for callee-saved registers.
  // Everything else is referenced relative to whatever register
  // getFrameRegister() returns.
  unsigned FrameReg;

#if CH >= CH9_3 //3
  if (ZCPUFI->isOutArgFI(FrameIndex) || ZCPUFI->isDynAllocFI(FrameIndex) ||
      (FrameIndex >= MinCSFI && FrameIndex <= MaxCSFI))
    FrameReg = ZCPU::SP;
  else
    FrameReg = getFrameRegister(MF);
#else
  FrameReg = ZCPU::SP;
#endif //#if CH >= CH9_3 //3

  // Calculate final offset.
  // - There is no need to change the offset if the frame object is one of the
  //   following: an outgoing argument, pointer to a dynamically allocated
  //   stack space or a $gp restore location,
  // - If the frame object is any of the following, its offset must be adjusted
  //   by adding the size of the stack:
  //   incoming argument, callee-saved register location or local variable.
  int64_t Offset;
#if CH >= CH9_3 //1
#ifdef ENABLE_GPRESTORE //2
  if (ZCPUFI->isOutArgFI(FrameIndex) || ZCPUFI->isGPFI(FrameIndex) ||
      ZCPUFI->isDynAllocFI(FrameIndex))
    Offset = spOffset;
  else
#endif
#endif //#if CH >= CH9_3 //1
    Offset = spOffset + (int64_t)stackSize;

  Offset    += MI.getOperand(i+1).getImm();

  DEBUG(errs() << "Offset     : " << Offset << "\n" << "<--------->\n");

  // If MI is not a debug value, make sure Offset fits in the 16-bit immediate
  // field.
  if (!MI.isDebugValue() && !isInt<16>(Offset)) {
	assert("(!MI.isDebugValue() && !isInt<16>(Offset))");
  }

  MI.getOperand(i).ChangeToRegister(FrameReg, false);
  MI.getOperand(i+1).ChangeToImmediate(Offset);
#endif // #if CH >= CH3_5
}
//}

bool
ZCPURegisterInfo::requiresRegisterScavenging(const MachineFunction &MF) const {
  return true;
}

bool
ZCPURegisterInfo::trackLivenessAfterRegAlloc(const MachineFunction &MF) const {
  return true;
}

// pure virtual method
unsigned ZCPURegisterInfo::
getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();
  return TFI->hasFP(MF) ? (ZCPU::FP) :
                          (ZCPU::SP);
}

#endif // #if CH >= CH3_1
