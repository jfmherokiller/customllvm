//===- ZCPUTargetMachine.cpp - Define TargetMachine for ZCPU -==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file defines the ZCPU-specific subclass of TargetMachine.
///
//===----------------------------------------------------------------------===//

#include "ZCPU.h"
#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "ZCPUTargetMachine.h"
#include "ZCPUTargetObjectFile.h"
#include "ZCPUTargetTransformInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/RegAllocRegistry.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/Scalar.h"

using namespace llvm;

#define DEBUG_TYPE "zcpu"

extern "C" void LLVMInitializeZCPUTarget() {
    // Register the target.
    RegisterTargetMachine<ZCPUTargetMachine> X(TheZCPUTarget32);
}

//===----------------------------------------------------------------------===//
// ZCPU Lowering public interface.
//===----------------------------------------------------------------------===//

static Reloc::Model getEffectiveRelocModel(Optional<Reloc::Model> RM) {
    return Reloc::Static;
}

/// Create an ZCPU architecture model.
///
ZCPUTargetMachine::ZCPUTargetMachine(
        const Target &T, const Triple &TT, StringRef CPU, StringRef FS,
        const TargetOptions &Options, Optional<Reloc::Model> RM,
        CodeModel::Model CM, CodeGenOpt::Level OL)
        : LLVMTargetMachine(T,
                            "e-p:64:64:64-i64:64:64-f64:64:64-n64:64",
                            TT, CPU, FS, Options, getEffectiveRelocModel(RM),
                            CM, OL), TLOF(make_unique<ZCPUTargetObjectFile>()) {
    // ZCPU type-checks expressions, but a noreturn function with a return
    // type that doesn't match the context will cause a check failure. So we lower
    // LLVM 'unreachable' to ISD::TRAP and then lower that to ZCPU's
    // 'unreachable' expression which is meant for that case.
    this->Options.TrapUnreachable = true;

    initAsmInfo();

    // Note that we don't use setRequiresStructuredCFG(true). It disables
    // optimizations than we're ok with, and want, such as critical edge
    // splitting and tail merging.
}

ZCPUTargetMachine::~ZCPUTargetMachine() {}

const ZCPUSubtarget *
ZCPUTargetMachine::getSubtargetImpl(const Function &F) const {
    Attribute CPUAttr = F.getFnAttribute("target-cpu");
    Attribute FSAttr = F.getFnAttribute("target-features");

    std::string CPU = !CPUAttr.hasAttribute(Attribute::None)
                      ? CPUAttr.getValueAsString().str()
                      : TargetCPU;
    std::string FS = !FSAttr.hasAttribute(Attribute::None)
                     ? FSAttr.getValueAsString().str()
                     : TargetFS;

    auto &I = SubtargetMap[CPU + FS];
    if (!I) {
        // This needs to be done before we create a new subtarget since any
        // creation will depend on the TM and the code generation flags on the
        // function that reside in TargetOptions.
        resetTargetOptions(F);
        I = llvm::make_unique<ZCPUSubtarget>(TargetTriple, CPU, FS, *this);
    }
    return I.get();
}

namespace {
/// ZCPU Code Generator Pass Configuration Options.
    class ZCPUPassConfig final : public TargetPassConfig {
    public:
        ZCPUPassConfig(ZCPUTargetMachine *TM, PassManagerBase &PM) : TargetPassConfig(TM, PM) {}

        ZCPUTargetMachine &getZCPUTargetMachine() const {
            return getTM<ZCPUTargetMachine>();
        }

        void addIRPasses() override;

        bool addInstSelector() override;

        void addPostRegAlloc() override;

        bool addGCPasses() override { return false; }

        void addPreEmitPass() override;
    };
} // end anonymous namespace

TargetIRAnalysis ZCPUTargetMachine::getTargetIRAnalysis() {
    return TargetIRAnalysis([this](const Function &F) {
        return TargetTransformInfo(ZCPUTTIImpl(this, F));
    });
}

TargetPassConfig *ZCPUTargetMachine::createPassConfig(PassManagerBase &PM) {
    return new ZCPUPassConfig(this, PM);
}

//===----------------------------------------------------------------------===//
// The following functions are called from lib/CodeGen/Passes.cpp to modify
// the CodeGen pass sequence.
//===----------------------------------------------------------------------===//

void ZCPUPassConfig::addIRPasses() {
    addPass(createLowerAtomicPass());

    TargetPassConfig::addIRPasses();
}

bool ZCPUPassConfig::addInstSelector() {
    (void) TargetPassConfig::addInstSelector();
    addPass(createZCPUISelDag(getZCPUTargetMachine(), getOptLevel()));
    return false;
}

void ZCPUPassConfig::addPostRegAlloc() {
    disablePass(&ShrinkWrapID);
    disablePass(&MachineCopyPropagationID);
    disablePass(&PostRASchedulerID);
    disablePass(&FuncletLayoutID);
    disablePass(&StackMapLivenessID);
    disablePass(&LiveDebugValuesID);
    disablePass(&PatchableFunctionID);

    TargetPassConfig::addPostRegAlloc();
}

void ZCPUPassConfig::addPreEmitPass() {
    TargetPassConfig::addPreEmitPass();

}
