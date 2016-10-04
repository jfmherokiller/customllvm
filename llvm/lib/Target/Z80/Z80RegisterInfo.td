//===- Z80RegisterInfo.td - Describe the Z80 Register Info ----*- tblgen -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Declaration that describes the Z80 register file
//===----------------------------------------------------------------------===//

class Z80Reg<string n, bits<16> Enc = 0, int Cost = 0>
  : Register<n> {
  let Namespace  = "Z80";
  let HWEncoding = Enc;
  let CostPerUse = Cost;
}

class Z80SubRegIndex<int size, int offset = 0> : SubRegIndex<size, offset> {
  let Namespace = "Z80";
}

def subreg_hi : Z80SubRegIndex<8, 8>;
def subreg_lo : Z80SubRegIndex<8, 0>;

class Z80RegWithSubRegs<string n, bits<16> Enc = 0, list<Register> subregs = [], int Cost = 0>
  : Z80Reg<n, Enc, Cost> {
  let SubRegs = subregs;
  let SubRegIndices = [subreg_hi, subreg_lo];
  let CoveredBySubRegs = 1;
}

class Z80Reg8Class<dag reglist>
  : RegisterClass<"Z80", [i8], 8, reglist> {
}

class Z80Reg16Class<dag reglist>
  : RegisterClass<"Z80", [i16], 8, reglist> {
}


//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//

// 8 bit registers
def A : Z80Reg<"eax", 7>;
def B : Z80Reg<"ebx", 0>;
def C : Z80Reg<"ecx", 1>;
def D : Z80Reg<"edx", 2>;
def E : Z80Reg<"r7", 3>;
def H : Z80Reg<"r8", 4>;
def L : Z80Reg<"r9", 5>;
def XH : Z80Reg<"r10", 4, 1>;
def XL : Z80Reg<"r11", 5, 1>;
def YH : Z80Reg<"r12", 4, 1>;
def YL : Z80Reg<"r13", 5, 1>;

// Flags register
def FLAGS : Z80Reg<"f">;

// 16 bit registers
def BC : Z80Reg<"R0", 0>;
def DE : Z80Reg<"R1", 1>;
def HL : Z80Reg<"R3",2>;
def AF : Z80Reg<"R4",3>;
def IX : Z80Reg<"R5", 4>;
def IY : Z80Reg<"R6", 5>;

def SP : Z80Reg<"esp">;
def PC : Z80Reg<"ip">;

def GR8  : Z80Reg8Class<(add A, B, C, D, E, H, L)>;
def GR16 : Z80Reg16Class<(add BC, DE, HL, IX, IY)>;
def BR16 : Z80Reg16Class<(add BC, DE, HL)>;
def IR16 : Z80Reg16Class<(add IX, IY)>;

def EXR16 : Z80Reg16Class<(add HL, DE)>;
