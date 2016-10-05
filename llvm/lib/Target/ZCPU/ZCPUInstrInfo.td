//===-- ZCPUInstrInfo.td - ZCPU Instruction defs -------------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source 
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file describes the ZCPU instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// ZCPU Instruction Format Definitions.
//===----------------------------------------------------------------------===//

include "ZCPUInstrFormats.td"

//===----------------------------------------------------------------------===//
// ZCPU Type Profiles.
//===----------------------------------------------------------------------===//
def SDT_ZCPUCall         : SDTypeProfile<0, -1, [SDTCisVT<0, iPTR>]>;
def SDT_ZCPUCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i16>]>;
def SDT_ZCPUCallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i16>, SDTCisVT<1, i16>]>;
def SDT_ZCPUWrapper      : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>,SDTCisPtrTy<0>]>;
def SDT_ZCPUCp           : SDTypeProfile<0, 2, [SDTCisSameAs<0, 1>]>;
def SDT_ZCPUSelectCC     : SDTypeProfile<1, 3, [SDTCisSameAs<0, 1>,
                                               SDTCisSameAs<1, 2>,
                                               SDTCisVT<3, i8>]>;
def SDT_ZCPUBrCC         : SDTypeProfile<0, 2, [SDTCisVT<0, OtherVT>,
                                               SDTCisVT<1, i8>]>;
def SDT_ZCPUShift        : SDTypeProfile<1, 2, [SDTCisSameAs<0, 1>,
                                               SDTCisVT<2, i8>]>;
