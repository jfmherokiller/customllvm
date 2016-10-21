//===-- ZCPUSEFrameLowering.cpp - ZCPU Frame Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the ZCPU implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "ZCPUSEFrameLowering.h"
#if CH >= CH3_1

#if CH >= CH3_5
#include "ZCPUAnalyzeImmediate.h"
#endif
#include "ZCPUMachineFunction.h"
#include "ZCPUSEInstrInfo.h"
#include "ZCPUSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

ZCPUSEFrameLowering::ZCPUSEFrameLowering(const ZCPUSubtarget &STI)
    : ZCPUFrameLowering(STI, STI.stackAlignment()) {}

//@emitPrologue {
void ZCPUSEFrameLowering::emitPrologue(MachineFunction &MF,
                                       MachineBasicBlock &MBB) const {
#if CH >= CH3_5 //1
  assert(&MF.front() == &MBB && "Shrink-wrapping not yet supported");
  MachineFrameInfo *MFI    = MF.getFrameInfo();
  ZCPUFunctionInfo *ZCPUFI = MF.getInfo<ZCPUFunctionInfo>();

  const ZCPUSEInstrInfo &TII =
    *static_cast<const ZCPUSEInstrInfo*>(STI.getInstrInfo());
  const ZCPURegisterInfo &RegInfo =
      *static_cast<const ZCPURegisterInfo *>(STI.getRegisterInfo());

  MachineBasicBlock::iterator MBBI = MBB.begin();
  DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  ZCPUABIInfo ABI = STI.getABI();
  unsigned SP = ZCPU::SP;
#if CH >= CH9_3 //1
  unsigned FP = ZCPU::FP;
  unsigned ZERO = ZCPU::ZERO;
  unsigned ADDu = ZCPU::ADDu;
#endif
  const TargetRegisterClass *RC = &ZCPU::GPROutRegClass;

  // First, compute final stack size.
  uint64_t StackSize = MFI->getStackSize();

  // No need to allocate space on the stack.
  if (StackSize == 0 && !MFI->adjustsStack()) return;

  MachineModuleInfo &MMI = MF.getMMI();
  const MCRegisterInfo *MRI = MMI.getContext().getRegisterInfo();
  MachineLocation DstML, SrcML;

  // Adjust stack.
  TII.adjustStackPtr(SP, -StackSize, MBB, MBBI);

  // emit ".cfi_def_cfa_offset StackSize"
  unsigned CFIIndex = MMI.addFrameInst(
      MCCFIInstruction::createDefCfaOffset(nullptr, -StackSize));
  BuildMI(MBB, MBBI, dl, TII.get(TargetOpcode::CFI_INSTRUCTION))
      .addCFIIndex(CFIIndex);

  const std::vector<CalleeSavedInfo> &CSI = MFI->getCalleeSavedInfo();

  if (CSI.size()) {
    // Find the instruction past the last instruction that saves a callee-saved
    // register to the stack.
    for (unsigned i = 0; i < CSI.size(); ++i)
      ++MBBI;

    // Iterate over list of callee-saved registers and emit .cfi_offset
    // directives.
    for (std::vector<CalleeSavedInfo>::const_iterator I = CSI.begin(),
           E = CSI.end(); I != E; ++I) {
      int64_t Offset = MFI->getObjectOffset(I->getFrameIdx());
      unsigned Reg = I->getReg();
      {
        // Reg is in CPURegs.
        unsigned CFIIndex = MMI.addFrameInst(MCCFIInstruction::createOffset(
            nullptr, MRI->getDwarfRegNum(Reg, 1), Offset));
        BuildMI(MBB, MBBI, dl, TII.get(TargetOpcode::CFI_INSTRUCTION))
            .addCFIIndex(CFIIndex);
      }
    }
  }

#if CH >= CH9_3 //1.5
  if (ZCPUFI->callsEhReturn()) {
    // Insert instructions that spill eh data registers.
    for (int I = 0; I < ABI.EhDataRegSize(); ++I) {
      if (!MBB.isLiveIn(ABI.GetEhDataReg(I)))
        MBB.addLiveIn(ABI.GetEhDataReg(I));
      TII.storeRegToStackSlot(MBB, MBBI, ABI.GetEhDataReg(I), false,
                              ZCPUFI->getEhDataRegFI(I), RC, &RegInfo);
    }

    // Emit .cfi_offset directives for eh data registers.
    for (int I = 0; I < ABI.EhDataRegSize(); ++I) {
      int64_t Offset = MFI->getObjectOffset(ZCPUFI->getEhDataRegFI(I));
      unsigned Reg = MRI->getDwarfRegNum(ABI.GetEhDataReg(I), true);
      unsigned CFIIndex = MMI.addFrameInst(
          MCCFIInstruction::createOffset(nullptr, Reg, Offset));
      BuildMI(MBB, MBBI, dl, TII.get(TargetOpcode::CFI_INSTRUCTION))
          .addCFIIndex(CFIIndex);
    }
  }
#endif

#if CH >= CH9_3 //2
  // if framepointer enabled, set it to point to the stack pointer.
  if (hasFP(MF)) {
    if (ZCPUFI->callsEhDwarf()) {
      BuildMI(MBB, MBBI, dl, TII.get(ADDu), ZCPU::V0).addReg(FP).addReg(ZERO)
        .setMIFlag(MachineInstr::FrameSetup);
    }
    //@ Insert instruction "move $fp, $sp" at this location.
    BuildMI(MBB, MBBI, dl, TII.get(ADDu), FP).addReg(SP).addReg(ZERO)
      .setMIFlag(MachineInstr::FrameSetup);

    // emit ".cfi_def_cfa_register $fp"
    unsigned CFIIndex = MMI.addFrameInst(MCCFIInstruction::createDefCfaRegister(
        nullptr, MRI->getDwarfRegNum(FP, true)));
    BuildMI(MBB, MBBI, dl, TII.get(TargetOpcode::CFI_INSTRUCTION))
        .addCFIIndex(CFIIndex);
  }
//@ENABLE_GPRESTORE {
#ifdef ENABLE_GPRESTORE
  // Restore GP from the saved stack location
  if (ZCPUFI->needGPSaveRestore()) {
    unsigned Offset = MFI->getObjectOffset(ZCPUFI->getGPFI());
    BuildMI(MBB, MBBI, dl, TII.get(ZCPU::CPRESTORE)).addImm(Offset)
      .addReg(ZCPU::GP);
  }
#endif
//@ENABLE_GPRESTORE }
#endif
#endif // #if CH >= CH3_5 //1
}
//}

