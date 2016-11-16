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
class IRegImmSignIntBinyOP<bits<24> opcode,string instr, string asmstr = "">
    : ZCPUInst<opcode,asmstr> {
  dag OutOperandList = (outs BothNormAndExtendedInt:$dst);
  dag InOperandList  = (ins BothNormAndExtendedInt:$src1 ,i64imm:$src2);
  let Pattern        = [(set BothNormAndExtendedInt:$dst,(!cast<dag>(!strconcat("s",instr)) BothNormAndExtendedInt:$src1,imm:$src2))];
}
class IRegImmUnSignIntBinyOP<bits<24> opcode, string instr,string asmstr = "">
    : ZCPUInst<opcode,asmstr> {
  dag OutOperandList = (outs BothNormAndExtendedInt:$dst);
  dag InOperandList  = (ins BothNormAndExtendedInt:$src1 ,i64imm:$src2);
  let Pattern        = [(set BothNormAndExtendedInt:$dst,(!cast<dag>(!strconcat("u",instr)) BothNormAndExtendedInt:$src1,imm:$src2))];
}
class IRegRegSignIntBinyOP<bits<24> opcode,string instr, string asmstr = "">
    : ZCPUInst<opcode,asmstr> {
  dag OutOperandList = (outs BothNormAndExtendedInt:$dst);
  dag InOperandList  = (ins BothNormAndExtendedInt:$src1 ,BothNormAndExtendedInt:$src2);
  let Pattern        = [(set BothNormAndExtendedInt:$dst,(!cast<dag>(!strconcat("s",instr)) BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2))];
}
class IRegRegUnSignIntBinyOP<bits<24> opcode, string instr,string asmstr = "">
    : ZCPUInst<opcode,asmstr> {
  dag OutOperandList = (outs BothNormAndExtendedInt:$dst);
  dag InOperandList  = (ins BothNormAndExtendedInt:$src1 ,BothNormAndExtendedInt:$src2);
  let Pattern        = [(set BothNormAndExtendedInt:$dst,(!cast<dag>(!strconcat("u",instr)) BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2))];
}
class IRegRegIntBinyOP<bits<24> opcode,string instr, string asmstr = "">
    : ZCPUInst<opcode,asmstr> {
  dag OutOperandList = (outs BothNormAndExtendedInt:$dst);
  dag InOperandList  = (ins BothNormAndExtendedInt:$src1 ,BothNormAndExtendedInt:$src2);
  let Pattern        = [(set BothNormAndExtendedInt:$dst,(!cast<dag>(instr) BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2))];
}
class IRegImmIntBinyOP<bits<24> opcode,string instr, string asmstr = "">
    : ZCPUInst<opcode,asmstr> {
  dag OutOperandList = (outs BothNormAndExtendedInt:$dst);
  dag InOperandList  = (ins BothNormAndExtendedInt:$src1 ,i64imm:$src2);
  let Pattern        = [(set BothNormAndExtendedInt:$dst,(!cast<dag>(instr) BothNormAndExtendedInt:$src1,imm:$src2))];
}
class IRegRegFloatBinyOP<bits<24> opcode, string instr,string asmstr = "">
    : ZCPUInst<opcode,asmstr> {
  dag OutOperandList = (outs BothNormAndExtendedFloat:$dst);
  dag InOperandList  = (ins BothNormAndExtendedFloat:$src1 ,BothNormAndExtendedFloat:$src2);
  let Pattern        = [(set BothNormAndExtendedFloat:$dst,(!cast<dag>(!strconcat("f",instr)) BothNormAndExtendedFloat:$src1,BothNormAndExtendedFloat:$src2))];
}
class IRegImmFloatBinyOP<bits<24> opcode, string instr,string asmstr = "">
    : ZCPUInst<opcode,asmstr> {
  dag OutOperandList = (outs BothNormAndExtendedFloat:$dst);
  dag InOperandList  = (ins BothNormAndExtendedFloat:$src1 ,f64imm:$src2);
  let Pattern        = [(set BothNormAndExtendedFloat:$dst,(!cast<dag>(!strconcat("f",instr)) BothNormAndExtendedFloat:$src1,fpimm:$src2))];
}
class IPsudo<dag oops, dag iops, list<dag> pattern, string asmstr = "">
    : ZCPUInst<0,asmstr> {

  dag OutOperandList = oops;
  dag InOperandList  = iops;
  let Pattern        = pattern;
}

def brtarget : Operand<OtherVT>;

multiclass BinyOPSignUnSigned <bits<24> opcode,string instr, string asmstr = ""> {
let Constraints = "$src1 = $dst" in {
    def regregSignedInt :IRegRegSignIntBinyOP<opcode,instr,asmstr>;
    def regregUnSignedInt :IRegRegUnSignIntBinyOP<opcode,instr,asmstr>;
    def regimmSignedInt :IRegImmSignIntBinyOP<opcode,instr,asmstr>;
    def regimmUnSignedInt :IRegImmUnSignIntBinyOP<opcode,instr,asmstr>;
    }
}
multiclass BinyFloatIntOP <bits<24> opcode,string instr, string asmstr = ""> {
let Constraints = "$src1 = $dst" in {
    def regregint: IRegRegIntBinyOP<opcode,instr,asmstr>;
    def regregfloat: IRegRegFloatBinyOP<opcode,instr,asmstr>;
    //def regimmfloat: IRegImmFloatBinyOP<opcode,instr,asmstr>;
    def regimmint: IRegImmIntBinyOP<opcode,instr,asmstr>;
    }
}
