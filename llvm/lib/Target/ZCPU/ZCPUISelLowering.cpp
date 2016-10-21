//===-- ZCPUISelLowering.cpp - ZCPU DAG Lowering Implementation -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that ZCPU uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//
#include "ZCPUISelLowering.h"
#if CH >= CH3_1

#if CH >= CH3_2
#include "MCTargetDesc/ZCPUBaseInfo.h"
#endif
#include "ZCPUMachineFunction.h"
#include "ZCPUTargetMachine.h"
#include "ZCPUTargetObjectFile.h"
#include "ZCPUSubtarget.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "cpu0-lower"

#if CH >= CH9_1 //1
STATISTIC(NumTailCalls, "Number of tail calls");
#endif

#if CH >= CH6_1 //1
SDValue ZCPUTargetLowering::getGlobalReg(SelectionDAG &DAG, EVT Ty) const {
  ZCPUFunctionInfo *FI = DAG.getMachineFunction().getInfo<ZCPUFunctionInfo>();
  return DAG.getRegister(FI->getGlobalBaseReg(), Ty);
}

//@getTargetNode(GlobalAddressSDNode
SDValue ZCPUTargetLowering::getTargetNode(GlobalAddressSDNode *N, EVT Ty,
                                          SelectionDAG &DAG,
                                          unsigned Flag) const {
  return DAG.getTargetGlobalAddress(N->getGlobal(), SDLoc(N), Ty, 0, Flag);
}

//@getTargetNode(ExternalSymbolSDNode
SDValue ZCPUTargetLowering::getTargetNode(ExternalSymbolSDNode *N, EVT Ty,
                                          SelectionDAG &DAG,
                                          unsigned Flag) const {
  return DAG.getTargetExternalSymbol(N->getSymbol(), Ty, Flag);
}
#endif

#if CH >= CH8_1 //1
SDValue ZCPUTargetLowering::getTargetNode(BlockAddressSDNode *N, EVT Ty,
                                          SelectionDAG &DAG,
                                          unsigned Flag) const {
  return DAG.getTargetBlockAddress(N->getBlockAddress(), Ty, 0, Flag);
}

SDValue ZCPUTargetLowering::getTargetNode(JumpTableSDNode *N, EVT Ty,
                                          SelectionDAG &DAG,
                                          unsigned Flag) const {
  return DAG.getTargetJumpTable(N->getIndex(), Ty, Flag);
}
#endif

//@3_1 1 {
const char *ZCPUTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case ZCPUISD::JmpLink:           return "ZCPUISD::JmpLink";
  case ZCPUISD::TailCall:          return "ZCPUISD::TailCall";
  case ZCPUISD::Hi:                return "ZCPUISD::Hi";
  case ZCPUISD::Lo:                return "ZCPUISD::Lo";
  case ZCPUISD::GPRel:             return "ZCPUISD::GPRel";
  case ZCPUISD::Ret:               return "ZCPUISD::Ret";
  case ZCPUISD::EH_RETURN:         return "ZCPUISD::EH_RETURN";
  case ZCPUISD::DivRem:            return "ZCPUISD::DivRem";
  case ZCPUISD::DivRemU:           return "ZCPUISD::DivRemU";
  case ZCPUISD::Wrapper:           return "ZCPUISD::Wrapper";
#if CH >= CH12_1 //0.5
  case ZCPUISD::Sync:              return "ZCPUISD::Sync";
#endif //#if CH >= CH12_1 //0.5
  default:                         return NULL;
  }
}
//@3_1 1 }

//@ZCPUTargetLowering {
ZCPUTargetLowering::ZCPUTargetLowering(const ZCPUTargetMachine &TM,
                                       const ZCPUSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI), ABI(TM.getABI()) {

#if CH >= CH3_2

#if CH >= CH7_1 //1
  // ZCPU does not have i1 type, so use i32 for
  // setcc operations results (slt, sgt, ...).
  setBooleanContents(ZeroOrOneBooleanContent);
  setBooleanVectorContents(ZeroOrNegativeOneBooleanContent);

  // Load extented operations for i1 types must be promoted
  for (MVT VT : MVT::integer_valuetypes()) {
    setLoadExtAction(ISD::EXTLOAD,  VT, MVT::i1,  Promote);
    setLoadExtAction(ISD::ZEXTLOAD, VT, MVT::i1,  Promote);
    setLoadExtAction(ISD::SEXTLOAD, VT, MVT::i1,  Promote);
  }
#endif

#if CH >= CH8_1 //2
  // Used by legalize types to correctly generate the setcc result.
  // Without this, every float setcc comes with a AND/OR with the result,
  // we don't want this, since the fpcmp result goes to a flag register,
  // which is used implicitly by brcond and select operations.
  AddPromotedToType(ISD::SETCC, MVT::i1, MVT::i32);
#endif

  // ZCPU Custom Operations
#if CH >= CH6_1 //2
  setOperationAction(ISD::GlobalAddress,      MVT::i32,   Custom);
#endif
#if CH >= CH12_1 //1
  setOperationAction(ISD::GlobalTLSAddress,   MVT::i32,   Custom);
#endif
#if CH >= CH8_1 //3
  setOperationAction(ISD::BlockAddress,       MVT::i32,   Custom);
  setOperationAction(ISD::JumpTable,          MVT::i32,   Custom);
#endif
#if CH >= CH8_2 //1
  setOperationAction(ISD::SELECT,             MVT::i32,   Custom);
#endif
#if CH >= CH8_1 //4
  setOperationAction(ISD::BRCOND,             MVT::Other, Custom);
#endif
#if CH >= CH9_3 //0.5
  setOperationAction(ISD::EH_RETURN, MVT::Other, Custom);
#endif
#if CH >= CH9_3 //1
  setOperationAction(ISD::VASTART,            MVT::Other, Custom);
#endif

#if CH >= CH7_1 //2
  // Handle i64 shl
  setOperationAction(ISD::SHL_PARTS,          MVT::i32,   Expand);
  setOperationAction(ISD::SRA_PARTS,          MVT::i32,   Expand);
  setOperationAction(ISD::SRL_PARTS,          MVT::i32,   Expand);
#endif

#if CH >= CH9_3 //0.7
  setOperationAction(ISD::ADD,                MVT::i32,   Custom);
#endif

#if CH >= CH4_1 //1
  setOperationAction(ISD::SDIV, MVT::i32, Expand);
  setOperationAction(ISD::SREM, MVT::i32, Expand);
  setOperationAction(ISD::UDIV, MVT::i32, Expand);
  setOperationAction(ISD::UREM, MVT::i32, Expand);
#endif

  // Operations not directly supported by ZCPU.
#if CH >= CH8_1 //5
  setOperationAction(ISD::BR_JT,             MVT::Other, Expand);
  setOperationAction(ISD::BR_CC,             MVT::i32, Expand);
#endif
#if CH >= CH8_2 //2
  setOperationAction(ISD::SELECT_CC,         MVT::i32, Expand);
  setOperationAction(ISD::SELECT_CC,         MVT::Other, Expand);
#endif
#if CH >= CH7_1 //3
  setOperationAction(ISD::CTPOP,             MVT::i32,   Expand);
  setOperationAction(ISD::CTTZ,              MVT::i32,   Expand);
  setOperationAction(ISD::CTTZ_ZERO_UNDEF,   MVT::i32,   Expand);
  setOperationAction(ISD::CTLZ_ZERO_UNDEF,   MVT::i32,   Expand);
#endif
#if CH >= CH4_2 //1.2
  // ZCPU doesn't have sext_inreg, replace them with shl/sra.
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1 , Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8 , Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16 , Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i32 , Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::Other , Expand);
#endif

#if CH >= CH9_3 //2
  setOperationAction(ISD::DYNAMIC_STACKALLOC, MVT::i32,  Expand);

  //@vararg 1 {
  // Support va_arg(): variable numbers (not fixed numbers) of arguments 
  //  (parameters) for function all
  setOperationAction(ISD::VAARG,             MVT::Other, Expand);
  setOperationAction(ISD::VACOPY,            MVT::Other, Expand);
  setOperationAction(ISD::VAEND,             MVT::Other, Expand);
  
  //@llvm.stacksave
  // Use the default for now
  setOperationAction(ISD::STACKSAVE,         MVT::Other, Expand);
  setOperationAction(ISD::STACKRESTORE,      MVT::Other, Expand);
#endif

#if CH >= CH12_1 //1.5
  setOperationAction(ISD::ATOMIC_LOAD,       MVT::i32,    Expand);
  setOperationAction(ISD::ATOMIC_LOAD,       MVT::i64,    Expand);
  setOperationAction(ISD::ATOMIC_STORE,      MVT::i32,    Expand);
  setOperationAction(ISD::ATOMIC_STORE,      MVT::i64,    Expand);

  setInsertFencesForAtomic(true);
#endif

#if CH >= CH9_3 //2.5
  setOperationAction(ISD::BSWAP, MVT::i32, Expand);
  setOperationAction(ISD::BSWAP, MVT::i64, Expand);
#endif

#if CH >= CH4_1 //2
  setTargetDAGCombine(ISD::SDIVREM);
  setTargetDAGCombine(ISD::UDIVREM);
#endif

//- Set .align 2
// It will emit .align 2 later
  setMinFunctionAlignment(2);

#if CH >= CH9_3 //3
  setStackPointerRegisterToSaveRestore(ZCPU::SP);
#endif

#if CH >= CH12_1 //2
  setExceptionPointerRegister(ZCPU::A0);
  setExceptionSelectorRegister(ZCPU::A1);
#endif

#endif // #if CH >= CH3_2
}

const ZCPUTargetLowering *ZCPUTargetLowering::create(const ZCPUTargetMachine &TM,
                                                     const ZCPUSubtarget &STI) {
  return llvm::createZCPUSETargetLowering(TM, STI);
}

#if CH >= CH7_1 //3.5
EVT ZCPUTargetLowering::getSetCCResultType(const DataLayout &, LLVMContext &,
                                           EVT VT) const {
  if (!VT.isVector())
    return MVT::i32;
  return VT.changeVectorElementTypeToInteger();
}
#endif

#if CH >= CH4_1 //3
static SDValue performDivRemCombine(SDNode *N, SelectionDAG& DAG,
                                    TargetLowering::DAGCombinerInfo &DCI,
                                    const ZCPUSubtarget &Subtarget) {
  if (DCI.isBeforeLegalizeOps())
    return SDValue();

  EVT Ty = N->getValueType(0);
  unsigned LO = ZCPU::LO;
  unsigned HI = ZCPU::HI;
  unsigned Opc = N->getOpcode() == ISD::SDIVREM ? ZCPUISD::DivRem :
                                                  ZCPUISD::DivRemU;
  SDLoc DL(N);

  SDValue DivRem = DAG.getNode(Opc, DL, MVT::Glue,
                               N->getOperand(0), N->getOperand(1));
  SDValue InChain = DAG.getEntryNode();
  SDValue InGlue = DivRem;

  // insert MFLO
  if (N->hasAnyUseOfValue(0)) {
    SDValue CopyFromLo = DAG.getCopyFromReg(InChain, DL, LO, Ty,
                                            InGlue);
    DAG.ReplaceAllUsesOfValueWith(SDValue(N, 0), CopyFromLo);
    InChain = CopyFromLo.getValue(1);
    InGlue = CopyFromLo.getValue(2);
  }

  // insert MFHI
  if (N->hasAnyUseOfValue(1)) {
    SDValue CopyFromHi = DAG.getCopyFromReg(InChain, DL,
                                            HI, Ty, InGlue);
    DAG.ReplaceAllUsesOfValueWith(SDValue(N, 1), CopyFromHi);
  }

  return SDValue();
}

SDValue ZCPUTargetLowering::PerformDAGCombine(SDNode *N, DAGCombinerInfo &DCI)
  const {
  SelectionDAG &DAG = DCI.DAG;
  unsigned Opc = N->getOpcode();

  switch (Opc) {
  default: break;
  case ISD::SDIVREM:
  case ISD::UDIVREM:
    return performDivRemCombine(N, DAG, DCI, Subtarget);
  }

  return SDValue();
}
#endif

#if CH >= CH6_1 //3
SDValue ZCPUTargetLowering::
LowerOperation(SDValue Op, SelectionDAG &DAG) const
{
  switch (Op.getOpcode())
  {
#if CH >= CH8_1 //6
  case ISD::BRCOND:             return lowerBRCOND(Op, DAG);
#endif //#if CH >= CH8_1
  case ISD::GlobalAddress:      return lowerGlobalAddress(Op, DAG);
#if CH >= CH12_1 //3
  case ISD::GlobalTLSAddress:   return lowerGlobalTLSAddress(Op, DAG);
#endif
#if CH >= CH8_1 //7
  case ISD::BlockAddress:       return lowerBlockAddress(Op, DAG);
  case ISD::JumpTable:          return lowerJumpTable(Op, DAG);
#endif
#if CH >= CH8_2 //3
  case ISD::SELECT:             return lowerSELECT(Op, DAG);
#endif
#if CH >= CH9_3 //4
  case ISD::VASTART:            return lowerVASTART(Op, DAG);
#endif //#if CH >= CH9_3 //4
#if CH >= CH9_3 //4.5
  case ISD::FRAMEADDR:          return lowerFRAMEADDR(Op, DAG);
  case ISD::RETURNADDR:         return lowerRETURNADDR(Op, DAG);
  case ISD::EH_RETURN:          return lowerEH_RETURN(Op, DAG);
  case ISD::ADD:                return lowerADD(Op, DAG);
#endif //#if CH >= CH9_3 //4.5
#if CH >= CH12_1 //7
  case ISD::ATOMIC_FENCE:       return lowerATOMIC_FENCE(Op, DAG);
#endif //#if CH >= CH12_1 //7
  }
  return SDValue();
}
#endif

