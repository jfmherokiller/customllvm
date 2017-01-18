def SDT_ZCPUAdjDynAlloc  : SDTypeProfile<1,  1, [SDTCisVT<0, i64>,SDTCisVT<1, i64>]>;
def ZCPUAdjDynAlloc : SDNode<"ZCPUISD::ADJDYNALLOC", SDT_ZCPUAdjDynAlloc>;

let Defs = [ESP], Uses = [ESP] in {
  def ADJDYNALLOC : IPsudo<(outs BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src),[(set BothNormAndExtendedInt:$dst, (ZCPUAdjDynAlloc BothNormAndExtendedInt:$src))],"#ADJDYNALLOC $dst $src">;
}
multiclass BrcondPatsCmp<RegisterClass RC, Instruction JEQOp, Instruction JNEOp,
Instruction JLTOp, Instruction JGTOp, Instruction JLEOp, Instruction JGEOp,
Instruction CMPOp,Instruction CMPIOp> {
def : Pat<(brcond (i64 (seteq RC:$lhs, RC:$rhs)), bb:$dst),
          (JEQOp (CMPOp RC:$lhs, RC:$rhs), bb:$dst)>;
def : Pat<(brcond (i64 (setueq RC:$lhs, RC:$rhs)), bb:$dst),
          (JEQOp (CMPOp RC:$lhs, RC:$rhs), bb:$dst)>;
def : Pat<(brcond (i64 (setne RC:$lhs, RC:$rhs)), bb:$dst),
          (JNEOp (CMPOp RC:$lhs, RC:$rhs), bb:$dst)>;
def : Pat<(brcond (i64 (setune RC:$lhs, RC:$rhs)), bb:$dst),
          (JNEOp (CMPOp RC:$lhs, RC:$rhs), bb:$dst)>;
def : Pat<(brcond (i64 (setlt RC:$lhs, RC:$rhs)), bb:$dst),
          (JLTOp (CMPOp RC:$lhs, RC:$rhs), bb:$dst)>;
def : Pat<(brcond (i64 (setult RC:$lhs, RC:$rhs)), bb:$dst),
          (JLTOp (CMPOp RC:$lhs, RC:$rhs), bb:$dst)>;
def : Pat<(brcond (i64 (setgt RC:$lhs, RC:$rhs)), bb:$dst),
          (JGTOp (CMPOp RC:$lhs, RC:$rhs), bb:$dst)>;
def : Pat<(brcond (i64 (setugt RC:$lhs, RC:$rhs)), bb:$dst),
          (JGTOp (CMPOp RC:$lhs, RC:$rhs), bb:$dst)>;
def : Pat<(brcond (i64 (setle RC:$lhs, RC:$rhs)), bb:$dst),
          (JLEOp (CMPOp RC:$lhs, RC:$rhs), bb:$dst)>;
def : Pat<(brcond (i64 (setule RC:$lhs, RC:$rhs)), bb:$dst),
          (JLEOp (CMPOp RC:$lhs, RC:$rhs), bb:$dst)>;
def : Pat<(brcond (i64 (setge RC:$lhs, RC:$rhs)), bb:$dst),
          (JGEOp (CMPOp RC:$lhs, RC:$rhs), bb:$dst)>;
def : Pat<(brcond (i64 (setuge RC:$lhs, RC:$rhs)), bb:$dst),
          (JGEOp (CMPOp RC:$lhs, RC:$rhs), bb:$dst)>;

def : Pat<(brcond RC:$cond, bb:$dst),
          (JNEOp (CMPOp RC:$cond, 0), bb:$dst)>;
}
let isBranch = 1 in {
def JNE:I<001,(outs), (ins BothNormAndExtendedInt:$ra,brtarget:$dst),[], "JNE\t$dst">; //Jump to PTR if result is not equal
def JNZ:I<001,(outs), (ins BothNormAndExtendedInt:$ra,brtarget:$dst),[], "JNZ\t$dst">; //Jump to PTR if result is not zero
def JMP:I<002,(outs), (ins brtarget:$dst),[(br bb:$dst)], "JMP\t$dst">; //Jump to PTR
def JG:I<003,(outs), (ins BothNormAndExtendedInt:$ra,brtarget:$dst),[], "JG\t$dst">; //Jump to PTR if result is greater
def JNLE:I<003,(outs), (ins BothNormAndExtendedInt:$ra,brtarget:$dst),[], "JNLE\t$dst">; //Jump to PTR if result is not less or equal
def JGE:I<004,(outs), (ins BothNormAndExtendedInt:$ra,brtarget:$dst),[], "JGE\t$dst">; //Jump to PTR if result is greater or equal
def JNL:I<004,(outs), (ins BothNormAndExtendedInt:$ra,brtarget:$dst),[], "JNL\t$dst">; //Jump to PTR if result is not less
def JL:I<005,(outs), (ins BothNormAndExtendedInt:$ra,brtarget:$dst),[], "JL\t$dst">; //Jump to PTR if result is less
def JNGE:I<005,(outs), (ins BothNormAndExtendedInt:$ra,brtarget:$dst),[], "JNGE\t$dst">; //Jump to PTR if result is not greater or equal
def JLE:I<006,(outs), (ins BothNormAndExtendedInt:$ra,brtarget:$dst),[], "JLE\t$dst">; //Jump to PTR if result is less or equal
def JNG:I<006,(outs), (ins BothNormAndExtendedInt:$ra,brtarget:$dst),[], "JNG\t$dst">; //Jump to PTR if result is not greater
def JE:I<007,(outs), (ins BothNormAndExtendedInt:$ra,brtarget:$dst),[], "JE\t$dst">; //Jump to PTR if result is equal
def JZ:I<007,(outs), (ins BothNormAndExtendedInt:$ra,brtarget:$dst),[], "JZ\t$dst">; //Jump to PTR if result is zero
def JMPF:I<069,(outs), (ins),[], "JMPF">; //Jump to PTR in code segment CS
}

def CMP:I<015,(outs BothNormAndExtendedInt:$ra), (ins BothNormAndExtendedInt:$rb, BothNormAndExtendedInt:$rc),[], "CMP\t$rb,$rc">; //Compare X and Y. Use with conditional branching instructions
def CMPImm:I<015,(outs BothNormAndExtendedInt:$ra), (ins BothNormAndExtendedInt:$rb, i64imm:$rc),[], "CMP\t$rb,$rc">; //Compare X and Y. Use with conditional branching instructions

defm : BrcondPatsCmp<BothNormAndExtendedInt, JE, JNE, JL, JG, JLE, JGE, CMP,CMPImm>;

