//===-- ZCPUCallingConv.td - ZCPU Calling Conventions --------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This describes the calling conventions for the ZCPU architectures.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// ZCPU Return Value Calling Convention
//===----------------------------------------------------------------------===//

def RetCC_ZCPU : CallingConv<[
  // i48 are returned in register EAX
  CCIfType<[i48], CCAssignToReg<[EAX]>>,
]>;

//===----------------------------------------------------------------------===//
// ZCPU Argument Calling Conventions
//===----------------------------------------------------------------------===//

def CC_ZCPU : CallingConv<[
  // Assign i48 arguments in registers
  CCIfType<[i48], CCAssignToReg<[EAX, EBX, EDX]>>,

  // Assign other i48 arguments to stack slots
  CCIfType<[i48], CCAssignToStack<1, 1>>,
]>;

//===----------------------------------------------------------------------===//
// Callee-saved Registers.
//===----------------------------------------------------------------------===//

def CSR_NoRegs : CalleeSavedRegs<(add)>;

def CSR_16 : CalleeSavedRegs<(add BC, DE, IX, IY)>;