//===----------------------------------------------------------------------===//
//  Lower helper functions
//===----------------------------------------------------------------------===//

#if CH >= CH9_1 //2
// addLiveIn - This helper function adds the specified physical register to the
// MachineFunction as a live in value.  It also creates a corresponding
// virtual register for it.
static unsigned
addLiveIn(MachineFunction &MF, unsigned PReg, const TargetRegisterClass *RC)
{
  unsigned VReg = MF.getRegInfo().createVirtualRegister(RC);
  MF.getRegInfo().addLiveIn(PReg, VReg);
  return VReg;
}
#endif

#if CH >= CH12_1 //8
MachineBasicBlock *
ZCPUTargetLowering::EmitInstrWithCustomInserter(MachineInstr *MI,
                                                MachineBasicBlock *BB) const {
  switch (MI->getOpcode()) {
  default:
    llvm_unreachable("Unexpected instr type to insert");
  case ZCPU::ATOMIC_LOAD_ADD_I8:
    return emitAtomicBinaryPartword(MI, BB, 1, ZCPU::ADDu);
  case ZCPU::ATOMIC_LOAD_ADD_I16:
    return emitAtomicBinaryPartword(MI, BB, 2, ZCPU::ADDu);
  case ZCPU::ATOMIC_LOAD_ADD_I32:
    return emitAtomicBinary(MI, BB, 4, ZCPU::ADDu);

  case ZCPU::ATOMIC_LOAD_AND_I8:
    return emitAtomicBinaryPartword(MI, BB, 1, ZCPU::AND);
  case ZCPU::ATOMIC_LOAD_AND_I16:
    return emitAtomicBinaryPartword(MI, BB, 2, ZCPU::AND);
  case ZCPU::ATOMIC_LOAD_AND_I32:
    return emitAtomicBinary(MI, BB, 4, ZCPU::AND);

  case ZCPU::ATOMIC_LOAD_OR_I8:
    return emitAtomicBinaryPartword(MI, BB, 1, ZCPU::OR);
  case ZCPU::ATOMIC_LOAD_OR_I16:
    return emitAtomicBinaryPartword(MI, BB, 2, ZCPU::OR);
  case ZCPU::ATOMIC_LOAD_OR_I32:
    return emitAtomicBinary(MI, BB, 4, ZCPU::OR);

  case ZCPU::ATOMIC_LOAD_XOR_I8:
    return emitAtomicBinaryPartword(MI, BB, 1, ZCPU::XOR);
  case ZCPU::ATOMIC_LOAD_XOR_I16:
    return emitAtomicBinaryPartword(MI, BB, 2, ZCPU::XOR);
  case ZCPU::ATOMIC_LOAD_XOR_I32:
    return emitAtomicBinary(MI, BB, 4, ZCPU::XOR);

  case ZCPU::ATOMIC_LOAD_NAND_I8:
    return emitAtomicBinaryPartword(MI, BB, 1, 0, true);
  case ZCPU::ATOMIC_LOAD_NAND_I16:
    return emitAtomicBinaryPartword(MI, BB, 2, 0, true);
  case ZCPU::ATOMIC_LOAD_NAND_I32:
    return emitAtomicBinary(MI, BB, 4, 0, true);

  case ZCPU::ATOMIC_LOAD_SUB_I8:
    return emitAtomicBinaryPartword(MI, BB, 1, ZCPU::SUBu);
  case ZCPU::ATOMIC_LOAD_SUB_I16:
    return emitAtomicBinaryPartword(MI, BB, 2, ZCPU::SUBu);
  case ZCPU::ATOMIC_LOAD_SUB_I32:
    return emitAtomicBinary(MI, BB, 4, ZCPU::SUBu);

  case ZCPU::ATOMIC_SWAP_I8:
    return emitAtomicBinaryPartword(MI, BB, 1, 0);
  case ZCPU::ATOMIC_SWAP_I16:
    return emitAtomicBinaryPartword(MI, BB, 2, 0);
  case ZCPU::ATOMIC_SWAP_I32:
    return emitAtomicBinary(MI, BB, 4, 0);

  case ZCPU::ATOMIC_CMP_SWAP_I8:
    return emitAtomicCmpSwapPartword(MI, BB, 1);
  case ZCPU::ATOMIC_CMP_SWAP_I16:
    return emitAtomicCmpSwapPartword(MI, BB, 2);
  case ZCPU::ATOMIC_CMP_SWAP_I32:
    return emitAtomicCmpSwap(MI, BB, 4);
  }
}

// This function also handles ZCPU::ATOMIC_SWAP_I32 (when BinOpcode == 0), and
// ZCPU::ATOMIC_LOAD_NAND_I32 (when Nand == true)
MachineBasicBlock *
ZCPUTargetLowering::emitAtomicBinary(MachineInstr *MI, MachineBasicBlock *BB,
                                     unsigned Size, unsigned BinOpcode,
                                     bool Nand) const {
  assert((Size == 4) && "Unsupported size for EmitAtomicBinary.");

  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &RegInfo = MF->getRegInfo();
  const TargetRegisterClass *RC = getRegClassFor(MVT::getIntegerVT(Size * 8));
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  DebugLoc DL = MI->getDebugLoc();
  unsigned LL, SC, AND, XOR, ZERO, BEQ;

  LL = ZCPU::LL;
  SC = ZCPU::SC;
  AND = ZCPU::AND;
  XOR = ZCPU::XOR;
  ZERO = ZCPU::ZERO;
  BEQ = ZCPU::BEQ;

  unsigned OldVal = MI->getOperand(0).getReg();
  unsigned Ptr = MI->getOperand(1).getReg();
  unsigned Incr = MI->getOperand(2).getReg();

  unsigned StoreVal = RegInfo.createVirtualRegister(RC);
  unsigned AndRes = RegInfo.createVirtualRegister(RC);
  unsigned AndRes2 = RegInfo.createVirtualRegister(RC);
  unsigned Success = RegInfo.createVirtualRegister(RC);

  // insert new blocks after the current block
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineBasicBlock *loopMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *exitMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineFunction::iterator It = BB;
  ++It;
  MF->insert(It, loopMBB);
  MF->insert(It, exitMBB);

  // Transfer the remainder of BB and its successor edges to exitMBB.
  exitMBB->splice(exitMBB->begin(), BB,
                  std::next(MachineBasicBlock::iterator(MI)), BB->end());
  exitMBB->transferSuccessorsAndUpdatePHIs(BB);

  //  thisMBB:
  //    ...
  //    fallthrough --> loopMBB
  BB->addSuccessor(loopMBB);
  loopMBB->addSuccessor(loopMBB);
  loopMBB->addSuccessor(exitMBB);

  //  loopMBB:
  //    ll oldval, 0(ptr)
  //    <binop> storeval, oldval, incr
  //    sc success, storeval, 0(ptr)
  //    beq success, $0, loopMBB
  BB = loopMBB;
  BuildMI(BB, DL, TII->get(LL), OldVal).addReg(Ptr).addImm(0);
  if (Nand) {
    //  and andres, oldval, incr
    //  xor storeval, $0, andres
    //  xor storeval2, $0, storeval
    BuildMI(BB, DL, TII->get(AND), AndRes).addReg(OldVal).addReg(Incr);
    BuildMI(BB, DL, TII->get(XOR), StoreVal).addReg(ZERO).addReg(AndRes);
    BuildMI(BB, DL, TII->get(XOR), AndRes2).addReg(ZERO).addReg(AndRes);
  } else if (BinOpcode) {
    //  <binop> storeval, oldval, incr
    BuildMI(BB, DL, TII->get(BinOpcode), StoreVal).addReg(OldVal).addReg(Incr);
  } else {
    StoreVal = Incr;
  }
  BuildMI(BB, DL, TII->get(SC), Success).addReg(StoreVal).addReg(Ptr).addImm(0);
  BuildMI(BB, DL, TII->get(BEQ)).addReg(Success).addReg(ZERO).addMBB(loopMBB);

  MI->eraseFromParent(); // The instruction is gone now.

  return exitMBB;
}

MachineBasicBlock *ZCPUTargetLowering::emitSignExtendToI32InReg(
    MachineInstr *MI, MachineBasicBlock *BB, unsigned Size, unsigned DstReg,
    unsigned SrcReg) const {
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  DebugLoc DL = MI->getDebugLoc();

  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &RegInfo = MF->getRegInfo();
  const TargetRegisterClass *RC = getRegClassFor(MVT::i32);
  unsigned ScrReg = RegInfo.createVirtualRegister(RC);

  assert(Size < 32);
  int64_t ShiftImm = 32 - (Size * 8);

  BuildMI(BB, DL, TII->get(ZCPU::SHL), ScrReg).addReg(SrcReg).addImm(ShiftImm);
  BuildMI(BB, DL, TII->get(ZCPU::SRA), DstReg).addReg(ScrReg).addImm(ShiftImm);

  return BB;
}

MachineBasicBlock *ZCPUTargetLowering::emitAtomicBinaryPartword(
    MachineInstr *MI, MachineBasicBlock *BB, unsigned Size, unsigned BinOpcode,
    bool Nand) const {
  assert((Size == 1 || Size == 2) &&
         "Unsupported size for EmitAtomicBinaryPartial.");

  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &RegInfo = MF->getRegInfo();
  const TargetRegisterClass *RC = getRegClassFor(MVT::i32);
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  DebugLoc DL = MI->getDebugLoc();

  unsigned Dest = MI->getOperand(0).getReg();
  unsigned Ptr = MI->getOperand(1).getReg();
  unsigned Incr = MI->getOperand(2).getReg();

  unsigned AlignedAddr = RegInfo.createVirtualRegister(RC);
  unsigned ShiftAmt = RegInfo.createVirtualRegister(RC);
  unsigned Mask = RegInfo.createVirtualRegister(RC);
  unsigned Mask2 = RegInfo.createVirtualRegister(RC);
  unsigned Mask3 = RegInfo.createVirtualRegister(RC);
  unsigned NewVal = RegInfo.createVirtualRegister(RC);
  unsigned OldVal = RegInfo.createVirtualRegister(RC);
  unsigned Incr2 = RegInfo.createVirtualRegister(RC);
  unsigned MaskLSB2 = RegInfo.createVirtualRegister(RC);
  unsigned PtrLSB2 = RegInfo.createVirtualRegister(RC);
  unsigned MaskUpper = RegInfo.createVirtualRegister(RC);
  unsigned AndRes = RegInfo.createVirtualRegister(RC);
  unsigned BinOpRes = RegInfo.createVirtualRegister(RC);
  unsigned BinOpRes2 = RegInfo.createVirtualRegister(RC);
  unsigned MaskedOldVal0 = RegInfo.createVirtualRegister(RC);
  unsigned StoreVal = RegInfo.createVirtualRegister(RC);
  unsigned MaskedOldVal1 = RegInfo.createVirtualRegister(RC);
  unsigned SrlRes = RegInfo.createVirtualRegister(RC);
  unsigned Success = RegInfo.createVirtualRegister(RC);

  // insert new blocks after the current block
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineBasicBlock *loopMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *sinkMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *exitMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineFunction::iterator It = BB;
  ++It;
  MF->insert(It, loopMBB);
  MF->insert(It, sinkMBB);
  MF->insert(It, exitMBB);

  // Transfer the remainder of BB and its successor edges to exitMBB.
  exitMBB->splice(exitMBB->begin(), BB,
                  std::next(MachineBasicBlock::iterator(MI)), BB->end());
  exitMBB->transferSuccessorsAndUpdatePHIs(BB);

  BB->addSuccessor(loopMBB);
  loopMBB->addSuccessor(loopMBB);
  loopMBB->addSuccessor(sinkMBB);
  sinkMBB->addSuccessor(exitMBB);

  //  thisMBB:
  //    addiu   masklsb2,$0,-4                # 0xfffffffc
  //    and     alignedaddr,ptr,masklsb2
  //    andi    ptrlsb2,ptr,3
  //    sll     shiftamt,ptrlsb2,3
  //    ori     maskupper,$0,255               # 0xff
  //    sll     mask,maskupper,shiftamt
  //    xor     mask2,$0,mask
  //    xor     mask3,$0,mask2
  //    sll     incr2,incr,shiftamt

  int64_t MaskImm = (Size == 1) ? 255 : 65535;
  BuildMI(BB, DL, TII->get(ZCPU::ADDiu), MaskLSB2)
    .addReg(ZCPU::ZERO).addImm(-4);
  BuildMI(BB, DL, TII->get(ZCPU::AND), AlignedAddr)
    .addReg(Ptr).addReg(MaskLSB2);
  BuildMI(BB, DL, TII->get(ZCPU::ANDi), PtrLSB2).addReg(Ptr).addImm(3);
  if (Subtarget.isLittle()) {
    BuildMI(BB, DL, TII->get(ZCPU::SHL), ShiftAmt).addReg(PtrLSB2).addImm(3);
  } else {
    unsigned Off = RegInfo.createVirtualRegister(RC);
    BuildMI(BB, DL, TII->get(ZCPU::XORi), Off)
      .addReg(PtrLSB2).addImm((Size == 1) ? 3 : 2);
    BuildMI(BB, DL, TII->get(ZCPU::SHL), ShiftAmt).addReg(Off).addImm(3);
  }
  BuildMI(BB, DL, TII->get(ZCPU::ORi), MaskUpper)
    .addReg(ZCPU::ZERO).addImm(MaskImm);
  BuildMI(BB, DL, TII->get(ZCPU::SHLV), Mask)
    .addReg(MaskUpper).addReg(ShiftAmt);
  BuildMI(BB, DL, TII->get(ZCPU::XOR), Mask2).addReg(ZCPU::ZERO).addReg(Mask);
  BuildMI(BB, DL, TII->get(ZCPU::XOR), Mask3).addReg(ZCPU::ZERO).addReg(Mask2);
  BuildMI(BB, DL, TII->get(ZCPU::SHLV), Incr2).addReg(Incr).addReg(ShiftAmt);

  // atomic.load.binop
  // loopMBB:
  //   ll      oldval,0(alignedaddr)
  //   binop   binopres,oldval,incr2
  //   and     newval,binopres,mask
  //   and     maskedoldval0,oldval,mask3
  //   or      storeval,maskedoldval0,newval
  //   sc      success,storeval,0(alignedaddr)
  //   beq     success,$0,loopMBB

  // atomic.swap
  // loopMBB:
  //   ll      oldval,0(alignedaddr)
  //   and     newval,incr2,mask
  //   and     maskedoldval0,oldval,mask3
  //   or      storeval,maskedoldval0,newval
  //   sc      success,storeval,0(alignedaddr)
  //   beq     success,$0,loopMBB

  BB = loopMBB;
  unsigned LL = ZCPU::LL;
  BuildMI(BB, DL, TII->get(LL), OldVal).addReg(AlignedAddr).addImm(0);
  if (Nand) {
    //  and andres, oldval, incr2
    //  xor binopres,  $0, andres
    //  xor binopres2, $0, binopres
    //  and newval, binopres, mask
    BuildMI(BB, DL, TII->get(ZCPU::AND), AndRes).addReg(OldVal).addReg(Incr2);
    BuildMI(BB, DL, TII->get(ZCPU::XOR), BinOpRes)
      .addReg(ZCPU::ZERO).addReg(AndRes);
    BuildMI(BB, DL, TII->get(ZCPU::XOR), BinOpRes2)
      .addReg(ZCPU::ZERO).addReg(BinOpRes);
    BuildMI(BB, DL, TII->get(ZCPU::AND), NewVal).addReg(BinOpRes).addReg(Mask);
  } else if (BinOpcode) {
    //  <binop> binopres, oldval, incr2
    //  and newval, binopres, mask
    BuildMI(BB, DL, TII->get(BinOpcode), BinOpRes).addReg(OldVal).addReg(Incr2);
    BuildMI(BB, DL, TII->get(ZCPU::AND), NewVal).addReg(BinOpRes).addReg(Mask);
  } else { // atomic.swap
    //  and newval, incr2, mask
    BuildMI(BB, DL, TII->get(ZCPU::AND), NewVal).addReg(Incr2).addReg(Mask);
  }

  BuildMI(BB, DL, TII->get(ZCPU::AND), MaskedOldVal0)
    .addReg(OldVal).addReg(Mask2);
  BuildMI(BB, DL, TII->get(ZCPU::OR), StoreVal)
    .addReg(MaskedOldVal0).addReg(NewVal);
  unsigned SC = ZCPU::SC;
  BuildMI(BB, DL, TII->get(SC), Success)
    .addReg(StoreVal).addReg(AlignedAddr).addImm(0);
  BuildMI(BB, DL, TII->get(ZCPU::BEQ))
    .addReg(Success).addReg(ZCPU::ZERO).addMBB(loopMBB);

  //  sinkMBB:
  //    and     maskedoldval1,oldval,mask
  //    srl     srlres,maskedoldval1,shiftamt
  //    sign_extend dest,srlres
  BB = sinkMBB;

  BuildMI(BB, DL, TII->get(ZCPU::AND), MaskedOldVal1)
    .addReg(OldVal).addReg(Mask);
  BuildMI(BB, DL, TII->get(ZCPU::SHRV), SrlRes)
      .addReg(MaskedOldVal1).addReg(ShiftAmt);
  BB = emitSignExtendToI32InReg(MI, BB, Size, Dest, SrlRes);

  MI->eraseFromParent(); // The instruction is gone now.

  return exitMBB;
}

