//ZCPURegisterInfo.td-Describe the ZCPU Registers -*- tablegen -*-
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file describes the ZCPU register classes and some nominal
/// physical registers.
///
//===----------------------------------------------------------------------===//

class ZCPUReg<string n> : Register<n> {
  let Namespace = "ZCPU";
}

class ZCPURegClass<list<ValueType> regTypes, int alignment, dag regList>
     : RegisterClass<"ZCPU", regTypes, alignment, regList>;

//===----------------------------------------------------------------------===//
// Registers
//===----------------------------------------------------------------------===//

// Special registers used as the frame and stack pointer.
//
// ZCPU may someday supports mixed 32-bit and 64-bit heaps in the same
// application, which requires separate width FP and SP.
foreach i = 0-31 in {
  def R#i  : ZCPUReg<"%r"#i>;  // 32-bit
}

//segment regs
def CS: ZCPUReg<"CS">;  //16 Code segment
def SS: ZCPUReg<"SS">;  //17 Stack segment
def DS: ZCPUReg<"DS">;  //18 Data segment
def ES: ZCPUReg<"ES">;  //19 Extra segment
def GS: ZCPUReg<"GS">;  //20 User segment
def FS: ZCPUReg<"FS">;  //21 User segment
def KS: ZCPUReg<"KS">;  //22 Key segment
def LS: ZCPUReg<"LS">;  //23 Library segment
//segment regs
//readonlyregs
def XEIP: ZCPUReg<"XEIP">;
def CPAGE: ZCPUReg<"CPAGE">; //31 Current page number
def PPAGE: ZCPUReg<"PPAGE">; //41 Previous page ID
def SerialNo: ZCPUReg<"SerialNo">; //48 Processor serial number
def CODEBYTES: ZCPUReg<"CODEBYTES">;
def TimerDT: ZCPUReg<"TimerDT">; //62 Current timer discrete step
def RAMSize: ZCPUReg<"RAMSize">; //43 Amount of internal memory
//readonlyregs
def ESP  : ZCPUReg<"ESP">;
def EAX  : ZCPUReg<"EAX">;
def FP32 : ZCPUReg<"%FP32">;
def FP64 : ZCPUReg<"%FP64">;
def SP32 : ZCPUReg<"%SP32">;
def SP64 : ZCPUReg<"%SP64">;

// The register allocation framework requires register classes have at least
// one register, so we define a few for the floating point register classes
// since we otherwise don't need a physical register in those classes.
def F32_0 : ZCPUReg<"%f32.0">;
def F64_0 : ZCPUReg<"%f64.0">;

// The expression stack "register". This is an opaque entity which serves to
// order uses and defs that must remain in LIFO order.
def EXPR_STACK : ZCPUReg<"STACK">;

// The incoming arguments "register". This is an opaque entity which serves to
// order the ARGUMENT instructions that are emulating live-in registers and
// must not be scheduled below other instructions.
def ARGUMENTS : ZCPUReg<"ARGUMENTS">;

//===----------------------------------------------------------------------===//
//  Register classes
//===----------------------------------------------------------------------===//

def I32 : ZCPURegClass<[i32], 32, (add FP32, SP32,ESP,EAX, (sequence "R%u", 0, 31))>;
def I64 : ZCPURegClass<[i64], 64, (add FP64, SP64)>;
def F32 : ZCPURegClass<[f32], 32, (add F32_0)>;
def F64 : ZCPURegClass<[f64], 64, (add F64_0)>;
