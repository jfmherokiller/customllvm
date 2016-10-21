//===---- ZCPUABIInfo.cpp - Information about ZCPU ABI's ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ZCPUConfig.h"
#if CH >= CH3_1

#include "ZCPUABIInfo.h"
#include "ZCPURegisterInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<bool>
EnableZCPUS32Calls("cpu0-s32-calls", cl::Hidden,
                    cl::desc("ZCPU S32 call: use stack only to pass arguments.\
                    "), cl::init(false));

namespace {
static const MCPhysReg O32IntRegs[4] = {ZCPU::A0, ZCPU::A1};
static const MCPhysReg S32IntRegs = {};
}

const ArrayRef<MCPhysReg> ZCPUABIInfo::GetByValArgRegs() const {
  if (IsO32())
    return makeArrayRef(O32IntRegs);
  if (IsS32())
    return makeArrayRef(S32IntRegs);
  llvm_unreachable("Unhandled ABI");
}

const ArrayRef<MCPhysReg> ZCPUABIInfo::GetVarArgRegs() const {
  if (IsO32())
    return makeArrayRef(O32IntRegs);
  if (IsS32())
    return makeArrayRef(S32IntRegs);
  llvm_unreachable("Unhandled ABI");
}

unsigned ZCPUABIInfo::GetCalleeAllocdArgSizeInBytes(CallingConv::ID CC) const {
  if (IsO32())
    return CC != 0;
  if (IsS32())
    return 0;
  llvm_unreachable("Unhandled ABI");
}

ZCPUABIInfo ZCPUABIInfo::computeTargetABI() {
  ZCPUABIInfo abi(ABI::Unknown);

  if (EnableZCPUS32Calls)
    abi = ABI::S32;
  else
    abi = ABI::O32;
  // Assert exactly one ABI was chosen.
  assert(abi.ThisABI != ABI::Unknown);

  return abi;
}

unsigned ZCPUABIInfo::GetStackPtr() const {
  return ZCPU::SP;
}

unsigned ZCPUABIInfo::GetFramePtr() const {
  return ZCPU::FP;
}

unsigned ZCPUABIInfo::GetNullPtr() const {
  return ZCPU::ZERO;
}

unsigned ZCPUABIInfo::GetEhDataReg(unsigned I) const {
  static const unsigned EhDataReg[] = {
    ZCPU::A0, ZCPU::A1
  };

  return EhDataReg[I];
}

int ZCPUABIInfo::EhDataRegSize() const {
  if (ThisABI == ABI::S32)
    return 0;
  else
    return 2;
}
#endif // CH >= CH3_1
