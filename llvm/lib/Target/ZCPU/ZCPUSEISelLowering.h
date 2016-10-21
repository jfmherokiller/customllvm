//===-- ZCPUISEISelLowering.h - ZCPUISE DAG Lowering Interface ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of ZCPUITargetLowering specialized for cpu032/64.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUSEISELLOWERING_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUSEISELLOWERING_H

#include "ZCPUConfig.h"
#if CH >= CH3_1

#include "ZCPUISelLowering.h"
#include "ZCPURegisterInfo.h"

namespace llvm {
  class ZCPUSETargetLowering : public ZCPUTargetLowering  {
  public:
    explicit ZCPUSETargetLowering(const ZCPUTargetMachine &TM,
                                  const ZCPUSubtarget &STI);

    SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;
  private:
#if CH >= CH9_1
    bool isEligibleForTailCallOptimization(const ZCPUCC &ZCPUCCInfo,
                                     unsigned NextStackOffset,
                                     const ZCPUFunctionInfo& FI) const override;
#endif
  };
}

#endif // #if CH >= CH3_1

#endif // ZCPUISEISELLOWERING_H