MachineBasicBlock * ZCPUTargetLowering::emitAtomicCmpSwap(MachineInstr *MI,
                                                          MachineBasicBlock *BB,
                                                          unsigned Size) const {
  assert((Size == 4) && "Unsupported size for EmitAtomicCmpSwap.");

  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &RegInfo = MF->getRegInfo();
  const TargetRegisterClass *RC = getRegClassFor(MVT::getIntegerVT(Size * 8));
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  DebugLoc DL = MI->getDebugLoc();
  unsigned LL, SC, ZERO, BNE, BEQ;

  LL = ZCPU::LL;
  SC = ZCPU::SC;
  ZERO = ZCPU::ZERO;
  BNE = ZCPU::BNE;
  BEQ = ZCPU::BEQ;

  unsigned Dest    = MI->getOperand(0).getReg();
  unsigned Ptr     = MI->getOperand(1).getReg();
  unsigned OldVal  = MI->getOperand(2).getReg();
  unsigned NewVal  = MI->getOperand(3).getReg();

  unsigned Success = RegInfo.createVirtualRegister(RC);

  // insert new blocks after the current block
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineBasicBlock *loop1MBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *loop2MBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *exitMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineFunction::iterator It = BB;
  ++It;
  MF->insert(It, loop1MBB);
  MF->insert(It, loop2MBB);
  MF->insert(It, exitMBB);

  // Transfer the remainder of BB and its successor edges to exitMBB.
  exitMBB->splice(exitMBB->begin(), BB,
                  std::next(MachineBasicBlock::iterator(MI)), BB->end());
  exitMBB->transferSuccessorsAndUpdatePHIs(BB);

  //  thisMBB:
  //    ...
  //    fallthrough --> loop1MBB
  BB->addSuccessor(loop1MBB);
  loop1MBB->addSuccessor(exitMBB);
  loop1MBB->addSuccessor(loop2MBB);
  loop2MBB->addSuccessor(loop1MBB);
  loop2MBB->addSuccessor(exitMBB);

  // loop1MBB:
  //   ll dest, 0(ptr)
  //   bne dest, oldval, exitMBB
  BB = loop1MBB;
  BuildMI(BB, DL, TII->get(LL), Dest).addReg(Ptr).addImm(0);
  BuildMI(BB, DL, TII->get(BNE))
    .addReg(Dest).addReg(OldVal).addMBB(exitMBB);

  // loop2MBB:
  //   sc success, newval, 0(ptr)
  //   beq success, $0, loop1MBB
  BB = loop2MBB;
  BuildMI(BB, DL, TII->get(SC), Success)
    .addReg(NewVal).addReg(Ptr).addImm(0);
  BuildMI(BB, DL, TII->get(BEQ))
    .addReg(Success).addReg(ZERO).addMBB(loop1MBB);

  MI->eraseFromParent(); // The instruction is gone now.

  return exitMBB;
}

MachineBasicBlock *
ZCPUTargetLowering::emitAtomicCmpSwapPartword(MachineInstr *MI,
                                              MachineBasicBlock *BB,
                                              unsigned Size) const {
  assert((Size == 1 || Size == 2) &&
      "Unsupported size for EmitAtomicCmpSwapPartial.");

  MachineFunction *MF = BB->getParent();
  MachineRegisterInfo &RegInfo = MF->getRegInfo();
  const TargetRegisterClass *RC = getRegClassFor(MVT::i32);
  const TargetInstrInfo *TII = Subtarget.getInstrInfo();
  DebugLoc DL = MI->getDebugLoc();

  unsigned Dest    = MI->getOperand(0).getReg();
  unsigned Ptr     = MI->getOperand(1).getReg();
  unsigned CmpVal  = MI->getOperand(2).getReg();
  unsigned NewVal  = MI->getOperand(3).getReg();

  unsigned AlignedAddr = RegInfo.createVirtualRegister(RC);
  unsigned ShiftAmt = RegInfo.createVirtualRegister(RC);
  unsigned Mask = RegInfo.createVirtualRegister(RC);
  unsigned Mask2 = RegInfo.createVirtualRegister(RC);
  unsigned Mask3 = RegInfo.createVirtualRegister(RC);
  unsigned ShiftedCmpVal = RegInfo.createVirtualRegister(RC);
  unsigned OldVal = RegInfo.createVirtualRegister(RC);
  unsigned MaskedOldVal0 = RegInfo.createVirtualRegister(RC);
  unsigned ShiftedNewVal = RegInfo.createVirtualRegister(RC);
  unsigned MaskLSB2 = RegInfo.createVirtualRegister(RC);
  unsigned PtrLSB2 = RegInfo.createVirtualRegister(RC);
  unsigned MaskUpper = RegInfo.createVirtualRegister(RC);
  unsigned MaskedCmpVal = RegInfo.createVirtualRegister(RC);
  unsigned MaskedNewVal = RegInfo.createVirtualRegister(RC);
  unsigned MaskedOldVal1 = RegInfo.createVirtualRegister(RC);
  unsigned StoreVal = RegInfo.createVirtualRegister(RC);
  unsigned SrlRes = RegInfo.createVirtualRegister(RC);
  unsigned Success = RegInfo.createVirtualRegister(RC);

  // insert new blocks after the current block
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineBasicBlock *loop1MBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *loop2MBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *sinkMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *exitMBB = MF->CreateMachineBasicBlock(LLVM_BB);
  MachineFunction::iterator It = BB;
  ++It;
  MF->insert(It, loop1MBB);
  MF->insert(It, loop2MBB);
  MF->insert(It, sinkMBB);
  MF->insert(It, exitMBB);

  // Transfer the remainder of BB and its successor edges to exitMBB.
  exitMBB->splice(exitMBB->begin(), BB,
                  std::next(MachineBasicBlock::iterator(MI)), BB->end());
  exitMBB->transferSuccessorsAndUpdatePHIs(BB);

  BB->addSuccessor(loop1MBB);
  loop1MBB->addSuccessor(sinkMBB);
  loop1MBB->addSuccessor(loop2MBB);
  loop2MBB->addSuccessor(loop1MBB);
  loop2MBB->addSuccessor(sinkMBB);
  sinkMBB->addSuccessor(exitMBB);

  // FIXME: computation of newval2 can be moved to loop2MBB.
  //  thisMBB:
  //    addiu   masklsb2,$0,-4                # 0xfffffffc
  //    and     alignedaddr,ptr,masklsb2
  //    andi    ptrlsb2,ptr,3
  //    shl     shiftamt,ptrlsb2,3
  //    ori     maskupper,$0,255               # 0xff
  //    shl     mask,maskupper,shiftamt
  //    xor     mask2,$0,mask
  //    xor     mask3,$0,mask2
  //    andi    maskedcmpval,cmpval,255
  //    shl     shiftedcmpval,maskedcmpval,shiftamt
  //    andi    maskednewval,newval,255
  //    shl     shiftednewval,maskednewval,shiftamt
  int64_t MaskImm = (Size == 1) ? 255 : 65535;
  BuildMI(BB, DL, TII->get(ZCPU::ADDiu), MaskLSB2)
    .addReg(ZCPU::ZERO).addImm(-4);
  BuildMI(BB, DL, TII->get(ZCPU::AND), AlignedAddr)
    .addReg(Ptr).addReg(MaskLSB2);
  BuildMI(BB, DL, TII->get(ZCPU::ANDi), PtrLSB2).addReg(Ptr).addImm(3);
  if (Subtarget.isLittle()) {
    BuildMI(BB, DL, TII->get(ZCPU::SHL), ShiftAmt).addReg(PtrLSB2).addImm(3);
  } else {
    unsigned Off = RegInfo.createVirtualRegister(RC);
    BuildMI(BB, DL, TII->get(ZCPU::XORi), Off)
      .addReg(PtrLSB2).addImm((Size == 1) ? 3 : 2);
    BuildMI(BB, DL, TII->get(ZCPU::SHL), ShiftAmt).addReg(Off).addImm(3);
  }
  BuildMI(BB, DL, TII->get(ZCPU::ORi), MaskUpper)
    .addReg(ZCPU::ZERO).addImm(MaskImm);
  BuildMI(BB, DL, TII->get(ZCPU::SHLV), Mask)
    .addReg(MaskUpper).addReg(ShiftAmt);
  BuildMI(BB, DL, TII->get(ZCPU::XOR), Mask2).addReg(ZCPU::ZERO).addReg(Mask);
  BuildMI(BB, DL, TII->get(ZCPU::XOR), Mask3).addReg(ZCPU::ZERO).addReg(Mask2);
  BuildMI(BB, DL, TII->get(ZCPU::ANDi), MaskedCmpVal)
    .addReg(CmpVal).addImm(MaskImm);
  BuildMI(BB, DL, TII->get(ZCPU::SHLV), ShiftedCmpVal)
    .addReg(MaskedCmpVal).addReg(ShiftAmt);
  BuildMI(BB, DL, TII->get(ZCPU::ANDi), MaskedNewVal)
    .addReg(NewVal).addImm(MaskImm);
  BuildMI(BB, DL, TII->get(ZCPU::SHLV), ShiftedNewVal)
    .addReg(MaskedNewVal).addReg(ShiftAmt);

  //  loop1MBB:
  //    ll      oldval,0(alginedaddr)
  //    and     maskedoldval0,oldval,mask
  //    bne     maskedoldval0,shiftedcmpval,sinkMBB
  BB = loop1MBB;
  unsigned LL = ZCPU::LL;
  BuildMI(BB, DL, TII->get(LL), OldVal).addReg(AlignedAddr).addImm(0);
  BuildMI(BB, DL, TII->get(ZCPU::AND), MaskedOldVal0)
    .addReg(OldVal).addReg(Mask);
  BuildMI(BB, DL, TII->get(ZCPU::BNE))
    .addReg(MaskedOldVal0).addReg(ShiftedCmpVal).addMBB(sinkMBB);

  //  loop2MBB:
  //    and     maskedoldval1,oldval,mask3
  //    or      storeval,maskedoldval1,shiftednewval
  //    sc      success,storeval,0(alignedaddr)
  //    beq     success,$0,loop1MBB
  BB = loop2MBB;
  BuildMI(BB, DL, TII->get(ZCPU::AND), MaskedOldVal1)
    .addReg(OldVal).addReg(Mask3);
  BuildMI(BB, DL, TII->get(ZCPU::OR), StoreVal)
    .addReg(MaskedOldVal1).addReg(ShiftedNewVal);
  unsigned SC = ZCPU::SC;
  BuildMI(BB, DL, TII->get(SC), Success)
      .addReg(StoreVal).addReg(AlignedAddr).addImm(0);
  BuildMI(BB, DL, TII->get(ZCPU::BEQ))
      .addReg(Success).addReg(ZCPU::ZERO).addMBB(loop1MBB);

  //  sinkMBB:
  //    srl     srlres,maskedoldval0,shiftamt
  //    sign_extend dest,srlres
  BB = sinkMBB;

  BuildMI(BB, DL, TII->get(ZCPU::SHRV), SrlRes)
      .addReg(MaskedOldVal0).addReg(ShiftAmt);
  BB = emitSignExtendToI32InReg(MI, BB, Size, Dest, SrlRes);

  MI->eraseFromParent();   // The instruction is gone now.

  return exitMBB;
}
#endif

