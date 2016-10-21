//===-- ZCPUMCAsmInfo.h - ZCPU Asm Info ------------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the ZCPUMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUMCASMINFO_H
#define LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUMCASMINFO_H

#include "ZCPUConfig.h"
#if CH >= CH3_2

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
  class Triple;

  class ZCPUMCAsmInfo : public MCAsmInfoELF {
    void anchor() override;
  public:
    explicit ZCPUMCAsmInfo(const Triple &TheTriple);
  };

} // namespace llvm

#endif // #if CH >= CH3_2

#endif
