


let isBranch = 1 in {
def JNE:I<001,(outs), (ins brtarget:$dst),[], "JNE\t$dst">; //Jump to PTR if result is not equal
def JNZ:I<001,(outs), (ins brtarget:$dst),[], "JNZ\t$dst">; //Jump to PTR if result is not zero
def JMP:I<002,(outs), (ins brtarget:$dst),[(br bb:$dst)], "JMP\t$dst">; //Jump to PTR
def JG:I<003,(outs), (ins brtarget:$dst),[], "JG\t$dst">; //Jump to PTR if result is greater
def JNLE:I<003,(outs), (ins brtarget:$dst),[], "JNLE\t$dst">; //Jump to PTR if result is not less or equal
def JGE:I<004,(outs), (ins brtarget:$dst),[], "JGE\t$dst">; //Jump to PTR if result is greater or equal
def JNL:I<004,(outs), (ins brtarget:$dst),[], "JNL\t$dst">; //Jump to PTR if result is not less
def JL:I<005,(outs), (ins brtarget:$dst),[], "JL\t$dst">; //Jump to PTR if result is less
def JNGE:I<005,(outs), (ins brtarget:$dst),[], "JNGE\t$dst">; //Jump to PTR if result is not greater or equal
def JLE:I<006,(outs), (ins brtarget:$dst),[], "JLE\t$dst">; //Jump to PTR if result is less or equal
def JNG:I<006,(outs), (ins brtarget:$dst),[], "JNG\t$dst">; //Jump to PTR if result is not greater
def JE:I<007,(outs), (ins brtarget:$dst),[], "JE\t$dst">; //Jump to PTR if result is equal
def JZ:I<007,(outs), (ins brtarget:$dst),[], "JZ\t$dst">; //Jump to PTR if result is zero
def JMPF:I<069,(outs), (ins),[], "JMPF">; //Jump to PTR in code segment CS
}

defm ADD:BinyOP<010,"add","ADD\t$src1,$src2">;  //X = X + Y
defm SUB:BinyOP<011,"sub","SUB\t$src1,$src2">; //X = X - Y
defm MUL:BinyOP<012,"mul","MUL\t$src1,$src2">; //X = X * Y
//defm DIV:BinyOP<012,"div","DIV\t$src1,$src2">; //X = X / Y
//def SUB:I<011,(outs BothNormAndExtended:$dst), (ins BothNormAndExtended:$src1,BothNormAndExtended:$src2),[(set BothNormAndExtended:$dst,(sub BothNormAndExtended:$src1,BothNormAndExtended:$src2))], "SUB\t$src1,$src2">; //X = X - Y
//def MUL:I<012,(outs BothNormAndExtended:$dst), (ins BothNormAndExtended:$src1,BothNormAndExtended:$src2),[(set BothNormAndExtended:$dst,(mul BothNormAndExtended:$src1,BothNormAndExtended:$src2))], "MUL\t$src1,$src2">; //X = X * Y
def DIV:I<011,(outs BothNormAndExtended:$dst), (ins BothNormAndExtended:$src1,BothNormAndExtended:$src2),[(set BothNormAndExtended:$dst,(sdiv BothNormAndExtended:$src1,BothNormAndExtended:$src2))], "DIV\t$src1,$src2">; //X = X - Y

def CPUID:I<008,(outs), (ins ),[], "CPUID">; //Write processor information variable IDX into EAX register
def PUSH:I<009,(outs), (ins),[], "PUSH">; //Push X onto processor stack

//def DIV:I<013,(outs), (ins ),[], "DIV">; //X = X / Y
def MOV:I<014,(outs), (ins ),[], "MOV">; //X = Y
def CMP:I<015,(outs), (ins ),[], "CMP">; //Compare X and Y. Use with conditional branching instructions
def RD:I<016,(outs), (ins ),[], "RD">; //Read value from memory by pointer PTR
def WD:I<017,(outs), (ins ),[], "WD">; //Write value to memory by pointer PTR
def MIN:I<018,(outs), (ins ),[], "MIN">; //Set X to smaller value out of X and Y
def MAX:I<019,(outs), (ins ),[], "MAX">; //Set X to bigger value out of X and Y
def INC:I<020,(outs), (ins ),[], "INC">; //Increase X by one
def DEC:I<021,(outs), (ins ),[], "DEC">; //Decrease X by one
def NEG:I<022,(outs BothNormAndExtended:$dst), (ins BothNormAndExtended:$src1),[(set BothNormAndExtended:$dst, (ineg BothNormAndExtended:$src1))], "NEG\t$dst">; //Change sign of X
def RAND:I<023,(outs), (ins),[], "RAND">; //Set X to random value

