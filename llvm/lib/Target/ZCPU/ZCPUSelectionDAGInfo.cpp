//===-- ZCPUSelectionDAGInfo.cpp - ZCPU SelectionDAG Info -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the ZCPUSelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#include "ZCPUTargetMachine.h"
using namespace llvm;

ZCPUSelectionDAGInfo::ZCPUSelectionDAGInfo(const ZCPUTargetMachine &tm)
  : TargetSelectionDAGInfo(tm)
{
}

ZCPUSelectionDAGInfo::~ZCPUSelectionDAGInfo()
{
}
