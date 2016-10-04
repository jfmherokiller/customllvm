//===-- ZCPUSelectionDAGInfo.h - ZCPU SelectionDAG Info -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the ZCPU subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef ZCPUSELECTIONDAGINFO_H
#define ZCPUSELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {
  class ZCPUSelectionDAGInfo : public TargetSelectionDAGInfo {
  public:
    explicit ZCPUSelectionDAGInfo(const ZCPUTargetMachine &tm);
    ~ZCPUSelectionDAGInfo();
  }; // end class ZCPUSelectionDAGInfo
} // end namespace llvm

#endif
