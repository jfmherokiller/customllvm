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

#define DEBUG_TYPE "wasm"

extern "C" void LLVMInitializeZCPUTarget() {
  // Register the target.
  RegisterTargetMachine<ZCPUTargetMachine> X(TheZCPUTarget32);
  RegisterTargetMachine<ZCPUTargetMachine> Y(TheZCPUTarget64);
}

//===----------------------------------------------------------------------===//
// ZCPU Lowering public interface.
//===----------------------------------------------------------------------===//

static Reloc::Model getEffectiveRelocModel(Optional<Reloc::Model> RM) {
  if (!RM.hasValue())
    return Reloc::PIC_;
  return *RM;
}

/// Create an ZCPU architecture model.
///
ZCPUTargetMachine::ZCPUTargetMachine(
    const Target &T, const Triple &TT, StringRef CPU, StringRef FS,
    const TargetOptions &Options, Optional<Reloc::Model> RM,
    CodeModel::Model CM, CodeGenOpt::Level OL)
    : LLVMTargetMachine(T,
                        TT.isArch64Bit() ? "e-m:e-p:64:64-i64:64-n32:64-S128"
                                         : "e-m:e-p:32:32-i64:64-n32:64-S128",
                        TT, CPU, FS, Options, getEffectiveRelocModel(RM),
                        CM, OL),
      TLOF(make_unique<ZCPUTargetObjectFile>()) {
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
  ZCPUPassConfig(ZCPUTargetMachine *TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  ZCPUTargetMachine &getZCPUTargetMachine() const {
    return getTM<ZCPUTargetMachine>();
  }

  FunctionPass *createTargetRegisterAllocator(bool) override;

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

TargetPassConfig *
ZCPUTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new ZCPUPassConfig(this, PM);
}

FunctionPass *ZCPUPassConfig::createTargetRegisterAllocator(bool) {
  return nullptr; // No reg alloc
}

//===----------------------------------------------------------------------===//
// The following functions are called from lib/CodeGen/Passes.cpp to modify
// the CodeGen pass sequence.
//===----------------------------------------------------------------------===//

void ZCPUPassConfig::addIRPasses() {
  if (TM->Options.ThreadModel == ThreadModel::Single)
    // In "single" mode, atomics get lowered to non-atomics.
    addPass(createLowerAtomicPass());
  else
    // Expand some atomic operations. ZCPUTargetLowering has hooks which
    // control specifically what gets lowered.
    addPass(createAtomicExpandPass(TM));

  // Optimize "returned" function attributes.
  if (getOptLevel() != CodeGenOpt::None)
    addPass(createZCPUOptimizeReturned());

  TargetPassConfig::addIRPasses();
}

bool ZCPUPassConfig::addInstSelector() {
  (void)TargetPassConfig::addInstSelector();
  addPass(
      createZCPUISelDag(getZCPUTargetMachine(), getOptLevel()));
  // Run the argument-move pass immediately after the ScheduleDAG scheduler
  // so that we can fix up the ARGUMENT instructions before anything else
  // sees them in the wrong place.
  addPass(createZCPUArgumentMove());
  // Set the p2align operands. This information is present during ISel, however
  // it's inconvenient to collect. Collect it now, and update the immediate
  // operands.
  addPass(createZCPUSetP2AlignOperands());
  return false;
}

void ZCPUPassConfig::addPostRegAlloc() {
  // TODO: The following CodeGen passes don't currently support code containing
  // virtual registers. Consider removing their restrictions and re-enabling
  // them.

  // Has no asserts of its own, but was not written to handle virtual regs.
  disablePass(&ShrinkWrapID);

  // These functions all require the AllVRegsAllocated property.
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

  // Now that we have a prologue and epilogue and all frame indices are
  // rewritten, eliminate SP and FP. This allows them to be stackified,
  // colored, and numbered with the rest of the registers.
  addPass(createZCPUReplacePhysRegs());

  if (getOptLevel() != CodeGenOpt::None) {
    // LiveIntervals isn't commonly run this late. Re-establish preconditions.
    addPass(createZCPUPrepareForLiveIntervals());

    // Depend on LiveIntervals and perform some optimizations on it.
    addPass(createZCPUOptimizeLiveIntervals());

    // Prepare store instructions for register stackifying.
    addPass(createZCPUStoreResults());

    // Mark registers as representing wasm's expression stack. This is a key
    // code-compression technique in ZCPU. We run this pass (and
    // StoreResults above) very late, so that it sees as much code as possible,
    // including code emitted by PEI and expanded by late tail duplication.
    addPass(createZCPURegStackify());

    // Run the register coloring pass to reduce the total number of registers.
    // This runs after stackification so that it doesn't consider registers
    // that become stackified.
    addPass(createZCPURegColoring());
  }

  // Eliminate multiple-entry loops.
  addPass(createZCPUFixIrreducibleControlFlow());

  // Put the CFG in structured form; insert BLOCK and LOOP markers.
  addPass(createZCPUCFGStackify());

  // Lower br_unless into br_if.
  addPass(createZCPULowerBrUnless());

  // Perform the very last peephole optimizations on the code.
  if (getOptLevel() != CodeGenOpt::None)
    addPass(createZCPUPeephole());

  // Create a mapping from LLVM CodeGen virtual registers to wasm registers.
  addPass(createZCPURegNumbering());
}
