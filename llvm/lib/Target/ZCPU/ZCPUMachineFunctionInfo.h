//===-- ZCPUMachineFuctionInfo.h - ZCPU machine function info -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares ZCPU-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef ZCPUMACHINEFUNCTIONINFO_H
#define ZCPUMACHINEFUNCTIONINFO_H

#include "llvm/CodeGen/MachineFunction.h"

namespace llvm {
  class ZCPUMachineFunctionInfo : public MachineFunctionInfo {
    virtual void anchor();

    // needFP - Flag which indicate to use FP
    bool needFP;

    // CalleeSavedFrameSize - Size of the callee-saved register portion of the
    // stack frame in bytes.
    unsigned CalleeSavedFrameSize;
  public:
    explicit ZCPUMachineFunctionInfo(MachineFunction &MF)
      : needFP(false), CalleeSavedFrameSize(0) {}

    unsigned getCalleeSavedFrameSize() { return CalleeSavedFrameSize; }
    void setCalleeSavedFrameSize(unsigned bytes) {
      CalleeSavedFrameSize = bytes;
    }
    bool isNeedFP() const { return needFP; }
    void setNeedFP() { needFP = true; }
  }; // end class ZCPUMachineFunctionInfo
} // end namespace llvm

#endif
