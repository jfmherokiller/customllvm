//===-- ZCPULongBranch.cpp - Emit long branches ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass expands a branch or jump instruction into a long branch if its
// offset is too large to fit into its immediate field.
//
// FIXME: Fix pc-region jump instructions which cross 256MB segment boundaries.
//===----------------------------------------------------------------------===//

#include "ZCPU.h"

#if CH >= CH8_2

#include "MCTargetDesc/ZCPUBaseInfo.h"
#include "ZCPUMachineFunction.h"
#include "ZCPUTargetMachine.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegisterInfo.h"

using namespace llvm;

#define DEBUG_TYPE "cpu0-long-branch"

STATISTIC(LongBranches, "Number of long branches.");

static cl::opt<bool> ForceLongBranch(
  "force-cpu0-long-branch",
  cl::init(false),
  cl::desc("ZCPU: Expand all branches to long format."),
  cl::Hidden);

namespace {
  typedef MachineBasicBlock::iterator Iter;
  typedef MachineBasicBlock::reverse_iterator ReverseIter;

  struct MBBInfo {
    uint64_t Size, Address;
    bool HasLongBranch;
    MachineInstr *Br;

    MBBInfo() : Size(0), HasLongBranch(false), Br(nullptr) {}
  };

  class ZCPULongBranch : public MachineFunctionPass {

  public:
    static char ID;
    ZCPULongBranch(TargetMachine &tm)
        : MachineFunctionPass(ID), TM(tm),
          IsPIC(TM.getRelocationModel() == Reloc::PIC_),
          ABI(static_cast<const ZCPUTargetMachine &>(TM).getABI()) {}

    const char *getPassName() const override {
      return "ZCPU Long Branch";
    }

    bool runOnMachineFunction(MachineFunction &F) override;

  private:
    void splitMBB(MachineBasicBlock *MBB);
    void initMBBInfo();
    int64_t computeOffset(const MachineInstr *Br);
    void replaceBranch(MachineBasicBlock &MBB, Iter Br, DebugLoc DL,
                       MachineBasicBlock *MBBOpnd);
    void expandToLongBranch(MBBInfo &Info);

    const TargetMachine &TM;
    MachineFunction *MF;
    SmallVector<MBBInfo, 16> MBBInfos;
    bool IsPIC;
    ZCPUABIInfo ABI;
    unsigned LongBranchSeqSize;
  };

  char ZCPULongBranch::ID = 0;
} // end of anonymous namespace

/// createZCPULongBranchPass - Returns a pass that converts branches to long
/// branches.
FunctionPass *llvm::createZCPULongBranchPass(ZCPUTargetMachine &tm) {
  return new ZCPULongBranch(tm);
}

/// Iterate over list of Br's operands and search for a MachineBasicBlock
/// operand.
static MachineBasicBlock *getTargetMBB(const MachineInstr &Br) {
  for (unsigned I = 0, E = Br.getDesc().getNumOperands(); I < E; ++I) {
    const MachineOperand &MO = Br.getOperand(I);

    if (MO.isMBB())
      return MO.getMBB();
  }

  llvm_unreachable("This instruction does not have an MBB operand.");
}

// Traverse the list of instructions backwards until a non-debug instruction is
// found or it reaches E.
static ReverseIter getNonDebugInstr(ReverseIter B, ReverseIter E) {
  for (; B != E; ++B)
    if (!B->isDebugValue())
      return B;

  return E;
}

// Split MBB if it has two direct jumps/branches.
void ZCPULongBranch::splitMBB(MachineBasicBlock *MBB) {
  ReverseIter End = MBB->rend();
  ReverseIter LastBr = getNonDebugInstr(MBB->rbegin(), End);

  // Return if MBB has no branch instructions.
  if ((LastBr == End) ||
      (!LastBr->isConditionalBranch() && !LastBr->isUnconditionalBranch()))
    return;

  ReverseIter FirstBr = getNonDebugInstr(std::next(LastBr), End);

  // MBB has only one branch instruction if FirstBr is not a branch
  // instruction.
  if ((FirstBr == End) ||
      (!FirstBr->isConditionalBranch() && !FirstBr->isUnconditionalBranch()))
    return;

  assert(!FirstBr->isIndirectBranch() && "Unexpected indirect branch found.");

  // Create a new MBB. Move instructions in MBB to the newly created MBB.
  MachineBasicBlock *NewMBB =
    MF->CreateMachineBasicBlock(MBB->getBasicBlock());

  // Insert NewMBB and fix control flow.
  MachineBasicBlock *Tgt = getTargetMBB(*FirstBr);
  NewMBB->transferSuccessors(MBB);
  NewMBB->removeSuccessor(Tgt);
  MBB->addSuccessor(NewMBB);
  MBB->addSuccessor(Tgt);
  MF->insert(std::next(MachineFunction::iterator(MBB)), NewMBB);

  NewMBB->splice(NewMBB->end(), MBB, (++LastBr).base(), MBB->end());
}

