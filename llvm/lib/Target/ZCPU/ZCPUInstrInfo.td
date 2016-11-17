// ZCPUInstrInfo.td-Describe the ZCPU Instructions-*- tablegen -*-
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief ZCPU Instruction definitions.
///
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// ZCPU Instruction Predicate Definitions.
//===----------------------------------------------------------------------===//



//===----------------------------------------------------------------------===//
// ZCPU-specific DAG Node Types.
//===----------------------------------------------------------------------===//
def SDT_ZCPUCallSeqStart : SDCallSeqStart<[SDTCisVT<0, iPTR>]>;
def SDT_ZCPUCallSeqEnd :   SDCallSeqEnd<[SDTCisVT<0, iPTR>, SDTCisVT<1, iPTR>]>;

def ZCPUcallseq_start :SDNode<"ISD::CALLSEQ_START", SDT_ZCPUCallSeqStart,[SDNPHasChain, SDNPOutGlue]>;
def ZCPUcallseq_end :SDNode<"ISD::CALLSEQ_END", SDT_ZCPUCallSeqEnd,[SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

//===----------------------------------------------------------------------===//
// ZCPU-specific DAG Nodes.
//===----------------------------------------------------------------------===//
def addr : ComplexPattern<iPTR, 2, "SelectAddr", [frameindex], [SDNPWantParent]>;
//===----------------------------------------------------------------------===//
// ZCPU-specific Operands.
//===----------------------------------------------------------------------===//
def mem : Operand<iPTR> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops BothNormAndExtendedInt,i64imm);
  let EncoderMethod = "getMemEncoding";
}

//===----------------------------------------------------------------------===//
// ZCPU Instruction Format Definitions.
//===----------------------------------------------------------------------===//

include "ZCPUInstrFormats.td"



def i64immSExt32 : ImmLeaf<i64, [{ return isInt<32>(Imm); }]>;

//===----------------------------------------------------------------------===//
// Additional instructions.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Additional sets of instructions.
//===----------------------------------------------------------------------===//


include "zcpuInstr.td"
include "vectorInstr.td"
include "zgpuInstr.td"
include "zspuInstr.td"
include "zcpuArith.td"
include "copypasta.td"