//===- ZCPUSubtarget.cpp - ZCPU Subtarget Information ---------------*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the ZCPU specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "ZCPUSubtarget.h"
#include "ZCPU.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "ZCPUGenSubtargetInfo.inc"

using namespace llvm;

ZCPUSubtarget::ZCPUSubtarget(const std::string &TT, const std::string &CPU,
  const std::string &FS)
  : ZCPUGenSubtargetInfo(TT, CPU, FS)
{}
