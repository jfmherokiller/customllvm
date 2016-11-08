//==-- ZCPUTargetStreamer.cpp - ZCPU Target Streamer Methods --=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file defines ZCPU-specific target streamer classes.
/// These are for implementing support for target-specific assembly directives.
///
//===----------------------------------------------------------------------===//

#include "ZCPUTargetStreamer.h"
#include "InstPrinter/ZCPUInstPrinter.h"
#include "ZCPUMCTargetDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

ZCPUTargetStreamer::ZCPUTargetStreamer(MCStreamer &S)
        : MCTargetStreamer(S) {}

ZCPUTargetAsmStreamer::ZCPUTargetAsmStreamer(
        MCStreamer &S, formatted_raw_ostream &OS)
        : ZCPUTargetStreamer(S), OS(OS) {

}

ZCPUTargetELFStreamer::ZCPUTargetELFStreamer(MCStreamer &S)
        : ZCPUTargetStreamer(S) {}

static void PrintTypes(formatted_raw_ostream &OS, ArrayRef<MVT> Types) {
    bool First = true;
    for (MVT Type : Types) {
        if (First)
            First = false;
        else
            OS << ", ";
        OS << ZCPU::TypeToString(Type);
    }
    OS << '\n';
}

void ZCPUTargetAsmStreamer::emitParam(ArrayRef<MVT> Types) {
    OS << "\t.param  \t";
    PrintTypes(OS, Types);
}

void ZCPUTargetAsmStreamer::emitResult(ArrayRef<MVT> Types) {
    OS << "\t.result \t";
    PrintTypes(OS, Types);
}

void ZCPUTargetAsmStreamer::emitLocal(ArrayRef<MVT> Types) {
    OS << "\t.local  \t";
    PrintTypes(OS, Types);
}

void ZCPUTargetAsmStreamer::emitEndFunc() { OS << "\t.endfunc\n"; }

void ZCPUTargetAsmStreamer::emitIndirectFunctionType(
        StringRef name, SmallVectorImpl<MVT> &SignatureVTs, size_t NumResults) {
    OS << "\t.functype\t" << name;
    if (NumResults == 0) OS << ", void";
    for (auto Ty : SignatureVTs) {
        OS << ", " << ZCPU::TypeToString(Ty);
    }
    OS << "\n";
}

// FIXME: What follows is not the real binary encoding.

static void EncodeTypes(MCStreamer &Streamer, ArrayRef<MVT> Types) {
    Streamer.EmitIntValue(Types.size(), sizeof(uint64_t));
    for (MVT Type : Types)
        Streamer.EmitIntValue(Type.SimpleTy, sizeof(uint64_t));
}

void ZCPUTargetELFStreamer::emitParam(ArrayRef<MVT> Types) {
    Streamer.EmitIntValue(ZCPU::DotParam, sizeof(uint64_t));
    EncodeTypes(Streamer, Types);
}

void ZCPUTargetELFStreamer::emitResult(ArrayRef<MVT> Types) {
    Streamer.EmitIntValue(ZCPU::DotResult, sizeof(uint64_t));
    EncodeTypes(Streamer, Types);
}

void ZCPUTargetELFStreamer::emitLocal(ArrayRef<MVT> Types) {
    Streamer.EmitIntValue(ZCPU::DotLocal, sizeof(uint64_t));
    EncodeTypes(Streamer, Types);
}

void ZCPUTargetELFStreamer::emitEndFunc() {
    Streamer.EmitIntValue(ZCPU::DotEndFunc, sizeof(uint64_t));
}
