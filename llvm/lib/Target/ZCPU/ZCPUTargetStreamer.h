//===-- ZCPUTargetStreamer.h - ZCPU Target Streamer ------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUTARGETSTREAMER_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUTARGETSTREAMER_H

#include "ZCPUConfig.h"
#if CH >= CH5_1

#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"

namespace llvm {

class ZCPUTargetStreamer : public MCTargetStreamer {
public:
  ZCPUTargetStreamer(MCStreamer &S);
};

// This part is for ascii assembly output
class ZCPUTargetAsmStreamer : public ZCPUTargetStreamer {
  formatted_raw_ostream &OS;

public:
  ZCPUTargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);
};

}

#endif // #if CH >= CH5_1
#endif
