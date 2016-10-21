//===-- ZCPUSEInstrInfo.cpp - ZCPU32/64 Instruction Information -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the ZCPU32/64 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "ZCPUSEInstrInfo.h"
#if CH >= CH3_1

#if CH >= CH3_2
#include "InstPrinter/ZCPUInstPrinter.h"
#endif
#include "ZCPUMachineFunction.h"
#include "ZCPUTargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

ZCPUSEInstrInfo::ZCPUSEInstrInfo(const ZCPUSubtarget &STI)
    : ZCPUInstrInfo(STI),
      RI(STI) {}

const ZCPURegisterInfo &ZCPUSEInstrInfo::getRegisterInfo() const {
  return RI;
}

#if CH >= CH4_1
void ZCPUSEInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator I, DebugLoc DL,
                                  unsigned DestReg, unsigned SrcReg,
                                  bool KillSrc) const {
  unsigned Opc = 0, ZeroReg = 0;

  if (ZCPU::CPURegsRegClass.contains(DestReg)) { // Copy to CPU Reg.
    if (ZCPU::CPURegsRegClass.contains(SrcReg))
      Opc = ZCPU::ADDu, ZeroReg = ZCPU::ZERO;
    else if (SrcReg == ZCPU::HI)
      Opc = ZCPU::MFHI, SrcReg = 0;
    else if (SrcReg == ZCPU::LO)
      Opc = ZCPU::MFLO, SrcReg = 0;
  }
  else if (ZCPU::CPURegsRegClass.contains(SrcReg)) { // Copy from CPU Reg.
    if (DestReg == ZCPU::HI)
      Opc = ZCPU::MTHI, DestReg = 0;
    else if (DestReg == ZCPU::LO)
      Opc = ZCPU::MTLO, DestReg = 0;
  }

  assert(Opc && "Cannot copy registers");

  MachineInstrBuilder MIB = BuildMI(MBB, I, DL, get(Opc));

  if (DestReg)
    MIB.addReg(DestReg, RegState::Define);

  if (ZeroReg)
    MIB.addReg(ZeroReg);

  if (SrcReg)
    MIB.addReg(SrcReg, getKillRegState(KillSrc));
}
#endif

#if CH >= CH9_1 //1
void ZCPUSEInstrInfo::
storeRegToStack(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                unsigned SrcReg, bool isKill, int FI,
                const TargetRegisterClass *RC, const TargetRegisterInfo *TRI,
                int64_t Offset) const {
  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();
  MachineMemOperand *MMO = GetMemOperand(MBB, FI, MachineMemOperand::MOStore);

  unsigned Opc = 0;

  Opc = ZCPU::ST;
  assert(Opc && "Register class not handled!");
  BuildMI(MBB, I, DL, get(Opc)).addReg(SrcReg, getKillRegState(isKill))
    .addFrameIndex(FI).addImm(Offset).addMemOperand(MMO);
}

void ZCPUSEInstrInfo::
loadRegFromStack(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                 unsigned DestReg, int FI, const TargetRegisterClass *RC,
                 const TargetRegisterInfo *TRI, int64_t Offset) const {
  DebugLoc DL;
  if (I != MBB.end()) DL = I->getDebugLoc();
  MachineMemOperand *MMO = GetMemOperand(MBB, FI, MachineMemOperand::MOLoad);
  unsigned Opc = 0;

  Opc = ZCPU::LD;
  assert(Opc && "Register class not handled!");
  BuildMI(MBB, I, DL, get(Opc), DestReg).addFrameIndex(FI).addImm(Offset)
    .addMemOperand(MMO);
}
#endif //#if CH >= CH9_1 //1

#if CH >= CH3_4 //1
//@expandPostRAPseudo
/// Expand Pseudo instructions into real backend instructions
bool ZCPUSEInstrInfo::expandPostRAPseudo(MachineBasicBlock::iterator MI) const {
//@expandPostRAPseudo-body
  MachineBasicBlock &MBB = *MI->getParent();

  switch(MI->getDesc().getOpcode()) {
  default:
    return false;
  case ZCPU::RetLR:
    expandRetLR(MBB, MI);
    break;
#if CH >= CH9_3 //1
  case ZCPU::ZCPUeh_return32:
    expandEhReturn(MBB, MI);
    break;
#endif //#if CH >= CH9_3 //1
  }

  MBB.erase(MI);
  return true;
}
#endif //#if CH >= CH3_4 //1

