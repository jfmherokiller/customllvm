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

include "ZCPUSchedule.td"
include "ZCPUInstrInfo.td"

//include "ZCPUCallingConv.td"


def ZCPUInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// ZCPU Subtarget features                                                    //
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// ZCPU processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, ZCPUGenericItineraries, Features>;

def : Proc<"ZCPU",  []>;

def ZCPUAsmParser : AsmParser {
  let ShouldEmitMatchRegisterName = 0;
}

def ZCPUAsmParserVariant : AsmParserVariant {
  int Variant = 0;
}

// Will generate ZCPUGenAsmWrite.inc included by ZCPUInstPrinter.cpp, contents 
//  as follows,
// void ZCPUInstPrinter::printInstruction(const MCInst *MI, raw_ostream &O) {...}
// const char *ZCPUInstPrinter::getRegisterName(unsigned RegNo) {...}
def ZCPU : Target {
// def ZCPUInstrInfo : InstrInfo as before.
  let InstructionSet = ZCPUInstrInfo;
//#if CH >= CH11_1 2
  let AssemblyParsers = [ZCPUAsmParser];
  let AssemblyParserVariants = [ZCPUAsmParserVariant];
//#endif
}
