//===-- ZCPUMCAsmInfo.cpp - ZCPU asm properties -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains the declarations of the ZCPUMCAsmInfo
/// properties.
///
//===----------------------------------------------------------------------===//

#include "ZCPUMCAsmInfo.h"
#include "llvm/ADT/Triple.h"
using namespace llvm;

#define DEBUG_TYPE "zcpu-mc-asm-info"

ZCPUMCAsmInfo::~ZCPUMCAsmInfo() {}

ZCPUMCAsmInfo::ZCPUMCAsmInfo(const Triple &T) {
  PointerSize = CalleeSaveStackSlotSize = 48;

  // TODO: What should MaxInstLength be?

  UseDataRegionDirectives = true;

  // Use .skip instead of .zero because .zero is confusing when used with two
  // arguments (it doesn't actually zero things out).
  ZeroDirective = "\t.skip\t";

  Data8bitsDirective = "\t.int8\t";
  Data16bitsDirective = "\t.int16\t";
  Data32bitsDirective = "\t.int32\t";
  Data64bitsDirective = "\t.int64\t";

  AlignmentIsInBytes = false;
  COMMDirectiveAlignmentIsInBytes = false;
  LCOMMDirectiveAlignmentType = LCOMM::Log2Alignment;

  SupportsDebugInformation = true;

  // For now, ZCPU does not support exceptions.
  ExceptionsType = ExceptionHandling::None;

  // TODO: UseIntegratedAssembler?

  // ZCPU's stack is never executable.
  UsesNonexecutableStackSection = false;
}
