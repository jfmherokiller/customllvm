//a place for me to throw the stuff i dont understand yet

let OperandNamespace = "ZCPU" in {

let OperandType = "OPERAND_BASIC_BLOCK" in
def bb_op : Operand<OtherVT>;

let OperandType = "OPERAND_FP64IMM" in
def f64imm_op : Operand<f64>;

} // OperandNamespace = "WebAssembly"


def SDT_WebAssemblyCallSeqStart2 : SDCallSeqStart<[SDTCisVT<0, iPTR>]>;
def SDT_WebAssemblyCallSeqEnd2 :SDCallSeqEnd<[SDTCisVT<0, iPTR>, SDTCisVT<1, iPTR>]>;
def SDT_WebAssemblyCall0_a    : SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>;
def SDT_WebAssemblyCall1_a    : SDTypeProfile<1, -1, [SDTCisPtrTy<1>]>;
//def SDT_WebAssemblyBrTable_a  : SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>;
def SDT_WebAssemblyArgument : SDTypeProfile<1, 1, [SDTCisVT<1, i32>]>;
def SDT_WebAssemblyReturn_a   : SDTypeProfile<0, -1, []>;
def SDT_WebAssemblyWrapper_a  : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>,SDTCisPtrTy<0>]>;

def WebAssemblycallseq_start : SDNode<"ISD::CALLSEQ_START", SDT_WebAssemblyCallSeqStart2, [SDNPHasChain, SDNPOutGlue]>;
def WebAssemblycallseq_end :SDNode<"ISD::CALLSEQ_END", SDT_WebAssemblyCallSeqEnd2,[SDNPHasChain, SDNPOptInGlue, SDNPOutGlue]>;
def WebAssemblycall0 : SDNode<"ZCPUISD::CALL0",SDT_WebAssemblyCall0_a,[SDNPHasChain, SDNPVariadic]>;
def WebAssemblycall1 : SDNode<"ZCPUISD::CALL1",SDT_WebAssemblyCall1_a,[SDNPHasChain, SDNPVariadic]>;
//def WebAssemblybr_table : SDNode<"ZCPUISD::BR_TABLE",SDT_WebAssemblyBrTable,[SDNPHasChain, SDNPVariadic]>;
def WebAssemblyargument : SDNode<"ZCPUISD::ARGUMENT",SDT_WebAssemblyArgument>;
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
  defm : CALL<BothNormAndExtendedInt32, "i32.">;
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

let isMoveImm = 1, isAsCheapAsAMove = 1, isReMaterializable = 1 in {
def CONST_BothNormAndExtendedInt32 : I<0,(outs BothNormAndExtendedInt32:$res), (ins i32imm:$imm),
                  [(set BothNormAndExtendedInt32:$res, imm:$imm)],
                  "i32.const\t$res, $imm">;
def CONST_BothNormAndExtendedInt : I<0,(outs BothNormAndExtendedInt:$res), (ins i64imm:$imm),
                  [(set BothNormAndExtendedInt:$res, imm:$imm)],
                  "i64.const\t$res, $imm">;
def CONST_BothNormAndExtendedFloat : I<0,(outs BothNormAndExtendedFloat:$res), (ins f64imm_op:$imm),
                  [(set BothNormAndExtendedFloat:$res, fpimm:$imm)],
                  "f64.const\t$res, $imm">;
} // isMoveImm = 1, isAsCheapAsAMove = 1, isReMaterializable = 1

multiclass ARGUMENT<ZCPURegClass vt> {
  let hasSideEffects = 1, Uses = [ARGUMENTS], isCodeGenOnly = 1 in
  def ARGUMENT_#vt : I<0,(outs vt:$res), (ins i32imm:$argno),
                       [(set vt:$res, (WebAssemblyargument timm:$argno))]>;
}
defm : ARGUMENT<BothNormAndExtendedInt32>;
defm : ARGUMENT<BothNormAndExtendedInt>;
defm : ARGUMENT<BothNormAndExtendedFloat>;

multiclass RETURN<ZCPURegClass vt> {
  def RETURN_#vt : I<0,(outs), (ins vt:$val), [(WebAssemblyreturn vt:$val)],
                     "return  \t$val">;
  // Equivalent to RETURN_#vt, for use at the end of a function when wasm
  // semantics return by falling off the end of the block.
  let isCodeGenOnly = 1 in
  def FALLTHROUGH_RETURN_#vt : I<0,(outs), (ins vt:$val), []>;
}

let isTerminator = 1, hasCtrlDep = 1, isBarrier = 1 in {
let isReturn = 1 in {
  defm : RETURN<BothNormAndExtendedInt32>;
  defm : RETURN<BothNormAndExtendedInt>;
  defm : RETURN<BothNormAndExtendedFloat>;
  def RETURN_VOID : I<0,(outs), (ins), [(WebAssemblyreturn)], "ret">;

  // This is to RETURN_VOID what FALLTHROUGH_RETURN_#vt is to RETURN_#vt.
  let isCodeGenOnly = 1 in
  def FALLTHROUGH_RETURN_VOID : I<0,(outs), (ins), []>;
} // isReturn = 1
  def UNREACHABLE : I<0,(outs), (ins), [(trap)], "unreachable">;
} // isTerminator = 1, hasCtrlDep = 1, isBarrier = 1

// Patterns for matching a direct call to a global address.
def : Pat<(i32 (WebAssemblycall1 (WebAssemblywrapper tglobaladdr:$callee))),
          (CALL_BothNormAndExtendedInt32 tglobaladdr:$callee)>;
def : Pat<(i64 (WebAssemblycall1 (WebAssemblywrapper tglobaladdr:$callee))),
          (CALL_BothNormAndExtendedInt tglobaladdr:$callee)>;
def : Pat<(f64 (WebAssemblycall1 (WebAssemblywrapper tglobaladdr:$callee))),
          (CALL_BothNormAndExtendedFloat tglobaladdr:$callee)>;
def : Pat<(WebAssemblycall0 (WebAssemblywrapper tglobaladdr:$callee)),
          (CALL_VOID tglobaladdr:$callee)>;

// Patterns for matching a direct call to an external symbol.
def : Pat<(i32 (WebAssemblycall1 (WebAssemblywrapper texternalsym:$callee))),
          (CALL_BothNormAndExtendedInt32 texternalsym:$callee)>;
def : Pat<(i64 (WebAssemblycall1 (WebAssemblywrapper texternalsym:$callee))),
          (CALL_BothNormAndExtendedInt texternalsym:$callee)>;
def : Pat<(f64 (WebAssemblycall1 (WebAssemblywrapper texternalsym:$callee))),
          (CALL_BothNormAndExtendedFloat texternalsym:$callee)>;
def : Pat<(WebAssemblycall0 (WebAssemblywrapper texternalsym:$callee)),
          (CALL_VOID texternalsym:$callee)>;
