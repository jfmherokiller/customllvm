//===-- ZCPUSERegisterInfo.cpp - ZCPU Register Information ------== -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the ZCPU implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "ZCPUSERegisterInfo.h"
#if CH >= CH3_1

using namespace llvm;

#define DEBUG_TYPE "cpu0-reg-info"

ZCPUSERegisterInfo::ZCPUSERegisterInfo(const ZCPUSubtarget &ST)
  : ZCPURegisterInfo(ST) {}

const TargetRegisterClass *
ZCPUSERegisterInfo::intRegClass(unsigned Size) const {
  return &ZCPU::CPURegsRegClass;
}

#endif
