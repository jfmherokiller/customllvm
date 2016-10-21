//===-- ZCPU.td - Describe the ZCPU Target Machine ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Calling Conv, Instruction Descriptions
//===----------------------------------------------------------------------===//

include "ZCPUSchedule.td"
include "ZCPUInstrInfo.td"
//#if CH >= CH3_1 1
include "ZCPUCallingConv.td"
//#endif

def ZCPUInstrInfo : InstrInfo;

//#if CH >= CH3_1 2
// Without this will have error: 'cpu032I' is not a recognized processor for 
//  this target (ignoring processor)
//===----------------------------------------------------------------------===//
// ZCPU Subtarget features                                                    //
//===----------------------------------------------------------------------===//


def FeatureChapter3_1  : SubtargetFeature<"ch3_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter3_2  : SubtargetFeature<"ch3_2", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter3_3  : SubtargetFeature<"ch3_3", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter3_4  : SubtargetFeature<"ch3_4", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter3_5  : SubtargetFeature<"ch3_5", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter4_1  : SubtargetFeature<"ch4_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter4_2  : SubtargetFeature<"ch4_2", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter5_1  : SubtargetFeature<"ch5_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter6_1  : SubtargetFeature<"ch6_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter7_1  : SubtargetFeature<"ch7_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter8_1  : SubtargetFeature<"ch8_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter8_2  : SubtargetFeature<"ch8_2", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter9_1  : SubtargetFeature<"ch9_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter9_2  : SubtargetFeature<"ch9_2", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter9_3  : SubtargetFeature<"ch9_3", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter10_1 : SubtargetFeature<"ch10_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter11_1 : SubtargetFeature<"ch11_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter11_2 : SubtargetFeature<"ch11_2", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapter12_1 : SubtargetFeature<"ch12_1", "HasChapterDummy", "true",
                                "Enable Chapter instructions.">;
def FeatureChapterAll  : SubtargetFeature<"chall", "HasChapterDummy", "true",
                                "Enable Chapter instructions.",
                                [FeatureChapter3_1, FeatureChapter3_2, 
                                 FeatureChapter3_3, FeatureChapter3_4, 
                                 FeatureChapter3_5, 
                                 FeatureChapter4_1, FeatureChapter4_2, 
                                 FeatureChapter5_1, FeatureChapter6_1, 
                                 FeatureChapter7_1, FeatureChapter8_1, 
                                 FeatureChapter8_2, FeatureChapter9_1, 
                                 FeatureChapter9_2, FeatureChapter9_3, 
                                 FeatureChapter10_1, 
                                 FeatureChapter11_1, FeatureChapter11_2, 
                                 FeatureChapter12_1]>;


def FeatureCmp         : SubtargetFeature<"cmp", "HasCmp", "true",
                                "Enable 'cmp' instructions.">;
def FeatureSlt         : SubtargetFeature<"slt", "HasSlt", "true",
                                "Enable 'slt' instructions.">;
def FeatureZCPU32I     : SubtargetFeature<"cpu032I", "ZCPUArchVersion", 
                                "ZCPU32I", "ZCPU32I ISA Support",
                                [FeatureCmp, FeatureChapterAll]>;
def FeatureZCPU32II    : SubtargetFeature<"cpu032II", "ZCPUArchVersion",                      
                               "ZCPU32II", "ZCPU32II ISA Support (slt)",
                                [FeatureCmp, FeatureSlt, FeatureChapterAll]>;

//===----------------------------------------------------------------------===//
// ZCPU processors supported.
//===----------------------------------------------------------------------===//

class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, ZCPUGenericItineraries, Features>;

def : Proc<"cpu032I",  [FeatureZCPU32I]>;
def : Proc<"cpu032II", [FeatureZCPU32II]>;
// Above make ZCPUGenSubtargetInfo.inc set feature bit as the following order
// enum {
//   FeatureCmp =  1ULL << 0,
//   FeatureZCPU32I =  1ULL << 1,
//   FeatureZCPU32II =  1ULL << 2,
//   FeatureSlt =  1ULL << 3
// };
//#endif

//#if CH >= CH11_1 1
def ZCPUAsmParser : AsmParser {
  let ShouldEmitMatchRegisterName = 0;
}

def ZCPUAsmParserVariant : AsmParserVariant {
  int Variant = 0;

  // Recognize hard coded registers.
  string RegisterPrefix = "$";
}
//#endif

// Will generate ZCPUGenAsmWrite.inc included by ZCPUInstPrinter.cpp, contents 
//  as follows,
// void ZCPUInstPrinter::printInstruction(const MCInst *MI, raw_ostream &O) {...}
// const char *ZCPUInstPrinter::getRegisterName(unsigned RegNo) {...}
def ZCPU : Target {
// def ZCPUInstrInfo : InstrInfo as before.
  let InstructionSet = ZCPUInstrInfo;
//#if CH >= CH11_1 2
  let AssemblyParsers = [ZCPUAsmParser];
  let AssemblyParserVariants = [ZCPUAsmParserVariant];
//#endif
}
