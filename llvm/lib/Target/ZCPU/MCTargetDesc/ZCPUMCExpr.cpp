//===-- ZCPUMCExpr.cpp - ZCPU specific MC expression classes --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ZCPU.h"

#if CH >= CH5_1

#include "ZCPUMCExpr.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCObjectStreamer.h"

using namespace llvm;

#define DEBUG_TYPE "cpu0mcexpr"

bool ZCPUMCExpr::isSupportedBinaryExpr(MCSymbolRefExpr::VariantKind VK,
                                       const MCBinaryExpr *BE) {
  switch (VK) {
  case MCSymbolRefExpr::VK_ZCPU_ABS_LO:
  case MCSymbolRefExpr::VK_ZCPU_ABS_HI:
    break;
  default:
    return false;
  }

  // We support expressions of the form "(sym1 binop1 sym2) binop2 const",
  // where "binop2 const" is optional.
  if (isa<MCBinaryExpr>(BE->getLHS())) {
    if (!isa<MCConstantExpr>(BE->getRHS()))
      return false;
    BE = cast<MCBinaryExpr>(BE->getLHS());
  }
  return (isa<MCSymbolRefExpr>(BE->getLHS())
          && isa<MCSymbolRefExpr>(BE->getRHS()));
}

const ZCPUMCExpr*
ZCPUMCExpr::create(MCSymbolRefExpr::VariantKind VK, const MCExpr *Expr,
                   MCContext &Ctx) {
  VariantKind Kind;
  switch (VK) {
  case MCSymbolRefExpr::VK_ZCPU_ABS_LO:
    Kind = VK_ZCPU_LO;
    break;
  case MCSymbolRefExpr::VK_ZCPU_ABS_HI:
    Kind = VK_ZCPU_HI;
    break;
  default:
    llvm_unreachable("Invalid kind!");
  }

  return new (Ctx) ZCPUMCExpr(Kind, Expr);
}

void ZCPUMCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {
  switch (Kind) {
  default: llvm_unreachable("Invalid kind!");
  case VK_ZCPU_LO: OS << "%lo"; break;
  case VK_ZCPU_HI: OS << "%hi"; break;
  }

  OS << '(';
  Expr->print(OS, MAI);
  OS << ')';
}

bool
ZCPUMCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                      const MCAsmLayout *Layout,
                                      const MCFixup *Fixup) const {
  return getSubExpr()->evaluateAsRelocatable(Res, Layout, Fixup);
}

void ZCPUMCExpr::visitUsedExpr(MCStreamer &Streamer) const {
  Streamer.visitUsedExpr(*getSubExpr());
}

#endif
