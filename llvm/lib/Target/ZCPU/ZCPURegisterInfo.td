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


//magical R Regs
foreach i = 0-31 in {
  def R#i  : ZCPUReg<"%r"#i>;  // 32-bit
}
//magical R Regs

def IP: ZCPUReg<"IP">; //00 Instruction pointer
def EAX: ZCPUReg<"EAX">; //01 General purpose register A
def EBX: ZCPUReg<"EBX">; //02 General purpose register B
def ECX: ZCPUReg<"ECX">; //03 General purpose register C
def EDX: ZCPUReg<"EDX">; //04 General purpose register D
def ESI: ZCPUReg<"ESI">; //05 Source index
def EDI: ZCPUReg<"EDI">; //06 Destanation index
def ESP: ZCPUReg<"ESP">; //07 Stack pointer
def EBP: ZCPUReg<"EBP">; //08 Base pointer
def ESZ: ZCPUReg<"ESZ">; //09 Stack size
//segment regs
def CS: ZCPUReg<"CS">; //16 Code segment
def SS: ZCPUReg<"SS">; //17 Stack segment
def DS: ZCPUReg<"DS">; //18 Data segment
def ES: ZCPUReg<"ES">; //19 Extra segment
def GS: ZCPUReg<"GS">; //20 User segment
def FS: ZCPUReg<"FS">; //21 User segment
def KS: ZCPUReg<"KS">; //22 Key segment
def LS: ZCPUReg<"LS">; //23 Library segment
//segment regs
def IDTR: ZCPUReg<"IDTR">; //24 Interrupt descriptor table pointer
def CMPR: ZCPUReg<"CMPR">; //25 Comparsion result register
def XEIP: ZCPUReg<"XEIP">; //26 Pointer to start of currently executed instruction
def LADD: ZCPUReg<"LADD">; //27 Current interrupt code
def LINT: ZCPUReg<"LINT">; //28 Current interrupt number
def TMR: ZCPUReg<"TMR">; //29 Instruction/cycle counter
def TIMER: ZCPUReg<"TIMER">; //30 Internal precise timer
def CPAGE: ZCPUReg<"CPAGE">; //31 Current page number
def IF: ZCPUReg<"IF">; //32 Interrupts enabled flag
def PF: ZCPUReg<"PF">; //33 Protected mode flag
def EF: ZCPUReg<"EF">; //34 Extended mode flag
def NIF: ZCPUReg<"NIF">; //35 Next cycle interrupt enabled flag state
def MF: ZCPUReg<"MF">; //36 Extended memory mapping flag
def PTBL: ZCPUReg<"PTBL">; //37 Page table offset
def PTBE: ZCPUReg<"PTBE">; //38 Page table number of entries
def PCAP: ZCPUReg<"PCAP">; //39 Processor paging system capability
def RQCAP: ZCPUReg<"RQCAP">; //40 Processor delayed memory request capability
def PPAGE: ZCPUReg<"PPAGE">; //41 Previous page ID
def MEMRQ: ZCPUReg<"MEMRQ">; //42 Type of the memory request
def RAMSize: ZCPUReg<"RAMSize">; //43 Amount of internal memory
def External: ZCPUReg<"External">; //44 External I/O operation
def BusLock: ZCPUReg<"BusLock">; //45 Is bus locked for read/write
def Idle: ZCPUReg<"Idle">; //46 Should CPU skip some cycles
def INTR: ZCPUReg<"INTR">; //47 Handling an interrupt
def SerialNo: ZCPUReg<"SerialNo">; //48 Processor serial number
def CODEBYTES: ZCPUReg<"CODEBYTES">; //49 Amount of bytes executed so far
def BPREC: ZCPUReg<"BPREC">; //50 Binary precision level
def IPREC: ZCPUReg<"IPREC">; //51 Integer precision level
def NIDT: ZCPUReg<"NIDT">; //52 Number of interrupt descriptor table entries
def BlockStart: ZCPUReg<"BlockStart">; //53 Start offset of the block
def BlockSize: ZCPUReg<"BlockSize">; //54 Block size
def VMODE: ZCPUReg<"VMODE">; //55 Vector mode (2: 2D, 3: 3D)
def XTRL: ZCPUReg<"XTRL">; //56 Runlevel for external memory access
def HaltPort: ZCPUReg<"HaltPort">; //57 Halt until this port changes value
def HWDEBUG: ZCPUReg<"HWDEBUG">; //58 Hardware debug mode active
def DBGSTATE: ZCPUReg<"DBGSTATE">; //59 Hardware debug mode state
def DBGADDR: ZCPUReg<"DBGADDR">; //60 Hardware debug mode address/parameter
def CRL: ZCPUReg<"CRL">; //61 Current runlevel
def TimerDT: ZCPUReg<"TimerDT">; //62 Current timer discrete step
def MEMADDR: ZCPUReg<"MEMADDR">; //63 Address reqested by the memory operation
def TimerMode: ZCPUReg<"TimerMode">; //64 Timer mode (off, instructions, seconds)
def TimerRate: ZCPUReg<"TimerRate">; //65 Timer rate
def TimerPrevTime: ZCPUReg<"TimerPrevTime">; //66 Previous timer fire time
def TimerAddress: ZCPUReg<"TimerAddress">; //67 Number of external interrupt to call when timer fires

def RM: ZCPUReg<"RM">;

def FP32 : ZCPUReg<"%FP32">;
def FP64 : ZCPUReg<"%FP64">;
def SP32 : ZCPUReg<"%SP32">;
def SP64 : ZCPUReg<"%SP64">;

// The register allocation framework requires register classes have at least
// one register, so we define a few for the floating point register classes
// since we otherwise don't need a physical register in those classes.
def F32_0 : ZCPUReg<"%f32.0">;
def F64_0 : ZCPUReg<"%f64.0">;


//===----------------------------------------------------------------------===//
//  Register classes
//===----------------------------------------------------------------------===//

def I32 : ZCPURegClass<[i32], 32, (add FP32, SP32,ESP,EAX, (sequence "R%u", 0, 31))>;
def I64 : ZCPURegClass<[i64], 64, (add FP64, SP64)>;
def F32 : ZCPURegClass<[f32], 32, (add F32_0)>;
def F64 : ZCPURegClass<[f64], 64, (add F64_0)>;
def GPRInt : ZCPURegClass<[i64], 64, (add IP,EAX,EBX,ECX,EDX,ESI,EDI,ESP,EBP)>;
def GPRFloat : ZCPURegClass<[f64], 64, (add EAX,EBX,ECX,EDX,ESI,EDI,ESP,EBP)>;

def ExtendedGPRFloat:ZCPURegClass<[f64], 64,(sequence "R%u", 0, 31)>;
def ExtendedGPRInt:ZCPURegClass<[f64], 64,(sequence "R%u", 0, 31)>;

def SegmentRegs : ZCPURegClass<[i64],32, (add CS,SS,DS,ES,GS,FS,KS,LS)>;