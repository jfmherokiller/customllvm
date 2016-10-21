//===-- ZCPUMachineFunctionInfo.cpp - Private data used for ZCPU ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ZCPUMachineFunction.h"
#if CH >= CH3_1

#if CH >= CH3_2
#include "MCTargetDesc/ZCPUBaseInfo.h"
#endif
#include "ZCPUInstrInfo.h"
#include "ZCPUSubtarget.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

using namespace llvm;

bool FixGlobalBaseReg;

// class ZCPUCallEntry.
ZCPUCallEntry::ZCPUCallEntry(StringRef N) {
#ifndef NDEBUG
  Name = N;
  Val = nullptr;
#endif
}

ZCPUCallEntry::ZCPUCallEntry(const GlobalValue *V) {
#ifndef NDEBUG
  Val = V;
#endif
}

bool ZCPUCallEntry::isConstant(const MachineFrameInfo *) const {
  return false;
}

bool ZCPUCallEntry::isAliased(const MachineFrameInfo *) const {
  return false;
}

bool ZCPUCallEntry::mayAlias(const MachineFrameInfo *) const {
  return false;
}

void ZCPUCallEntry::printCustom(raw_ostream &O) const {
  O << "ZCPUCallEntry: ";
#ifndef NDEBUG
  if (Val)
    O << Val->getName();
  else
    O << Name;
#endif
}

ZCPUFunctionInfo::~ZCPUFunctionInfo() {}

#if CH >= CH6_1
bool ZCPUFunctionInfo::globalBaseRegFixed() const {
  return FixGlobalBaseReg;
}

bool ZCPUFunctionInfo::globalBaseRegSet() const {
  return GlobalBaseReg;
}

unsigned ZCPUFunctionInfo::getGlobalBaseReg() {
  return GlobalBaseReg = ZCPU::GP;
}
#endif

#if CH >= CH3_5
void ZCPUFunctionInfo::createEhDataRegsFI() {
  for (int I = 0; I < 2; ++I) {
    const TargetRegisterClass *RC = &ZCPU::CPURegsRegClass;

    EhDataRegFI[I] = MF.getFrameInfo()->CreateStackObject(RC->getSize(),
        RC->getAlignment(), false);
  }
}
#endif

#if CH >= CH9_2
MachinePointerInfo ZCPUFunctionInfo::callPtrInfo(StringRef Name) {
  std::unique_ptr<const ZCPUCallEntry> &E = ExternalCallEntries[Name];

  if (!E)
    E = llvm::make_unique<ZCPUCallEntry>(Name);

  return MachinePointerInfo(E.get());
}

MachinePointerInfo ZCPUFunctionInfo::callPtrInfo(const GlobalValue *Val) {
  std::unique_ptr<const ZCPUCallEntry> &E = GlobalCallEntries[Val];

  if (!E)
    E = llvm::make_unique<ZCPUCallEntry>(Val);

  return MachinePointerInfo(E.get());
}
#endif

void ZCPUFunctionInfo::anchor() { }

#endif // #if CH >= CH3_1
