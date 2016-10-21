//===---- ZCPUABIInfo.h - Information about ZCPU ABI's --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUABIINFO_H
#define LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUABIINFO_H

#include "ZCPUConfig.h"
#if CH >= CH3_1

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/Triple.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/MC/MCRegisterInfo.h"

namespace llvm {

class MCTargetOptions;
class StringRef;
class TargetRegisterClass;

class ZCPUABIInfo {
public:
  enum class ABI { Unknown, O32, S32 };

protected:
  ABI ThisABI;

public:
  ZCPUABIInfo(ABI ThisABI) : ThisABI(ThisABI) {}

  static ZCPUABIInfo Unknown() { return ZCPUABIInfo(ABI::Unknown); }
  static ZCPUABIInfo O32() { return ZCPUABIInfo(ABI::O32); }
  static ZCPUABIInfo S32() { return ZCPUABIInfo(ABI::S32); }
  static ZCPUABIInfo computeTargetABI();

  bool IsKnown() const { return ThisABI != ABI::Unknown; }
  bool IsO32() const { return ThisABI == ABI::O32; }
  bool IsS32() const { return ThisABI == ABI::S32; }
  ABI GetEnumValue() const { return ThisABI; }

  /// The registers to use for byval arguments.
  const ArrayRef<MCPhysReg> GetByValArgRegs() const;

  /// The registers to use for the variable argument list.
  const ArrayRef<MCPhysReg> GetVarArgRegs() const;

  /// Obtain the size of the area allocated by the callee for arguments.
  /// CallingConv::FastCall affects the value for O32.
  unsigned GetCalleeAllocdArgSizeInBytes(CallingConv::ID CC) const;

  /// Ordering of ABI's
  /// ZCPUGenSubtargetInfo.inc will use this to resolve conflicts when given
  /// multiple ABI options.
  bool operator<(const ZCPUABIInfo Other) const {
    return ThisABI < Other.GetEnumValue();
  }

  unsigned GetStackPtr() const;
  unsigned GetFramePtr() const;
  unsigned GetNullPtr() const;

  unsigned GetEhDataReg(unsigned I) const;
  int EhDataRegSize() const;
};
}

#endif // CH >= CH3_1
#endif
