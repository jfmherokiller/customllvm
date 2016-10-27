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

// Unary and binary instructions, for the local types that ZCPU supports.
multiclass UnaryInt<SDNode node, string name> {
  def _I32 : I<(outs I32:$dst), (ins I32:$src),
               [(set I32:$dst, (node I32:$src))],
               !strconcat("i32.", !strconcat(name, "\t$dst, $src"))>;
}
multiclass BinaryInt<SDNode node, string name> {
  def _I32 : I<(outs I32:$dst), (ins I32:$lhs, I32:$rhs),
               [(set I32:$dst, (node I32:$lhs, I32:$rhs))],
               !strconcat("i32.", !strconcat(name, "\t$dst, $lhs, $rhs"))>;
}
multiclass UnaryFP<SDNode node, string name> {
  def _F64 : I<(outs F64:$dst), (ins F64:$src),
               [(set F64:$dst, (node F64:$src))],
               !strconcat("f64.", !strconcat(name, "\t$dst, $src"))>;
}
multiclass BinaryFP<SDNode node, string name> {
  def _F64 : I<(outs F64:$dst), (ins F64:$lhs, F64:$rhs),
               [(set F64:$dst, (node F64:$lhs, F64:$rhs))],
               !strconcat("f64.", !strconcat(name, "\t$dst, $lhs, $rhs"))>;
}
multiclass ComparisonInt<CondCode cond, string name> {
  def _I32 : I<(outs I32:$dst), (ins I32:$lhs, I32:$rhs),
               [(set I32:$dst, (setcc I32:$lhs, I32:$rhs, cond))],
               !strconcat("i32.", !strconcat(name, "\t$dst, $lhs, $rhs"))>;
}
multiclass ComparisonFP<CondCode cond, string name> {
  def _F64 : I<(outs I32:$dst), (ins F64:$lhs, F64:$rhs),
               [(set I32:$dst, (setcc F64:$lhs, F64:$rhs, cond))],
               !strconcat("f64.", !strconcat(name, "\t$dst, $lhs, $rhs"))>;
}