//===----------------------------------------------------------------------===//
//  Misc Lower Operation implementation
//===----------------------------------------------------------------------===//
#if CH >= CH8_1 //8
SDValue ZCPUTargetLowering::
lowerBRCOND(SDValue Op, SelectionDAG &DAG) const
{
  return Op;
}
#endif

#if CH >= CH8_2 //4
SDValue ZCPUTargetLowering::
lowerSELECT(SDValue Op, SelectionDAG &DAG) const
{
  return Op;
}
#endif

#if CH >= CH6_1 //4
SDValue ZCPUTargetLowering::lowerGlobalAddress(SDValue Op,
                                               SelectionDAG &DAG) const {
  //@lowerGlobalAddress }
  SDLoc DL(Op);
  const ZCPUTargetObjectFile *TLOF =
        static_cast<const ZCPUTargetObjectFile *>(
            getTargetMachine().getObjFileLowering());
  //@lga 1 {
  EVT Ty = Op.getValueType();
  GlobalAddressSDNode *N = cast<GlobalAddressSDNode>(Op);
  const GlobalValue *GV = N->getGlobal();
  //@lga 1 }

  if (getTargetMachine().getRelocationModel() != Reloc::PIC_) {
    //@ %gp_rel relocation
    if (TLOF->IsGlobalInSmallSection(GV, getTargetMachine())) {
      SDValue GA = DAG.getTargetGlobalAddress(GV, DL, MVT::i32, 0,
                                              ZCPUII::MO_GPREL);
      SDValue GPRelNode = DAG.getNode(ZCPUISD::GPRel, DL,
                                      DAG.getVTList(MVT::i32), GA);
      SDValue GPReg = DAG.getRegister(ZCPU::GP, MVT::i32);
      return DAG.getNode(ISD::ADD, DL, MVT::i32, GPReg, GPRelNode);
    }

    //@ %hi/%lo relocation
    return getAddrNonPIC(N, Ty, DAG);
  }

  if (GV->hasInternalLinkage() || (GV->hasLocalLinkage() && !isa<Function>(GV)))
    return getAddrLocal(N, Ty, DAG);

  //@large section
  if (!TLOF->IsGlobalInSmallSection(GV, getTargetMachine()))
    return getAddrGlobalLargeGOT(N, Ty, DAG, ZCPUII::MO_GOT_HI16,
                                 ZCPUII::MO_GOT_LO16, DAG.getEntryNode(), 
                                 MachinePointerInfo::getGOT());
  return getAddrGlobal(N, Ty, DAG, ZCPUII::MO_GOT16, DAG.getEntryNode(), 
                       MachinePointerInfo::getGOT());
}
#endif

#if CH >= CH12_1 //4
SDValue ZCPUTargetLowering::
lowerGlobalTLSAddress(SDValue Op, SelectionDAG &DAG) const
{
  // If the relocation model is PIC, use the General Dynamic TLS Model or
  // Local Dynamic TLS model, otherwise use the Initial Exec or
  // Local Exec TLS Model.

  GlobalAddressSDNode *GA = cast<GlobalAddressSDNode>(Op);
  SDLoc DL(GA);
  const GlobalValue *GV = GA->getGlobal();
  EVT PtrVT = getPointerTy(DAG.getDataLayout());

  TLSModel::Model model = getTargetMachine().getTLSModel(GV);

  if (model == TLSModel::GeneralDynamic || model == TLSModel::LocalDynamic) {
    // General Dynamic and Local Dynamic TLS Model.
    unsigned Flag = (model == TLSModel::LocalDynamic) ? ZCPUII::MO_TLSLDM
                                                      : ZCPUII::MO_TLSGD;

    SDValue TGA = DAG.getTargetGlobalAddress(GV, DL, PtrVT, 0, Flag);
    SDValue Argument = DAG.getNode(ZCPUISD::Wrapper, DL, PtrVT,
                                   getGlobalReg(DAG, PtrVT), TGA);
    unsigned PtrSize = PtrVT.getSizeInBits();
    IntegerType *PtrTy = Type::getIntNTy(*DAG.getContext(), PtrSize);

    SDValue TlsGetAddr = DAG.getExternalSymbol("__tls_get_addr", PtrVT);

    ArgListTy Args;
    ArgListEntry Entry;
    Entry.Node = Argument;
    Entry.Ty = PtrTy;
    Args.push_back(Entry);

    TargetLowering::CallLoweringInfo CLI(DAG);
    CLI.setDebugLoc(DL).setChain(DAG.getEntryNode())
      .setCallee(CallingConv::C, PtrTy, TlsGetAddr, std::move(Args), 0);
    std::pair<SDValue, SDValue> CallResult = LowerCallTo(CLI);

    SDValue Ret = CallResult.first;

    if (model != TLSModel::LocalDynamic)
      return Ret;

    SDValue TGAHi = DAG.getTargetGlobalAddress(GV, DL, PtrVT, 0,
                                               ZCPUII::MO_DTP_HI);
    SDValue Hi = DAG.getNode(ZCPUISD::Hi, DL, PtrVT, TGAHi);
    SDValue TGALo = DAG.getTargetGlobalAddress(GV, DL, PtrVT, 0,
                                               ZCPUII::MO_DTP_LO);
    SDValue Lo = DAG.getNode(ZCPUISD::Lo, DL, PtrVT, TGALo);
    SDValue Add = DAG.getNode(ISD::ADD, DL, PtrVT, Hi, Ret);
    return DAG.getNode(ISD::ADD, DL, PtrVT, Add, Lo);
  }

  SDValue Offset;
  if (model == TLSModel::InitialExec) {
    // Initial Exec TLS Model
    SDValue TGA = DAG.getTargetGlobalAddress(GV, DL, PtrVT, 0,
                                             ZCPUII::MO_GOTTPREL);
    TGA = DAG.getNode(ZCPUISD::Wrapper, DL, PtrVT, getGlobalReg(DAG, PtrVT),
                      TGA);
    Offset = DAG.getLoad(PtrVT, DL,
                         DAG.getEntryNode(), TGA, MachinePointerInfo(),
                         false, false, false, 0);
  } else {
    // Local Exec TLS Model
    assert(model == TLSModel::LocalExec);
    SDValue TGAHi = DAG.getTargetGlobalAddress(GV, DL, PtrVT, 0,
                                               ZCPUII::MO_TP_HI);
    SDValue TGALo = DAG.getTargetGlobalAddress(GV, DL, PtrVT, 0,
                                               ZCPUII::MO_TP_LO);
    SDValue Hi = DAG.getNode(ZCPUISD::Hi, DL, PtrVT, TGAHi);
    SDValue Lo = DAG.getNode(ZCPUISD::Lo, DL, PtrVT, TGALo);
    Offset = DAG.getNode(ISD::ADD, DL, PtrVT, Hi, Lo);
  }
  return Offset;
}
#endif

#if CH >= CH8_1 //9
SDValue ZCPUTargetLowering::lowerBlockAddress(SDValue Op,
                                              SelectionDAG &DAG) const {
  BlockAddressSDNode *N = cast<BlockAddressSDNode>(Op);
  EVT Ty = Op.getValueType();

  if (getTargetMachine().getRelocationModel() != Reloc::PIC_)
    return getAddrNonPIC(N, Ty, DAG);

  return getAddrLocal(N, Ty, DAG);
}

SDValue ZCPUTargetLowering::
lowerJumpTable(SDValue Op, SelectionDAG &DAG) const
{
  JumpTableSDNode *N = cast<JumpTableSDNode>(Op);
  EVT Ty = Op.getValueType();

  if (getTargetMachine().getRelocationModel() != Reloc::PIC_)
    return getAddrNonPIC(N, Ty, DAG);

  return getAddrLocal(N, Ty, DAG);
}
#endif

#if CH >= CH9_3 //5
SDValue ZCPUTargetLowering::lowerVASTART(SDValue Op, SelectionDAG &DAG) const {
  MachineFunction &MF = DAG.getMachineFunction();
  ZCPUFunctionInfo *FuncInfo = MF.getInfo<ZCPUFunctionInfo>();

  SDLoc DL = SDLoc(Op);
  SDValue FI = DAG.getFrameIndex(FuncInfo->getVarArgsFrameIndex(),
                                 getPointerTy(MF.getDataLayout()));

  // vastart just stores the address of the VarArgsFrameIndex slot into the
  // memory location argument.
  const Value *SV = cast<SrcValueSDNode>(Op.getOperand(2))->getValue();
  return DAG.getStore(Op.getOperand(0), DL, FI, Op.getOperand(1),
                      MachinePointerInfo(SV), false, false, 0);
}
#endif

#if CH >= CH9_3 //5.5
SDValue ZCPUTargetLowering::
lowerFRAMEADDR(SDValue Op, SelectionDAG &DAG) const {
  // check the depth
  assert((cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue() == 0) &&
         "Frame address can only be determined for current frame.");

  MachineFrameInfo *MFI = DAG.getMachineFunction().getFrameInfo();
  MFI->setFrameAddressIsTaken(true);
  EVT VT = Op.getValueType();
  SDLoc DL(Op);
  SDValue FrameAddr = DAG.getCopyFromReg(
      DAG.getEntryNode(), DL, ZCPU::FP, VT);
  return FrameAddr;
}

SDValue ZCPUTargetLowering::lowerRETURNADDR(SDValue Op,
                                            SelectionDAG &DAG) const {
  if (verifyReturnAddressArgumentIsConstant(Op, DAG))
    return SDValue();

  // check the depth
  assert((cast<ConstantSDNode>(Op.getOperand(0))->getZExtValue() == 0) &&
         "Return address can be determined only for current frame.");

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  MVT VT = Op.getSimpleValueType();
  unsigned LR = ZCPU::LR;
  MFI->setReturnAddressIsTaken(true);

  // Return LR, which contains the return address. Mark it an implicit live-in.
  unsigned Reg = MF.addLiveIn(LR, getRegClassFor(VT));
  return DAG.getCopyFromReg(DAG.getEntryNode(), SDLoc(Op), Reg, VT);
}

// An EH_RETURN is the result of lowering llvm.eh.return which in turn is
// generated from __builtin_eh_return (offset, handler)
// The effect of this is to adjust the stack pointer by "offset"
// and then branch to "handler".
SDValue ZCPUTargetLowering::lowerEH_RETURN(SDValue Op, SelectionDAG &DAG)
                                                                     const {
  MachineFunction &MF = DAG.getMachineFunction();
  ZCPUFunctionInfo *ZCPUFI = MF.getInfo<ZCPUFunctionInfo>();

  ZCPUFI->setCallsEhReturn();
  SDValue Chain     = Op.getOperand(0);
  SDValue Offset    = Op.getOperand(1);
  SDValue Handler   = Op.getOperand(2);
  SDLoc DL(Op);
  EVT Ty = MVT::i32;

  // Store stack offset in V1, store jump target in V0. Glue CopyToReg and
  // EH_RETURN nodes, so that instructions are emitted back-to-back.
  unsigned OffsetReg = ZCPU::V1;
  unsigned AddrReg = ZCPU::V0;
  Chain = DAG.getCopyToReg(Chain, DL, OffsetReg, Offset, SDValue());
  Chain = DAG.getCopyToReg(Chain, DL, AddrReg, Handler, Chain.getValue(1));
  return DAG.getNode(ZCPUISD::EH_RETURN, DL, MVT::Other, Chain,
                     DAG.getRegister(OffsetReg, Ty),
                     DAG.getRegister(AddrReg, getPointerTy(MF.getDataLayout())),
                     Chain.getValue(1));
}

SDValue ZCPUTargetLowering::lowerADD(SDValue Op, SelectionDAG &DAG) const {
  if (Op->getOperand(0).getOpcode() != ISD::FRAMEADDR
      || cast<ConstantSDNode>
        (Op->getOperand(0).getOperand(0))->getZExtValue() != 0
      || Op->getOperand(1).getOpcode() != ISD::FRAME_TO_ARGS_OFFSET)
    return SDValue();

  MachineFunction &MF = DAG.getMachineFunction();
  ZCPUFunctionInfo *ZCPUFI = MF.getInfo<ZCPUFunctionInfo>();

  ZCPUFI->setCallsEhDwarf();
  return Op;
}
#endif

#if CH >= CH12_1 //9
SDValue ZCPUTargetLowering::lowerATOMIC_FENCE(SDValue Op,
                                              SelectionDAG &DAG) const {
  // FIXME: Need pseudo-fence for 'singlethread' fences
  // FIXME: Set SType for weaker fences where supported/appropriate.
  unsigned SType = 0;
  SDLoc DL(Op);
  return DAG.getNode(ZCPUISD::Sync, DL, MVT::Other, Op.getOperand(0),
                     DAG.getConstant(SType, DL, MVT::i32));
}
#endif

