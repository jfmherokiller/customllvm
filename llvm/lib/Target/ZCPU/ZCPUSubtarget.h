//=- ZCPUSubtarget.h - Define Subtarget for the ZCPU -*- C++ -*-//
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
/// TargetSubtarget.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUSUBTARGET_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUSUBTARGET_H

#include "ZCPUFrameLowering.h"
#include "ZCPUISelLowering.h"
#include "ZCPUInstrInfo.h"
#include "ZCPUSelectionDAGInfo.h"
#include "llvm/Target/TargetSubtargetInfo.h"
#include <string>

#define GET_SUBTARGETINFO_HEADER

#include "ZCPUGenSubtargetInfo.inc"

namespace llvm {

    class ZCPUSubtarget final : public ZCPUGenSubtargetInfo {

        /// String name of used CPU.
        std::string CPUString;

        /// What processor and OS we're targeting.
        Triple TargetTriple;

        ZCPUFrameLowering FrameLowering;
        ZCPUInstrInfo InstrInfo;
        ZCPUSelectionDAGInfo TSInfo;
        ZCPUTargetLowering TLInfo;

        /// Initializes using CPUString and the passed in feature string so that we
        /// can use initializer lists for subtarget initialization.
        ZCPUSubtarget &initializeSubtargetDependencies(StringRef FS);

    public:
        unsigned getStackAlignment() const { return stackAlignment; }
        /// This constructor initializes the data members to match that
        /// of the specified triple.
        ZCPUSubtarget(const Triple &TT, const std::string &CPU,
                      const std::string &FS, const TargetMachine &TM);

        const ZCPUSelectionDAGInfo *getSelectionDAGInfo() const override {
            return &TSInfo;
        }

        const ZCPUFrameLowering *getFrameLowering() const override {
            return &FrameLowering;
        }

        const ZCPUTargetLowering *getTargetLowering() const override {
            return &TLInfo;
        }

        const ZCPUInstrInfo *getInstrInfo() const override {
            return &InstrInfo;
        }

        const ZCPURegisterInfo *getRegisterInfo() const override {
            return &getInstrInfo()->getRegisterInfo();
        }

        const Triple &getTargetTriple() const { return TargetTriple; }

        bool enableMachineScheduler() const override { return true; }

        bool useAA() const override;

        /// Parses features string setting specified subtarget options. Definition of
        /// function is auto generated by tblgen.
        void ParseSubtargetFeatures(StringRef CPU, StringRef FS);

    protected:
        unsigned stackAlignment = 4;
    };

} // end namespace llvm

#endif
