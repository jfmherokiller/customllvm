
#ifndef LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUMCTARGETDESC_H
#define LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUMCTARGETDESC_H
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/DataTypes.h"
#include <string>
namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCRelocationInfo;
class MCTargetOptions;
class Target;
class Triple;
class StringRef;
class raw_ostream;
class raw_pwrite_stream;
extern Target TheZCPUTarget;
}
// Defines symbolic names for X86 registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "ZCPUGenRegisterInfo.inc"
#endif