#if CH >= CH9_1 //3
//===----------------------------------------------------------------------===//
// TODO: Implement a generic logic using tblgen that can support this.
// ZCPU 32 ABI rules:
// ---
//===----------------------------------------------------------------------===//

// Passed in stack only.
static bool CC_ZCPUS32(unsigned ValNo, MVT ValVT, MVT LocVT,
                       CCValAssign::LocInfo LocInfo, ISD::ArgFlagsTy ArgFlags,
                       CCState &State) {
  // Do not process byval args here.
  if (ArgFlags.isByVal())
    return true;

  // Promote i8 and i16
  if (LocVT == MVT::i8 || LocVT == MVT::i16) {
    LocVT = MVT::i32;
    if (ArgFlags.isSExt())
      LocInfo = CCValAssign::SExt;
    else if (ArgFlags.isZExt())
      LocInfo = CCValAssign::ZExt;
    else
      LocInfo = CCValAssign::AExt;
  }

  unsigned OrigAlign = ArgFlags.getOrigAlign();
  unsigned Offset = State.AllocateStack(ValVT.getSizeInBits() >> 3,
                                        OrigAlign);
  State.addLoc(CCValAssign::getMem(ValNo, ValVT, Offset, LocVT, LocInfo));
  return false;
}

// Passed first two i32 arguments in registers and others in stack.
static bool CC_ZCPUO32(unsigned ValNo, MVT ValVT, MVT LocVT,
                       CCValAssign::LocInfo LocInfo, ISD::ArgFlagsTy ArgFlags,
                       CCState &State) {
  static const MCPhysReg IntRegs[] = { ZCPU::A0, ZCPU::A1 };

  // Do not process byval args here.
  if (ArgFlags.isByVal())
    return true;

  // Promote i8 and i16
  if (LocVT == MVT::i8 || LocVT == MVT::i16) {
    LocVT = MVT::i32;
    if (ArgFlags.isSExt())
      LocInfo = CCValAssign::SExt;
    else if (ArgFlags.isZExt())
      LocInfo = CCValAssign::ZExt;
    else
      LocInfo = CCValAssign::AExt;
  }

  unsigned Reg;

  // f32 and f64 are allocated in A0, A1 when either of the following
  // is true: function is vararg, argument is 3rd or higher, there is previous
  // argument which is not f32 or f64.
  bool AllocateFloatsInIntReg = true;
  unsigned OrigAlign = ArgFlags.getOrigAlign();
  bool isI64 = (ValVT == MVT::i32 && OrigAlign == 8);

  if (ValVT == MVT::i32 || (ValVT == MVT::f32 && AllocateFloatsInIntReg)) {
    Reg = State.AllocateReg(IntRegs);
    // If this is the first part of an i64 arg,
    // the allocated register must be A0.
    if (isI64 && (Reg == ZCPU::A1))
      Reg = State.AllocateReg(IntRegs);
    LocVT = MVT::i32;
  } else if (ValVT == MVT::f64 && AllocateFloatsInIntReg) {
    // Allocate int register. If first
    // available register is ZCPU::A1, shadow it too.
    Reg = State.AllocateReg(IntRegs);
    if (Reg == ZCPU::A1)
      Reg = State.AllocateReg(IntRegs);
    State.AllocateReg(IntRegs);
    LocVT = MVT::i32;
  } else
    llvm_unreachable("Cannot handle this ValVT.");

  if (!Reg) {
    unsigned Offset = State.AllocateStack(ValVT.getSizeInBits() >> 3,
                                          OrigAlign);
    State.addLoc(CCValAssign::getMem(ValNo, ValVT, Offset, LocVT, LocInfo));
  } else
    State.addLoc(CCValAssign::getReg(ValNo, ValVT, Reg, LocVT, LocInfo));

  return false;
}
#endif // #if CH >= CH9_1

#include "ZCPUGenCallingConv.inc"

#if CH >= CH9_1 //4
//===----------------------------------------------------------------------===//
//                  Call Calling Convention Implementation
//===----------------------------------------------------------------------===//

static const MCPhysReg O32IntRegs[] = {
  ZCPU::A0, ZCPU::A1
};
#endif // #if CH >= CH9_1

#if CH >= CH9_2 //1
SDValue
ZCPUTargetLowering::passArgOnStack(SDValue StackPtr, unsigned Offset,
                                   SDValue Chain, SDValue Arg, SDLoc DL,
                                   bool IsTailCall, SelectionDAG &DAG) const {
  if (!IsTailCall) {
    SDValue PtrOff =
        DAG.getNode(ISD::ADD, DL, getPointerTy(DAG.getDataLayout()), StackPtr,
                    DAG.getIntPtrConstant(Offset, DL));
    return DAG.getStore(Chain, DL, Arg, PtrOff, MachinePointerInfo(), false,
                        false, 0);
  }

  MachineFrameInfo *MFI = DAG.getMachineFunction().getFrameInfo();
  int FI = MFI->CreateFixedObject(Arg.getValueSizeInBits() / 8, Offset, false);
  SDValue FIN = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
  return DAG.getStore(Chain, DL, Arg, FIN, MachinePointerInfo(),
                      /*isVolatile=*/ true, false, 0);
}

void ZCPUTargetLowering::
getOpndList(SmallVectorImpl<SDValue> &Ops,
            std::deque< std::pair<unsigned, SDValue> > &RegsToPass,
            bool IsPICCall, bool GlobalOrExternal, bool InternalLinkage,
            CallLoweringInfo &CLI, SDValue Callee, SDValue Chain) const {
  // T9 should contain the address of the callee function if
  // -reloction-model=pic or it is an indirect call.
  if (IsPICCall || !GlobalOrExternal) {
    unsigned T9Reg = ZCPU::T9;
    RegsToPass.push_front(std::make_pair(T9Reg, Callee));
  } else
    Ops.push_back(Callee);

  // Insert node "GP copy globalreg" before call to function.
  //
  // R_ZCPU_CALL* operators (emitted when non-internal functions are called
  // in PIC mode) allow symbols to be resolved via lazy binding.
  // The lazy binding stub requires GP to point to the GOT.
  if (IsPICCall && !InternalLinkage) {
    unsigned GPReg = ZCPU::GP;
    EVT Ty = MVT::i32;
    RegsToPass.push_back(std::make_pair(GPReg, getGlobalReg(CLI.DAG, Ty)));
  }

  // Build a sequence of copy-to-reg nodes chained together with token
  // chain and flag operands which copy the outgoing args into registers.
  // The InFlag in necessary since all emitted instructions must be
  // stuck together.
  SDValue InFlag;

  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i) {
    Chain = CLI.DAG.getCopyToReg(Chain, CLI.DL, RegsToPass[i].first,
                                 RegsToPass[i].second, InFlag);
    InFlag = Chain.getValue(1);
  }

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (unsigned i = 0, e = RegsToPass.size(); i != e; ++i)
    Ops.push_back(CLI.DAG.getRegister(RegsToPass[i].first,
                                      RegsToPass[i].second.getValueType()));

  // Add a register mask operand representing the call-preserved registers.
  const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
  const uint32_t *Mask = 
      TRI->getCallPreservedMask(CLI.DAG.getMachineFunction(), CLI.CallConv);
  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(CLI.DAG.getRegisterMask(Mask));

  if (InFlag.getNode())
    Ops.push_back(InFlag);
}
#endif // #if CH >= CH9_2 //1

#if CH >= CH9_1 //5
//@LowerCall {
/// LowerCall - functions arguments are copied from virtual regs to
/// (physical regs)/(stack frame), CALLSEQ_START and CALLSEQ_END are emitted.
SDValue
ZCPUTargetLowering::LowerCall(TargetLowering::CallLoweringInfo &CLI,
                              SmallVectorImpl<SDValue> &InVals) const {
#if CH >= CH9_2 //2
  SelectionDAG &DAG                     = CLI.DAG;
  SDLoc DL                              = CLI.DL;
  SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
  SmallVectorImpl<SDValue> &OutVals     = CLI.OutVals;
  SmallVectorImpl<ISD::InputArg> &Ins   = CLI.Ins;
  SDValue Chain                         = CLI.Chain;
  SDValue Callee                        = CLI.Callee;
  bool &IsTailCall                      = CLI.IsTailCall;
  CallingConv::ID CallConv              = CLI.CallConv;
  bool IsVarArg                         = CLI.IsVarArg;

  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  const TargetFrameLowering *TFL = MF.getSubtarget().getFrameLowering();
  ZCPUFunctionInfo *FuncInfo = MF.getInfo<ZCPUFunctionInfo>();
  bool IsPIC = getTargetMachine().getRelocationModel() == Reloc::PIC_;
  ZCPUFunctionInfo *ZCPUFI = MF.getInfo<ZCPUFunctionInfo>();

  // Analyze operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(),
                 ArgLocs, *DAG.getContext());
  ZCPUCC::SpecialCallingConvType SpecialCallingConv =
    getSpecialCallingConv(Callee);
  ZCPUCC ZCPUCCInfo(CallConv, ABI.IsO32(), 
                    CCInfo, SpecialCallingConv);

  ZCPUCCInfo.analyzeCallOperands(Outs, IsVarArg,
                                 Subtarget.abiUsesSoftFloat(),
                                 Callee.getNode(), CLI.getArgs());

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NextStackOffset = CCInfo.getNextStackOffset();

#if CH >= CH9_3 //6
#ifdef ENABLE_GPRESTORE
  if (!ZCPUReserveGP) {
    // If this is the first call, create a stack frame object that points to
    // a location to which .cprestore saves $gp.
    if (IsPIC && ZCPUFI->globalBaseRegFixed() && !ZCPUFI->getGPFI())
      ZCPUFI->setGPFI(MFI->CreateFixedObject(4, 0, true));
    if (ZCPUFI->needGPSaveRestore())
      MFI->setObjectOffset(ZCPUFI->getGPFI(), NextStackOffset);
  }
#endif
#endif //#if CH >= CH9_3 //6

  //@TailCall 1 {
  // Check if it's really possible to do a tail call.
  if (IsTailCall)
    IsTailCall =
      isEligibleForTailCallOptimization(ZCPUCCInfo, NextStackOffset,
                                        *MF.getInfo<ZCPUFunctionInfo>());

  if (!IsTailCall && CLI.CS && CLI.CS->isMustTailCall())
    report_fatal_error("failed to perform tail call elimination on a call "
                       "site marked musttail");

  if (IsTailCall)
    ++NumTailCalls;
  //@TailCall 1 }

  // Chain is the output chain of the last Load/Store or CopyToReg node.
  // ByValChain is the output chain of the last Memcpy node created for copying
  // byval arguments to the stack.
  unsigned StackAlignment = TFL->getStackAlignment();
  NextStackOffset = RoundUpToAlignment(NextStackOffset, StackAlignment);
  SDValue NextStackOffsetVal = DAG.getIntPtrConstant(NextStackOffset, DL, true);

  //@TailCall 2 {
  if (!IsTailCall)
    Chain = DAG.getCALLSEQ_START(Chain, NextStackOffsetVal, DL);
  //@TailCall 2 }

  SDValue StackPtr =
      DAG.getCopyFromReg(Chain, DL, ZCPU::SP,
                         getPointerTy(DAG.getDataLayout()));

  // With EABI is it possible to have 16 args on registers.
  std::deque< std::pair<unsigned, SDValue> > RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  ZCPUCC::byval_iterator ByValArg = ZCPUCCInfo.byval_begin();

  //@1 {
  // Walk the register/memloc assignments, inserting copies/loads.
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
  //@1 }
    SDValue Arg = OutVals[i];
    CCValAssign &VA = ArgLocs[i];
    MVT LocVT = VA.getLocVT();
    ISD::ArgFlagsTy Flags = Outs[i].Flags;

    //@ByVal Arg {
    if (Flags.isByVal()) {
      assert(Flags.getByValSize() &&
             "ByVal args of size 0 should have been ignored by front-end.");
      assert(ByValArg != ZCPUCCInfo.byval_end());
      assert(!IsTailCall &&
             "Do not tail-call optimize if there is a byval argument.");
      passByValArg(Chain, DL, RegsToPass, MemOpChains, StackPtr, MFI, DAG, Arg,
                   ZCPUCCInfo, *ByValArg, Flags, Subtarget.isLittle());
      ++ByValArg;
      continue;
    }
    //@ByVal Arg }

    // Promote the value if needed.
    switch (VA.getLocInfo()) {
    default: llvm_unreachable("Unknown loc info!");
    case CCValAssign::Full:
      break;
    case CCValAssign::SExt:
      Arg = DAG.getNode(ISD::SIGN_EXTEND, DL, LocVT, Arg);
      break;
    case CCValAssign::ZExt:
      Arg = DAG.getNode(ISD::ZERO_EXTEND, DL, LocVT, Arg);
      break;
    case CCValAssign::AExt:
      Arg = DAG.getNode(ISD::ANY_EXTEND, DL, LocVT, Arg);
      break;
    }

    // Arguments that can be passed on register must be kept at
    // RegsToPass vector
    if (VA.isRegLoc()) {
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), Arg));
      continue;
    }

    // Register can't get to this point...
    assert(VA.isMemLoc());

    // emit ISD::STORE whichs stores the
    // parameter value to a stack Location
    MemOpChains.push_back(passArgOnStack(StackPtr, VA.getLocMemOffset(),
                                         Chain, Arg, DL, IsTailCall, DAG));
  }

  // Transform all store nodes into one single node because all store
  // nodes are independent of each other.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);

  // If the callee is a GlobalAddress/ExternalSymbol node (quite common, every
  // direct call is) turn it into a TargetGlobalAddress/TargetExternalSymbol
  // node so that legalize doesn't hack it.
  bool IsPICCall = IsPIC; // true if calls are translated to
                                         // jalr $t9
  bool GlobalOrExternal = false, InternalLinkage = false;
  SDValue CalleeLo;
  EVT Ty = Callee.getValueType();

  if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee)) {
    if (IsPICCall) {
      const GlobalValue *Val = G->getGlobal();
      InternalLinkage = Val->hasInternalLinkage();

      if (InternalLinkage)
        Callee = getAddrLocal(G, Ty, DAG);
      else
        Callee = getAddrGlobal(G, Ty, DAG, ZCPUII::MO_GOT_CALL, Chain,
                               FuncInfo->callPtrInfo(Val));
    } else
      Callee = DAG.getTargetGlobalAddress(G->getGlobal(), DL,
                                          getPointerTy(DAG.getDataLayout()), 0,
                                          ZCPUII::MO_NO_FLAG);
    GlobalOrExternal = true;
  }
  else if (ExternalSymbolSDNode *S = dyn_cast<ExternalSymbolSDNode>(Callee)) {
    const char *Sym = S->getSymbol();

    if (!IsPIC) // static
      Callee = DAG.getTargetExternalSymbol(Sym,
                                           getPointerTy(DAG.getDataLayout()),
                                           ZCPUII::MO_NO_FLAG);
    else // PIC
      Callee = getAddrGlobal(S, Ty, DAG, ZCPUII::MO_GOT_CALL, Chain,
                             FuncInfo->callPtrInfo(Sym));

    GlobalOrExternal = true;
  }

  SmallVector<SDValue, 8> Ops(1, Chain);
  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);

  getOpndList(Ops, RegsToPass, IsPICCall, GlobalOrExternal, InternalLinkage,
              CLI, Callee, Chain);

  //@TailCall 3 {
  if (IsTailCall)
    return DAG.getNode(ZCPUISD::TailCall, DL, MVT::Other, Ops);
  //@TailCall 3 }

  Chain = DAG.getNode(ZCPUISD::JmpLink, DL, NodeTys, Ops);
  SDValue InFlag = Chain.getValue(1);

  // Create the CALLSEQ_END node.
  Chain = DAG.getCALLSEQ_END(Chain, NextStackOffsetVal,
                             DAG.getIntPtrConstant(0, DL, true), InFlag, DL);
  InFlag = Chain.getValue(1);

  // Handle result values, copying them out of physregs into vregs that we
  // return.
  return LowerCallResult(Chain, InFlag, CallConv, IsVarArg,
                         Ins, DL, DAG, InVals, CLI.Callee.getNode(), CLI.RetTy);
