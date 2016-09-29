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
class ZCPUReg<string n, bits<16> Enc = 0, int Cost = 0>
  : Register<n> {
  let Namespace  = "ZCPU";
  let HWEncoding = Enc;
  let CostPerUse = Cost;
}

//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//
//main Registers
def EAX : ZCPUReg<"eax", 0>;
def EBX : ZCPUReg<"ebx", 1>;
def ECX : ZCPUReg<"ecx", 2>;
def EDX : ZCPUReg<"edx", 3>;
def ESI : ZCPUReg<"esi", 4>;
def EDI : ZCPUReg<"edi", 5>;
def ESP : ZCPUReg<"esp", 6>;
def EBP : ZCPUReg<"ebp", 7>;
def IP : ZCPUReg<"ip", 8>;
//Extended Registers
def R0 : ZCPUReg<"R0", 0>;
def R1 : ZCPUReg<"R1", 1>;
def R2 : ZCPUReg<"R2", 2>;
def R3 : ZCPUReg<"R3", 3>;
def R4 : ZCPUReg<"R4", 4>;
def R5 : ZCPUReg<"R5", 5>;
def R6 : ZCPUReg<"R6", 6>;
def R7 : ZCPUReg<"R7", 7>;
def R8 : ZCPUReg<"R8", 8>;
def R9 : ZCPUReg<"R9", 9>;
def R10 : ZCPUReg<"R10", 10>;
def R11 : ZCPUReg<"R11", 11>;
def R12 : ZCPUReg<"R12", 12>;
def R13 : ZCPUReg<"R13", 13>;
def R14 : ZCPUReg<"R14", 14>;
def R15 : ZCPUReg<"R15", 15>;
def R16 : ZCPUReg<"R16", 16>;
def R17 : ZCPUReg<"R17", 17>;
def R18 : ZCPUReg<"R18", 18>;
def R19 : ZCPUReg<"R19", 19>;
def R20 : ZCPUReg<"R20", 20>;
def R21 : ZCPUReg<"R21", 21>;
def R22 : ZCPUReg<"R22", 22>;
def R23 : ZCPUReg<"R23", 23>;
def R24 : ZCPUReg<"R24", 24>;
def R25 : ZCPUReg<"R25", 25>;
def R26 : ZCPUReg<"R26", 26>;
def R27 : ZCPUReg<"R27", 27>;
def R28 : ZCPUReg<"R28", 28>;
def R30 : ZCPUReg<"R30", 30>;
def R31 : ZCPUReg<"R31", 31>;
//stack segment Registers
def CS : ZCPUReg<"cs", 1>;
def DS : ZCPUReg<"ds", 3>;
def SS : ZCPUReg<"ss", 2>;
def ES : ZCPUReg<"es", 0>;
def FS : ZCPUReg<"fs", 4>;
def GS : ZCPUReg<"gs", 5>;
def KS : ZCPUReg<"ks", 6>;
def LS : ZCPUReg<"ls", 7>;

//=====================
//register classes
//=====================
def GRZCPU : RegisterClass<"ZCPU", [i32], 64,
                         (add EAX,EBX,ECX, EDX,ESI,EDI,ESP,EBP,IP)>;
def SEGMENT_REG : RegisterClass<"ZCPU", [i16], 16, (add CS, DS, SS, ES, FS, GS)>;