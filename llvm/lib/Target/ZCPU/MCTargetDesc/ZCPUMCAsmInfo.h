//===-- ZCPUMCAsmInfo.h - ZCPU asm properties --------------------*- C++ -*--===//
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

#ifndef ZCPUTARGETASMINFO_H
#define ZCPUTARGETASMINFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {
  class StringRef;
  class Target;

  class ZCPUMCAsmInfo : public MCAsmInfo {
  public:
    explicit ZCPUMCAsmInfo(StringRef TT);
  }; // end class ZCPUMCAsmInfo
} // end namespace llvm

#endif
