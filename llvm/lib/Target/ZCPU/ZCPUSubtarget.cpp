//===-- ZCPUSubtarget.cpp - ZCPU Subtarget Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the ZCPU specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "ZCPUSubtarget.h"

#if CH >= CH3_1
#include "ZCPUMachineFunction.h"
#include "ZCPU.h"
#include "ZCPURegisterInfo.h"

#include "ZCPUTargetMachine.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "zcpu-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "ZCPUGenSubtargetInfo.inc"

#if CH >= CH4_1 //1
static cl::opt<bool> EnableOverflowOpt
                ("zcpu-enable-overflow", cl::Hidden, cl::init(false),
                 cl::desc("Use trigger overflow instructions add and sub \
                 instead of non-overflow instructions addu and subu"));
#endif

#if CH >= CH6_1 //1
static cl::opt<bool> UseSmallSectionOpt
                ("cpu0-use-small-section", cl::Hidden, cl::init(false),
                 cl::desc("Use small section. Only work when -relocation-model="
                 "static. pic always not use small section."));

static cl::opt<bool> ReserveGPOpt
                ("cpu0-reserve-gp", cl::Hidden, cl::init(false),
                 cl::desc("Never allocate $gp to variable"));

static cl::opt<bool> NoCploadOpt
                ("cpu0-no-cpload", cl::Hidden, cl::init(false),
                 cl::desc("No issue .cpload"));

bool ZCPUReserveGP;
bool ZCPUNoCpload;
#endif

extern bool FixGlobalBaseReg;

/// Select the ZCPU CPU for the given triple and cpu name.
/// FIXME: Merge with the copy in ZCPUMCTargetDesc.cpp
static StringRef selectZCPUCPU(Triple TT, StringRef CPU) {
  if (CPU.empty() || CPU == "generic") {
    if (TT.getArch() == Triple::zcpu || TT.getArch() == Triple::zcpuel)
      CPU = "zcpu32II";
  }
  return CPU;
}

void ZCPUSubtarget::anchor() { }

//@1 {
ZCPUSubtarget::ZCPUSubtarget(const Triple &TT, const std::string &CPU,
                             const std::string &FS, bool little, 
                             const ZCPUTargetMachine &_TM) :
//@1 }
  // ZCPUGenSubtargetInfo will display features by llc -march=cpu0 -mcpu=help
  ZCPUGenSubtargetInfo(TT, CPU, FS),
  IsLittle(little), TM(_TM), TargetTriple(TT), TSInfo(),
      InstrInfo(
          ZCPUInstrInfo::create(initializeSubtargetDependencies(CPU, FS, TM))),
      FrameLowering(ZCPUFrameLowering::create(*this)),
      TLInfo(ZCPUTargetLowering::create(TM, *this)) {

#if CH >= CH4_1 //2
  EnableOverflow = EnableOverflowOpt;
#endif
#if CH >= CH6_1 //2
  // Set UseSmallSection.
  UseSmallSection = UseSmallSectionOpt;
  ZCPUReserveGP = ReserveGPOpt;
  ZCPUNoCpload = NoCploadOpt;
#ifdef ENABLE_GPRESTORE
  if (TM.getRelocationModel() == Reloc::Static == Reloc::Static && !UseSmallSection && !ZCPUReserveGP)
    FixGlobalBaseReg = false;
  else
#endif
    FixGlobalBaseReg = true;
#endif //#if CH >= CH6_1
}

ZCPUSubtarget &
ZCPUSubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS,
                                               const TargetMachine &TM) {
  std::string CPUName = selectZCPUCPU(TargetTriple, CPU);

  if (CPUName == "help")
    CPUName = "zcpu32II";
  
  if (CPUName == "zcpu32I")
    ZCPUArchVersion = ZCPU32I;
  else if (CPUName == "zcpu32II")
    ZCPUArchVersion = ZCPU32II;

  if (isZCPU32I()) {
    HasCmp = true;
    HasSlt = false;
  }
  else if (isZCPU32II()) {
    HasCmp = true;
    HasSlt = true;
  }
  else {
    errs() << "-mcpu must be empty(default:cpu032II), cpu032I or cpu032II" << "\n";
  }

  // Parse features string.
  ParseSubtargetFeatures(CPUName, FS);
  // Initialize scheduling itinerary for the specified CPU.
  InstrItins = getInstrItineraryForCPU(CPUName);

  return *this;
}

bool ZCPUSubtarget::abiUsesSoftFloat() const {
//  return TM->Options.UseSoftFloat;
  return true;
}

const ZCPUABIInfo &ZCPUSubtarget::getABI() const { return TM.getABI(); }

#endif // #if CH >= CH3_1
