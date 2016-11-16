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

def ZCPUcallseq_start :
    SDNode<"ISD::CALLSEQ_START", SDT_ZCPUCallSeqStart,[SDNPHasChain, SDNPOutGlue]>;
def ZCPUcallseq_end :SDNode<"ISD::CALLSEQ_END", SDT_ZCPUCallSeqEnd,[SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

//===----------------------------------------------------------------------===//
// ZCPU-specific DAG Nodes.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// ZCPU-specific Operands.
//===----------------------------------------------------------------------===//


//===----------------------------------------------------------------------===//
// ZCPU Instruction Format Definitions.
//===----------------------------------------------------------------------===//

include "ZCPUInstrFormats.td"

def loadi32 : PatFrag<(ops node:$ptr), (i32 (unindexedload node:$ptr)), [{
  LoadSDNode *LD = cast<LoadSDNode>(N);
  ISD::LoadExtType ExtType = LD->getExtensionType();
  if (ExtType == ISD::NON_EXTLOAD)
    return true;
  if (ExtType == ISD::EXTLOAD)
    return LD->getAlignment() >= 4 && !LD->isVolatile();
  return false;
}]>;

// An 'and' node with a single use.
def and_su : PatFrag<(ops node:$lhs, node:$rhs), (and node:$lhs, node:$rhs), [{
  return N->hasOneUse();
}]>;
// An 'srl' node with a single use.
def srl_su : PatFrag<(ops node:$lhs, node:$rhs), (srl node:$lhs, node:$rhs), [{
  return N->hasOneUse();
}]>;
// An 'trunc' node with a single use.
def trunc_su : PatFrag<(ops node:$src), (trunc node:$src), [{
  return N->hasOneUse();
}]>;
def i64immSExt32 : ImmLeaf<i64, [{ return isInt<32>(Imm); }]>;
def loadi64  : PatFrag<(ops node:$ptr), (i64 (load node:$ptr))>;
def loadf64  : PatFrag<(ops node:$ptr), (f64 (load node:$ptr))>;
//===----------------------------------------------------------------------===//
// Additional instructions.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Additional sets of instructions.
//===----------------------------------------------------------------------===//

include "copypasta.td"
include "zcpuInstr.td"
include "vectorInstr.td"
include "zgpuInstr.td"
include "zspuInstr.td"
include "zcpuArith.td"
