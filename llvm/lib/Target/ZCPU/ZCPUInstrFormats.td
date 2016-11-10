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
// Normal instructions.
class I<bits<24> opcode,dag oops, dag iops, list<dag> pattern, string asmstr = "">
    : ZCPUInst<opcode,asmstr> {

  dag OutOperandList = oops;
  dag InOperandList  = iops;
  let Pattern        = pattern;
}
class IRegReg<bits<24> opcode,string instr, string asmstr = "">
    : ZCPUInst<opcode,asmstr> {
  dag OutOperandList = (outs BothNormAndExtended:$dst);
  dag InOperandList  = (ins BothNormAndExtended:$src1 ,BothNormAndExtended:$src2);
  let Pattern        = [(set BothNormAndExtended:$dst,(!cast<dag>(instr) BothNormAndExtended:$src1,BothNormAndExtended:$src2))];
}
class IRegRegFloatBinyOP<bits<24> opcode, string instr,string asmstr = "">
    : ZCPUInst<opcode,asmstr> {
  dag OutOperandList = (outs BothNormAndExtended:$dst);
  dag InOperandList  = (ins BothNormAndExtended:$src1 ,BothNormAndExtended:$src2);
  let Pattern        = [(set BothNormAndExtended:$dst,(!cast<dag>(!strconcat("f",instr)) BothNormAndExtended:$src1,BothNormAndExtended:$src2))];
}
class IPsudo<dag oops, dag iops, list<dag> pattern, string asmstr = "">
    : ZCPUInst<0,asmstr> {

  dag OutOperandList = oops;
  dag InOperandList  = iops;
  let Pattern        = pattern;
}
def brtarget : Operand<OtherVT>;

multiclass BinyOP <bits<24> opcode,string instr, string asmstr = ""> {
let Constraints = "$src1 = $dst" in {
    def regreg: IRegReg<opcode,instr,asmstr>;
    def regfloat: IRegRegFloatBinyOP<opcode,instr,asmstr>;
    }
}
