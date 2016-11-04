def VADD:I<250,(outs), (ins),[], "VADD">; //X = X + Y
def VSUB:I<251,(outs), (ins),[], "VSUB">; //X = X - Y
def VMUL:I<252,(outs), (ins),[], "VMUL">; //X = X * SCALAR Y
def VDOT:I<253,(outs), (ins),[], "VDOT">; //X = X dot Y
def VCROSS:I<254,(outs), (ins),[], "VCROSS">; //X = X cross Y
def VMOV:I<255,(outs), (ins),[], "VMOV">; //X = Y
def VNORM:I<256,(outs), (ins),[], "VNORM">; //X = NORMALIZE(Y)
def VCOLORNORM:I<257,(outs), (ins),[], "VCOLORNORM">; //Normalize color (clamp it to RGB range)
def LOOPXY:I<259,(outs), (ins),[], "LOOPXY">; //2D loop by ECX/EDX registers
def MADD:I<260,(outs), (ins),[], "MADD">; //X = X + Y
def MSUB:I<261,(outs), (ins),[], "MSUB">; //X = X - Y
def MMUL:I<262,(outs), (ins),[], "MMUL">; //X = X * Y
def MROTATE:I<263,(outs), (ins),[], "MROTATE">; //Rotation matrix based on rotation vector
def MSCALE:I<264,(outs), (ins),[], "MSCALE">; //Scaling matrix based on scaling vector
def MPERSPECTIVE:I<265,(outs), (ins),[], "MPERSPECTIVE">; //Perspective matrix based on FOV and near/far planes
def MTRANSLATE:I<266,(outs), (ins),[], "MTRANSLATE">; //Translation matrix based on translation vector
def MLOOKAT:I<267,(outs), (ins),[], "MLOOKAT">; //Lookat matrix based on three vectors
def MMOV:I<268,(outs), (ins),[], "MMOV">; //X = Y
def VLEN:I<269,(outs), (ins),[], "VLEN">; //X = Sqrt(Y dot Y)
def MIDENT:I<270,(outs), (ins),[], "MIDENT">; //Load identity matrix
def VMODEI:I<273,(outs), (ins),[], "VMODE">; //Set vector math mode
def VDIV:I<295,(outs), (ins),[], "VDIV">; //VEC = VEC / Y
def VTRANSFORM:I<296,(outs), (ins),[], "VTRANSFORM">; //X = X * MATRIX