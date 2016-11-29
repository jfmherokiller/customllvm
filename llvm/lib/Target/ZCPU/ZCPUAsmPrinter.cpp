//===-- ZCPUAsmPrinter.cpp - ZCPU LLVM assembly writer ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file contains a printer that converts from our internal
/// representation of machine-dependent LLVM code to the ZCPU assembly
/// language.
///
//===----------------------------------------------------------------------===//

#include "ZCPU.h"
#include "InstPrinter/ZCPUInstPrinter.h"
#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "MCTargetDesc/ZCPUTargetStreamer.h"
#include "ZCPUMCInstLower.h"
#include "ZCPUMachineFunctionInfo.h"
#include "ZCPURegisterInfo.h"
#include "ZCPUSubtarget.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/CodeGen/Analysis.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

namespace {

    class ZCPUAsmPrinter final : public AsmPrinter {
        const MachineRegisterInfo *MRI;
        const ZCPUFunctionInfo *MFI;

    public:
        ZCPUAsmPrinter(TargetMachine &TM, std::unique_ptr<MCStreamer> Streamer)
                : AsmPrinter(TM, std::move(Streamer)), MRI(nullptr), MFI(nullptr) {

        }

    private:
        const char *getPassName() const override {
            return "ZCPU Assembly Printer";
        }

        //===------------------------------------------------------------------===//
        // MachineFunctionPass Implementation.
        //===------------------------------------------------------------------===//

        bool runOnMachineFunction(MachineFunction &MF) override {
            MRI = &MF.getRegInfo();
            MFI = MF.getInfo<ZCPUFunctionInfo>();
            return AsmPrinter::runOnMachineFunction(MF);
        }

        //===------------------------------------------------------------------===//
        // AsmPrinter Implementation.
        //===------------------------------------------------------------------===//

        void EmitEndOfAsmFile(Module &M) override;

        void EmitJumpTableInfo() override;

        void EmitConstantPool() override;

        void EmitFunctionBodyStart() override;

        void EmitFunctionBodyEnd() override;

        void EmitInstruction(const MachineInstr *MI) override;

        const MCExpr *lowerConstant(const Constant *CV) override;

        bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                             unsigned AsmVariant, const char *ExtraCode,
                             raw_ostream &OS) override;

        bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNo,
                                   unsigned AsmVariant, const char *ExtraCode,
                                   raw_ostream &OS) override;

        MVT getRegType(unsigned RegNo) const;

        const char *toString(MVT VT) const;

        std::string regToString(const MachineOperand &MO);

        ZCPUTargetStreamer *getTargetStreamer();
    };

} // end anonymous namespace

//===----------------------------------------------------------------------===//
// Helpers.
//===----------------------------------------------------------------------===//

MVT ZCPUAsmPrinter::getRegType(unsigned RegNo) const {
    const TargetRegisterClass *TRC = MRI->getRegClass(RegNo);
    for (MVT T : {MVT::i64,MVT::f64})
        if (TRC->hasType(T))
            return T;
    DEBUG(errs() << "Unknown type for register number: " << RegNo);
    llvm_unreachable("Unknown register type");
    return MVT::Other;
}

const char *ZCPUAsmPrinter::toString(MVT VT) const {
    return ZCPU::TypeToString(VT);
}

std::string ZCPUAsmPrinter::regToString(const MachineOperand &MO) {
    unsigned RegNo = MO.getReg();
    assert(TargetRegisterInfo::isVirtualRegister(RegNo) &&
           "Unlowered physical register encountered during assembly printing");
    assert(!MFI->isVRegStackified(RegNo));
    unsigned WAReg = MFI->getWAReg(RegNo);
    assert(WAReg != ZCPUFunctionInfo::UnusedReg);
    return '$' + utostr(WAReg);
}

ZCPUTargetStreamer *ZCPUAsmPrinter::getTargetStreamer() {
    MCTargetStreamer *TS = OutStreamer->getTargetStreamer();
    return static_cast<ZCPUTargetStreamer *>(TS);
}

//===----------------------------------------------------------------------===//
// ZCPUAsmPrinter Implementation.
//===----------------------------------------------------------------------===//
static void ComputeLegalValueVTs(const Function &F, const TargetMachine &TM,
                                 Type *Ty, SmallVectorImpl<MVT> &ValueVTs) {
    const DataLayout &DL(F.getParent()->getDataLayout());
    const ZCPUTargetLowering &TLI =
            *TM.getSubtarget<ZCPUSubtarget>(F).getTargetLowering();
    SmallVector<EVT, 4> VTs;
    ComputeValueVTs(TLI, DL, Ty, VTs);

    for (EVT VT : VTs) {
        unsigned NumRegs = TLI.getNumRegisters(F.getContext(), VT);
        MVT RegisterVT = TLI.getRegisterType(F.getContext(), VT);
        for (unsigned i = 0; i != NumRegs; ++i)
            ValueVTs.push_back(RegisterVT);
    }
}

