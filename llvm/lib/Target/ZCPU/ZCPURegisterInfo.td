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

class ZCPUReg<bits<16> Enc,string n> : Register<n> {
  let HWEncoding = Enc;
  let Namespace = "ZCPU";
}

class ZCPURegClass<list<ValueType> regTypes, int alignment, dag regList>
     : RegisterClass<"ZCPU", regTypes, alignment, regList>;

//===----------------------------------------------------------------------===//
// Registers
//===----------------------------------------------------------------------===//


//magical R Regs
foreach i = 0-31 in {
  def R#i  : ZCPUReg<!add( 68,#i),"%r"#i>;  // 32-bit
}
//magical R Regs

def IP: ZCPUReg<0,"IP">; //00 Instruction pointer
def EAX: ZCPUReg<1,"EAX">; //01 General purpose register A
def EBX: ZCPUReg<2,"EBX">; //02 General purpose register B
def ECX: ZCPUReg<3,"ECX">; //03 General purpose register C
def EDX: ZCPUReg<4,"EDX">; //04 General purpose register D
def ESI: ZCPUReg<5,"ESI">; //05 Source index
def EDI: ZCPUReg<6,"EDI">; //06 Destanation index
def ESP: ZCPUReg<7,"ESP">; //07 Stack pointer
def EBP: ZCPUReg<8,"EBP">; //08 Base pointer
def ESZ: ZCPUReg<9,"ESZ">; //09 Stack size
//segment regs
def CS: ZCPUReg<16,"CS">; //16 Code segment
def SS: ZCPUReg<17,"SS">; //17 Stack segment
def DS: ZCPUReg<18,"DS">; //18 Data segment
def ES: ZCPUReg<19,"ES">; //19 Extra segment
def GS: ZCPUReg<20,"GS">; //20 User segment
def FS: ZCPUReg<21,"FS">; //21 User segment
def KS: ZCPUReg<22,"KS">; //22 Key segment
def LS: ZCPUReg<23,"LS">; //23 Library segment
//segment regs
def IDTR: ZCPUReg<24,"IDTR">; //24 Interrupt descriptor table pointer
def CMPR: ZCPUReg<25,"CMPR">; //25 Comparsion result register
def XEIP: ZCPUReg<26,"XEIP">; //26 Pointer to start of currently executed instruction
def LADD: ZCPUReg<27,"LADD">; //27 Current interrupt code
def LINT: ZCPUReg<28,"LINT">; //28 Current interrupt number
def TMR: ZCPUReg<29,"TMR">; //29 Instruction/cycle counter
def TIMER: ZCPUReg<30,"TIMER">; //30 Internal precise timer
def CPAGE: ZCPUReg<31,"CPAGE">; //31 Current page number
def IF: ZCPUReg<32,"IF">; //32 Interrupts enabled flag
def PF: ZCPUReg<33,"PF">; //33 Protected mode flag
def EF: ZCPUReg<34,"EF">; //34 Extended mode flag
def NIF: ZCPUReg<35,"NIF">; //35 Next cycle interrupt enabled flag state
def MF: ZCPUReg<36,"MF">; //36 Extended memory mapping flag
def PTBL: ZCPUReg<37,"PTBL">; //37 Page table offset
def PTBE: ZCPUReg<38,"PTBE">; //38 Page table number of entries
def PCAP: ZCPUReg<39,"PCAP">; //39 Processor paging system capability
def RQCAP: ZCPUReg<40,"RQCAP">; //40 Processor delayed memory request capability
def PPAGE: ZCPUReg<41,"PPAGE">; //41 Previous page ID
def MEMRQ: ZCPUReg<42,"MEMRQ">; //42 Type of the memory request
def RAMSize: ZCPUReg<43,"RAMSize">; //43 Amount of internal memory
def External: ZCPUReg<44,"External">; //44 External I/O operation
def BusLock: ZCPUReg<45,"BusLock">; //45 Is bus locked for read/write
def Idle: ZCPUReg<46,"Idle">; //46 Should CPU skip some cycles
def INTR: ZCPUReg<47,"INTR">; //47 Handling an interrupt
def SerialNo: ZCPUReg<48,"SerialNo">; //48 Processor serial number
def CODEBYTES: ZCPUReg<49,"CODEBYTES">; //49 Amount of bytes executed so far
def BPREC: ZCPUReg<50,"BPREC">; //50 Binary precision level
def IPREC: ZCPUReg<51,"IPREC">; //51 Integer precision level
def NIDT: ZCPUReg<52,"NIDT">; //52 Number of interrupt descriptor table entries
def BlockStart: ZCPUReg<53,"BlockStart">; //53 Start offset of the block
def BlockSize: ZCPUReg<54,"BlockSize">; //54 Block size
def VMODE: ZCPUReg<55,"VMODE">; //55 Vector mode (2: 2D, 3: 3D)
def XTRL: ZCPUReg<56,"XTRL">; //56 Runlevel for external memory access
def HaltPort: ZCPUReg<57,"HaltPort">; //57 Halt until this port changes value
def HWDEBUG: ZCPUReg<58,"HWDEBUG">; //58 Hardware debug mode active
def DBGSTATE: ZCPUReg<59,"DBGSTATE">; //59 Hardware debug mode state
def DBGADDR: ZCPUReg<60,"DBGADDR">; //60 Hardware debug mode address/parameter
def CRL: ZCPUReg<61,"CRL">; //61 Current runlevel
def TimerDT: ZCPUReg<62,"TimerDT">; //62 Current timer discrete step
def MEMADDR: ZCPUReg<63,"MEMADDR">; //63 Address reqested by the memory operation
def TimerMode: ZCPUReg<64,"TimerMode">; //64 Timer mode (off, instructions, seconds)
def TimerRate: ZCPUReg<65,"TimerRate">; //65 Timer rate
def TimerPrevTime: ZCPUReg<66,"TimerPrevTime">; //66 Previous timer fire time
def TimerAddress: ZCPUReg<67,"TimerAddress">; //67 Number of external interrupt to call when timer fires