// Fill MBBInfos.
void ZCPULongBranch::initMBBInfo() {
  // Split the MBBs if they have two branches. Each basic block should have at
  // most one branch after this loop is executed.
  for (MachineFunction::iterator I = MF->begin(), E = MF->end(); I != E;)
    splitMBB(I++);

  MF->RenumberBlocks();
  MBBInfos.clear();
  MBBInfos.resize(MF->size());

  const ZCPUInstrInfo *TII =
      static_cast<const ZCPUInstrInfo *>(MF->getSubtarget().getInstrInfo());
  for (unsigned I = 0, E = MBBInfos.size(); I < E; ++I) {
    MachineBasicBlock *MBB = MF->getBlockNumbered(I);

    // Compute size of MBB.
    for (MachineBasicBlock::instr_iterator MI = MBB->instr_begin();
         MI != MBB->instr_end(); ++MI)
      MBBInfos[I].Size += TII->GetInstSizeInBytes(&*MI);

    // Search for MBB's branch instruction.
    ReverseIter End = MBB->rend();
    ReverseIter Br = getNonDebugInstr(MBB->rbegin(), End);

    if ((Br != End) && !Br->isIndirectBranch() &&
        (Br->isConditionalBranch() ||
         (Br->isUnconditionalBranch() &&
          TM.getRelocationModel() == Reloc::PIC_)))
      MBBInfos[I].Br = (++Br).base();
  }
}

// Compute offset of branch in number of bytes.
int64_t ZCPULongBranch::computeOffset(const MachineInstr *Br) {
  int64_t Offset = 0;
  int ThisMBB = Br->getParent()->getNumber();
  int TargetMBB = getTargetMBB(*Br)->getNumber();

  // Compute offset of a forward branch.
  if (ThisMBB < TargetMBB) {
    for (int N = ThisMBB + 1; N < TargetMBB; ++N)
      Offset += MBBInfos[N].Size;

    return Offset + 4;
  }

  // Compute offset of a backward branch.
  for (int N = ThisMBB; N >= TargetMBB; --N)
    Offset += MBBInfos[N].Size;

  return -Offset + 4;
}

// Replace Br with a branch which has the opposite condition code and a
// MachineBasicBlock operand MBBOpnd.
void ZCPULongBranch::replaceBranch(MachineBasicBlock &MBB, Iter Br,
                                   DebugLoc DL, MachineBasicBlock *MBBOpnd) {
  const ZCPUInstrInfo *TII = static_cast<const ZCPUInstrInfo *>(
      MBB.getParent()->getSubtarget().getInstrInfo());
  unsigned NewOpc = TII->getOppositeBranchOpc(Br->getOpcode());
  const MCInstrDesc &NewDesc = TII->get(NewOpc);

  MachineInstrBuilder MIB = BuildMI(MBB, Br, DL, NewDesc);

  for (unsigned I = 0, E = Br->getDesc().getNumOperands(); I < E; ++I) {
    MachineOperand &MO = Br->getOperand(I);

    if (!MO.isReg()) {
      assert(MO.isMBB() && "MBB operand expected.");
      break;
    }

    MIB.addReg(MO.getReg());
  }

  MIB.addMBB(MBBOpnd);

  if (Br->hasDelaySlot()) {
    // Bundle the instruction in the delay slot to the newly created branch
    // and erase the original branch.
    assert(Br->isBundledWithSucc());
    MachineBasicBlock::instr_iterator II(Br);
    MIBundleBuilder(&*MIB).append((++II)->removeFromBundle());
  }
  Br->eraseFromParent();
}

