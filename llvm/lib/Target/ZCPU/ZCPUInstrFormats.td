//===-- ZCPUInstrFormats.td - ZCPU Instruction Formats -----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Describe ZCPU instructions format
//
//  CPU INSTRUCTION FORMATS
//
//  opcode  - operation code.
//  ra      - dst reg.
//  rb      - src reg.
//  cx      - immediate
//
//===----------------------------------------------------------------------===//

// ZCPU Instruction Format
class ZCPUInst<string cstr> : Instruction {
  field bits<0> Inst; // Instruction encoding.
  let Namespace   = "ZCPU";
  let Pattern     = [];
  let Constraints = cstr;
}
// Normal instructions
class I<dag oops, dag iops, list<dag> pattern, string cstr = "">
    : ZCPUInst<cstr> {
  dag OutOperandList = oops;
  dag InOperandList  = iops;
  let Pattern        = pattern;
}
multiclass UnaryInt<SDNode node> {
  def _I48 : I<(outs Int48:$dst), (ins Int48:$src),
               [(set Int48:$dst, (node Int48:$src))]>;
}
multiclass BinaryInt<SDNode node> {
  def _I48 : I<(outs Int48:$dst), (ins Int48:$lhs, Int48:$rhs),
               [(set Int48:$dst, (node Int48:$lhs, Int48:$rhs))]>;
}
multiclass UnaryFP<SDNode node> {
  def _F64 : I<(outs Float64:$dst), (ins Float64:$src),
               [(set Float64:$dst, (node Float64:$src))]>;
}
multiclass BinaryFP<SDNode node> {
  def _F64 : I<(outs Float64:$dst), (ins Float64:$lhs, Float64:$rhs),
               [(set Float64:$dst, (node Float64:$lhs, Float64:$rhs))]>;
}