//def RM: ZCPUReg<"RM">;
def ARGUMENTS : ZCPUReg<99,"ARGUMENTS">;

//===----------------------------------------------------------------------===//
//  Register classes
//===----------------------------------------------------------------------===//


def GPRInt : ZCPURegClass<[i64], 64, (add IP,EAX,EBX,ECX,EDX,ESI,EDI,ESP,EBP)>;
def GPRFloat : ZCPURegClass<[f64], 64, (add EAX,EBX,ECX,EDX,ESI,EDI,ESP,EBP)>;
def BothNormAndExtendedInt: ZCPURegClass<[i64],64,(add EAX,EBX,ECX,EDX,ESI,EDI,ESP,EBP,(sequence "R%u", 0, 31))>;
def BothNormAndExtendedFloat: ZCPURegClass<[f64],64,(add EAX,EBX,ECX,EDX,ESI,EDI,ESP,EBP,(sequence "R%u", 0, 31))>;
def BothNormAndExtendedInt32: ZCPURegClass<[i32],32,(add EAX,EBX,ECX,EDX,ESI,EDI,ESP,EBP,(sequence "R%u", 0, 31))>;
//def BothNormAndExtended: ZCPURegClass<[i64,f64],64,(add EAX,EBX,ECX,EDX,ESI,EDI,ESP,EBP,(sequence "R%u", 0, 31))>;
def ExtendedGPRFloat:ZCPURegClass<[f64], 64,(sequence "R%u", 0, 31)>;
def ExtendedGPRInt:ZCPURegClass<[f64], 64,(sequence "R%u", 0, 31)>;

def SegmentRegs : ZCPURegClass<[i64],64, (add CS,SS,DS,ES,GS,FS,KS,LS)>;