
// Call sequence markers. These have an immediate which represents the amount of
// stack space to allocate or free, which is used for varargs lowering.

//let Uses = [ESP], Defs = [ESP], isCodeGenOnly = 1 in {
//def ADJCALLSTACKDOWN : IPsudo<(outs), (ins i32imm:$amt),
//                         [(ZCPUcallseq_start timm:$amt)]>;
//def ADJCALLSTACKUP : IPsudo<(outs), (ins i32imm:$amt, i32imm:$amt2),
//                       [(ZCPUcallseq_end timm:$amt, timm:$amt2)]>;
//} // isCodeGenOnly = 1

def RetCC_ZCPU : CallingConv<[
  CCIfType<[i8, i16, i32], CCPromoteToType<i64>>,
  CCIfType<[f16,f32],CCPromoteToType<f64>>,
  CCIfType<[i64,f64], CCAssignToReg<[EAX, EBX, ECX, EDX,R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,R13,R14,R15,R16,R17,R18,R19,R20,R21,R22,R23,R24,R25,R26,R27,R28,R29,R30,R31]>>,
  CCIfType<[i64, f64], CCAssignToStack<8, 8>>
]>;
def CSR_Saved : CalleeSavedRegs<(add EAX,EBX,ECX,EDX)>;
def CSR_NoRegs : CalleeSavedRegs<(add)>;