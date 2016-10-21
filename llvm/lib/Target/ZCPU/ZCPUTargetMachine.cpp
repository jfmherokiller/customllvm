//===-- ZCPUTargetMachine.cpp - Define TargetMachine for ZCPU -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about ZCPU target spec.
//
//===----------------------------------------------------------------------===//

#include "ZCPUTargetMachine.h"
#include "ZCPU.h"

#if CH >= CH3_3 //0.5
#include "ZCPUSEISelDAGToDAG.h"
#endif
#if CH >= CH3_1
#include "ZCPUSubtarget.h"
#include "ZCPUTargetObjectFile.h"
#endif
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

#define DEBUG_TYPE "cpu0"

extern "C" void LLVMInitializeZCPUTarget() {
#if CH >= CH3_1
  // Register the target.
  //- Big endian Target Machine
  RegisterTargetMachine<ZCPUebTargetMachine> X(TheZCPUTarget);
  //- Little endian Target Machine
  RegisterTargetMachine<ZCPUelTargetMachine> Y(TheZCPUelTarget);
#endif
}

#if CH >= CH3_1

static std::string computeDataLayout(const Triple &TT, StringRef CPU,
                                     const TargetOptions &Options,
                                     bool isLittle) {
  std::string Ret = "";
  // There are both little and big endian cpu0.
  if (isLittle)
    Ret += "e";
  else
    Ret += "E";

  Ret += "-m:m";

  // Pointers are 32 bit on some ABIs.
  Ret += "-p:32:32";

  // 8 and 16 bit integers only need to have natural alignment, but try to
  // align them to 32 bits. 64 bit integers have natural alignment.
  Ret += "-i8:8:32-i16:16:32-i64:64";

  // 32 bit registers are always available and the stack is at least 64 bit
  // aligned.
  Ret += "-n32-S64";

  return Ret;
}

// DataLayout --> Big-endian, 32-bit pointer/ABI/alignment
// The stack is always 8 byte aligned
// On function prologue, the stack is created by decrementing
// its pointer. Once decremented, all references are done with positive
// offset from the stack/frame pointer, using StackGrowsUp enables
// an easier handling.
// Using CodeModel::Large enables different CALL behavior.
ZCPUTargetMachine::ZCPUTargetMachine(const Target &T, const Triple &TT,
                                     StringRef CPU, StringRef FS,
                                     const TargetOptions &Options,
                                     Reloc::Model RM, CodeModel::Model CM,
                                     CodeGenOpt::Level OL, bool isLittle)
  //- Default is big endian
    : LLVMTargetMachine(T, computeDataLayout(TT, CPU, Options, isLittle), TT,
                        CPU, FS, Options, RM, CM, OL),
      isLittle(isLittle), TLOF(make_unique<ZCPUTargetObjectFile>()),
      ABI(ZCPUABIInfo::computeTargetABI()),
      DefaultSubtarget(TT, CPU, FS, isLittle, *this) {
  // initAsmInfo will display features by llc -march=cpu0 -mcpu=help on 3.7 but
  // not on 3.6
  initAsmInfo();
}

ZCPUTargetMachine::~ZCPUTargetMachine() {}

void ZCPUebTargetMachine::anchor() { }

ZCPUebTargetMachine::ZCPUebTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Reloc::Model RM, CodeModel::Model CM,
                                         CodeGenOpt::Level OL)
    : ZCPUTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, false) {}

void ZCPUelTargetMachine::anchor() { }

ZCPUelTargetMachine::ZCPUelTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         Reloc::Model RM, CodeModel::Model CM,
                                         CodeGenOpt::Level OL)
    : ZCPUTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, true) {}

const ZCPUSubtarget *
ZCPUTargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  std::string CPU = !CPUAttr.hasAttribute(Attribute::None)
                        ? CPUAttr.getValueAsString().str()
                        : TargetCPU;
  std::string FS = !FSAttr.hasAttribute(Attribute::None)
                       ? FSAttr.getValueAsString().str()
                       : TargetFS;

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = llvm::make_unique<ZCPUSubtarget>(TargetTriple, CPU, FS, isLittle,
                                         *this);
  }
  return I.get();
}

namespace {
//@ZCPUPassConfig {
/// ZCPU Code Generator Pass Configuration Options.
class ZCPUPassConfig : public TargetPassConfig {
public:
  ZCPUPassConfig(ZCPUTargetMachine *TM, PassManagerBase &PM)
    : TargetPassConfig(TM, PM) {}

  ZCPUTargetMachine &getZCPUTargetMachine() const {
    return getTM<ZCPUTargetMachine>();
  }

  const ZCPUSubtarget &getZCPUSubtarget() const {
    return *getZCPUTargetMachine().getSubtargetImpl();
  }
#if CH >= CH12_1 //1
  void addIRPasses() override;
#endif
#if CH >= CH3_3 //1
  bool addInstSelector() override;
#endif
#if CH >= CH8_2 //1
  void addPreEmitPass() override;
#endif
#if CH >= CH9_3 //1
#ifdef ENABLE_GPRESTORE
  void addPreRegAlloc() override;
#endif
#endif //#if CH >= CH9_3 //1
};
} // namespace

TargetPassConfig *ZCPUTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new ZCPUPassConfig(this, PM);
}

#if CH >= CH12_1 //2
void ZCPUPassConfig::addIRPasses() {
  TargetPassConfig::addIRPasses();
  addPass(createAtomicExpandPass(&getZCPUTargetMachine()));
}
#endif

#if CH >= CH3_3 //2
// Install an instruction selector pass using
// the ISelDag to gen ZCPU code.
bool ZCPUPassConfig::addInstSelector() {
  addPass(createZCPUSEISelDag(getZCPUTargetMachine()));
  return false;
}
#endif

#if CH >= CH9_3 //2
#ifdef ENABLE_GPRESTORE
void ZCPUPassConfig::addPreRegAlloc() {
  if (!ZCPUReserveGP) {
    // $gp is a caller-saved register.
    addPass(createZCPUEmitGPRestorePass(getZCPUTargetMachine()));
  }
  return;
}
#endif
#endif //#if CH >= CH9_3 //2

#if CH >= CH8_2 //2
// Implemented by targets that want to run passes immediately before
// machine code is emitted. return true if -print-machineinstrs should
// print out the code after the passes.
void ZCPUPassConfig::addPreEmitPass() {
  ZCPUTargetMachine &TM = getZCPUTargetMachine();
//@8_2 1{
  addPass(createZCPUDelJmpPass(TM));
//@8_2 1}
  addPass(createZCPUDelaySlotFillerPass(TM));
//@8_2 2}
  addPass(createZCPULongBranchPass(TM));
  return;
}
#endif

#endif // #if CH >= CH3_1