//===----------------------------------------------------------------------===//
// ZCPU ESPecific Node Definitions.
//===----------------------------------------------------------------------===//
def ZCPUcall          : SDNode<"ZCPUISD::CALL", SDT_ZCPUCall,
                     [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;
def ZCPUcallseq_start : SDNode<"ISD::CALLSEQ_START", SDT_ZCPUCallSeqStart,
                     [SDNPHasChain, SDNPOutGlue]>;
def ZCPUcallseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_ZCPUCallSeqEnd,
                     [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def ZCPUret           : SDNode<"ZCPUISD::RET", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;
def ZCPUwrapper       : SDNode<"ZCPUISD::WRAPPER", SDT_ZCPUWrapper>;
def ZCPUrlc           : SDNode<"ZCPUISD::RLC", SDTIntUnaryOp, [SDNPOutGlue]>;
def ZCPUrrc           : SDNode<"ZCPUISD::RRC", SDTIntUnaryOp, [SDNPOutGlue]>;
def ZCPUrl            : SDNode<"ZCPUISD::RL",  SDTIntUnaryOp,
                     [SDNPOutGlue, SDNPInGlue]>;
def ZCPUrr            : SDNode<"ZCPUISD::RR",  SDTIntUnaryOp, 
                     [SDNPOutGlue, SDNPInGlue]>;
def ZCPUsla           : SDNode<"ZCPUISD::SLA", SDTIntUnaryOp, [SDNPOutGlue]>;
def ZCPUsra           : SDNode<"ZCPUISD::SRA", SDTIntUnaryOp, [SDNPOutGlue]>;
def ZCPUsll           : SDNode<"ZCPUISD::SLL", SDTIntUnaryOp, [SDNPOutGlue]>;
def ZCPUsrl           : SDNode<"ZCPUISD::SRL", SDTIntUnaryOp, [SDNPOutGlue]>;
def ZCPUshl           : SDNode<"ZCPUISD::SHL", SDT_ZCPUShift, []>;
def ZCPUlshr          : SDNode<"ZCPUISD::LSHR", SDT_ZCPUShift, []>;
def ZCPUashr          : SDNode<"ZCPUISD::ASHR", SDT_ZCPUShift, []>;
def ZCPUcp            : SDNode<"ZCPUISD::CP", SDT_ZCPUCp, [SDNPOutGlue]>;
def ZCPUselectcc      : SDNode<"ZCPUISD::SELECT_CC", SDT_ZCPUSelectCC,
                     [SDNPInGlue]>;
def ZCPUbrcc          : SDNode<"ZCPUISD::BR_CC", SDT_ZCPUBrCC,
                     [SDNPHasChain, SDNPInGlue]>;
def ZCPUscf           : SDNode<"ZCPUISD::SCF", SDTNone, [SDNPOutGlue]>;
def ZCPUccf           : SDNode<"ZCPUISD::CCF", SDTNone,
                     [SDNPOutGlue, SDNPInGlue]>;
//===----------------------------------------------------------------------===//
// Operand Definitions.
//===----------------------------------------------------------------------===//

def brtarget : Operand<OtherVT> {
  let EncoderMethod = "getBREncoding";
}

def calltarget : Operand<iPTR> {
  let EncoderMethod = "getBREncoding";
}

def cc : Operand<i8> {
  let PrintMethod = "printCCOperand";
}

def xmem : Operand<i16> {
  let PrintMethod   = "printXMemOperand";
  let EncoderMethod = "getXMemOpValue";
  let MIOperandInfo = (ops NormRegs, i8imm);
}

//===----------------------------------------------------------------------===//
// Complex and other pattern definitions.
//===----------------------------------------------------------------------===//

def xaddr : ComplexPattern<iPTR, 2, "SelectXAddr", [], []>;
def iaddr : ComplexPattern<iPTR, 1, "SelectIAddr", [], []>;

//===----------------------------------------------------------------------===//
// Instruction list.
//===----------------------------------------------------------------------===//
let Defs = [ESP], Uses = [ESP] in {
  def ADJCALLSTACKDOWN : PseudoI<(outs), (ins i16imm:$amt),
    [(ZCPUcallseq_start timm:$amt)]>;
  def ADJCALLSTACKUP   : PseudoI<(outs), (ins i16imm:$amt1, i16imm:$amt2),
    [(ZCPUcallseq_end timm:$amt1, timm:$amt2)]>;
}

let usesCustomInserter = 1 in {
  def SELECT8  : PseudoI<(outs NormRegs:$dst),  (ins NormRegs:$src,  NormRegs:$src2,  cc:$cc),
    [(set NormRegs:$dst,  (ZCPUselectcc NormRegs:$src,  NormRegs:$src2,  imm:$cc))]>;
  def SELECT16 : PseudoI<(outs NormRegs:$dst), (ins NormRegs:$src, NormRegs:$src2, cc:$cc),
    [(set NormRegs:$dst, (ZCPUselectcc NormRegs:$src, NormRegs:$src2, imm:$cc))]>;
  def SHL8   : PseudoI<(outs NormRegs:$dst), (ins NormRegs:$src, NormRegs:$cnt),
    [(set NormRegs:$dst, (ZCPUshl NormRegs:$src, NormRegs:$cnt))]>;
  def LSHR8  : PseudoI<(outs NormRegs:$dst), (ins NormRegs:$src, NormRegs:$cnt),
    [(set NormRegs:$dst, (ZCPUlshr NormRegs:$src, NormRegs:$cnt))]>;
  def ASHR8  : PseudoI<(outs NormRegs:$dst), (ins NormRegs:$src, NormRegs:$cnt),
    [(set NormRegs:$dst, (ZCPUashr NormRegs:$src, NormRegs:$cnt))]>;
  def SHL16  : PseudoI<(outs NormRegs:$dst), (ins NormRegs:$src, NormRegs:$cnt),
    [(set NormRegs:$dst, (ZCPUshl NormRegs:$src, NormRegs:$cnt))]>;
  def LSHR16 : PseudoI<(outs NormRegs:$dst), (ins NormRegs:$src, NormRegs:$cnt),
    [(set NormRegs:$dst, (ZCPUlshr NormRegs:$src, NormRegs:$cnt))]>;
  def ASHR16 : PseudoI<(outs NormRegs:$dst), (ins NormRegs:$src, NormRegs:$cnt),
    [(set NormRegs:$dst, (ZCPUashr NormRegs:$src, NormRegs:$cnt))]>;
}
//===----------------------------------------------------------------------===//
//  Miscellaneous Instructions.
//===----------------------------------------------------------------------===//

let Defs = [EAX], Uses = [EAX] in {
  def CPL : I<0x2F, (outs), (ins), "cpl", [(set EAX, (not EAX))]>;
  def NEG : I<0x44, (outs), (ins), "neg", [(set EAX, (ineg EAX))]>, ED;
}

let Uses = [ESP] in 
  def ADD16rESP : I<0x39, (outs NormRegs:$dst), (ins), "add\t{$dst, ESP}", []>;
let Defs = [ESP] in
  def LD16ESPr  : I<0xF9, (outs), (ins NormRegs:$src), "ld\t{ESP, $src}", []>;

let Defs = [ESP], Uses = [ESP] in {
  let mayLoad = 1 in
    def POP16r  : IRp<0xC1, (outs NormRegs:$reg), (ins), "pop\t{$reg}", []>;
  let mayStore = 1 in
    def PUSH16r : IRp<0xC5, (outs), (ins NormRegs:$reg), "push\t{$reg}", []>;
}
//===----------------------------------------------------------------------===//
// Control Flow Instructions.
//===----------------------------------------------------------------------===//
let isCall = 1, Uses = [ESP] in {
  def CALL : II16<0xCD, (outs), (ins calltarget:$dst, variable_ops),
    "call\t{$dst}", [(ZCPUcall imm:$dst)]>;
}

let isReturn = 1, isTerminator = 1, isBarrier = 1 in
def RET : I<0xC9, (outs), (ins), "ret", [(ZCPUret)]>;

let isBranch = 1, isTerminator = 1 in {
  let isBarrier = 1 in
  def JP : II16<0xC3, (outs), (ins brtarget:$dst),
    "jp\t{$dst}", [(br bb:$dst)]>;
  let Uses = [EAX] in
  def JPCC : IRyI16<0xC2, (outs), (ins brtarget:$dst, cc:$Ry),
    "jp\t{$Ry, $dst}", [(ZCPUbrcc bb:$dst, imm:$Ry)]>;
}

//===----------------------------------------------------------------------===//
// Load Instructions.
//===----------------------------------------------------------------------===//
let neverHasSideEffects = 1 in
def LD8rr : IRyRz<0x40, (outs NormRegs:$dst), (ins NormRegs:$src),
  "ld\t{$dst, $src}", []>;

let isReMaterializable = 1, isAsCheapAsAMove = 1 in {
  def LD8ri  : IRyI8<0x06,  (outs NormRegs:$dst),  (ins i8imm:$src),
    "ld\t{$dst, $src}", [(set NormRegs:$dst, imm:$src)]>;
  def LD16ri : IRpI16<0x01, (outs NormRegs:$dst), (ins i16imm:$src),
    "ld\t{$dst, $src}", [(set NormRegs:$dst, imm:$src)]>;
}

let canFoldAsLoad = 1, isReMaterializable = 1 in {
  let Defs = [EAX] in
  def LD8Am : II16<0x3EAX, (outs), (ins i16imm:$src),
    "ld\t{EAX, ($src)}", [(set EAX, (load iaddr:$src))]>;
  let Uses = [EAX] in
  def LD8rHL : IRy<0x46, (outs NormRegs:$dst), (ins),
    "ld\t{$dst, (eax)}", [(set NormRegs:$dst, (load EAX))]>;
  def LD16rm : IRpI16<0x4B, (outs NormRegs:$Rp), (ins i16imm:$Imm),
    "ld\t{$Rp, ($Imm)}", [(set NormRegs:$Rp, (load iaddr:$Imm))]>, ED;
  def LD16rxm : PseudoI<(outs NormRegs:$dst), (ins xmem:$src),
    [(set NormRegs:$dst, (load xaddr:$src))]>;
  def LD8rxm : IRyI8<0x46, (outs NormRegs:$dst), (ins xmem:$src),
    "ld\t{$dst, ($src)}", [(set NormRegs:$dst, (load xaddr:$src))]>;
}
let Uses = [EAX] in
def LD8mA : II16<0x32, (outs), (ins i16imm:$dst),
  "ld\t{($dst), a}", [(store EAX, iaddr:$dst)]>;
let Uses = [EAX] in {
  def LD8HLr : IRz<0x70, (outs), (ins NormRegs:$src),
    "ld\t{(EAX), $src}", [(store NormRegs:$src, EAX)]>;
  def LD8HLi : II8<0x36, (outs), (ins i8imm:$src),
    "ld\t{(EAX), $src}", [(store (i8 imm:$src), EAX)]>;
}
def LD16mr : IRpI16<0x43, (outs), (ins i16imm:$Imm, NormRegs:$Rp),
  "ld\t{($Imm), $Rp}", [(store NormRegs:$Rp, iaddr:$Imm)]>, ED;
def LD16xmr : PseudoI<(outs), (ins xmem:$dst, NormRegs:$src),
  [(store NormRegs:$src, xaddr:$dst)]>;
def LD16xmi : PseudoI<(outs), (ins xmem:$dst, i16imm:$src),
  [(store (i16 imm:$src), xaddr:$dst)]>;
def LD8xmr : II8Rz<0x70, (outs), (ins xmem:$dst, NormRegs:$Rz),
  "ld\t{($dst), $Rz}", [(store NormRegs:$Rz, xaddr:$dst)]>;
def LD8xmi : II8I8<0x36, (outs), (ins xmem:$dst, i8imm:$Imm2),
  "ld\t{($dst), $Imm2}", [(store (i8 imm:$Imm2), xaddr:$dst)]>;
//===----------------------------------------------------------------------===//
// Arithmetic Instructions
//===----------------------------------------------------------------------===//

class ALUOpType<bits<3> val, SDNode ops, string asm> {
  field bits<3> Value = val;
  SDNode Node = ops;
  string Asm = asm;
}

def ALU_ADD : ALUOpType<0, add,   "add\tEAX, ">;
def ALU_ADC : ALUOpType<1, adde,  "adc\tEAX, ">;
def ALU_SUB : ALUOpType<2, sub,   "sub\t">;
def ALU_SBC : ALUOpType<3, sube,  "sbc\tEAX, ">;
def ALU_AND : ALUOpType<4, and,   "and\t">;
def ALU_XOR : ALUOpType<5, xor,   "xor\t">;
def ALU_OR  : ALUOpType<6, or,    "or\t">;
def ALU_CP  : ALUOpType<7, ZCPUcp, "cp\t">;


class ALUIr<ALUOpType alu, list<dag> pattern>
  : IRz<0x80, (outs), (ins NormRegs:$Rz), alu.Asm#"$Rz", pattern> {
  ALUOpType aluType = alu;
  let Inst{5-3} = aluType.Value;
}
class ALUIi<ALUOpType alu, list<dag> pattern>
  : II8<0xC6, (outs), (ins i8imm:$Imm), alu.Asm#"$Imm", pattern> {
  ALUOpType aluType = alu;
  let Inst{5-3} = aluType.Value;
}
class ALUIxm<ALUOpType alu, list<dag> pattern>
  : II8<0x86, (outs), (ins xmem:$Imm), alu.Asm#"($Imm)", pattern> {
  ALUOpType aluType = alu;
  let Inst{5-3} = aluType.Value;
}

