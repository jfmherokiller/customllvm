//===-- ZCPUInstrInfo.cpp - ZCPU Instruction Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the ZCPU implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "ZCPUInstrInfo.h"
#if CH >= CH3_1

#include "ZCPUTargetMachine.h"
#include "ZCPUMachineFunction.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "ZCPUGenInstrInfo.inc"

// Pin the vtable to this file.
void ZCPUInstrInfo::anchor() {}

//@ZCPUInstrInfo {
ZCPUInstrInfo::ZCPUInstrInfo(const ZCPUSubtarget &STI)
    : 
#if CH >= CH9_2
      ZCPUGenInstrInfo(ZCPU::ADJCALLSTACKDOWN, ZCPU::ADJCALLSTACKUP),
#endif
      Subtarget(STI) {}

const ZCPUInstrInfo *ZCPUInstrInfo::create(ZCPUSubtarget &STI) {
  return llvm::createZCPUSEInstrInfo(STI);
}

#if CH >= CH9_1 //1
MachineMemOperand *ZCPUInstrInfo::GetMemOperand(MachineBasicBlock &MBB, int FI,
                                                unsigned Flag) const {
  MachineFunction &MF = *MBB.getParent();
  MachineFrameInfo &MFI = *MF.getFrameInfo();
  unsigned Align = MFI.getObjectAlignment(FI);

  return MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(FI), Flag,
                                 MFI.getObjectSize(FI), Align);
}
#endif

#if CH >= CH3_3
MachineInstr*
ZCPUInstrInfo::emitFrameIndexDebugValue(MachineFunction &MF, int FrameIx,
                                        uint64_t Offset, const MDNode *MDPtr,
                                        DebugLoc DL) const {
  MachineInstrBuilder MIB = BuildMI(MF, DL, get(ZCPU::DBG_VALUE))
    .addFrameIndex(FrameIx).addImm(0).addImm(Offset).addMetadata(MDPtr);
  return &*MIB;
}

//@GetInstSizeInBytes {
/// Return the number of bytes of code the specified instruction may be.
unsigned ZCPUInstrInfo::GetInstSizeInBytes(const MachineInstr *MI) const {
//@GetInstSizeInBytes - body
  switch (MI->getOpcode()) {
  default:
    return MI->getDesc().getSize();
#if CH >= CH11_2
  case  TargetOpcode::INLINEASM: {       // Inline Asm: Variable size.
    const MachineFunction *MF = MI->getParent()->getParent();
    const char *AsmStr = MI->getOperand(0).getSymbolName();
    return getInlineAsmLength(AsmStr, *MF->getTarget().getMCAsmInfo());
  }
#endif
  }
}
#endif

#endif // #if CH >= CH3_1