void ZCPUAsmPrinter::EmitEndOfAsmFile(Module &M) {
    for (const auto &F : M) {
        // Emit function type info for all undefined functions
        if (F.isDeclarationForLinker() && !F.isIntrinsic()) {
            SmallVector<MVT, 4> SignatureVTs;
            ComputeLegalValueVTs(F, TM, F.getReturnType(), SignatureVTs);
            size_t NumResults = SignatureVTs.size();
            if (SignatureVTs.size() > 1) {
                // ZCPU currently can't lower returns of multiple values without
                // demoting to sret (see ZCPUTargetLowering::CanLowerReturn). So
                // replace multiple return values with a pointer parameter.
                SignatureVTs.clear();
                SignatureVTs.push_back(
                        MVT::getIntegerVT(M.getDataLayout().getPointerSizeInBits()));
                NumResults = 0;
            }

            for (auto &Arg : F.args()) {
                ComputeLegalValueVTs(F, TM, Arg.getType(), SignatureVTs);
            }

            getTargetStreamer()->emitIndirectFunctionType(F.getName(), SignatureVTs,
                                                          NumResults);
        }
    }
}

void ZCPUAsmPrinter::EmitConstantPool() {
    assert(MF->getConstantPool()->getConstants().empty() &&
           "ZCPU disables constant pools");
}

void ZCPUAsmPrinter::EmitJumpTableInfo() {
    // Nothing to do; jump tables are incorporated into the instruction stream.
}

void ZCPUAsmPrinter::EmitFunctionBodyStart() {
    AsmPrinter::EmitFunctionBodyStart();
}

void ZCPUAsmPrinter::EmitFunctionBodyEnd() {
    getTargetStreamer()->emitEndFunc();
}

void ZCPUAsmPrinter::EmitInstruction(const MachineInstr *MI) {
    DEBUG(dbgs() << "EmitInstruction: " << *MI << '\n');
    switch (MI->getOpcode()) {
        case ZCPU::FALLTHROUGH_RETURN_VOID:
            // This instruction represents the implicit return at the end of a
            // function body with no return value.
            if (isVerbose()) {
                OutStreamer->AddComment("fallthrough-return");
                OutStreamer->AddBlankLine();
            }
            break;
        default: {
            ZCPUMCInstLower MCInstLowering(OutContext, *this);
            MCInst TmpInst;
            MCInstLowering.Lower(MI, TmpInst);
            EmitToStreamer(*OutStreamer, TmpInst);
            break;
        }
    }
}

const MCExpr *ZCPUAsmPrinter::lowerConstant(const Constant *CV) {
    return AsmPrinter::lowerConstant(CV);
}

bool ZCPUAsmPrinter::PrintAsmOperand(const MachineInstr *MI,
                                     unsigned OpNo, unsigned AsmVariant,
                                     const char *ExtraCode,
                                     raw_ostream &OS) {

    if (AsmVariant != 0)
        report_fatal_error("There are no defined alternate asm variants");

    // First try the generic code, which knows about modifiers like 'c' and 'n'.
    if (!AsmPrinter::PrintAsmOperand(MI, OpNo, AsmVariant, ExtraCode, OS))
        return false;

    if (!ExtraCode) {
        const MachineOperand &MO = MI->getOperand(OpNo);
        switch (MO.getType()) {
            case MachineOperand::MO_Immediate:
                OS << MO.getImm();
                return false;
            case MachineOperand::MO_Register:
                OS << regToString(MO);
                return false;
            case MachineOperand::MO_GlobalAddress:
                getSymbol(MO.getGlobal())->print(OS, MAI);
                printOffset(MO.getOffset(), OS);
                return false;
            case MachineOperand::MO_ExternalSymbol:
                GetExternalSymbolSymbol(MO.getSymbolName())->print(OS, MAI);
                printOffset(MO.getOffset(), OS);
                return false;
            case MachineOperand::MO_MachineBasicBlock:
                MO.getMBB()->getSymbol()->print(OS, MAI);
                return false;
            default:
                break;
        }
    }
    return true;
}

bool ZCPUAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
                                           unsigned OpNo,
                                           unsigned AsmVariant,
                                           const char *ExtraCode,
                                           raw_ostream &OS) {
    if (AsmVariant != 0)
        report_fatal_error("There are no defined alternate asm variants");

    if (!ExtraCode) {
        OS << regToString(MI->getOperand(OpNo));
        return false;
    }

    return AsmPrinter::PrintAsmMemoryOperand(MI, OpNo, AsmVariant, ExtraCode, OS);
}
// Force static initialization.
extern "C" void LLVMInitializeZCPUAsmPrinter() {
    RegisterAsmPrinter<ZCPUAsmPrinter> X(TheZCPUTarget32);
}



