//===-- ZCPUSEISelDAGToDAG.h - A Dag to Dag Inst Selector for ZCPUSE -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of ZCPUDAGToDAGISel specialized for cpu032.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUSEISELDAGTODAG_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUSEISELDAGTODAG_H

#include "ZCPUConfig.h"
#if CH >= CH3_3

#include "ZCPUISelDAGToDAG.h"

namespace llvm {

class ZCPUSEDAGToDAGISel : public ZCPUDAGToDAGISel {

public:
  explicit ZCPUSEDAGToDAGISel(ZCPUTargetMachine &TM) : ZCPUDAGToDAGISel(TM) {}

private:

  bool runOnMachineFunction(MachineFunction &MF) override;

  std::pair<bool, SDNode*> selectNode(SDNode *Node) override;

  void processFunctionAfterISel(MachineFunction &MF) override;

  // Insert instructions to initialize the global base register in the
  // first MBB of the function.
//  void initGlobalBaseReg(MachineFunction &MF);

#if CH >= CH4_1
  std::pair<SDNode*, SDNode*> SelectMULT(SDNode *N, unsigned Opc, SDLoc DL,
                                         EVT Ty, bool HasLo, bool HasHi);
#endif

#if CH >= CH7_1
  SDNode *selectAddESubE(unsigned MOp, SDValue InFlag, SDValue CmpLHS,
                         SDLoc DL, SDNode *Node) const;
#endif
};

FunctionPass *createZCPUSEISelDag(ZCPUTargetMachine &TM);

}

#endif // #if CH >= CH3_3

#endif