#else // CH >= CH9_2 //2
  return CLI.Chain;
#endif
}
//@LowerCall }
#endif // #if CH >= CH9_1

#if CH >= CH9_2 //3
/// LowerCallResult - Lower the result values of a call into the
/// appropriate copies out of appropriate physical registers.
SDValue
ZCPUTargetLowering::LowerCallResult(SDValue Chain, SDValue InFlag,
                                    CallingConv::ID CallConv, bool IsVarArg,
                                    const SmallVectorImpl<ISD::InputArg> &Ins,
                                    SDLoc DL, SelectionDAG &DAG,
                                    SmallVectorImpl<SDValue> &InVals,
                                    const SDNode *CallNode,
                                    const Type *RetTy) const {
  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(),
		 RVLocs, *DAG.getContext());
		 
  ZCPUCC ZCPUCCInfo(CallConv, ABI.IsO32(), CCInfo);

  ZCPUCCInfo.analyzeCallResult(Ins, Subtarget.abiUsesSoftFloat(),
                               CallNode, RetTy);

  // Copy all of the result registers out of their specified physreg.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    SDValue Val = DAG.getCopyFromReg(Chain, DL, RVLocs[i].getLocReg(),
                                     RVLocs[i].getLocVT(), InFlag);
    Chain = Val.getValue(1);
    InFlag = Val.getValue(2);

    if (RVLocs[i].getValVT() != RVLocs[i].getLocVT())
      Val = DAG.getNode(ISD::BITCAST, DL, RVLocs[i].getValVT(), Val);

    InVals.push_back(Val);
  }

  return Chain;
}
#endif

//===----------------------------------------------------------------------===//
//@            Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

#if CH >= CH3_1
//@LowerFormalArguments {
/// LowerFormalArguments - transform physical registers into virtual registers
/// and generate load operations for arguments places on the stack.
SDValue
ZCPUTargetLowering::LowerFormalArguments(SDValue Chain,
                                         CallingConv::ID CallConv,
                                         bool IsVarArg,
                                      const SmallVectorImpl<ISD::InputArg> &Ins,
                                         SDLoc DL, SelectionDAG &DAG,
                                         SmallVectorImpl<SDValue> &InVals)
                                          const {
#if CH >= CH3_4
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  ZCPUFunctionInfo *ZCPUFI = MF.getInfo<ZCPUFunctionInfo>();

  ZCPUFI->setVarArgsFrameIndex(0);

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(),
                 ArgLocs, *DAG.getContext());
  ZCPUCC ZCPUCCInfo(CallConv, ABI.IsO32(), 
                    CCInfo);
  ZCPUFI->setFormalArgInfo(CCInfo.getNextStackOffset(),
                           ZCPUCCInfo.hasByValArg());
#endif

#if CH >= CH9_1 //6
  Function::const_arg_iterator FuncArg =
    DAG.getMachineFunction().getFunction()->arg_begin();
  bool UseSoftFloat = Subtarget.abiUsesSoftFloat();

  ZCPUCCInfo.analyzeFormalArguments(Ins, UseSoftFloat, FuncArg);

  // Used with vargs to acumulate store chains.
  std::vector<SDValue> OutChains;

  unsigned CurArgIdx = 0;
  ZCPUCC::byval_iterator ByValArg = ZCPUCCInfo.byval_begin();

  //@2 {
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
  //@2 }
    CCValAssign &VA = ArgLocs[i];
    std::advance(FuncArg, Ins[i].OrigArgIndex - CurArgIdx);
    CurArgIdx = Ins[i].OrigArgIndex;
    EVT ValVT = VA.getValVT();
    ISD::ArgFlagsTy Flags = Ins[i].Flags;
    bool IsRegLoc = VA.isRegLoc();

    //@byval pass {
    if (Flags.isByVal()) {
      assert(Flags.getByValSize() &&
             "ByVal args of size 0 should have been ignored by front-end.");
      assert(ByValArg != ZCPUCCInfo.byval_end());
      copyByValRegs(Chain, DL, OutChains, DAG, Flags, InVals, &*FuncArg,
                    ZCPUCCInfo, *ByValArg);
      ++ByValArg;
      continue;
    }
    //@byval pass }
    // Arguments stored on registers
    if (ABI.IsO32() && IsRegLoc) {
      MVT RegVT = VA.getLocVT();
      unsigned ArgReg = VA.getLocReg();
      const TargetRegisterClass *RC = getRegClassFor(RegVT);

      // Transform the arguments stored on
      // physical registers into virtual ones
      unsigned Reg = addLiveIn(DAG.getMachineFunction(), ArgReg, RC);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, RegVT);

      // If this is an 8 or 16-bit value, it has been passed promoted
      // to 32 bits.  Insert an assert[sz]ext to capture this, then
      // truncate to the right size.
      if (VA.getLocInfo() != CCValAssign::Full) {
        unsigned Opcode = 0;
        if (VA.getLocInfo() == CCValAssign::SExt)
          Opcode = ISD::AssertSext;
        else if (VA.getLocInfo() == CCValAssign::ZExt)
          Opcode = ISD::AssertZext;
        if (Opcode)
          ArgValue = DAG.getNode(Opcode, DL, RegVT, ArgValue,
                                 DAG.getValueType(ValVT));
        ArgValue = DAG.getNode(ISD::TRUNCATE, DL, ValVT, ArgValue);
      }

      // Handle floating point arguments passed in integer registers.
      if ((RegVT == MVT::i32 && ValVT == MVT::f32) ||
          (RegVT == MVT::i64 && ValVT == MVT::f64))
        ArgValue = DAG.getNode(ISD::BITCAST, DL, ValVT, ArgValue);
      InVals.push_back(ArgValue);
    } else { // VA.isRegLoc()

      // sanity check
      assert(VA.isMemLoc());

      // The stack pointer offset is relative to the caller stack frame.
      int FI = MFI->CreateFixedObject(ValVT.getSizeInBits()/8,
                                      VA.getLocMemOffset(), true);

      // Create load nodes to retrieve arguments from the stack
      SDValue FIN = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
      SDValue Load = DAG.getLoad(ValVT, DL, Chain, FIN,
                                 MachinePointerInfo::getFixedStack(FI),
                                 false, false, false, 0);
      InVals.push_back(Load);
      OutChains.push_back(Load.getValue(1));
    }
  }

//@Ordinary struct type: 1 {
  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    // The cpu0 ABIs for returning structs by value requires that we copy
    // the sret argument into $v0 for the return. Save the argument into
    // a virtual register so that we can access it from the return points.
    if (Ins[i].Flags.isSRet()) {
      unsigned Reg = ZCPUFI->getSRetReturnReg();
      if (!Reg) {
        Reg = MF.getRegInfo().createVirtualRegister(
            getRegClassFor(MVT::i32));
        ZCPUFI->setSRetReturnReg(Reg);
      }
      SDValue Copy = DAG.getCopyToReg(DAG.getEntryNode(), DL, Reg, InVals[i]);
      Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, Copy, Chain);
      break;
    }
  }
//@Ordinary struct type: 1 }

#if CH >= CH9_3 //7
  if (IsVarArg)
    writeVarArgRegs(OutChains, ZCPUCCInfo, Chain, DL, DAG);
#endif

  // All stores are grouped in one node to allow the matching between
  // the size of Ins and InVals. This only happens when on varg functions
  if (!OutChains.empty()) {
    OutChains.push_back(Chain);
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, OutChains);
  }
#endif // #if CH >= CH9_1

  return Chain;
}
// @LowerFormalArguments }
#endif // #if CH >= CH3_1

//===----------------------------------------------------------------------===//
//@              Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

#if CH >= CH9_2 //4
bool
ZCPUTargetLowering::CanLowerReturn(CallingConv::ID CallConv,
                                   MachineFunction &MF, bool IsVarArg,
                                   const SmallVectorImpl<ISD::OutputArg> &Outs,
                                   LLVMContext &Context) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, MF,
                 RVLocs, Context);
  return CCInfo.CheckReturn(Outs, RetCC_ZCPU);
}
#endif

#if CH >= CH3_1 //LowerReturn
SDValue
ZCPUTargetLowering::LowerReturn(SDValue Chain,
                                CallingConv::ID CallConv, bool IsVarArg,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                const SmallVectorImpl<SDValue> &OutVals,
                                SDLoc DL, SelectionDAG &DAG) const {
#if CH >= CH3_4 //in LowerReturn
  // CCValAssign - represent the assignment of
  // the return value to a location
  SmallVector<CCValAssign, 16> RVLocs;
  MachineFunction &MF = DAG.getMachineFunction();

  // CCState - Info about the registers and stack slot.
  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs,
                 *DAG.getContext());
  ZCPUCC ZCPUCCInfo(CallConv, ABI.IsO32(), 
                    CCInfo);

  // Analyze return values.
  ZCPUCCInfo.analyzeReturn(Outs, Subtarget.abiUsesSoftFloat(),
                           MF.getFunction()->getReturnType());

  SDValue Flag;
  SmallVector<SDValue, 4> RetOps(1, Chain);

  // Copy the result values into the output registers.
  for (unsigned i = 0; i != RVLocs.size(); ++i) {
    SDValue Val = OutVals[i];
    CCValAssign &VA = RVLocs[i];
    assert(VA.isRegLoc() && "Can only return in registers!");

    if (RVLocs[i].getValVT() != RVLocs[i].getLocVT())
      Val = DAG.getNode(ISD::BITCAST, DL, RVLocs[i].getLocVT(), Val);

    Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Flag);

    // Guarantee that all emitted copies are stuck together with flags.
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
  }

//@Ordinary struct type: 2 {
  // The cpu0 ABIs for returning structs by value requires that we copy
  // the sret argument into $v0 for the return. We saved the argument into
  // a virtual register in the entry block, so now we copy the value out
  // and into $v0.
  if (MF.getFunction()->hasStructRetAttr()) {
    ZCPUFunctionInfo *ZCPUFI = MF.getInfo<ZCPUFunctionInfo>();
    unsigned Reg = ZCPUFI->getSRetReturnReg();

    if (!Reg)
      llvm_unreachable("sret virtual register not created in the entry block");
    SDValue Val =
        DAG.getCopyFromReg(Chain, DL, Reg, getPointerTy(DAG.getDataLayout()));
    unsigned V0 = ZCPU::V0;

    Chain = DAG.getCopyToReg(Chain, DL, V0, Val, Flag);
    Flag = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(V0, getPointerTy(DAG.getDataLayout())));
  }
//@Ordinary struct type: 2 }

  RetOps[0] = Chain;  // Update chain.

  // Add the flag if we have it.
  if (Flag.getNode())
    RetOps.push_back(Flag);

  // Return on ZCPU is always a "ret $lr"
  return DAG.getNode(ZCPUISD::Ret, DL, MVT::Other, RetOps);
#else // #if CH >= CH3_4
  return DAG.getNode(ZCPUISD::Ret, DL, MVT::Other,
                     Chain, DAG.getRegister(ZCPU::LR, MVT::i32));
#endif
}
#endif // #if CH >= CH3_1

#if CH >= CH11_2
//===----------------------------------------------------------------------===//
//                           ZCPU Inline Assembly Support
//===----------------------------------------------------------------------===//

/// getConstraintType - Given a constraint letter, return the type of
/// constraint it is for this target.
ZCPUTargetLowering::ConstraintType 
ZCPUTargetLowering::getConstraintType(StringRef Constraint) const
{
  // ZCPU specific constraints
  // GCC config/mips/constraints.md
  // 'c' : A register suitable for use in an indirect
  //       jump. This will always be $t9 for -mabicalls.
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
      default : break;
      case 'c':
        return C_RegisterClass;
      case 'R':
        return C_Memory;
    }
  }
  return TargetLowering::getConstraintType(Constraint);
}

