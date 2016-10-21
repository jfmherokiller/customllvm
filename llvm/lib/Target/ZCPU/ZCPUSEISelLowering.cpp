//===-- ZCPUSEISelLowering.cpp - ZCPUSE DAG Lowering Interface --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of ZCPUTargetLowering specialized for cpu032.
//
//===----------------------------------------------------------------------===//
#include "ZCPUMachineFunction.h"
#include "ZCPUSEISelLowering.h"
#if CH >= CH3_1

#include "ZCPURegisterInfo.h"
#include "ZCPUTargetMachine.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetInstrInfo.h"

using namespace llvm;

#define DEBUG_TYPE "cpu0-isel"

static cl::opt<bool>
EnableZCPUTailCalls("enable-cpu0-tail-calls", cl::Hidden,
                    cl::desc("ZCPU: Enable tail calls."), cl::init(false));

//@ZCPUSETargetLowering {
ZCPUSETargetLowering::ZCPUSETargetLowering(const ZCPUTargetMachine &TM,
                                           const ZCPUSubtarget &STI)
    : ZCPUTargetLowering(TM, STI) {
//@ZCPUSETargetLowering body {
  // Set up the register classes
  addRegisterClass(MVT::i32, &ZCPU::CPURegsRegClass);

#if CH >= CH12_1 //1
  setOperationAction(ISD::ATOMIC_FENCE,       MVT::Other, Custom);
#endif

// must, computeRegisterProperties - Once all of the register classes are 
//  added, this allows us to compute derived properties we expose.
  computeRegisterProperties(Subtarget.getRegisterInfo());
}

SDValue ZCPUSETargetLowering::LowerOperation(SDValue Op,
                                             SelectionDAG &DAG) const {

  return ZCPUTargetLowering::LowerOperation(Op, DAG);
}

const ZCPUTargetLowering *
llvm::createZCPUSETargetLowering(const ZCPUTargetMachine &TM,
                                 const ZCPUSubtarget &STI) {
  return new ZCPUSETargetLowering(TM, STI);
}

#if CH >= CH9_1
bool ZCPUSETargetLowering::
isEligibleForTailCallOptimization(const ZCPUCC &ZCPUCCInfo,
                                  unsigned NextStackOffset,
                                  const ZCPUFunctionInfo& FI) const {
  if (!EnableZCPUTailCalls)
    return false;

  // Return false if either the callee or caller has a byval argument.
  if (ZCPUCCInfo.hasByValArg() || FI.hasByvalArg())
    return false;

  // Return true if the callee's argument area is no larger than the
  // caller's.
  return NextStackOffset <= FI.getIncomingArgSize();
}
#endif

#endif // #if CH >= CH3_1
