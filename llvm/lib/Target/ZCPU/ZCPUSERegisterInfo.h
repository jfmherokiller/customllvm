//===-- ZCPUSERegisterInfo.h - ZCPU32 Register Information ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the ZCPU32/64 implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUSEREGISTERINFO_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUSEREGISTERINFO_H

#include "ZCPUConfig.h"
#if CH >= CH3_1

#include "ZCPURegisterInfo.h"

namespace llvm {
class ZCPUSEInstrInfo;

class ZCPUSERegisterInfo : public ZCPURegisterInfo {
public:
  ZCPUSERegisterInfo(const ZCPUSubtarget &Subtarget);

  const TargetRegisterClass *intRegClass(unsigned Size) const override;
};

} // end namespace llvm

#endif // #if CH >= CH3_1

#endif