multiclass ALUI<ALUOpType alu> {
  def r  : ALUIr<alu, [(set EAX, (alu.Node EAX, NormRegs:$Rz))]>;
  def i  : ALUIi<alu, [(set EAX, (alu.Node EAX, imm:$Imm))]>;
  def xm : ALUIxm<alu, [(set EAX, (alu.Node EAX, (load xaddr:$Imm)))]>;
}

multiclass ALUICP<ALUOpType alu> {
  def r  : ALUIr<alu, [(ZCPUcp EAX, NormRegs:$Rz)]>;
  def i  : ALUIi<alu, [(ZCPUcp EAX, imm:$Imm)]>;
  def xm : ALUIxm<alu, [(ZCPUcp EAX, (load xaddr:$Imm))]>;
}

class RSOpType<bits<3> val, SDNode ops, string asm> {
  field bits<3> Value = val;
  SDNode Node = ops;
  string Asm = asm;
}

def RS_RLC : RSOpType<0, ZCPUrlc, "rlc">;
def RS_RRC : RSOpType<1, ZCPUrrc, "rrc">;
def RS_RL  : RSOpType<2, ZCPUrl,  "rl">;
def RS_RR  : RSOpType<3, ZCPUrr,  "rr">;
def RS_SLA : RSOpType<4, ZCPUsla, "sla">;
def RS_SRA : RSOpType<5, ZCPUsra, "sra">;
def RS_SLL : RSOpType<6, ZCPUsll, "sll">;
def RS_SRL : RSOpType<7, ZCPUsrl, "srl">;

