//===-- ZCPUTargetObjectFile.h - ZCPU Object Info -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file declares the ZCPU-specific subclass of
/// TargetLoweringObjectFile.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUTARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUTARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {

    class ZCPUTargetObjectFile final : public TargetLoweringObjectFileELF {
    public:
        void Initialize(MCContext &Ctx, const TargetMachine &TM) override;
    };

} // end namespace llvm

#endif
