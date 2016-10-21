//===-- ZCPUMCTargetDesc.cpp - ZCPU Target Descriptions -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides ZCPU specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "ZCPUMCTargetDesc.h"
#if CH >= CH3_2 //1
#include "InstPrinter/ZCPUInstPrinter.h"
#include "ZCPUMCAsmInfo.h"
#endif
#if CH >= CH5_1
#include "ZCPUTargetStreamer.h"
#endif
#include "llvm/MC/MachineLocation.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#include "ZCPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "ZCPUGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "ZCPUGenRegisterInfo.inc"

#if CH >= CH3_2 //2
//@1 {
/// Select the ZCPU Architecture Feature for the given triple and cpu name.
/// The function will be called at command 'llvm-objdump -d' for ZCPU elf input.
static StringRef selectZCPUArchFeature(const Triple &TT, StringRef CPU) {
  std::string ZCPUArchFeature;
  if (CPU.empty() || CPU == "generic") {
    if (TT.getArch() == Triple::cpu0 || TT.getArch() == Triple::cpu0el) {
      if (CPU.empty() || CPU == "cpu032II") {
        ZCPUArchFeature = "+cpu032II";
      }
      else {
        if (CPU == "cpu032I") {
          ZCPUArchFeature = "+cpu032I";
        }
      }
    }
  }
  return ZCPUArchFeature;
}
//@1 }

static MCInstrInfo *createZCPUMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitZCPUMCInstrInfo(X); // defined in ZCPUGenInstrInfo.inc
  return X;
}

static MCRegisterInfo *createZCPUMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitZCPUMCRegisterInfo(X, ZCPU::SW); // defined in ZCPUGenRegisterInfo.inc
  return X;
}

static MCSubtargetInfo *createZCPUMCSubtargetInfo(const Triple &TT,
                                                  StringRef CPU, StringRef FS) {
  std::string ArchFS = selectZCPUArchFeature(TT,CPU);
  if (!FS.empty()) {
    if (!ArchFS.empty())
      ArchFS = ArchFS + "," + FS.str();
    else
      ArchFS = FS;
  }
  return createZCPUMCSubtargetInfoImpl(TT, CPU, ArchFS);
// createZCPUMCSubtargetInfoImpl defined in ZCPUGenSubtargetInfo.inc
}

static MCAsmInfo *createZCPUMCAsmInfo(const MCRegisterInfo &MRI,
                                      const Triple &TT) {
  MCAsmInfo *MAI = new ZCPUMCAsmInfo(TT);

  unsigned SP = MRI.getDwarfRegNum(ZCPU::SP, true);
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(nullptr, SP, 0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

static MCCodeGenInfo *createZCPUMCCodeGenInfo(const Triple &TT, Reloc::Model RM,
                                              CodeModel::Model CM,
                                              CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();
  if (CM == CodeModel::JITDefault)
    RM = Reloc::Static;
  else if (RM == Reloc::Default)
    RM = Reloc::PIC_;
  X->initMCCodeGenInfo(RM, CM, OL); // defined in lib/MC/MCCodeGenInfo.cpp
  return X;
}

static MCInstPrinter *createZCPUMCInstPrinter(const Triple &T,
                                              unsigned SyntaxVariant,
                                              const MCAsmInfo &MAI,
                                              const MCInstrInfo &MII,
                                              const MCRegisterInfo &MRI) {
  return new ZCPUInstPrinter(MAI, MII, MRI);
}
#endif

#if CH >= CH5_1 //1
static MCStreamer *createMCStreamer(const Triple &TT, MCContext &Context, 
                                    MCAsmBackend &MAB, raw_pwrite_stream &OS, 
                                    MCCodeEmitter *Emitter, bool RelaxAll) {
  return createELFStreamer(Context, MAB, OS, Emitter, RelaxAll);
}

static MCTargetStreamer *createZCPUAsmTargetStreamer(MCStreamer &S,
                                                     formatted_raw_ostream &OS,
                                                     MCInstPrinter *InstPrint,
                                                     bool isVerboseAsm) {
  return new ZCPUTargetAsmStreamer(S, OS);
}
#endif

//@2 {
extern "C" void LLVMInitializeZCPUTargetMC() {
#if CH >= CH3_2 //3
  for (Target *T : {&TheZCPUTarget, &TheZCPUelTarget}) {
    // Register the MC asm info.
    RegisterMCAsmInfoFn X(*T, createZCPUMCAsmInfo);

    // Register the MC codegen info.
    TargetRegistry::RegisterMCCodeGenInfo(*T,
	                                      createZCPUMCCodeGenInfo);

    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(*T, createZCPUMCInstrInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(*T, createZCPUMCRegisterInfo);

#if CH >= CH5_1 //2
     // Register the elf streamer.
    TargetRegistry::RegisterELFStreamer(*T, createMCStreamer);

    // Register the asm target streamer.
    TargetRegistry::RegisterAsmTargetStreamer(*T, createZCPUAsmTargetStreamer);
#endif

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(*T,
	                                        createZCPUMCSubtargetInfo);
    // Register the MCInstPrinter.
    TargetRegistry::RegisterMCInstPrinter(*T,
	                                      createZCPUMCInstPrinter);
  }
#endif // #if CH >= CH3_2

#if CH >= CH5_1 //3
  // Register the MC Code Emitter
  TargetRegistry::RegisterMCCodeEmitter(TheZCPUTarget,
                                        createZCPUMCCodeEmitterEB);
  TargetRegistry::RegisterMCCodeEmitter(TheZCPUelTarget,
                                        createZCPUMCCodeEmitterEL);

  // Register the asm backend.
  TargetRegistry::RegisterMCAsmBackend(TheZCPUTarget,
                                       createZCPUAsmBackendEB32);
  TargetRegistry::RegisterMCAsmBackend(TheZCPUelTarget,
                                       createZCPUAsmBackendEL32);
#endif
}
//@2 }
