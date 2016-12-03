defm ADD:BinyFloatIntOP<010,"add","ADD\t$src1,$src2">;  //X = X + Y
defm SUB:BinyFloatIntOP<011,"sub","SUB\t$src1,$src2">; //X = X - Y
defm MUL:BinyFloatIntOP<012,"mul","MUL\t$src1,$src2">; //X = X * Y

defm IDIV:BinyOPSignUnSigned<013,"div","DIV\t$src1,$src2">; //X = X / Y
def FDIV:I<013,(outs BothNormAndExtendedFloat:$dst), (ins BothNormAndExtendedFloat:$src1,BothNormAndExtendedFloat:$src2),[(set BothNormAndExtendedFloat:$dst,(fdiv BothNormAndExtendedFloat:$src1,BothNormAndExtendedFloat:$src2))], "DIV\t$src1,$src2">; //X = X / Y
//def SUB:I<011,(outs BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2),[(set BothNormAndExtendedInt:$dst,(sub BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2))], "SUB\t$src1,$src2">; //X = X - Y
//def MUL:I<012,(outs BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2),[(set BothNormAndExtendedInt:$dst,(mul BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2))], "MUL\t$src1,$src2">; //X = X * Y
//def DIV:I<011,(outs BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2),[(set BothNormAndExtendedInt:$dst,(sdiv BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2))], "DIV\t$src1,$src2">; //X = X - Y




def CMP:I<015,(outs), (ins ),[], "CMP">; //Compare X and Y. Use with conditional branching instructions

def MIN:I<018,(outs), (ins ),[], "MIN">; //Set X to smaller value out of X and Y
def MAX:I<019,(outs), (ins ),[], "MAX">; //Set X to bigger value out of X and Y

def NEG:I<022,(outs BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src1),[(set BothNormAndExtendedInt:$dst, (ineg BothNormAndExtendedInt:$src1))], "NEG\t$dst">; //Change sign of X
def RAND:I<023,(outs), (ins),[], "RAND">; //Set X to random value



def BNOT:I<032,(outs), (ins),[], "BNOT">; //Flip all bits in the integer number
def FINT:I<033,(outs), (ins),[], "FINT">; //Force X to be an integer value
def FRND:I<034,(outs), (ins),[], "FRND">; //Round X to the nearest integer value
def RND:I<034,(outs), (ins),[], "RND">; //FRND
def FFRAC:I<035,(outs), (ins),[], "FFRAC">; //"Remove integer part of the fraction
def FINV:I<036,(outs), (ins),[], "FINV">; //X = 1 / X

let Constraints = "$src1 = $dst" in {
def INC:I<020,(outs BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src1),[(set BothNormAndExtendedInt:$dst,(add BothNormAndExtendedInt:$src1,1))], "INC\t$src1">; //Increase X by one
def DEC:I<021,(outs BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src1),[(set BothNormAndExtendedInt:$dst,(sub BothNormAndExtendedInt:$src1,1))], "DEC\t$src1">; //Decrease X by one
def FSHR:I<039,(outs  BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src1, i64imm:$src2),[(set BothNormAndExtendedInt:$dst,(sra BothNormAndExtendedInt:$src1,(i64 imm:$src2)))], "FSHR\t$src1,$src2">; //Divide X by 2 (does not floor)
def FSHL:I<038,(outs  BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src1, i64imm:$src2),[(set BothNormAndExtendedInt:$dst,(srl BothNormAndExtendedInt:$src1,(i64 imm:$src2)))], "FSHL\t$src1,$src2">; //Multiply X by 2 (does not floor)
def AND:I<050, (outs  BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2),[(set BothNormAndExtendedInt:$dst,(and BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2))], "AND\t$src1,$src2">; //Logical AND between X and Y
def OR:I<051,  (outs  BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2),[(set BothNormAndExtendedInt:$dst,(or BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2))],  "OR\t$src1,$src2">; //Logical OR between X and Y
def XOR:I<052, (outs  BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2),[(set BothNormAndExtendedInt:$dst,(xor BothNormAndExtendedInt:$src1,BothNormAndExtendedInt:$src2))], "XOR\t$src1,$src2">; //Logical XOR between X and Y
}
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
