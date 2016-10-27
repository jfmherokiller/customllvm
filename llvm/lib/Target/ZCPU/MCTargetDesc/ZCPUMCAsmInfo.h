//===-- ZCPUMCAsmInfo.h - ZCPU asm properties -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declaration of the ZCPUMCAsmInfo class.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUMCASMINFO_H
#define LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUMCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {

class Triple;

class ZCPUMCAsmInfo final : public MCAsmInfoELF {
public:
  explicit ZCPUMCAsmInfo(const Triple &T);
  ~ZCPUMCAsmInfo() override;
};

} // end namespace llvm

#endif
