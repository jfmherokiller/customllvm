//===-- llvm/Target/ZCPUTargetObjectFile.h - ZCPU Object Info ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUTARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUTARGETOBJECTFILE_H

#include "ZCPUConfig.h"
#if CH >= CH3_1

#include "ZCPUTargetMachine.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {
class ZCPUTargetMachine;
  class ZCPUTargetObjectFile : public TargetLoweringObjectFileELF {
    MCSection *SmallDataSection;
    MCSection *SmallBSSSection;
    const ZCPUTargetMachine *TM;
  public:

    void Initialize(MCContext &Ctx, const TargetMachine &TM) override;

#if CH >= CH6_1
    /// IsGlobalInSmallSection - Return true if this global address should be
    /// placed into small data/bss section.
    bool IsGlobalInSmallSection(const GlobalValue *GV,
                                const TargetMachine &TM, SectionKind Kind) const;
    bool IsGlobalInSmallSection(const GlobalValue *GV,
                                const TargetMachine &TM) const;
    bool IsGlobalInSmallSectionImpl(const GlobalValue *GV,
                                    const TargetMachine &TM) const;

    MCSection *SelectSectionForGlobal(const GlobalValue *GV, SectionKind Kind,
                                      Mangler &Mang,
                                      const TargetMachine &TM) const override;
#endif
  };
} // end namespace llvm

#endif // #if CH >= CH3_1

#endif