//@emitEpilogue {
void ZCPUSEFrameLowering::emitEpilogue(MachineFunction &MF,
                                 MachineBasicBlock &MBB) const {
#if CH >= CH3_5 //2
  MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
  MachineFrameInfo *MFI            = MF.getFrameInfo();
  ZCPUFunctionInfo *ZCPUFI = MF.getInfo<ZCPUFunctionInfo>();

  const ZCPUSEInstrInfo &TII =
      *static_cast<const ZCPUSEInstrInfo *>(STI.getInstrInfo());
  const ZCPURegisterInfo &RegInfo =
      *static_cast<const ZCPURegisterInfo *>(STI.getRegisterInfo());


  DebugLoc dl = MBBI->getDebugLoc();
  ZCPUABIInfo ABI = STI.getABI();
  unsigned SP = ZCPU::SP;
#if CH >= CH9_3 //3
  unsigned FP = ZCPU::FP;
  unsigned ZERO = ZCPU::ZERO;
  unsigned ADDu = ZCPU::ADDu;

  // if framepointer enabled, restore the stack pointer.
  if (hasFP(MF)) {
    // Find the first instruction that restores a callee-saved register.
    MachineBasicBlock::iterator I = MBBI;

    for (unsigned i = 0; i < MFI->getCalleeSavedInfo().size(); ++i)
      --I;

    // Insert instruction "move $sp, $fp" at this location.
    BuildMI(MBB, I, dl, TII.get(ADDu), SP).addReg(FP).addReg(ZERO);
  }
#endif

#if CH >= CH9_3 //4
  if (ZCPUFI->callsEhReturn()) {
    const TargetRegisterClass *RC = &ZCPU::GPROutRegClass;

    // Find first instruction that restores a callee-saved register.
    MachineBasicBlock::iterator I = MBBI;
    for (unsigned i = 0; i < MFI->getCalleeSavedInfo().size(); ++i)
      --I;

    // Insert instructions that restore eh data registers.
    for (int J = 0; J < ABI.EhDataRegSize(); ++J) {
      TII.loadRegFromStackSlot(MBB, I, ABI.GetEhDataReg(J),
                               ZCPUFI->getEhDataRegFI(J), RC, &RegInfo);
    }
  }
#endif

  // Get the number of bytes from FrameInfo
  uint64_t StackSize = MFI->getStackSize();

  if (!StackSize)
    return;

  // Adjust stack.
  TII.adjustStackPtr(SP, StackSize, MBB, MBBI);
#endif // #if CH >= CH3_5 //2
}
//}