/// Examine constraint type and operand type and determine a weight value.
/// This object must already have been set up with the operand type
/// and the current alternative constraint selected.
TargetLowering::ConstraintWeight
ZCPUTargetLowering::getSingleConstraintMatchWeight(
    AsmOperandInfo &info, const char *constraint) const {
  ConstraintWeight weight = CW_Invalid;
  Value *CallOperandVal = info.CallOperandVal;
    // If we don't have a value, we can't do a match,
    // but allow it at the lowest weight.
  if (!CallOperandVal)
    return CW_Default;
  Type *type = CallOperandVal->getType();
  // Look at the constraint type.
  switch (*constraint) {
  default:
    weight = TargetLowering::getSingleConstraintMatchWeight(info, constraint);
    break;
  case 'c': // $t9 for indirect jumps
    if (type->isIntegerTy())
      weight = CW_SpecificReg;
    break;
  case 'I': // signed 16 bit immediate
  case 'J': // integer zero
  case 'K': // unsigned 16 bit immediate
  case 'L': // signed 32 bit immediate where lower 16 bits are 0
  case 'N': // immediate in the range of -65535 to -1 (inclusive)
  case 'O': // signed 15 bit immediate (+- 16383)
  case 'P': // immediate in the range of 65535 to 1 (inclusive)
    if (isa<ConstantInt>(CallOperandVal))
      weight = CW_Constant;
    break;
  case 'R':
    weight = CW_Memory;
    break;
  }
  return weight;
}

/// This is a helper function to parse a physical register string and split it
/// into non-numeric and numeric parts (Prefix and Reg). The first boolean flag
/// that is returned indicates whether parsing was successful. The second flag
/// is true if the numeric part exists.
static std::pair<bool, bool>
parsePhysicalReg(const StringRef &C, std::string &Prefix,
                 unsigned long long &Reg) {
  if (C.front() != '{' || C.back() != '}')
    return std::make_pair(false, false);

  // Search for the first numeric character.
  StringRef::const_iterator I, B = C.begin() + 1, E = C.end() - 1;
  I = std::find_if(B, E, std::ptr_fun(isdigit));

  Prefix.assign(B, I - B);

  // The second flag is set to false if no numeric characters were found.
  if (I == E)
    return std::make_pair(true, false);

  // Parse the numeric characters.
  return std::make_pair(!getAsUnsignedInteger(StringRef(I, E - I), 10, Reg),
                        true);
}

std::pair<unsigned, const TargetRegisterClass *> ZCPUTargetLowering::
parseRegForInlineAsmConstraint(const StringRef &C, MVT VT) const {
  const TargetRegisterClass *RC;
  std::string Prefix;
  unsigned long long Reg;

  std::pair<bool, bool> R = parsePhysicalReg(C, Prefix, Reg);

  if (!R.first)
    return std::make_pair(0U, nullptr);
  if (!R.second)
    return std::make_pair(0U, nullptr);

 // Parse $0-$15.
  assert(Prefix == "$");
  RC = getRegClassFor((VT == MVT::Other) ? MVT::i32 : VT);

  assert(Reg < RC->getNumRegs());
  return std::make_pair(*(RC->begin() + Reg), RC);
}

/// Given a register class constraint, like 'r', if this corresponds directly
/// to an LLVM register class, return a register of 0 and the register class
/// pointer.
std::pair<unsigned, const TargetRegisterClass *>
ZCPUTargetLowering::getRegForInlineAsmConstraint(const TargetRegisterInfo *TRI,
                                                 StringRef Constraint,
                                                 MVT VT) const
{
  if (Constraint.size() == 1) {
    switch (Constraint[0]) {
    case 'r':
      if (VT == MVT::i32 || VT == MVT::i16 || VT == MVT::i8) {
        return std::make_pair(0U, &ZCPU::CPURegsRegClass);
      }
      if (VT == MVT::i64)
        return std::make_pair(0U, &ZCPU::CPURegsRegClass);
      // This will generate an error message
      return std::make_pair(0u, static_cast<const TargetRegisterClass*>(0));
    case 'c': // register suitable for indirect jump
      if (VT == MVT::i32)
        return std::make_pair((unsigned)ZCPU::T9, &ZCPU::CPURegsRegClass);
      assert("Unexpected type.");
    }
  }

  std::pair<unsigned, const TargetRegisterClass *> R;
  R = parseRegForInlineAsmConstraint(Constraint, VT);

  if (R.second)
    return R;

  return TargetLowering::getRegForInlineAsmConstraint(TRI, Constraint, VT);
}

/// LowerAsmOperandForConstraint - Lower the specified operand into the Ops
/// vector.  If it is invalid, don't add anything to Ops.
void ZCPUTargetLowering::LowerAsmOperandForConstraint(SDValue Op,
                                                     std::string &Constraint,
                                                     std::vector<SDValue>&Ops,
                                                     SelectionDAG &DAG) const {
  SDLoc DL(Op);
  SDValue Result;

  // Only support length 1 constraints for now.
  if (Constraint.length() > 1) return;

  char ConstraintLetter = Constraint[0];
  switch (ConstraintLetter) {
  default: break; // This will fall through to the generic implementation
  case 'I': // Signed 16 bit constant
    // If this fails, the parent routine will give an error
    if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
      EVT Type = Op.getValueType();
      int64_t Val = C->getSExtValue();
      if (isInt<16>(Val)) {
        Result = DAG.getTargetConstant(Val, DL, Type);
        break;
      }
    }
    return;
  case 'J': // integer zero
    if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
      EVT Type = Op.getValueType();
      int64_t Val = C->getZExtValue();
      if (Val == 0) {
        Result = DAG.getTargetConstant(0, DL, Type);
        break;
      }
    }
    return;
  case 'K': // unsigned 16 bit immediate
    if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
      EVT Type = Op.getValueType();
      uint64_t Val = (uint64_t)C->getZExtValue();
      if (isUInt<16>(Val)) {
        Result = DAG.getTargetConstant(Val, DL, Type);
        break;
      }
    }
    return;
  case 'L': // signed 32 bit immediate where lower 16 bits are 0
    if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
      EVT Type = Op.getValueType();
      int64_t Val = C->getSExtValue();
      if ((isInt<32>(Val)) && ((Val & 0xffff) == 0)){
        Result = DAG.getTargetConstant(Val, DL, Type);
        break;
      }
    }
    return;
  case 'N': // immediate in the range of -65535 to -1 (inclusive)
    if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
      EVT Type = Op.getValueType();
      int64_t Val = C->getSExtValue();
      if ((Val >= -65535) && (Val <= -1)) {
        Result = DAG.getTargetConstant(Val, DL, Type);
        break;
      }
    }
    return;
  case 'O': // signed 15 bit immediate
    if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
      EVT Type = Op.getValueType();
      int64_t Val = C->getSExtValue();
      if ((isInt<15>(Val))) {
        Result = DAG.getTargetConstant(Val, DL, Type);
        break;
      }
    }
    return;
  case 'P': // immediate in the range of 1 to 65535 (inclusive)
    if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
      EVT Type = Op.getValueType();
      int64_t Val = C->getSExtValue();
      if ((Val <= 65535) && (Val >= 1)) {
        Result = DAG.getTargetConstant(Val, DL, Type);
        break;
      }
    }
    return;
  }

  if (Result.getNode()) {
    Ops.push_back(Result);
    return;
  }

  TargetLowering::LowerAsmOperandForConstraint(Op, Constraint, Ops, DAG);
}

bool ZCPUTargetLowering::isLegalAddressingMode(const DataLayout &DL,
                                               const AddrMode &AM, Type *Ty,
                                               unsigned AS) const {
  // No global is ever allowed as a base.
  if (AM.BaseGV)
    return false;

  switch (AM.Scale) {
  case 0: // "r+i" or just "i", depending on HasBaseReg.
    break;
  case 1:
    if (!AM.HasBaseReg) // allow "r+i".
      break;
    return false; // disallow "r+r" or "r+r+i".
  default:
    return false;
  }

  return true;
}
#endif // #if CH >= CH11_2

#if CH >= CH7_1 //4
bool
ZCPUTargetLowering::isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const {
  // The ZCPU target isn't yet aware of offsets.
  return false;
}
#endif

#if CH >= CH9_2 //5
ZCPUTargetLowering::ZCPUCC::SpecialCallingConvType
  ZCPUTargetLowering::getSpecialCallingConv(SDValue Callee) const {
  ZCPUCC::SpecialCallingConvType SpecialCallingConv =
    ZCPUCC::NoSpecialCallingConv;
  return SpecialCallingConv;
}
#endif

#if CH >= CH3_4
ZCPUTargetLowering::ZCPUCC::ZCPUCC(
  CallingConv::ID CC, bool IsO32_, CCState &Info,
  ZCPUCC::SpecialCallingConvType SpecialCallingConv_)
  : CCInfo(Info), CallConv(CC), IsO32(IsO32_) {
  // Pre-allocate reserved argument area.
  CCInfo.AllocateStack(reservedArgArea(), 1);
}
#endif

#if CH >= CH9_2 //6
//@#if CH >= CH9_2 //6 {
void ZCPUTargetLowering::ZCPUCC::
analyzeCallOperands(const SmallVectorImpl<ISD::OutputArg> &Args,
                    bool IsVarArg, bool IsSoftFloat, const SDNode *CallNode,
                    std::vector<ArgListEntry> &FuncArgs) {
//@analyzeCallOperands body {
  assert((CallConv != CallingConv::Fast || !IsVarArg) &&
         "CallingConv::Fast shouldn't be used for vararg functions.");

  unsigned NumOpnds = Args.size();
  llvm::CCAssignFn *FixedFn = fixedArgFn();
#if CH >= CH9_3 //8
  llvm::CCAssignFn *VarFn = varArgFn();
#endif

  //@3 {
  for (unsigned I = 0; I != NumOpnds; ++I) {
  //@3 }
    MVT ArgVT = Args[I].VT;
    ISD::ArgFlagsTy ArgFlags = Args[I].Flags;
    bool R;

    if (ArgFlags.isByVal()) {
      handleByValArg(I, ArgVT, ArgVT, CCValAssign::Full, ArgFlags);
      continue;
    }

#if CH >= CH9_3 //9
    if (IsVarArg && !Args[I].IsFixed)
      R = VarFn(I, ArgVT, ArgVT, CCValAssign::Full, ArgFlags, CCInfo);
    else
#endif
    {
      MVT RegVT = getRegVT(ArgVT, FuncArgs[Args[I].OrigArgIndex].Ty, CallNode,
                           IsSoftFloat);
      R = FixedFn(I, ArgVT, RegVT, CCValAssign::Full, ArgFlags, CCInfo);
    }

    if (R) {
#ifndef NDEBUG
      dbgs() << "Call operand #" << I << " has unhandled type "
             << EVT(ArgVT).getEVTString();
#endif
      llvm_unreachable(nullptr);
    }
  }
}
//@#if CH >= CH9_2 //6 }
#endif

#if CH >= CH9_1 //7
void ZCPUTargetLowering::ZCPUCC::
analyzeFormalArguments(const SmallVectorImpl<ISD::InputArg> &Args,
                       bool IsSoftFloat, Function::const_arg_iterator FuncArg) {
  unsigned NumArgs = Args.size();
  llvm::CCAssignFn *FixedFn = fixedArgFn();
  unsigned CurArgIdx = 0;

  for (unsigned I = 0; I != NumArgs; ++I) {
    MVT ArgVT = Args[I].VT;
    ISD::ArgFlagsTy ArgFlags = Args[I].Flags;
    std::advance(FuncArg, Args[I].OrigArgIndex - CurArgIdx);
    CurArgIdx = Args[I].OrigArgIndex;

    if (ArgFlags.isByVal()) {
      handleByValArg(I, ArgVT, ArgVT, CCValAssign::Full, ArgFlags);
      continue;
    }

    MVT RegVT = getRegVT(ArgVT, FuncArg->getType(), nullptr, IsSoftFloat);

    if (!FixedFn(I, ArgVT, RegVT, CCValAssign::Full, ArgFlags, CCInfo))
      continue;

#ifndef NDEBUG
    dbgs() << "Formal Arg #" << I << " has unhandled type "
           << EVT(ArgVT).getEVTString();
#endif
    llvm_unreachable(nullptr);
  }
}
#endif // #if CH >= CH9_1

#if CH >= CH3_4 //analyzeReturn
template<typename Ty>
void ZCPUTargetLowering::ZCPUCC::
analyzeReturn(const SmallVectorImpl<Ty> &RetVals, bool IsSoftFloat,
              const SDNode *CallNode, const Type *RetTy) const {
  CCAssignFn *Fn;

  Fn = RetCC_ZCPU;

  for (unsigned I = 0, E = RetVals.size(); I < E; ++I) {
    MVT VT = RetVals[I].VT;
    ISD::ArgFlagsTy Flags = RetVals[I].Flags;
    MVT RegVT = this->getRegVT(VT, RetTy, CallNode, IsSoftFloat);

    if (Fn(I, VT, RegVT, CCValAssign::Full, Flags, this->CCInfo)) {
#ifndef NDEBUG
      dbgs() << "Call result #" << I << " has unhandled type "
             << EVT(VT).getEVTString() << '\n';
#endif
      llvm_unreachable(nullptr);
    }
  }
}

void ZCPUTargetLowering::ZCPUCC::
analyzeCallResult(const SmallVectorImpl<ISD::InputArg> &Ins, bool IsSoftFloat,
                  const SDNode *CallNode, const Type *RetTy) const {
  analyzeReturn(Ins, IsSoftFloat, CallNode, RetTy);
}