class RSIr<RSOpType rs>
  : IRz<0x00, (outs NormRegs:$dst), (ins NormRegs:$src), rs.Asm#"\t$src",
  [(set NormRegs:$dst, (rs.Node NormRegs:$src))]>, CB {
  let Inst{5-3} = rs.Value;
  let Constraints = "$src = $dst";
}

class RSIxm<RSOpType rs>
  : II8<0x06, (outs), (ins xmem:$Imm), rs.Asm#"\t($Imm)",
  [(store (rs.Node (i8 (load xaddr:$Imm))), xaddr:$Imm)]>, CB {
  let Inst{5-3} = rs.Value;
}

multiclass RSI<RSOpType rs> {
  def r  : RSIr<rs>;
  def xm : RSIxm<rs>;
}

let Uses = [EAX], Defs = [EAX] in {
  let isCommutable = 1 in {
    defm ADD8 : ALUI<ALU_ADD>;
    let Uses = [EAX] in
    defm ADC8 : ALUI<ALU_ADC>;
    defm AND8 : ALUI<ALU_AND>;
    defm XOR8 : ALUI<ALU_XOR>;
    defm OR8  : ALUI<ALU_OR>;
  }
  defm SUB8 : ALUI<ALU_SUB>;
  let Uses = [EAX] in
  defm SBC8 : ALUI<ALU_SBC>;
  let Defs = [EAX] in
  defm CP8  : ALUICP<ALU_CP>;
}

