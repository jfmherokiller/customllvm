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

class ZCPUSubRegIndex<int size, int offset = 0> : SubRegIndex<size, offset> {
  let Namespace = "ZCPU";
}

def subreg_hi : ZCPUSubRegIndex<8, 8>;
def subreg_lo : ZCPUSubRegIndex<8, 0>;

class ZCPURegWithSubRegs<string n, bits<16> Enc = 0, list<Register> subregs = [], int Cost = 0>
  : ZCPUReg<n, Enc, Cost> {
  let SubRegs = subregs;
  let SubRegIndices = [subreg_hi, subreg_lo];
  let CoveredBySubRegs = 1;
}

class ZCPUReg8Class<dag reglist>
  : RegisterClass<"ZCPU", [i8], 8, reglist> {
}

class ZCPUReg16Class<dag reglist>
  : RegisterClass<"ZCPU", [i16], 8, reglist> {
}


//===----------------------------------------------------------------------===//
//  Registers
//===----------------------------------------------------------------------===//

// 8 bit registers
def A : ZCPUReg<"eax", 7>;
def B : ZCPUReg<"ebx", 0>;
def C : ZCPUReg<"ecx", 1>;
def D : ZCPUReg<"edx", 2>;
def E : ZCPUReg<"r7", 3>;
def H : ZCPUReg<"r8", 4>;
def L : ZCPUReg<"r9", 5>;
def XH : ZCPUReg<"r10", 4, 1>;
def XL : ZCPUReg<"r11", 5, 1>;
def YH : ZCPUReg<"r12", 4, 1>;
def YL : ZCPUReg<"r13", 5, 1>;

// Flags register
def FLAGS : ZCPUReg<"f">;

// 16 bit registers
def BC : ZCPUReg<"R0", 0>;
def DE : ZCPUReg<"R1", 1>;
def HL : ZCPUReg<"R3",2>;
def AF : ZCPUReg<"R4",3>;
def IX : ZCPUReg<"R5", 4>;
def IY : ZCPUReg<"R6", 5>;

def SP : ZCPUReg<"esp">;
def PC : ZCPUReg<"ip">;

def GR8  : ZCPUReg8Class<(add A, B, C, D, E, H, L)>;
def GR16 : ZCPUReg16Class<(add BC, DE, HL, IX, IY)>;
def BR16 : ZCPUReg16Class<(add BC, DE, HL)>;
def IR16 : ZCPUReg16Class<(add IX, IY)>;

def EXR16 : ZCPUReg16Class<(add HL, DE)>;