void ZCPUTargetLowering::ZCPUCC::
analyzeReturn(const SmallVectorImpl<ISD::OutputArg> &Outs, bool IsSoftFloat,
              const Type *RetTy) const {
  analyzeReturn(Outs, IsSoftFloat, nullptr, RetTy);
}
#endif // #if CH >= CH3_4 //analyzeReturn

#if CH >= CH9_1 //8
void ZCPUTargetLowering::ZCPUCC::handleByValArg(unsigned ValNo, MVT ValVT,
                                                MVT LocVT,
                                                CCValAssign::LocInfo LocInfo,
                                                ISD::ArgFlagsTy ArgFlags) {
  assert(ArgFlags.getByValSize() && "Byval argument's size shouldn't be 0.");

  struct ByValArgInfo ByVal;
  unsigned RegSize = regSize();
  unsigned ByValSize = RoundUpToAlignment(ArgFlags.getByValSize(), RegSize);
  unsigned Align = std::min(std::max(ArgFlags.getByValAlign(), RegSize),
                            RegSize * 2);

  if (useRegsForByval())
    allocateRegs(ByVal, ByValSize, Align);

  // Allocate space on caller's stack.
  ByVal.Address = CCInfo.AllocateStack(ByValSize - RegSize * ByVal.NumRegs,
                                       Align);
  CCInfo.addLoc(CCValAssign::getMem(ValNo, ValVT, ByVal.Address, LocVT,
                                    LocInfo));
  ByValArgs.push_back(ByVal);
}

unsigned ZCPUTargetLowering::ZCPUCC::numIntArgRegs() const {
  return IsO32 ? array_lengthof(O32IntRegs) : 0;
}
#endif // #if CH >= CH9_1

#if CH >= CH3_4 //reservedArgArea
unsigned ZCPUTargetLowering::ZCPUCC::reservedArgArea() const {
  return (IsO32 && (CallConv != CallingConv::Fast)) ? 8 : 0;
}
#endif

#if CH >= CH9_1 //9
const ArrayRef<MCPhysReg> ZCPUTargetLowering::ZCPUCC::intArgRegs() const {
  return makeArrayRef(O32IntRegs);
}

llvm::CCAssignFn *ZCPUTargetLowering::ZCPUCC::fixedArgFn() const {
  if (IsO32)
    return CC_ZCPUO32;
  else // IsS32
    return CC_ZCPUS32;
}
#endif // #if CH >= CH9_1

#if CH >= CH9_3 //10
llvm::CCAssignFn *ZCPUTargetLowering::ZCPUCC::varArgFn() const {
  if (IsO32)
    return CC_ZCPUO32;
  else // IsS32
    return CC_ZCPUS32;
}
#endif

#if CH >= CH9_1 //10
void ZCPUTargetLowering::ZCPUCC::allocateRegs(ByValArgInfo &ByVal,
                                              unsigned ByValSize,
                                              unsigned Align) {
  unsigned RegSize = regSize(), NumIntArgRegs = numIntArgRegs();
  const ArrayRef<MCPhysReg> IntArgRegs = intArgRegs();
  assert(!(ByValSize % RegSize) && !(Align % RegSize) &&
         "Byval argument's size and alignment should be a multiple of"
         "RegSize.");

  ByVal.FirstIdx = CCInfo.getFirstUnallocated(IntArgRegs);

  // If Align > RegSize, the first arg register must be even.
  if ((Align > RegSize) && (ByVal.FirstIdx % 2)) {
    CCInfo.AllocateReg(IntArgRegs[ByVal.FirstIdx]);
    ++ByVal.FirstIdx;
  }

  // Mark the registers allocated.
  for (unsigned I = ByVal.FirstIdx; ByValSize && (I < NumIntArgRegs);
       ByValSize -= RegSize, ++I, ++ByVal.NumRegs)
    CCInfo.AllocateReg(IntArgRegs[I]);
}
#endif

#if CH >= CH3_4 //getRegVT
MVT ZCPUTargetLowering::ZCPUCC::getRegVT(MVT VT, const Type *OrigTy,
                                         const SDNode *CallNode,
                                         bool IsSoftFloat) const {
  if (IsSoftFloat || IsO32)
    return VT;

  return VT;
}
#endif

#if CH >= CH9_1 //11
void ZCPUTargetLowering::
copyByValRegs(SDValue Chain, SDLoc DL, std::vector<SDValue> &OutChains,
              SelectionDAG &DAG, const ISD::ArgFlagsTy &Flags,
              SmallVectorImpl<SDValue> &InVals, const Argument *FuncArg,
              const ZCPUCC &CC, const ByValArgInfo &ByVal) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  unsigned RegAreaSize = ByVal.NumRegs * CC.regSize();
  unsigned FrameObjSize = std::max(Flags.getByValSize(), RegAreaSize);
  int FrameObjOffset;

  const ArrayRef<MCPhysReg> ByValArgRegs = CC.intArgRegs();

  if (RegAreaSize)
    FrameObjOffset = (int)CC.reservedArgArea() -
      (int)((CC.numIntArgRegs() - ByVal.FirstIdx) * CC.regSize());
  else
    FrameObjOffset = ByVal.Address;

  // Create frame object.
  EVT PtrTy = getPointerTy(DAG.getDataLayout());
  int FI = MFI->CreateFixedObject(FrameObjSize, FrameObjOffset, true);
  SDValue FIN = DAG.getFrameIndex(FI, PtrTy);
  InVals.push_back(FIN);

  if (!ByVal.NumRegs)
    return;

  // Copy arg registers.
  MVT RegTy = MVT::getIntegerVT(CC.regSize() * 8);
  const TargetRegisterClass *RC = getRegClassFor(RegTy);

  for (unsigned I = 0; I < ByVal.NumRegs; ++I) {
    unsigned ArgReg = ByValArgRegs[ByVal.FirstIdx + I];
    unsigned VReg = addLiveIn(MF, ArgReg, RC);
    unsigned Offset = I * CC.regSize();
    SDValue StorePtr = DAG.getNode(ISD::ADD, DL, PtrTy, FIN,
                                   DAG.getConstant(Offset, DL, PtrTy));
    SDValue Store = DAG.getStore(Chain, DL, DAG.getRegister(VReg, RegTy),
                                 StorePtr, MachinePointerInfo(FuncArg, Offset),
                                 false, false, 0);
    OutChains.push_back(Store);
  }
}
#endif

#if CH >= CH9_2 //7
// Copy byVal arg to registers and stack.
void ZCPUTargetLowering::
passByValArg(SDValue Chain, SDLoc DL,
             std::deque< std::pair<unsigned, SDValue> > &RegsToPass,
             SmallVectorImpl<SDValue> &MemOpChains, SDValue StackPtr,
             MachineFrameInfo *MFI, SelectionDAG &DAG, SDValue Arg,
             const ZCPUCC &CC, const ByValArgInfo &ByVal,
             const ISD::ArgFlagsTy &Flags, bool isLittle) const {
  unsigned ByValSizeInBytes = Flags.getByValSize();
  unsigned OffsetInBytes = 0; // From beginning of struct
  unsigned RegSizeInBytes = CC.regSize();
  unsigned Alignment = std::min(Flags.getByValAlign(), RegSizeInBytes);
  EVT PtrTy = getPointerTy(DAG.getDataLayout()),
      RegTy = MVT::getIntegerVT(RegSizeInBytes * 8);

  if (ByVal.NumRegs) {
    const ArrayRef<MCPhysReg> ArgRegs = CC.intArgRegs();
    bool LeftoverBytes = (ByVal.NumRegs * RegSizeInBytes > ByValSizeInBytes);
    unsigned I = 0;

    // Copy words to registers.
    for (; I < ByVal.NumRegs - LeftoverBytes;
         ++I, OffsetInBytes += RegSizeInBytes) {
      SDValue LoadPtr = DAG.getNode(ISD::ADD, DL, PtrTy, Arg,
                                    DAG.getConstant(OffsetInBytes, DL, PtrTy));
      SDValue LoadVal = DAG.getLoad(RegTy, DL, Chain, LoadPtr,
                                    MachinePointerInfo(), false, false, false,
                                    Alignment);
      MemOpChains.push_back(LoadVal.getValue(1));
      unsigned ArgReg = ArgRegs[ByVal.FirstIdx + I];
      RegsToPass.push_back(std::make_pair(ArgReg, LoadVal));
    }

    // Return if the struct has been fully copied.
    if (ByValSizeInBytes == OffsetInBytes)
      return;

    // Copy the remainder of the byval argument with sub-word loads and shifts.
    if (LeftoverBytes) {
      assert((ByValSizeInBytes > OffsetInBytes) &&
             (ByValSizeInBytes < OffsetInBytes + RegSizeInBytes) &&
             "Size of the remainder should be smaller than RegSizeInBytes.");
      SDValue Val;

      for (unsigned LoadSizeInBytes = RegSizeInBytes / 2, TotalBytesLoaded = 0;
           OffsetInBytes < ByValSizeInBytes; LoadSizeInBytes /= 2) {
        unsigned RemainingSizeInBytes = ByValSizeInBytes - OffsetInBytes;

        if (RemainingSizeInBytes < LoadSizeInBytes)
          continue;

        // Load subword.
        SDValue LoadPtr = DAG.getNode(ISD::ADD, DL, PtrTy, Arg,
                                      DAG.getConstant(OffsetInBytes, DL, PtrTy));
        SDValue LoadVal = DAG.getExtLoad(
            ISD::ZEXTLOAD, DL, RegTy, Chain, LoadPtr, MachinePointerInfo(),
            MVT::getIntegerVT(LoadSizeInBytes * 8), false, false, false,
            Alignment);
        MemOpChains.push_back(LoadVal.getValue(1));

        // Shift the loaded value.
        unsigned Shamt;

        if (isLittle)
          Shamt = TotalBytesLoaded * 8;
        else
          Shamt = (RegSizeInBytes - (TotalBytesLoaded + LoadSizeInBytes)) * 8;

        SDValue Shift = DAG.getNode(ISD::SHL, DL, RegTy, LoadVal,
                                    DAG.getConstant(Shamt, DL, MVT::i32));

        if (Val.getNode())
          Val = DAG.getNode(ISD::OR, DL, RegTy, Val, Shift);
        else
          Val = Shift;

        OffsetInBytes += LoadSizeInBytes;
        TotalBytesLoaded += LoadSizeInBytes;
        Alignment = std::min(Alignment, LoadSizeInBytes);
      }

      unsigned ArgReg = ArgRegs[ByVal.FirstIdx + I];
      RegsToPass.push_back(std::make_pair(ArgReg, Val));
      return;
    }
  }

  // Copy remainder of byval arg to it with memcpy.
  unsigned MemCpySize = ByValSizeInBytes - OffsetInBytes;
  SDValue Src = DAG.getNode(ISD::ADD, DL, PtrTy, Arg,
                            DAG.getConstant(OffsetInBytes, DL, PtrTy));
  SDValue Dst = DAG.getNode(ISD::ADD, DL, PtrTy, StackPtr,
                            DAG.getIntPtrConstant(ByVal.Address, DL));
  Chain = DAG.getMemcpy(Chain, DL, Dst, Src,
                        DAG.getConstant(MemCpySize, DL, PtrTy),
                        Alignment, /*isVolatile=*/false, /*AlwaysInline=*/false,
                        /*isTailCall=*/false,
                        MachinePointerInfo(), MachinePointerInfo());
  MemOpChains.push_back(Chain);
}
#endif // #if CH >= CH9_2 //7

#if CH >= CH9_3 //11
void ZCPUTargetLowering::writeVarArgRegs(std::vector<SDValue> &OutChains,
                                         const ZCPUCC &CC, SDValue Chain,
                                         SDLoc DL, SelectionDAG &DAG) const {
  unsigned NumRegs = CC.numIntArgRegs();
  const ArrayRef<MCPhysReg> ArgRegs = CC.intArgRegs();
  const CCState &CCInfo = CC.getCCInfo();
  unsigned Idx = CCInfo.getFirstUnallocated(ArgRegs);
  unsigned RegSize = CC.regSize();
  MVT RegTy = MVT::getIntegerVT(RegSize * 8);
  const TargetRegisterClass *RC = getRegClassFor(RegTy);
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo *MFI = MF.getFrameInfo();
  ZCPUFunctionInfo *ZCPUFI = MF.getInfo<ZCPUFunctionInfo>();

  // Offset of the first variable argument from stack pointer.
  int VaArgOffset;

  if (NumRegs == Idx)
    VaArgOffset = RoundUpToAlignment(CCInfo.getNextStackOffset(), RegSize);
  else
    VaArgOffset = (int)CC.reservedArgArea() - (int)(RegSize * (NumRegs - Idx));

  // Record the frame index of the first variable argument
  // which is a value necessary to VASTART.
  int FI = MFI->CreateFixedObject(RegSize, VaArgOffset, true);
  ZCPUFI->setVarArgsFrameIndex(FI);

  // Copy the integer registers that have not been used for argument passing
  // to the argument register save area. For O32, the save area is allocated
  // in the caller's stack frame, while for N32/64, it is allocated in the
  // callee's stack frame.
  for (unsigned I = Idx; I < NumRegs; ++I, VaArgOffset += RegSize) {
    unsigned Reg = addLiveIn(MF, ArgRegs[I], RC);
    SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, RegTy);
    FI = MFI->CreateFixedObject(RegSize, VaArgOffset, true);
    SDValue PtrOff = DAG.getFrameIndex(FI, getPointerTy(DAG.getDataLayout()));
    SDValue Store = DAG.getStore(Chain, DL, ArgValue, PtrOff,
                                 MachinePointerInfo(), false, false, 0);
    cast<StoreSDNode>(Store.getNode())->getMemOperand()->setValue(
        (Value *)nullptr);
    OutChains.push_back(Store);
  }
}
#endif // #if CH >= CH9_3

#endif // #if CH >= CH3_1