let Defs = [EAX] in {
  defm RLC8 : RSI<RS_RLC>;
  defm RRC8 : RSI<RS_RRC>;
  let Uses = [EAX] in {
    defm RL8  : RSI<RS_RL>;
    defm RR8  : RSI<RS_RR>;
  }
  defm SLA8 : RSI<RS_SLA>;
  defm SRA8 : RSI<RS_SRA>;
  defm SLL8 : RSI<RS_SLL>;
  defm SRL8 : RSI<RS_SRL>;
}

let Uses = [EAX], Defs = [EAX] in {
  let isCommutable = 1 in {
    def ADD16r : IRp<0x09, (outs), (ins NormRegs:$src),
      "add\t{hl, $src}", [(set HL, (add HL, NormRegs:$src))]>;
    let Uses = [EAX] in
    def ADC16r : IRp<0x4EAX, (outs), (ins NormRegs:$src),
      "adc\t{hl, $src}", [(set EAX, (adde EAX, NormRegs:$src))]>, ED;
  }
  let Uses = [EAX] in
  def SBC16r : IRp<0x42, (outs), (ins NormRegs:$src),
    "sbc\t{hl, $src}", [(set EAX, (sube EAX, NormRegs:$src))]>, ED;
}

let Constraints = "$src = $dst" in {
  let Defs = [EAX] in {
    def INC8r : IRy<0x04, (outs NormRegs:$dst), (ins NormRegs:$src),
      "inc\t{$src}", [(set NormRegs:$dst, (add NormRegs:$src, 1))]>;
    def DEC8r : IRy<0x05, (outs NormRegs:$dst), (ins NormRegs:$src),
      "dec\t{$src}", [(set NormRegs:$dst, (add NormRegs:$src, -1))]>;
    let Constraints = "" in {
      def INC8xm : II8<0x34, (outs), (ins xmem:$Imm),
        "inc\t{($Imm)}", [(store (add (i8 (load xaddr:$Imm)), 1), xaddr:$Imm)]>;
      def DEC8xm : II8<0x35, (outs), (ins xmem:$Imm),
        "dec\t{($Imm)}", [(store (add (i8 (load xaddr:$Imm)), -1), xaddr:$Imm)]>;
    }
  }
  def INC16r : IRp<0x03, (outs NormRegs:$dst), (ins NormRegs:$src),
    "inc\t{$src}", [(set NormRegs:$dst, (add NormRegs:$src, 1))]>;
  def DEC16r : IRp<0x0B, (outs NormRegs:$dst), (ins NormRegs:$src),
    "dec\t{$src}", [(set NormRegs:$dst, (add NormRegs:$src, -1))]>;
}

//===----------------------------------------------------------------------===//
// Non-Instruction Patterns.
//===----------------------------------------------------------------------===//

// add, sub
def : Pat<(addc EAX, NormRegs:$src), (ADD8r NormRegs:$src)>;
def : Pat<(addc EAX, imm:$src), (ADD8r imm:$src)>;

def : Pat<(addc EAX, NormRegs:$src), (ADD16r NormRegs:$src)>;

def : Pat<(subc EAX, NormRegs:$src), (SUB8r NormRegs:$src)>;
def : Pat<(subc EAX, imm:$src), (SUB8r imm:$src)>;

// calls
def : Pat<(ZCPUcall (i48 tglobaladdr:$dst)), (CALL tglobaladdr:$dst)>;
def : Pat<(ZCPUcall (i48 texternalsym:$dst)), (CALL texternalsym:$dst)>;

// neg
def : Pat<(add (not EAX), 1), (NEG)>;

// GlobalAddress
def : Pat<(ZCPUwrapper tglobaladdr:$dst), (LD16ri tglobaladdr:$dst)>;
