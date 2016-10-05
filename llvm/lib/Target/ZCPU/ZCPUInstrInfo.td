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
//===----------------------------------------------------------------------===//
//  Miscellaneous Instructions.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Control Flow Instructions.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Load Instructions.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Arithmetic Instructions
//===----------------------------------------------------------------------===//


//===----------------------------------------------------------------------===//
// Non-Instruction Patterns.
//===----------------------------------------------------------------------===//

// add, sub


// calls


// GlobalAddress

