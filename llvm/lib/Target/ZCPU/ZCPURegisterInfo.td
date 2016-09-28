//===-- ZCPURegisterInfo.td - ZCPU Register defs ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Declarations that describe the ZCPU register file
//===----------------------------------------------------------------------===//
class ZCPUReg<string n, bits<64> Enc = 0, int Cost = 0>
  : Register<n> {
  let Namespace  = "ZCPU";
  let HWEncoding = Enc;
  let CostPerUse = Cost;
}

//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//
//main Registers
def EAX : ZCPUReg<"eax", 0>
def EBX : ZCPUReg<"ebx", 3>
def ECX : ZCPUReg<"ecx", 1>
def EDX : ZCPUReg<"edx", 2>
def ESI : ZCPUReg<"esi", 6>
def EDI : ZCPUReg<"edi", 7>
def ESP : ZCPUReg<"esp", 4>
def EBP : ZCPUReg<"ebp", 5>
def IP : ZCPUReg<"ip", 8>
//Extended Registers
R0
R1
R2
R3
R4
R5
R6
R7
R8
R9
R10
R11
R12
R13
R14
R15
R16
R17
R18
R19
R20
R21
R22
R23
R24
R25
R26
R27
R28
R30
R31
//stack segment Registers
def CS : ZCPUReg<"cs", 1>;
def DS : ZCPUReg<"ds", 3>;
def SS : ZCPUReg<"ss", 2>;
def ES : ZCPUReg<"es", 0>;
def FS : ZCPUReg<"fs", 4>;
def GS : ZCPUReg<"gs", 5>;
//TODO: These Registers can suck it