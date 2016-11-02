
// Call sequence markers. These have an immediate which represents the amount of
// stack space to allocate or free, which is used for varargs lowering.
let Uses = [ESP], Defs = [ESP] in {
def ADJCALLSTACKDOWN : I<(outs), (ins i32imm:$amt),
                         [(ZCPUcallseq_start timm:$amt)]>;
def ADJCALLSTACKUP : I<(outs), (ins i32imm:$amt, i32imm:$amt2),
                       [(ZCPUcallseq_end timm:$amt, timm:$amt2)]>;
} // isCodeGenOnly = 1


def CSR_Saved : CalleeSavedRegs<(add EAX)>;
def CSR_NoRegs : CalleeSavedRegs<(add)>;