//===-- ZCPU.td - Describe the ZCPU Target Machine -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces which we are implementing
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// ZCPU Subtarget features.
//
def FeatureZGPU
  : SubtargetFeature<"gpu", "IsZGPU", "true",
                     "Enable ZGPU instructions">;
def FeatureZSPU
  : SubtargetFeature<"gpu", "IsZSPU", "true",
                     "Enable ZSPU instructions">;
//===----------------------------------------------------------------------===//
// Register File, Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//
include "ZCPURegisterInfo.td"
include "ZCPUCallingConv.td"
//include "ZCPUSchedule.td"
//include "ZCPUInstrInfo.td"

def ZCPUInstrInfo : InstrInfo;

def ZCPUAsmParser : AsmParser {
  bit ShouldEmitMatchRegisterName = 0;
  }
//===----------------------------------------------------------------------===//
// ZCPU processors supported.
//===----------------------------------------------------------------------===//
class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, NoItineraries, Features>;

def : Proc<"generic",         []>;
def : Proc<"ZGPU",              [FeatureZGPU]>; 
def : Proc<"ZSPU",              [FeatureZSPU]>; 

//===----------------------------------------------------------------------===//
// Declare the target which we are implementing
//===----------------------------------------------------------------------===//

//def ZCPUAsmWriter : AsmWriter {
//  string AsmWriterClassName  = "InstPrinter";
//  int PassSubtarget = 1;
//  int Variant = 0;
//}

def ZCPU : Target {
  // Pull in Instruction Info:
//  let InstructionSet = ZCPUInstrInfo;
//  let AssemblyParsers  = [ZCPUAsmParser];
//  let AssemblyWriters = [ZCPUAsmWriter];
}
