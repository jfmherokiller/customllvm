//===-- ZCPURegisterInfo.td - ZCPU Register defs -----------*- tablegen -*-===//
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


// We have banks of 16 registers each.
class ZCPUReg<bits<16> Enc, string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "ZCPU";
}

// ZCPU CPU Registers
class ZCPUGPRReg<bits<16> Enc, string n> : ZCPUReg<Enc, n>;

// Co-processor 0 Registers
class ZCPUC0Reg<bits<16> Enc, string n> : ZCPUReg<Enc, n>;

//===----------------------------------------------------------------------===//
//@Registers
//===----------------------------------------------------------------------===//
// The register string, such as "9" or "gp" will show on "llvm-objdump -d"
//@ All registers definition
let Namespace = "ZCPU" in {
  //@ General Purpose Registers
  def ZERO : ZCPUGPRReg<0,  "zero">, DwarfRegNum<[0]>;
  def AT   : ZCPUGPRReg<1,  "1">,    DwarfRegNum<[1]>;
  def V0   : ZCPUGPRReg<2,  "2">,    DwarfRegNum<[2]>;
  def V1   : ZCPUGPRReg<3,  "3">,    DwarfRegNum<[3]>;
  def A0   : ZCPUGPRReg<4,  "4">,    DwarfRegNum<[4]>;
  def A1   : ZCPUGPRReg<5,  "5">,    DwarfRegNum<[5]>;
  def T9   : ZCPUGPRReg<6,  "t9">,   DwarfRegNum<[6]>;
  def T0   : ZCPUGPRReg<7,  "7">,    DwarfRegNum<[7]>;
  def T1   : ZCPUGPRReg<8,  "8">,    DwarfRegNum<[8]>;
  def S0   : ZCPUGPRReg<9,  "9">,    DwarfRegNum<[9]>;
  def S1   : ZCPUGPRReg<10, "10">,   DwarfRegNum<[10]>;
  def GP   : ZCPUGPRReg<11, "gp">,   DwarfRegNum<[11]>;
  def FP   : ZCPUGPRReg<12, "fp">,   DwarfRegNum<[12]>;
  def SP   : ZCPUGPRReg<13, "sp">,   DwarfRegNum<[13]>;
  def LR   : ZCPUGPRReg<14, "lr">,   DwarfRegNum<[14]>;
  def SW   : ZCPUGPRReg<15, "sw">,   DwarfRegNum<[15]>;
//  def MAR  : Register< 16, "mar">,  DwarfRegNum<[16]>;
//  def MDR  : Register< 17, "mdr">,  DwarfRegNum<[17]>;

//#if CH >= CH4_1 1
  // Hi/Lo registers number and name
  def HI   : ZCPUReg<0, "ac0">, DwarfRegNum<[18]>;
  def LO   : ZCPUReg<0, "ac0">, DwarfRegNum<[19]>;
//#endif
  def PC   : ZCPUC0Reg<0, "pc">,  DwarfRegNum<[20]>;
  def EPC  : ZCPUC0Reg<1, "epc">, DwarfRegNum<[21]>;
}

//===----------------------------------------------------------------------===//
//@Register Classes
//===----------------------------------------------------------------------===//

def CPURegs : RegisterClass<"ZCPU", [i32], 32, (add
  // Reserved
  ZERO, AT, 
  // Return Values and Arguments
  V0, V1, A0, A1, 
  // Not preserved across procedure calls
  T9, T0, T1,
  // Callee save
  S0, S1,
  // Reserved
  GP, FP, 
  SP, LR, SW)>;

//#if CH >= CH4_1 2
// Hi/Lo Registers class
def HILO   : RegisterClass<"ZCPU", [i32], 32, (add HI, LO)>;
//#endif

//@Status Registers class
def SR     : RegisterClass<"ZCPU", [i32], 32, (add SW)>;

//@Co-processor 0 Registers class
def C0Regs : RegisterClass<"ZCPU", [i32], 32, (add PC, EPC)>;
