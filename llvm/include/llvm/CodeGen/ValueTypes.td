//===- ValueTypes.td - ValueType definitions ---------------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Value types - These values correspond to the register types defined in the
// ValueTypes.h file.  If you update anything here, you must update it there as
// well!
//
//===----------------------------------------------------------------------===//

class ValueType<int size, int value> {
  string Namespace = "MVT";
  int Size = size;
  int Value = value;
}

def OtherVT: ValueType<0  ,  0>;   // "Other" value
def i1     : ValueType<1  ,  1>;   // One bit boolean value
def i8     : ValueType<8  ,  2>;   // 8-bit integer value
def i16    : ValueType<16 ,  3>;   // 16-bit integer value
def i32    : ValueType<32 ,  4>;   // 32-bit integer value
def i64    : ValueType<64 ,  5>;   // 64-bit integer value
def i128   : ValueType<128,  6>;   // 128-bit integer value
def i48 : ValueType<48    , 7>;
def f16    : ValueType<16 ,  8>;   // 16-bit floating point value
def f32    : ValueType<32 ,  9>;   // 32-bit floating point value
def f64    : ValueType<64 ,  10>;   // 64-bit floating point value
def f80    : ValueType<80 , 11>;   // 80-bit floating point value
def f128   : ValueType<128, 12>;   // 128-bit floating point value
def ppcf128: ValueType<128, 13>;   // PPC 128-bit floating point value

def v2i1   : ValueType<2 ,  14>;   //  2 x i1  vector value
def v4i1   : ValueType<4 ,  15>;   //  4 x i1  vector value
def v8i1   : ValueType<8 ,  16>;   //  8 x i1  vector value
def v16i1  : ValueType<16,  17>;   // 16 x i1  vector value
def v32i1  : ValueType<32 , 18>;   // 32 x i1  vector value
def v64i1  : ValueType<64 , 19>;   // 64 x i1  vector value
def v1i8   : ValueType<16, 20>;    //  1 x i8  vector value
def v2i8   : ValueType<16 , 21>;   //  2 x i8  vector value
def v4i8   : ValueType<32 , 22>;   //  4 x i8  vector value
def v8i8   : ValueType<64 , 23>;   //  8 x i8  vector value
def v16i8  : ValueType<128, 24>;   // 16 x i8  vector value
def v32i8  : ValueType<256, 25>;   // 32 x i8 vector value
def v64i8  : ValueType<512, 26>;   // 64 x i8 vector value
def v1i16  : ValueType<16 , 27>;   //  1 x i16 vector value
def v2i16  : ValueType<32 , 28>;   //  2 x i16 vector value
def v4i16  : ValueType<64 , 29>;   //  4 x i16 vector value
def v8i16  : ValueType<128, 30>;   //  8 x i16 vector value
def v16i16 : ValueType<256, 31>;   // 16 x i16 vector value
def v32i16 : ValueType<512, 32>;   // 32 x i16 vector value
def v1i32  : ValueType<32 , 33>;   //  1 x i32 vector value
def v2i32  : ValueType<64 , 34>;   //  2 x i32 vector value
def v4i32  : ValueType<128, 35>;   //  4 x i32 vector value
def v8i32  : ValueType<256, 36>;   //  8 x i32 vector value
def v16i32 : ValueType<512, 37>;   // 16 x i32 vector value
def v1i64  : ValueType<64 , 38>;   //  1 x i64 vector value
def v2i64  : ValueType<128, 39>;   //  2 x i64 vector value
def v4i64  : ValueType<256, 40>;   //  4 x i64 vector value
def v8i64  : ValueType<512, 41>;   //  8 x i64 vector value
def v16i64 : ValueType<1024,42>;   // 16 x i64 vector value
def v1i128 : ValueType<128, 43>;   //  1 x i128 vector value

def v2f16  : ValueType<32 , 44>;   //  2 x f16 vector value
def v4f16  : ValueType<64 , 45>;   //  4 x f16 vector value
def v8f16  : ValueType<128, 46>;   //  8 x f16 vector value
def v1f32  : ValueType<32 , 47>;   //  1 x f32 vector value
def v2f32  : ValueType<64 , 48>;   //  2 x f32 vector value
def v4f32  : ValueType<128, 49>;   //  4 x f32 vector value
def v8f32  : ValueType<256, 50>;   //  8 x f32 vector value
def v16f32 : ValueType<512, 51>;   // 16 x f32 vector value
def v1f64  : ValueType<64, 52>;    //  1 x f64 vector value
def v2f64  : ValueType<128, 53>;   //  2 x f64 vector value
def v4f64  : ValueType<256, 54>;   //  4 x f64 vector value
def v8f64  : ValueType<512, 55>;   //  8 x f64 vector value


def x86mmx : ValueType<64 , 56>;   // X86 MMX value
def FlagVT : ValueType<0  , 57>;   // Pre-RA sched glue
def isVoid : ValueType<0  , 58>;   // Produces no value
def untyped: ValueType<8  , 59>;   // Produces an untyped value

def MetadataVT: ValueType<0, 250>; // Metadata

// Pseudo valuetype mapped to the current pointer size to any address space.
// Should only be used in TableGen.
def iPTRAny   : ValueType<0, 251>;

// Pseudo valuetype to represent "vector of any size"
def vAny   : ValueType<0  , 252>;

// Pseudo valuetype to represent "float of any format"
def fAny   : ValueType<0  , 253>;

// Pseudo valuetype to represent "integer of any bit width"
def iAny   : ValueType<0  , 254>;

// Pseudo valuetype mapped to the current pointer size.
def iPTR   : ValueType<0  , 255>;

// Pseudo valuetype to represent "any type of any size".
def Any    : ValueType<0  , 256>;
