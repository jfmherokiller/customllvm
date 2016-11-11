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
    SDNode<"ISD::CALLSEQ_START", SDT_ZCPUCallSeqStart,
           [SDNPHasChain, SDNPOutGlue]>;
def ZCPUcallseq_end :
    SDNode<"ISD::CALLSEQ_END", SDT_ZCPUCallSeqEnd,
           [SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;

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

//===----------------------------------------------------------------------===//
// Additional instructions.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Additional sets of instructions.
//===----------------------------------------------------------------------===//
def SDT_ZCPUArgument : SDTypeProfile<1, 1, [SDTCisVT<1, i64>]>;
def SDTZCPURet     : SDTypeProfile<0, -1, []>;
def ZCPUretflag : SDNode<"ZCPUISD::RET_FLAG", SDTZCPURet,[SDNPHasChain]>;
def ZCPUiret : SDNode<"ZCPUISD::IRET", SDTZCPURet,[SDNPHasChain]>;
def immSExt64  : PatLeaf<(imm), [{ return isInt<64>(N->getSExtValue()); }]>;

include "zcpuInstr.td"
include "vectorInstr.td"
include "zgpuInstr.td"
include "zspuInstr.td"