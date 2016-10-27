//=- ZCPUSelectionDAGInfo.h - ZCPU SelectionDAG Info -*- C++ -*-//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file defines the ZCPU subclass for
/// SelectionDAGTargetInfo.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUSELECTIONDAGINFO_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUSELECTIONDAGINFO_H

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"

namespace llvm {

class ZCPUSelectionDAGInfo final : public SelectionDAGTargetInfo {
public:
  ~ZCPUSelectionDAGInfo() override;
};

} // end namespace llvm

#endif
