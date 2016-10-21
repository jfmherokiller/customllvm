//===-- ZCPUTargetStreamer.cpp - ZCPU Target Streamer Methods -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides ZCPU specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "InstPrinter/ZCPUInstPrinter.h"
#include "ZCPUMCTargetDesc.h"
#include "ZCPUTargetObjectFile.h"
#include "ZCPUTargetStreamer.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"

#if CH >= CH5_1

using namespace llvm;

ZCPUTargetStreamer::ZCPUTargetStreamer(MCStreamer &S)
    : MCTargetStreamer(S) {
}

ZCPUTargetAsmStreamer::ZCPUTargetAsmStreamer(MCStreamer &S,
                                             formatted_raw_ostream &OS)
    : ZCPUTargetStreamer(S), OS(OS) {}

#endif // #if CH >= CH5_1
