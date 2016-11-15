//a place for me to throw the stuff i dont understand yet


def SDT_WebAssemblyCallSeqStart2 : SDCallSeqStart<[SDTCisVT<0, iPTR>]>;
def SDT_WebAssemblyCallSeqEnd2 :SDCallSeqEnd<[SDTCisVT<0, iPTR>, SDTCisVT<1, iPTR>]>;
def SDT_WebAssemblyCall0_a    : SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>;
def SDT_WebAssemblyCall1_a    : SDTypeProfile<1, -1, [SDTCisPtrTy<1>]>;
def SDT_WebAssemblyBrTable_a  : SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>;
def SDT_WebAssemblyArgument_a : SDTypeProfile<1, 1, [SDTCisVT<1, i32>]>;
def SDT_WebAssemblyReturn_a   : SDTypeProfile<0, -1, []>;
def SDT_WebAssemblyWrapper_a  : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>,
                                                   SDTCisPtrTy<0>]>;

def WebAssemblycallseq_start : SDNode<"ISD::CALLSEQ_START", SDT_WebAssemblyCallSeqStart2, [SDNPHasChain, SDNPOutGlue]>;
def WebAssemblycallseq_end :SDNode<"ISD::CALLSEQ_END", SDT_WebAssemblyCallSeqEnd2,[SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def WebAssemblycall0 : SDNode<"ZCPUISD::CALL0",SDT_WebAssemblyCall0_a,[SDNPHasChain, SDNPVariadic]>;
def WebAssemblycall1 : SDNode<"ZCPUISD::CALL1",SDT_WebAssemblyCall1_a,[SDNPHasChain, SDNPVariadic]>;
//def WebAssemblybr_table : SDNode<"ZCPUISD::BR_TABLE",SDT_WebAssemblyBrTable,[SDNPHasChain, SDNPVariadic]>;
//def WebAssemblyargument : SDNode<"ZCPUISD::ARGUMENT",SDT_WebAssemblyArgument>;
def WebAssemblyreturn   : SDNode<"ZCPUISD::RET_FLAG",SDT_WebAssemblyReturn_a, [SDNPHasChain]>;
def WebAssemblywrapper  : SDNode<"ZCPUISD::Wrapper",SDT_WebAssemblyWrapper_a>;


// Call sequence markers. These have an immediate which represents the amount of
// stack space to allocate or free, which is used for varargs lowering.
let Uses = [ESP], Defs = [ESP], isCodeGenOnly = 1 in {
def ADJCALLSTACKDOWN : I<0,(outs), (ins i64imm:$amt),
                         [(WebAssemblycallseq_start timm:$amt)]>;
def ADJCALLSTACKUP : I<0,(outs), (ins i64imm:$amt, i64imm:$amt2),
                       [(WebAssemblycallseq_end timm:$amt, timm:$amt2)]>;
} // isCodeGenOnly = 1

multiclass CALL<ZCPURegClass vt, string prefix> {
  def CALL_#vt : I<0,(outs vt:$dst), (ins i64imm:$callee, variable_ops),
                   [(set vt:$dst, (WebAssemblycall1 (i64 imm:$callee)))],
                   !strconcat(prefix, "call\t$dst, $callee")>;
  def CALL_INDIRECT_#vt : I<0,(outs vt:$dst), (ins BothNormAndExtendedInt:$callee, variable_ops),
                            [(set vt:$dst, (WebAssemblycall1 BothNormAndExtendedInt:$callee))],
                            !strconcat(prefix, "call_indirect\t$dst, $callee")>;
}
let Uses = [ESP], isCall = 1 in {
  defm : CALL<BothNormAndExtendedInt, "i64.">;
  defm : CALL<BothNormAndExtendedFloat, "f64.">;

  def CALL_VOID : I<0,(outs), (ins i64imm:$callee, variable_ops),
                    [(WebAssemblycall0 (i64 imm:$callee))],
                    "call    \t$callee">;
  def CALL_INDIRECT_VOID : I<0,(outs), (ins BothNormAndExtendedInt:$callee, variable_ops),
                             [(WebAssemblycall0 BothNormAndExtendedInt:$callee)],
                             "call_indirect\t$callee">;
} // Uses = [SP32,SP64], isCall = 1

def : Pat<(i64 (WebAssemblycall1 (WebAssemblywrapper tglobaladdr:$callee))),
          (CALL_BothNormAndExtendedInt tglobaladdr:$callee)>;
def : Pat<(f64 (WebAssemblycall1 (WebAssemblywrapper tglobaladdr:$callee))),
          (CALL_BothNormAndExtendedFloat tglobaladdr:$callee)>;
def : Pat<(WebAssemblycall0 (WebAssemblywrapper tglobaladdr:$callee)),
          (CALL_VOID tglobaladdr:$callee)>;

// Patterns for matching a direct call to an external symbol.

def : Pat<(i64 (WebAssemblycall1 (WebAssemblywrapper texternalsym:$callee))),
          (CALL_BothNormAndExtendedInt texternalsym:$callee)>;
def : Pat<(f64 (WebAssemblycall1 (WebAssemblywrapper texternalsym:$callee))),
          (CALL_BothNormAndExtendedFloat texternalsym:$callee)>;
def : Pat<(WebAssemblycall0 (WebAssemblywrapper texternalsym:$callee)),
          (CALL_VOID texternalsym:$callee)>;
