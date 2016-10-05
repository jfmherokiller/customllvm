//===- ZCPURegisterInfo.td - Describe the ZCPU Register Info ----*- tblgen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Declaration that describes the ZCPU register file
//===----------------------------------------------------------------------===//

class ZCPUReg<string n, bits<16> Enc = 0, int Cost = 0>
  : Register<n> {
  let Namespace  = "ZCPU";
  let HWEncoding = Enc;
  let CostPerUse = Cost;
}

//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//


def EAX : ZCPUReg<"eax", 0>;
def EBX : ZCPUReg<"ebx", 1>;
def ECX : ZCPUReg<"ecx", 2>;
def EDX : ZCPUReg<"edx", 3>;
def ESI : ZCPUReg<"esi", 4>;
def EDI : ZCPUReg<"edi", 5>;


def FLAGS : ZCPUReg<"f">;


def ESP : ZCPUReg<"esp">;
def EBP : ZCPUReg<"ebp">;
def IP : ZCPUReg<"ip">;

def NormRegs : RegisterClass<"ZCPU", [i48], 48, (add EAX, EBX, ECX, EDX, ESI, EDI)>;
def ExtraRegs : RegisterClass<"ZCPU", [i48], 48, (sequence "R%u", 0, 31)>;