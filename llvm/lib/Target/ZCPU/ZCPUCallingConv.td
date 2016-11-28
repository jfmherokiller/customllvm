
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
  CCIfType<[i64,f64], CCAssignToReg<[EAX, EBX, ECX, EDX]>>,
  CCIfType<[i64, f64], CCAssignToStack<64, 64>>
]>;
def CSR_Saved : CalleeSavedRegs<(add EAX,EBX,ECX,EDX)>;
def CSR_NoRegs : CalleeSavedRegs<(add)>;