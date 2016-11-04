//=- ZCPUInstrFormats.td - ZCPU Instr. Formats -*- tablegen -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief ZCPU instruction format definitions.
///
//===----------------------------------------------------------------------===//
// ZCPU Instruction Format.
class ZCPUInst<bits<24> opcod,string asmstr> : Instruction {
  field bits<24> Inst = opcod; // Instruction encoding.
  let Namespace   = "ZCPU";
  let Pattern     = [];
  let AsmString   = asmstr;
}
class ZCPUInst2<string asmstr> : Instruction {
  field bits<0> Inst; // Instruction encoding.
  let Namespace   = "ZCPU";
  let Pattern     = [];
  let AsmString   = asmstr;
}
// Normal instructions.
class I<bits<24> opcode,dag oops, dag iops, list<dag> pattern, string asmstr = "">
    : ZCPUInst<opcode,asmstr> {

  dag OutOperandList = oops;
  dag InOperandList  = iops;
  let Pattern        = pattern;
}
class IPsudo<dag oops, dag iops, list<dag> pattern, string asmstr = "">
    : ZCPUInst2<asmstr> {

  dag OutOperandList = oops;
  dag InOperandList  = iops;
  let Pattern        = pattern;
}

