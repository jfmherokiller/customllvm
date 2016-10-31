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
def SDT_WebAssemblyCallSeqStart : SDCallSeqStart<[SDTCisVT<0, iPTR>]>;
def SDT_WebAssemblyCallSeqEnd :
    SDCallSeqEnd<[SDTCisVT<0, iPTR>, SDTCisVT<1, iPTR>]>;

def ZCPUcallseq_start :
    SDNode<"ISD::CALLSEQ_START", SDT_WebAssemblyCallSeqStart,
           [SDNPHasChain, SDNPOutGlue]>;
def ZCPUcallseq_end :
    SDNode<"ISD::CALLSEQ_END", SDT_WebAssemblyCallSeqEnd,
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

def NOP : I<(outs), (ins),[] ,"NOP">;

def LEA : I<(outs), (ins), [],"LEA">;
def STEF: I<(outs), (ins), [],"STEF">;
def CLEF: I<(outs), (ins),[] ,"CLEF">;
def TIMER: I<(outs), (ins),[], "TIMER">;
def MOV: I<(outs), (ins),[], "MOV">;
def RET: I<(outs), (ins),[], "RET">;
