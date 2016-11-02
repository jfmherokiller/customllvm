//- ZCPU.td - Describe the ZCPU Target Machine --*- tablegen -*-//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This is a target description file for the ZCPU architecture,
/// which is also known as "wasm".
///
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// ZCPU Subtarget features.
//===----------------------------------------------------------------------===//



//===----------------------------------------------------------------------===//
// Architectures.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Register File Description
//===----------------------------------------------------------------------===//

include "ZCPURegisterInfo.td"

//===----------------------------------------------------------------------===//
// Instruction Descriptions
//===----------------------------------------------------------------------===//

include "ZCPUInstrInfo.td"
include "ZCPUCallingConv.td"
def ZCPUInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// ZCPU Processors supported.
//===----------------------------------------------------------------------===//

// Minimal Viable Product.
def : ProcessorModel<"mvp", NoSchedModel, []>;

//===----------------------------------------------------------------------===//
// Target Declaration
//===----------------------------------------------------------------------===//

def ZCPU : Target {
  let InstructionSet = ZCPUInstrInfo;
}
