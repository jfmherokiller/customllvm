//===-- ZCPU.td - Describe the ZCPU Target Machine ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "ZCPUInstrInfo.td"

//include "ZCPUCallingConv.td"


def ZCPUInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// ZCPU Subtarget features                                                    //
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// ZCPU processors supported.
//===----------------------------------------------------------------------===//



def : ProcessorModel<"ZCPU",NoSchedModel,[]>;




def ZCPU : Target {
  let InstructionSet = ZCPUInstrInfo;
}