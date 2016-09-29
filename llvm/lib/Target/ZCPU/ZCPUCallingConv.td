//===-- ZCPUCallingConv.td - Calling Conventions ZCPU --*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the ZCPU
// architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// ZCPU Return Value Calling Convention
//===----------------------------------------------------------------------===//
def RetCC_ZCPU : CallingConv<[

  CCIfType<[], CCAssignToReg<[EAX]>>,
]>;

//===----------------------------------------------------------------------===//
// ZCPU Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_ZCPU : CallingConv<[
  // Assign arguments in registers
  CCIfType<[i64], CCAssignToReg<[EAX, ECX, EBP]>>,

  // Assign other arguments to stack slots
  CCIfType<[i64], CCAssignToStack<0, 0>>,
]>;
//===----------------------------------------------------------------------===//
// Callee-saved Registers.
//===----------------------------------------------------------------------===//

def CSR_NoRegs : CalleeSavedRegs<(add)>;
def CSR_ALLPASS : CalleeSavedRegs<(add ECX,EBP,EAX)>;