#if CH >= CH9_1 //1
bool ZCPUSEFrameLowering::
spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                          MachineBasicBlock::iterator MI,
                          const std::vector<CalleeSavedInfo> &CSI,
                          const TargetRegisterInfo *TRI) const {
  MachineFunction *MF = MBB.getParent();
  MachineBasicBlock *EntryBlock = MF->begin();
  const TargetInstrInfo &TII = *MF->getSubtarget().getInstrInfo();

  for (unsigned i = 0, e = CSI.size(); i != e; ++i) {
    // Add the callee-saved register as live-in. Do not add if the register is
    // LR and return address is taken, because it has already been added in
    // method ZCPUTargetLowering::LowerRETURNADDR.
    // It's killed at the spill, unless the register is LR and return address
    // is taken.
    unsigned Reg = CSI[i].getReg();
    bool IsRAAndRetAddrIsTaken = (Reg == ZCPU::LR)
        && MF->getFrameInfo()->isReturnAddressTaken();
    if (!IsRAAndRetAddrIsTaken)
      EntryBlock->addLiveIn(Reg);

    // Insert the spill to the stack frame.
    bool IsKill = !IsRAAndRetAddrIsTaken;
    const TargetRegisterClass *RC = TRI->getMinimalPhysRegClass(Reg);
    TII.storeRegToStackSlot(*EntryBlock, MI, Reg, IsKill,
                            CSI[i].getFrameIdx(), RC, TRI);
  }

  return true;
}
#endif

#if CH >= CH3_5 //3
//@hasReservedCallFrame {
bool
ZCPUSEFrameLowering::hasReservedCallFrame(const MachineFunction &MF) const {
  const MachineFrameInfo *MFI = MF.getFrameInfo();

  // Reserve call frame if the size of the maximum call frame fits into 16-bit
  // immediate field and there are no variable sized objects on the stack.
  // Make sure the second register scavenger spill slot can be accessed with one
  // instruction.
  return isInt<16>(MFI->getMaxCallFrameSize() + getStackAlignment()) &&
    !MFI->hasVarSizedObjects();
}
//}
#endif //#if CH >= CH3_5 //3

#if CH >= CH3_5 //4

/// Mark \p Reg and all registers aliasing it in the bitset.
static void setAliasRegs(MachineFunction &MF, BitVector &SavedRegs, unsigned Reg) {
  const TargetRegisterInfo *TRI = MF.getSubtarget().getRegisterInfo();
  for (MCRegAliasIterator AI(Reg, TRI, true); AI.isValid(); ++AI)
    SavedRegs.set(*AI);
}

//@determineCalleeSaves {
// This method is called immediately before PrologEpilogInserter scans the 
//  physical registers used to determine what callee saved registers should be 
//  spilled. This method is optional. 
void ZCPUSEFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                               BitVector &SavedRegs,
                                               RegScavenger *RS) const {
//@determineCalleeSaves-body
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
  ZCPUFunctionInfo *ZCPUFI = MF.getInfo<ZCPUFunctionInfo>();
  MachineRegisterInfo& MRI = MF.getRegInfo();
#if CH >= CH9_3 //5
  unsigned FP = ZCPU::FP;

  // Mark $fp as used if function has dedicated frame pointer.
  if (hasFP(MF))
    setAliasRegs(MF, SavedRegs, FP);

  //@callsEhReturn
  // Create spill slots for eh data registers if function calls eh_return.
  if (ZCPUFI->callsEhReturn())
    ZCPUFI->createEhDataRegsFI();
#endif

  if (MF.getFrameInfo()->hasCalls())
    setAliasRegs(MF, SavedRegs, ZCPU::LR);

  return;
}
//}
#endif // #if CH >= CH3_5 //4

const ZCPUFrameLowering *
llvm::createZCPUSEFrameLowering(const ZCPUSubtarget &ST) {
  return new ZCPUSEFrameLowering(ST);
}

#endif // #if CH >= CH3_1
