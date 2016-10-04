//===-- ZCPUFixupKinds.h - ZCPU Specific Fixup Entries ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef ZCPUFIXUPKINDS_H
#define ZCPUFIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
  namespace ZCPU {
    enum Fixups {

      // Marker
      LastTargetFixupKind = FirstTargetFixupKind,
      NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
    };
  } // end namespace ZCPU
} // end namespace llvm

#endif
