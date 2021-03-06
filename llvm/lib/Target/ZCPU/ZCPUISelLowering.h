//- ZCPUISelLowering.h - ZCPU DAG Lowering Interface -*- C++ -*-//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file defines the interfaces that ZCPU uses to lower LLVM
/// code into a selection DAG.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_ZCPUISELLOWERING_H
#define LLVM_LIB_TARGET_ZCPU_ZCPUISELLOWERING_H

#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetOptions.h"

namespace llvm {

    class ZCPUSubtarget;

    class ZCPUTargetMachine;

    namespace ZCPUISD {
        // X86 Specific DAG Nodes
        enum NodeType : unsigned {
            FIRST_NUMBER = ISD::BUILTIN_OP_END,
            ARGUMENT,
            RET_FLAG,
            Wrapper,
            CALL1,
            CALL0,
            IRET,
            ADJDYNALLOC,
        };
    }


    namespace ZCPU {
        FastISel *createFastISel(FunctionLoweringInfo &funcInfo,
                                 const TargetLibraryInfo *libInfo);
    }  // end namespace ZCPU

    // end namespace llvm
    class ZCPUTargetLowering final : public TargetLowering {
    public:
        ZCPUTargetLowering(const TargetMachine &TM,
                           const ZCPUSubtarget &STI);

    private:
        /// Keep a pointer to the ZCPUSubtarget around so that we can make the
        /// right decision when generating code for different targets.
        const ZCPUSubtarget *Subtarget;

        FastISel *createFastISel(FunctionLoweringInfo &FuncInfo, const TargetLibraryInfo *LibInfo) const override;

        MVT getScalarShiftAmountTy(const DataLayout &DL, EVT) const override;

        const char *getTargetNodeName(unsigned Opcode) const override;

        std::pair<unsigned, const TargetRegisterClass *> getRegForInlineAsmConstraint(
                const TargetRegisterInfo *TRI, StringRef Constraint,
                MVT VT) const override;

        bool isCheapToSpeculateCttz() const override;

        bool isCheapToSpeculateCtlz() const override;

        bool isLegalAddressingMode(const DataLayout &DL, const AddrMode &AM, Type *Ty,
                                   unsigned AS) const override;

        bool allowsMisalignedMemoryAccesses(EVT, unsigned AddrSpace, unsigned Align,
                                            bool *Fast) const override;

        bool isIntDivCheap(EVT VT, AttributeSet Attr) const override;

        SDValue LowerCall(CallLoweringInfo &CLI,
                          SmallVectorImpl<SDValue> &InVals) const override;

        bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                            bool isVarArg,
                            const SmallVectorImpl<ISD::OutputArg> &Outs,
                            LLVMContext &Context) const override;

        SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool isVarArg,
                            const SmallVectorImpl<ISD::OutputArg> &Outs,
                            const SmallVectorImpl<SDValue> &OutVals, const SDLoc &dl,
                            SelectionDAG &DAG) const override;

        SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                                     bool IsVarArg,
                                     const SmallVectorImpl<ISD::InputArg> &Ins,
                                     const SDLoc &DL, SelectionDAG &DAG,
                                     SmallVectorImpl<SDValue> &InVals) const override;

        // Custom lowering hooks.
        SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

        SDValue LowerFrameIndex(SDValue Op, SelectionDAG &DAG) const;

        SDValue LowerFRAMEADDR(SDValue Op, SelectionDAG &DAG) const;

        SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

        SDValue LowerExternalSymbol(SDValue Op, SelectionDAG &DAG) const;

        SDValue LowerBR_JT(SDValue Op, SelectionDAG &DAG) const;

        SDValue LowerJumpTable(SDValue Op, SelectionDAG &DAG) const;

        SDValue LowerVASTART(SDValue Op, SelectionDAG &DAG) const;

        SDValue LowerCopyToReg(SDValue Op, SelectionDAG &DAG) const;

        const MCPhysReg *getScratchRegisters(CallingConv::ID) const override;

        bool IsDesirableToPromoteOp(SDValue Op, EVT &PVT) const override;

        //void LowerAsmOperandForConstraint(SDValue Op, std::string &Constraint, std::vector<SDValue> &Ops, SelectionDAG &DAG)  const override;
        SDValue LowerSRA(SDValue value, SelectionDAG &dag) const;

        SDValue LowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) const;
    };
}
#endif
