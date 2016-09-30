#include "llvm/ADT/Triple.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetRegistry.h"
#include "ZCPUMCTargetDesc.h"

#if _MSC_VER
#include <intrin.h>
#endif
using namespace llvm;

#define GET_REGINFO_MC_DESC
#include "ZCPUGenRegisterInfo.inc"