// Expand branch instructions to long branches.
// TODO: This function has to be fixed for beqz16 and bnez16, because it
// currently assumes that all branches have 16-bit offsets, and will produce
// wrong code if branches whose allowed offsets are [-128, -126, ..., 126]
// are present.
void ZCPULongBranch::expandToLongBranch(MBBInfo &I) {
  MachineBasicBlock::iterator Pos;
  MachineBasicBlock *MBB = I.Br->getParent(), *TgtMBB = getTargetMBB(*I.Br);
  DebugLoc DL = I.Br->getDebugLoc();
  const BasicBlock *BB = MBB->getBasicBlock();
  MachineFunction::iterator FallThroughMBB = ++MachineFunction::iterator(MBB);
  MachineBasicBlock *LongBrMBB = MF->CreateMachineBasicBlock(BB);
  const ZCPUSubtarget &Subtarget =
      static_cast<const ZCPUSubtarget &>(MF->getSubtarget());
  const ZCPUInstrInfo *TII =
      static_cast<const ZCPUInstrInfo *>(Subtarget.getInstrInfo());

  MF->insert(FallThroughMBB, LongBrMBB);
  MBB->removeSuccessor(TgtMBB);
  MBB->addSuccessor(LongBrMBB);

  if (IsPIC) {
    MachineBasicBlock *BalTgtMBB = MF->CreateMachineBasicBlock(BB);
    MF->insert(FallThroughMBB, BalTgtMBB);
    LongBrMBB->addSuccessor(BalTgtMBB);
    BalTgtMBB->addSuccessor(TgtMBB);

    unsigned BalOp = ZCPU::BAL;

    // $longbr:
    //  addiu $sp, $sp, -8
    //  st $lr, 0($sp)
    //  lui $at, %hi($tgt - $baltgt)
    //  addiu $lr, $lr, %lo($tgt - $baltgt)
    //  bal $baltgt
    //  nop
    // $baltgt:
    //  addu $at, $lr, $at
    //  addiu $sp, $sp, 8
    //  ld $lr, 0($sp)
    //  jr $at
    //  nop
    // $fallthrough:
    //

    Pos = LongBrMBB->begin();

    BuildMI(*LongBrMBB, Pos, DL, TII->get(ZCPU::ADDiu), ZCPU::SP)
      .addReg(ZCPU::SP).addImm(-8);
    BuildMI(*LongBrMBB, Pos, DL, TII->get(ZCPU::ST)).addReg(ZCPU::LR)
      .addReg(ZCPU::SP).addImm(0);

    // LUi and ADDiu instructions create 32-bit offset of the target basic
    // block from the target of BAL instruction.  We cannot use immediate
    // value for this offset because it cannot be determined accurately when
    // the program has inline assembly statements.  We therefore use the
    // relocation expressions %hi($tgt-$baltgt) and %lo($tgt-$baltgt) which
    // are resolved during the fixup, so the values will always be correct.
    //
    // Since we cannot create %hi($tgt-$baltgt) and %lo($tgt-$baltgt)
    // expressions at this point (it is possible only at the MC layer),
    // we replace LUi and ADDiu with pseudo instructions
    // LONG_BRANCH_LUi and LONG_BRANCH_ADDiu, and add both basic
    // blocks as operands to these instructions.  When lowering these pseudo
    // instructions to LUi and ADDiu in the MC layer, we will create
    // %hi($tgt-$baltgt) and %lo($tgt-$baltgt) expressions and add them as
    // operands to lowered instructions.

    BuildMI(*LongBrMBB, Pos, DL, TII->get(ZCPU::LONG_BRANCH_LUi), ZCPU::AT)
      .addMBB(TgtMBB).addMBB(BalTgtMBB);
    BuildMI(*LongBrMBB, Pos, DL, TII->get(ZCPU::LONG_BRANCH_ADDiu), ZCPU::AT)
      .addReg(ZCPU::AT).addMBB(TgtMBB).addMBB(BalTgtMBB);
    MIBundleBuilder(*LongBrMBB, Pos)
        .append(BuildMI(*MF, DL, TII->get(BalOp)).addMBB(BalTgtMBB));

    Pos = BalTgtMBB->begin();

    BuildMI(*BalTgtMBB, Pos, DL, TII->get(ZCPU::ADDu), ZCPU::AT)
      .addReg(ZCPU::LR).addReg(ZCPU::AT);
    BuildMI(*BalTgtMBB, Pos, DL, TII->get(ZCPU::LD), ZCPU::LR)
      .addReg(ZCPU::SP).addImm(0);
    BuildMI(*BalTgtMBB, Pos, DL, TII->get(ZCPU::ADDiu), ZCPU::SP)
      .addReg(ZCPU::SP).addImm(8);

    MIBundleBuilder(*BalTgtMBB, Pos)
      .append(BuildMI(*MF, DL, TII->get(ZCPU::JR)).addReg(ZCPU::AT))
      .append(BuildMI(*MF, DL, TII->get(ZCPU::NOP)));

    assert(LongBrMBB->size() + BalTgtMBB->size() == LongBranchSeqSize);
  } else {
    // $longbr:
    //  jmp $tgt
    //  nop
    // $fallthrough:
    //
    Pos = LongBrMBB->begin();
    LongBrMBB->addSuccessor(TgtMBB);
    MIBundleBuilder(*LongBrMBB, Pos)
      .append(BuildMI(*MF, DL, TII->get(ZCPU::JMP)).addMBB(TgtMBB))
      .append(BuildMI(*MF, DL, TII->get(ZCPU::NOP)));

    assert(LongBrMBB->size() == LongBranchSeqSize);
  }

  if (I.Br->isUnconditionalBranch()) {
    // Change branch destination.
    assert(I.Br->getDesc().getNumOperands() == 1);
    I.Br->RemoveOperand(0);
    I.Br->addOperand(MachineOperand::CreateMBB(LongBrMBB));
  } else
    // Change branch destination and reverse condition.
    replaceBranch(*MBB, I.Br, DL, FallThroughMBB);
}

