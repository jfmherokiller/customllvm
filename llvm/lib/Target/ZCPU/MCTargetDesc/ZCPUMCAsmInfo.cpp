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
    HasSingleParameterDotFile = false;
    PointerSize = 64;
    CalleeSaveStackSlotSize = 64;
    UseDataRegionDirectives = false;


    // Use .skip instead of .zero because .zero is confusing when used with two
    // arguments (it doesn't actually zero things out).
    ZeroDirective = "\tZAP\t";

    Data8bitsDirective = "\tINT48\t";
    Data16bitsDirective = "\tINT48\t";
    Data32bitsDirective = "\tINT48\t";
    Data64bitsDirective = "\tINT48\t";
    CommentString = "//";
    AlignmentIsInBytes = false;
    HasDotTypeDotSizeDirective = false;
    AsciiDirective = "DB ";
    AscizDirective = "DB ";
    SupportsDebugInformation = false;

    // For now, ZCPU does not support exceptions.
    ExceptionsType = ExceptionHandling::None;

    // ZCPU's stack is never executable.
    UsesNonexecutableStackSection = true;
    NeedsLocalForSize = false;
    HasIdentDirective = false;
}