def LOOP:I<024,(outs), (ins),[], "LOOP">; //"If ECX is not set to 
def LOOPC:I<024,(outs), (ins),[], "LOOPC">; //"If ECX is not set to 
def LOOPA:I<025,(outs), (ins),[], "LOOPA">; //"If EAX is not set to 
def LOOPB:I<026,(outs), (ins),[], "LOOPB">; //"If EBX is not set to 
def LOOPD:I<027,(outs), (ins),[], "LOOPD">; //"If EDX is not set to

def SPG:I<028,(outs), (ins),[], "SPG">; //Make PAGE readonly
def CPG:I<029,(outs), (ins),[], "CPG">; //Make PAGE readable and writeable
def POP:I<030,(outs), (ins),[], "POP">; //Pop value off stack and write it into X
def CALL:I<031,(outs), (ins),[], "CALL">; //Call subroutine by address PTR
def BNOT:I<032,(outs), (ins),[], "BNOT">; //Flip all bits in the integer number
def FINT:I<033,(outs), (ins),[], "FINT">; //Force X to be an integer value
def FRND:I<034,(outs), (ins),[], "FRND">; //Round X to the nearest integer value
def RND:I<034,(outs), (ins),[], "RND">; //FRND
def FFRAC:I<035,(outs), (ins),[], "FFRAC">; //"Remove integer part of the 
def FINV:I<036,(outs), (ins),[], "FINV">; //X = 1 / X
def HALT:I<037,(outs), (ins),[], "HALT">; //Halt processor execution until PORT is written to
def FSHL:I<038,(outs), (ins),[], "FSHL">; //Multiply X by 2 (does not floor)
def FSHR:I<039,(outs), (ins),[], "FSHR">; //Divide X by 2 (does not floor)
let isReturn=1, isTerminator=1, hasDelaySlot=1, isBarrier=1, hasCtrlDep=1 in {
def RET:I<040,(outs), (ins i64imm:$adj, variable_ops),[(ZCPUretflag timm:$adj)], "RET">; //Return from a subroutine
def IRET:I<041,(outs), (ins i64imm:$adj),[(ZCPUiret timm:$adj)], "IRET">; //Return from an interrupt
def RETF:I<047,(outs), (ins),[], "RETF">; //Return from a far subroutine call
}
def STI:I<042,(outs), (ins),[], "STI">; //Enable interrupt handling
def CLI:I<043,(outs), (ins),[], "CLI">; //Disable interrupt handling
def STP:I<044,(outs), (ins),[], "STP">; //Enable protected mode
def CLP:I<045,(outs), (ins),[], "CLP">; //Disable protected mode

def STEF:I<048,(outs), (ins),[], "STEF">; //Enable extended mode
def CLEF:I<049,(outs), (ins),[], "CLEF">; //Disable extended mode
def AND:I<050,(outs), (ins),[], "AND">; //Logical AND between X and Y
def OR:I<051,(outs), (ins),[], "OR">; //Logical OR between X and Y
def XOR:I<052,(outs), (ins),[], "XOR">; //Logical XOR between X and Y
def FSIN:I<053,(outs), (ins),[], "FSIN">; //Write sine of X to Y
def FCOS:I<054,(outs), (ins),[], "FCOS">; //Write cosine of X to Y
def FTAN:I<055,(outs), (ins),[], "FTAN">; //Write tangent of X to Y
def FASIN:I<056,(outs), (ins),[], "FASIN">; //Write arcsine of X to Y
def FACOS:I<057,(outs), (ins),[], "FACOS">; //Write arccosine of X to Y
def FATAN:I<058,(outs), (ins),[], "FATAN">; //Write arctangent of X to Y
def MOD:I<059,(outs), (ins),[], "MOD">; //Write remainder of X/Y to Y
def BIT:I<060,(outs), (ins),[], "BIT">; //Test whether BIT of X is set. Use with conditional branching instructions
def SBIT:I<061,(outs), (ins),[], "SBIT">; //Set BIT of X
def CBIT:I<062,(outs), (ins),[], "CBIT">; //Clear BIT of X
def TBIT:I<063,(outs), (ins),[], "TBIT">; //Toggle BIT of X
def BAND:I<064,(outs), (ins),[], "BAND">; //Write result of binary AND between operands
def BOR:I<065,(outs), (ins),[], "BOR">; //Write result of binary OR between operands
def BXOR:I<066,(outs), (ins),[], "BXOR">; //Write result of binary XOR between operands
def BSHL:I<067,(outs), (ins),[], "BSHL">; //Shift bits of INT left by X
def BSHR:I<068,(outs), (ins),[], "BSHR">; //Shift bits of INT right by X

