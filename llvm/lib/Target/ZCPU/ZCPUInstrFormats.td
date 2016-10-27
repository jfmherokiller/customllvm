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
class ZCPUInst<string asmstr> : Instruction {
  field bits<0> Inst; // Instruction encoding.
  let Namespace   = "ZCPU";
  let Pattern     = [];
  let AsmString   = asmstr;
}
// Normal instructions.
class I<dag oops, dag iops, list<dag> pattern, string asmstr = "">
    : ZCPUInst<asmstr> {
  dag OutOperandList = oops;
  dag InOperandList  = iops;
  let Pattern        = pattern;
}

