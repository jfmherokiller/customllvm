//==-- ZCPUTargetStreamer.h - ZCPU Target Streamer -*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file declares ZCPU-specific target streamer classes.
/// These are for implementing support for target-specific assembly directives.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUTARGETSTREAMER_H
#define LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUTARGETSTREAMER_H

#include "llvm/CodeGen/MachineValueType.h"
#include "llvm/MC/MCStreamer.h"

namespace llvm {

    class MCELFStreamer;

/// ZCPU-specific streamer interface, to implement support
/// ZCPU-specific assembly directives.
    class ZCPUTargetStreamer : public MCTargetStreamer {
    public:
        explicit ZCPUTargetStreamer(MCStreamer &S);

        /// .param
        virtual void emitParam(ArrayRef<MVT> Types) = 0;

        /// .result
        virtual void emitResult(ArrayRef<MVT> Types) = 0;

        /// .local
        virtual void emitLocal(ArrayRef<MVT> Types) = 0;

        /// .endfunc
        virtual void emitEndFunc() = 0;

        /// .functype
        virtual void emitIndirectFunctionType(StringRef name,
                                              SmallVectorImpl<MVT> &SignatureVTs,
                                              size_t NumResults) {
            llvm_unreachable("emitIndirectFunctionType not implemented");
        }


    };

/// This part is for ascii assembly output
    class ZCPUTargetAsmStreamer final : public ZCPUTargetStreamer {
        formatted_raw_ostream &OS;

    public:
        ZCPUTargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);

        void emitParam(ArrayRef<MVT> Types) override;

        void emitResult(ArrayRef<MVT> Types) override;

        void emitLocal(ArrayRef<MVT> Types) override;

        void emitEndFunc() override;

        void emitIndirectFunctionType(StringRef name,
                                      SmallVectorImpl<MVT> &SignatureVTs,
                                      size_t NumResults) override;

        virtual void EmitValueToAlignment(unsigned ByteAlignment, int64_t Value = 0,
                                          unsigned ValueSize = 1,
                                          unsigned MaxBytesToEmit = 0) {};
    };

/// This part is for ELF object output
    class ZCPUTargetELFStreamer final : public ZCPUTargetStreamer {
    public:
        explicit ZCPUTargetELFStreamer(MCStreamer &S);

        void emitParam(ArrayRef<MVT> Types) override;

        void emitResult(ArrayRef<MVT> Types) override;

        void emitLocal(ArrayRef<MVT> Types) override;

        void emitEndFunc() override;
    };

} // end namespace llvm

#endif