static void emitGPDisp(MachineFunction &F, const ZCPUInstrInfo *TII) {
  MachineBasicBlock &MBB = F.front();
  MachineBasicBlock::iterator I = MBB.begin();
  DebugLoc DL = MBB.findDebugLoc(MBB.begin());
  BuildMI(MBB, I, DL, TII->get(ZCPU::LUi), ZCPU::V0)
    .addExternalSymbol("_gp_disp", ZCPUII::MO_ABS_HI);
  BuildMI(MBB, I, DL, TII->get(ZCPU::ADDiu), ZCPU::V0)
    .addReg(ZCPU::V0).addExternalSymbol("_gp_disp", ZCPUII::MO_ABS_LO);
  MBB.removeLiveIn(ZCPU::V0);
}

bool ZCPULongBranch::runOnMachineFunction(MachineFunction &F) {
  const ZCPUSubtarget &STI =
      static_cast<const ZCPUSubtarget &>(F.getSubtarget());
  const ZCPUInstrInfo *TII =
      static_cast<const ZCPUInstrInfo *>(STI.getInstrInfo());
  LongBranchSeqSize =
      !IsPIC ? 2 : 10;

  if (!STI.enableLongBranchPass())
    return false;
  if ((TM.getRelocationModel() == Reloc::PIC_) &&
      static_cast<const ZCPUTargetMachine &>(TM).getABI().IsO32() &&
      F.getInfo<ZCPUFunctionInfo>()->globalBaseRegSet())
    emitGPDisp(F, TII);

  MF = &F;
  initMBBInfo();

  SmallVectorImpl<MBBInfo>::iterator I, E = MBBInfos.end();
  bool EverMadeChange = false, MadeChange = true;

  while (MadeChange) {
    MadeChange = false;

    for (I = MBBInfos.begin(); I != E; ++I) {
      // Skip if this MBB doesn't have a branch or the branch has already been
      // converted to a long branch.
      if (!I->Br || I->HasLongBranch)
        continue;

      int ShVal = 4;
      int64_t Offset = computeOffset(I->Br) / ShVal;

      // Check if offset fits into 16-bit immediate field of branches.
      if (!ForceLongBranch && isInt<16>(Offset))
        continue;

      I->HasLongBranch = true;
      I->Size += LongBranchSeqSize * 4;
      ++LongBranches;
      EverMadeChange = MadeChange = true;
    }
  }

  if (!EverMadeChange)
    return true;

  // Compute basic block addresses.
  if (TM.getRelocationModel() == Reloc::PIC_) {
    uint64_t Address = 0;

    for (I = MBBInfos.begin(); I != E; Address += I->Size, ++I)
      I->Address = Address;
  }

  // Do the expansion.
  for (I = MBBInfos.begin(); I != E; ++I)
    if (I->HasLongBranch)
      expandToLongBranch(*I);

  MF->RenumberBlocks();

  return true;
}

#endif //#if CH >= CH8_1
