//===-- ZCPUMCTargetDesc.cpp - ZCPU Target Descriptions -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides ZCPU-specific target descriptions.
///
//===----------------------------------------------------------------------===//

#include "ZCPUMCTargetDesc.h"
#include "InstPrinter/ZCPUInstPrinter.h"
#include "ZCPUMCAsmInfo.h"
#include "ZCPUTargetStreamer.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "zcpu-mc-target-desc"

#define GET_INSTRINFO_MC_DESC

#include "ZCPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC

#include "ZCPUGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC

#include "ZCPUGenRegisterInfo.inc"

static MCAsmInfo *createMCAsmInfo(const MCRegisterInfo & /*MRI*/,
                                  const Triple &TT) {
    return new ZCPUMCAsmInfo(TT);
}

static void adjustCodeGenOpts(const Triple & /*TT*/, Reloc::Model /*RM*/,
                              CodeModel::Model &CM) {
    CodeModel::Model M = (CM == CodeModel::Default || CM == CodeModel::JITDefault) ? CodeModel::Large : CM;
    if (M != CodeModel::Large)
        report_fatal_error("Non-large code models are not supported yet");
}

static MCInstrInfo *createMCInstrInfo() {
    MCInstrInfo *X = new MCInstrInfo();
    InitZCPUMCInstrInfo(X);
    return X;
}

static MCRegisterInfo *createMCRegisterInfo(const Triple & /*T*/) {
    MCRegisterInfo *X = new MCRegisterInfo();
    InitZCPUMCRegisterInfo(X, 0);
    return X;
}

static MCInstPrinter *createMCInstPrinter(const Triple & /*T*/,
                                          unsigned SyntaxVariant,
                                          const MCAsmInfo &MAI,
                                          const MCInstrInfo &MII,
                                          const MCRegisterInfo &MRI) {
    assert(SyntaxVariant == 0 && "ZCPU only has one syntax variant");
    return new ZCPUInstPrinter(MAI, MII, MRI);
}

static MCCodeEmitter *createCodeEmitter(const MCInstrInfo &MCII,
                                        const MCRegisterInfo & /*MRI*/,
                                        MCContext & /*Ctx*/) {
    return createZCPUMCCodeEmitter(MCII);
}

static MCAsmBackend *createAsmBackend(const Target & /*T*/,
                                      const MCRegisterInfo & /*MRI*/,
                                      const Triple &TT, StringRef /*CPU*/) {
    return createZCPUAsmBackend(TT);
}

static MCSubtargetInfo *createMCSubtargetInfo(const Triple &TT, StringRef CPU,
                                              StringRef FS) {
    return createZCPUMCSubtargetInfoImpl(TT, CPU, FS);
}

static MCTargetStreamer *
createObjectTargetStreamer(MCStreamer &S, const MCSubtargetInfo & /*STI*/) {
    return new ZCPUTargetELFStreamer(S);
}

static MCTargetStreamer *createAsmTargetStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS,
                                                 MCInstPrinter * /*InstPrint*/,
                                                 bool /*isVerboseAsm*/) {
    return new ZCPUTargetAsmStreamer(S, OS);
}

// Force static initialization.
extern "C" void LLVMInitializeZCPUTargetMC() {
    for (Target *T : {&TheZCPUTarget32}) {
        // Register the MC asm info.
        RegisterMCAsmInfoFn X(*T, createMCAsmInfo);

        // Register the MC instruction info.
        TargetRegistry::RegisterMCInstrInfo(*T, createMCInstrInfo);

        // Register the MC codegen info.
        TargetRegistry::registerMCAdjustCodeGenOpts(*T, adjustCodeGenOpts);

        // Register the MC register info.
        TargetRegistry::RegisterMCRegInfo(*T, createMCRegisterInfo);

        // Register the MCInstPrinter.
        TargetRegistry::RegisterMCInstPrinter(*T, createMCInstPrinter);

        // Register the MC code emitter.
        TargetRegistry::RegisterMCCodeEmitter(*T, createCodeEmitter);

        // Register the ASM Backend.
        TargetRegistry::RegisterMCAsmBackend(*T, createAsmBackend);

        // Register the MC subtarget info.
        TargetRegistry::RegisterMCSubtargetInfo(*T, createMCSubtargetInfo);

        // Register the object target streamer.
        TargetRegistry::RegisterObjectTargetStreamer(*T,
                                                     createObjectTargetStreamer);
        // Register the asm target streamer.
        TargetRegistry::RegisterAsmTargetStreamer(*T, createAsmTargetStreamer);
    }
}
