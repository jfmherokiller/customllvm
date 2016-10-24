//===- ZCPUInstrInfo.td - Target Description for ZCPU Target -*- tablegen -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the ZCPU implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//
include "ZCPUInstrFormats.td"

defm ADD : BinaryInt<add>;
defm SUB : BinaryInt<sub>;
defm MUL : BinaryInt<mul>;
defm SDIV : BinaryInt<sdiv>;
defm UDIV : BinaryInt<udiv>;
defm SREM : BinaryInt<srem>;
defm UREM : BinaryInt<urem>;
defm AND : BinaryInt<and>;
defm IOR : BinaryInt<or>;
defm XOR : BinaryInt<xor>;
defm SHL : BinaryInt<shl>;
defm SHR : BinaryInt<srl>;
defm SAR : BinaryInt<sra>;