#if CH >= CH3_5 //2
/// Adjust SP by Amount bytes.
void ZCPUSEInstrInfo::adjustStackPtr(unsigned SP, int64_t Amount,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const {
  DebugLoc DL = I != MBB.end() ? I->getDebugLoc() : DebugLoc();
  unsigned ADDu = ZCPU::ADDu;
  unsigned ADDiu = ZCPU::ADDiu;

  if (isInt<16>(Amount))// addiu sp, sp, amount
    BuildMI(MBB, I, DL, get(ADDiu), SP).addReg(SP).addImm(Amount);
  else { // Expand immediate that doesn't fit in 16-bit.
    unsigned Reg = loadImmediate(Amount, MBB, I, DL, nullptr);
    BuildMI(MBB, I, DL, get(ADDu), SP).addReg(SP).addReg(Reg, RegState::Kill);
  }
}

/// This function generates the sequence of instructions needed to get the
/// result of adding register REG and immediate IMM.
unsigned
ZCPUSEInstrInfo::loadImmediate(int64_t Imm, MachineBasicBlock &MBB,
                               MachineBasicBlock::iterator II, DebugLoc DL,
                               unsigned *NewImm) const {
  ZCPUAnalyzeImmediate AnalyzeImm;
  unsigned Size = 32;
  unsigned LUi = ZCPU::LUi;
  unsigned ZEROReg = ZCPU::ZERO;
  unsigned ATReg = ZCPU::AT;
  bool LastInstrIsADDiu = NewImm;

  const ZCPUAnalyzeImmediate::InstSeq &Seq =
    AnalyzeImm.Analyze(Imm, Size, LastInstrIsADDiu);
  ZCPUAnalyzeImmediate::InstSeq::const_iterator Inst = Seq.begin();

  assert(Seq.size() && (!LastInstrIsADDiu || (Seq.size() > 1)));

  // The first instruction can be a LUi, which is different from other
  // instructions (ADDiu, ORI and SLL) in that it does not have a register
  // operand.
  if (Inst->Opc == LUi)
    BuildMI(MBB, II, DL, get(LUi), ATReg).addImm(SignExtend64<16>(Inst->ImmOpnd));
  else
    BuildMI(MBB, II, DL, get(Inst->Opc), ATReg).addReg(ZEROReg)
      .addImm(SignExtend64<16>(Inst->ImmOpnd));

  // Build the remaining instructions in Seq.
  for (++Inst; Inst != Seq.end() - LastInstrIsADDiu; ++Inst)
    BuildMI(MBB, II, DL, get(Inst->Opc), ATReg).addReg(ATReg)
      .addImm(SignExtend64<16>(Inst->ImmOpnd));

  if (LastInstrIsADDiu)
    *NewImm = Inst->ImmOpnd;

  return ATReg;
}
#endif //#if CH >= CH3_5 //2

#if CH >= CH3_4 //2
void ZCPUSEInstrInfo::expandRetLR(MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I) const {
  BuildMI(MBB, I, I->getDebugLoc(), get(ZCPU::RET)).addReg(ZCPU::LR);
}
#endif //#if CH >= CH3_4 //2

#if CH >= CH8_2 //1
/// getOppositeBranchOpc - Return the inverse of the specified
/// opcode, e.g. turning BEQ to BNE.
unsigned ZCPUSEInstrInfo::getOppositeBranchOpc(unsigned Opc) const {
  switch (Opc) {
  default:           llvm_unreachable("Illegal opcode!");
  case ZCPU::BEQ:    return ZCPU::BNE;
  case ZCPU::BNE:    return ZCPU::BEQ;
  }
}
#endif

#if CH >= CH9_3 //2
void ZCPUSEInstrInfo::expandEhReturn(MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const {
  // This pseudo instruction is generated as part of the lowering of
  // ISD::EH_RETURN. We convert it to a stack increment by OffsetReg, and
  // indirect jump to TargetReg
  unsigned ADDU = ZCPU::ADDu;
  unsigned SP = ZCPU::SP;
  unsigned LR = ZCPU::LR;
  unsigned T9 = ZCPU::T9;
  unsigned ZERO = ZCPU::ZERO;
  unsigned OffsetReg = I->getOperand(0).getReg();
  unsigned TargetReg = I->getOperand(1).getReg();

  // addu $lr, $v0, $zero
  // addu $sp, $sp, $v1
  // jr   $lr (via RetLR)
  const TargetMachine &TM = MBB.getParent()->getTarget();
  if (TM.getRelocationModel() == Reloc::PIC_)
    BuildMI(MBB, I, I->getDebugLoc(), get(ADDU), T9)
        .addReg(TargetReg)
        .addReg(ZERO);
  BuildMI(MBB, I, I->getDebugLoc(), get(ADDU), LR)
      .addReg(TargetReg)
      .addReg(ZERO);
  BuildMI(MBB, I, I->getDebugLoc(), get(ADDU), SP).addReg(SP).addReg(OffsetReg);
  expandRetLR(MBB, I);
}
#endif

const ZCPUInstrInfo *llvm::createZCPUSEInstrInfo(const ZCPUSubtarget &STI) {
  return new ZCPUSEInstrInfo(STI);
}

#endif // #if CH >= CH3_1