def NMIINT:I<070,(outs), (ins),[], "NMIINT">; //EXTINT
def EXTINT:I<070,(outs), (ins),[], "EXTINT">; //Call interrupt INTR as an external interrupt
def CNE:I<071,(outs), (ins),[], "CNE">; //Call subrotine if result is not equal
def CNZ:I<071,(outs), (ins),[], "CNZ">; //Call subrotine if result is not zero
def CG:I<073,(outs), (ins),[], "CG">; //Call subrotine if result is greater
def CNLE:I<073,(outs), (ins),[], "CNLE">; //Call subrotine if result is not less or equal
def CGE:I<074,(outs), (ins),[], "CGE">; //Call subrotine if result is greater or equal
def CNL:I<074,(outs), (ins),[], "CNL">; //Call subrotine if result is not less
def CL:I<075,(outs), (ins),[], "CL">; //Call subrotine if result is less
def CNGE:I<075,(outs), (ins),[], "CNGE">; //Call subrotine if result is not greater or equal
def CLE:I<076,(outs), (ins),[], "CLE">; //Call subrotine if result is less or equal
def CNG:I<076,(outs), (ins),[], "CNG">; //Call subrotine if result is not greater
def CE:I<077,(outs), (ins),[], "CE">; //Call subrotine if result is equal
def CZ:I<077,(outs), (ins),[], "CZ">; //Call subrotine if result is zero
def MCOPY:I<078,(outs), (ins),[], "MCOPY">; //Copy INT bytes from array pointed by ESI to EDI
def MXCHG:I<079,(outs), (ins),[], "MXCHG">; //Swap INT bytes between two arrays pointed by ESI and EDI
def FPWR:I<080,(outs), (ins),[], "FPWR">; //Raise X to power Y
def XCHG:I<081,(outs), (ins),[], "XCHG">; //Swap X and Y
def FLOG:I<082,(outs), (ins),[], "FLOG">; //FLN
def FLN:I<082,(outs), (ins),[], "FLN">; //Write logarithm (base e) of Y to X
def FLOG10:I<083,(outs), (ins),[], "FLOG10">; //Write logarithm (base 10) of Y to X
def IN:I<084,(outs), (ins),[], "IN">; //Input value from PORT to X
def OUT:I<085,(outs), (ins),[], "OUT">; //Write X to PORT
def FABS:I<086,(outs), (ins),[], "FABS">; //Write absolute value of Y to X
def FSGN:I<087,(outs), (ins),[], "FSGN">; //Write sign of Y to X
def FEXP:I<088,(outs), (ins),[], "FEXP">; //Write exponent of Y to X
def CALLF:I<089,(outs), (ins),[], "CALLF">; //Call subroutine by offset PTR in code segment CS
def FPI:I<090,(outs), (ins),[], "FPI">; //Set X to precise value of PI (3.1415926..)
def FE:I<091,(outs), (ins),[], "FE">; //Set X to precise value of E (2.7182818..)
def INT:I<092,(outs), (ins),[], "INT">; //Call interrupt INTR
def TPG:I<093,(outs), (ins),[], "TPG">; //"Test PAGE. Use branching instructions to test for zero on failur
def FCEIL:I<094,(outs), (ins),[], "FCEIL">; //Rounds X up to the next integer
def ERPG:I<095,(outs), (ins),[], "ERPG">; //Erase ROM page
def WRPG:I<096,(outs), (ins),[], "WRPG">; //Copy RAM page into ROM page
def RDPG:I<097,(outs), (ins),[], "RDPG">; //Read ROM page into RAM
def TIMERI:I<098,(outs), (ins),[], "TIMER">; //Set X to value of the internal processor timer
def LIDTR:I<099,(outs), (ins),[], "LIDTR">; //Set interrupt table pointer to PTR
let isIndirectBranch = 1 in {
def JNER:I<101,(outs), (ins),[], "JNER">; //Relative jump INT bytes forward if result is not equal
def JNZR:I<101,(outs), (ins),[], "JNZR">; //Relative jump INT bytes forward if result is not zero
def JMPR:I<102,(outs), (ins),[], "JMPR">; //Relative jump INT bytes forward
def JGR:I<103,(outs), (ins),[], "JGR">; //Relative jump INT bytes forward if result is greater
def JNLER:I<103,(outs), (ins),[], "JNLER">; //Relative jump INT bytes forward if result is not less or equal
def JGER:I<104,(outs), (ins),[], "JGER">; //Relative jump INT bytes forward if result is greater or equal
def JNLR:I<104,(outs), (ins),[], "JNLR">; //Relative jump INT bytes forward if result is not less
def JLR:I<105,(outs), (ins),[], "JLR">; //Relative jump INT bytes forward if result is less
def JNGER:I<105,(outs), (ins),[], "JNGER">; //Relative jump INT bytes forward if result is not greater or equal
def JLER:I<106,(outs), (ins),[], "JLER">; //Relative jump INT bytes forward if result is less or equal
def JNGR:I<106,(outs), (ins),[], "JNGR">; //Relative jump INT bytes forward if result is not greater
def JER:I<107,(outs), (ins),[], "JER">; //Relative jump INT bytes forward if result is equal
def JZR:I<107,(outs), (ins),[], "JZR">; //Relative jump INT bytes forward if result is zero
}
def LNEG:I<108,(outs), (ins),[], "LNEG">; //Logically negate X
def NMIRET:I<110,(outs), (ins),[], "NMIRET">; //EXTRET
def EXTRET:I<110,(outs), (ins),[], "EXTRET">; //Return from an external interrupt
def IDLE:I<111,(outs), (ins),[], "IDLE">; //Skip several processor cycles
def NOP:I<112,(outs), (ins),[], "NOP">; //Do nothing
def PUSHA:I<114,(outs), (ins),[], "PUSHA">; //Push all general purpose registers to stack
def POPA:I<115,(outs), (ins),[], "POPA">; //Pop all general purpose registers off stack
def STD2:I<116,(outs), (ins),[], "STD2">; //Enable hardware debug mode
def LEAVE:I<117,(outs), (ins),[], "LEAVE">; //Leave subroutine stack frame
def STM:I<118,(outs), (ins),[], "STM">; //Enable extended memory mode
def CLM:I<119,(outs), (ins),[], "CLM">; //Disable extended memory mode
def CPUGET:I<120,(outs), (ins),[], "CPUGET">; //Read internal processor register IDX
def CPUSET:I<121,(outs), (ins),[], "CPUSET">; //Write internal processor register IDX
def SPP:I<122,(outs), (ins),[], "SPP">; //Set page flag IDX
def CPP:I<123,(outs), (ins),[], "CPP">; //Clear page flag IDX
def SRL:I<124,(outs), (ins),[], "SRL">; //Set page runlevel to INT
def CRLI:I<125,(outs), (ins),[], "CRL">; //Write page runlevel to INT
def LEA:I<126,(outs), (ins),[], "LEA">; //Load absolute address fetched by operand Y into X
def BLOCK:I<127,(outs), (ins),[], "BLOCK">; //Make next instruction run on this block
def CMPAND:I<128,(outs), (ins),[], "CMPAND">; //"Compare X and 
def CMPOR:I<129,(outs), (ins),[], "CMPOR">; //"Compare X and 
def MSHIFT:I<130,(outs), (ins),[], "MSHIFT">; //Shift (and rotate) data pointed by ESI by OFFSET bytes
def SMAP:I<131,(outs), (ins),[], "SMAP">; //Remap PAGE1 to physical page PAGE2
def GMAP:I<132,(outs), (ins),[], "GMAP">; //Read what physical page PAGE is mapped to
def RSTACK:I<133,(outs), (ins),[], "RSTACK">; //Read value from stack at offset IDX (from address SS+IDX)
def SSTACK:I<134,(outs), (ins),[], "SSTACK">; //Write value to stack at offset IDX (to address SS+IDX)
def ENTER:I<135,(outs), (ins),[], "ENTER">; //Enter stack frame and allocate SIZE bytes on stack for local variables
def IRETP:I<136,(outs), (ins),[], "IRETP">; //"Set PTB
def EXTRETP:I<137,(outs), (ins),[], "EXTRETP">; //"Set PTB
def EXTRETA:I<140,(outs), (ins),[], "EXTRETA">; //Return from an external interrupt and restore R0-R31 registers
def EXTRETPA:I<141,(outs), (ins),[], "EXTRETPA">; //"Set PTB


