//=- ZCPUISelLowering.cpp - ZCPU DAG Lowering Implementation -==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file implements the ZCPUTargetLowering class.
///
//===----------------------------------------------------------------------===//

#include <llvm/Target/TargetLowering.h>
#include "ZCPUISelLowering.h"
#include "MCTargetDesc/ZCPUMCTargetDesc.h"
#include "ZCPUMachineFunctionInfo.h"
#include "ZCPUSubtarget.h"
#include "ZCPUTargetMachine.h"
#include "llvm/CodeGen/Analysis.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/IR/DiagnosticPrinter.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define DEBUG_TYPE "zcpu-lower"

ZCPUTargetLowering::ZCPUTargetLowering(const TargetMachine &TM, const ZCPUSubtarget &STI)
        : TargetLowering(TM), Subtarget(&STI) {
    auto MVTPtr = MVT::i32;

    // Booleans always contain 0 or 1.
    setBooleanContents(ZeroOrOneBooleanContent);
    // ZCPU does not produce floating-point exceptions on normal floating
    // point operations.
    setHasFloatingPointExceptions(false);
    // We don't know the microarchitecture here, so just reduce register pressure.
    setSchedulingPreference(Sched::RegPressure);
    // Tell ISel that we have a stack pointer.
    setStackPointerRegisterToSaveRestore(ZCPU::ESP);
    addRegisterClass(MVT::i64, &ZCPU::GPRIntRegClass);
    addRegisterClass(MVT::i64, &ZCPU::SegmentRegsRegClass);
    addRegisterClass(MVT::i64, &ZCPU::ExtendedGPRIntRegClass);
    addRegisterClass(MVT::f64, &ZCPU::GPRFloatRegClass);
    addRegisterClass(MVT::f64, &ZCPU::ExtendedGPRFloatRegClass);

    // Compute derived properties from the register classes.
    computeRegisterProperties(Subtarget->getRegisterInfo());

    // As a special case, these operators use the type to mean the type to
    // sign-extend from.
    for (auto T : {MVT::i1, MVT::i8, MVT::i16, MVT::i32})
        setOperationAction(ISD::SIGN_EXTEND_INREG, T, Expand);

    // Dynamic stack allocation: use the default expansion.
    setOperationAction(ISD::STACKSAVE, MVT::Other, Expand);
    setOperationAction(ISD::STACKRESTORE, MVT::Other, Expand);
    setOperationAction(ISD::DYNAMIC_STACKALLOC, MVTPtr, Expand);

    //setOperationAction(ISD::FrameIndex, MVT::i32, Custom);
    //setOperationAction(ISD::CopyToReg, MVT::Other, Custom);

    // Expand these forms; we pattern-match the forms that we can handle in isel.
    for (auto T : {MVT::i32, MVT::i64, MVT::f32, MVT::f64})
        for (auto Op : {ISD::BR_CC, ISD::SELECT_CC})
            setOperationAction(Op, T, Expand);

    // We have custom switch handling.
    //setOperationAction(ISD::BR_JT, MVT::Other, Custom);

    // ZCPU doesn't have:
    //  - Floating-point extending loads.
    //  - Floating-point truncating stores.
    //  - i1 extending loads.
    setLoadExtAction(ISD::EXTLOAD, MVT::f64, MVT::f32, Expand);
    for (auto T : MVT::integer_valuetypes())
        for (auto Ext : {ISD::EXTLOAD, ISD::ZEXTLOAD, ISD::SEXTLOAD})
            setLoadExtAction(Ext, T, MVT::i1, Promote);

    // We don't accept any truncstore of integer registers.
    setTruncStoreAction(MVT::i64, MVT::i32, Promote);
    setTruncStoreAction(MVT::i64, MVT::i16, Promote);
    setTruncStoreAction(MVT::i64, MVT::i8, Promote);
    setTruncStoreAction(MVT::i64, MVT::i1, Promote);
    setTruncStoreAction(MVT::f64, MVT::f32, Promote);
    setTruncStoreAction(MVT::f64, MVT::f16, Promote);

    setOperationAction(ISD::UINT_TO_FP, MVT::i1, Promote);
    setOperationAction(ISD::UINT_TO_FP, MVT::i8, Promote);
    setOperationAction(ISD::UINT_TO_FP, MVT::i16, Promote);
    setOperationAction(ISD::UINT_TO_FP, MVT::i32, Promote);

}

FastISel *ZCPUTargetLowering::createFastISel(
        FunctionLoweringInfo &FuncInfo, const TargetLibraryInfo *LibInfo) const {
    return ZCPU::createFastISel(FuncInfo, LibInfo);
}

bool ZCPUTargetLowering::isOffsetFoldingLegal(
        const GlobalAddressSDNode * /*GA*/) const {
    // All offsets can be folded.
    return true;
}

MVT ZCPUTargetLowering::getScalarShiftAmountTy(const DataLayout & /*DL*/, EVT VT) const {
    unsigned BitWidth = NextPowerOf2(VT.getSizeInBits() - 1);
    if (BitWidth > 1 && BitWidth < 8) BitWidth = 8;

    if (BitWidth > 64) {
        // The shift will be lowered to a libcall, and compiler-rt libcalls expect
        // the count to be an i32.
        BitWidth = 32;
        assert(BitWidth >= Log2_32_Ceil(VT.getSizeInBits()) &&
               "32-bit shift counts ought to be enough for anyone");
    }

    MVT Result = MVT::getIntegerVT(BitWidth);
    assert(Result != MVT::INVALID_SIMPLE_VALUE_TYPE &&
           "Unable to represent scalar shift amount type");
    return Result;
}

const char *ZCPUTargetLowering::getTargetNodeName(
        unsigned Opcode) const {
    switch (Opcode) {
        case ZCPUISD::RET_FLAG:               return "ZCPUISD::RET_FLAG";
        case ZCPUISD::IRET:               return "ZCPUISD::IRET";
        default:                         return NULL;
    }
}

std::pair<unsigned, const TargetRegisterClass *> ZCPUTargetLowering::getRegForInlineAsmConstraint(
        const TargetRegisterInfo *TRI, StringRef Constraint, MVT VT) const {
    // First, see if this is a constraint that directly corresponds to a
    // WebAssembly register class.
    if (Constraint.size() == 1) {
        switch (Constraint[0]) {
            case 'r':
                assert(VT != MVT::iPTR && "Pointer MVT not expected here");
                if (VT.isInteger() && !VT.isVector()) {
                    return std::make_pair(0U, &ZCPU::BothNormAndExtendedRegClass);
                }
                break;
            default:
                break;
        }
    }

    return TargetLowering::getRegForInlineAsmConstraint(TRI, Constraint, VT);
}

bool ZCPUTargetLowering::isCheapToSpeculateCttz() const {
    // Assume ctz is a relatively cheap operation.
    return true;
}

bool ZCPUTargetLowering::isCheapToSpeculateCtlz() const {
    // Assume clz is a relatively cheap operation.
    return true;
}

bool ZCPUTargetLowering::isLegalAddressingMode(const DataLayout &DL,
                                               const AddrMode &AM,
                                               Type *Ty,
                                               unsigned AS) const {
    // Everything else is legal.
    return true;
}

bool ZCPUTargetLowering::allowsMisalignedMemoryAccesses(EVT /*VT*/, unsigned /*AddrSpace*/, unsigned /*Align*/,
                                                        bool *Fast) const {
    return true;
}

bool ZCPUTargetLowering::isIntDivCheap(EVT VT, AttributeSet Attr) const {
    return true;
}

//===----------------------------------------------------------------------===//
// ZCPU Lowering private implementation.
//===----------------------------------------------------------------------===//
#include "ZCPUGenCallingConv.inc"
//===----------------------------------------------------------------------===//
// Lowering Code
//===----------------------------------------------------------------------===//
const MCPhysReg *ZCPUTargetLowering::getScratchRegisters(CallingConv::ID) const {
    static const MCPhysReg ScratchRegs[] = {ZCPU::R0,
                                            ZCPU::R1, ZCPU::R2, ZCPU::R3, ZCPU::R4,
                                            ZCPU::R5, ZCPU::R6, ZCPU::R7, ZCPU::R8,
                                            ZCPU::R9, ZCPU::R10, ZCPU::R10, ZCPU::R11,
                                            ZCPU::R12, ZCPU::R13, ZCPU::R14, ZCPU::R15,
                                            ZCPU::R16, ZCPU::R17, ZCPU::R18, ZCPU::R19,
                                            ZCPU::R20, ZCPU::R21, ZCPU::R22, ZCPU::R22,
                                            ZCPU::R23, ZCPU::R24, ZCPU::R25, ZCPU::R26,
                                            ZCPU::R27, ZCPU::R28, ZCPU::R29, ZCPU::R30,
                                            ZCPU::R31, 0};
    return ScratchRegs;
}

bool ZCPUTargetLowering::IsDesirableToPromoteOp(SDValue Op, EVT &PVT) const {
    EVT VT = Op.getValueType();
    if ((VT != MVT::i64) & (VT != MVT::f64)) {
        return true;
    } else {
        return false;
    }
}

static void fail(const SDLoc &DL, SelectionDAG &DAG, const char *msg) {
    MachineFunction &MF = DAG.getMachineFunction();
    DAG.getContext()->diagnose(
            DiagnosticInfoUnsupported(*MF.getFunction(), msg, DL.getDebugLoc()));
}

// Test whether the given calling convention is supported.
static bool CallingConvSupported(CallingConv::ID CallConv) {

return true;
}

SDValue ZCPUTargetLowering::LowerCall(
        CallLoweringInfo &CLI, SmallVectorImpl<SDValue> &InVals) const {
    SelectionDAG &DAG = CLI.DAG;
    SDLoc DL = CLI.DL;
    SDValue Chain = CLI.Chain;
    SDValue Callee = CLI.Callee;
    MachineFunction &MF = DAG.getMachineFunction();
    auto Layout = MF.getDataLayout();

    CallingConv::ID CallConv = CLI.CallConv;
    if (!CallingConvSupported(CallConv))
        fail(DL, DAG,
             "ZCPU doesn't support language-specific or target-specific "
                     "calling conventions yet");
    if (CLI.IsPatchPoint)
        fail(DL, DAG, "ZCPU doesn't support patch point yet");

    // ZCPU doesn't currently support explicit tail calls. If they are
    // required, fail. Otherwise, just disable them.
    if ((CallConv == CallingConv::Fast && CLI.IsTailCall &&
         MF.getTarget().Options.GuaranteedTailCallOpt) ||
        (CLI.CS && CLI.CS->isMustTailCall()))
        fail(DL, DAG, "ZCPU doesn't support tail call yet");
    CLI.IsTailCall = false;

    SmallVectorImpl<ISD::InputArg> &Ins = CLI.Ins;
    if (Ins.size() > 1)
        fail(DL, DAG, "ZCPU doesn't support more than 1 returned value yet");

    SmallVectorImpl<ISD::OutputArg> &Outs = CLI.Outs;
    SmallVectorImpl<SDValue> &OutVals = CLI.OutVals;
    for (unsigned i = 0; i < Outs.size(); ++i) {
        const ISD::OutputArg &Out = Outs[i];
        SDValue &OutVal = OutVals[i];
        if (Out.Flags.isNest())
            fail(DL, DAG, "ZCPU hasn't implemented nest arguments");
        if (Out.Flags.isInAlloca())
            fail(DL, DAG, "ZCPU hasn't implemented inalloca arguments");
        if (Out.Flags.isInConsecutiveRegs())
            fail(DL, DAG, "ZCPU hasn't implemented cons regs arguments");
        if (Out.Flags.isInConsecutiveRegsLast())
            fail(DL, DAG, "ZCPU hasn't implemented cons regs last arguments");
        if (Out.Flags.isByVal() && Out.Flags.getByValSize() != 0) {
            auto *MFI = MF.getFrameInfo();
            int FI = MFI->CreateStackObject(Out.Flags.getByValSize(),
                                            Out.Flags.getByValAlign(),
                    /*isSS=*/false);
            SDValue SizeNode =
                    DAG.getConstant(Out.Flags.getByValSize(), DL, MVT::i32);
            SDValue FINode = DAG.getFrameIndex(FI, getPointerTy(Layout));
            Chain = DAG.getMemcpy(
                    Chain, DL, FINode, OutVal, SizeNode, Out.Flags.getByValAlign(),
                    /*isVolatile*/ false, /*AlwaysInline=*/false,
                    /*isTailCall*/ false, MachinePointerInfo(), MachinePointerInfo());
            OutVal = FINode;
        }
    }

    bool IsVarArg = CLI.IsVarArg;
    unsigned NumFixedArgs = CLI.NumFixedArgs;

    auto PtrVT = getPointerTy(Layout);

    // Analyze operands of the call, assigning locations to each operand.
    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());

    if (IsVarArg) {
        // Outgoing non-fixed arguments are placed in a buffer. First
        // compute their offsets and the total amount of buffer space needed.
        for (SDValue Arg :
                make_range(OutVals.begin() + NumFixedArgs, OutVals.end())) {
            EVT VT = Arg.getValueType();
            assert(VT != MVT::iPTR && "Legalized args should be concrete");
            Type *Ty = VT.getTypeForEVT(*DAG.getContext());
            unsigned Offset = CCInfo.AllocateStack(Layout.getTypeAllocSize(Ty),
                                                   Layout.getABITypeAlignment(Ty));
            CCInfo.addLoc(CCValAssign::getMem(ArgLocs.size(), VT.getSimpleVT(),
                                              Offset, VT.getSimpleVT(),
                                              CCValAssign::Full));
        }
    }

    unsigned NumBytes = CCInfo.getAlignedCallFrameSize();

    SDValue FINode;
    if (IsVarArg && NumBytes) {
        // For non-fixed arguments, next emit stores to store the argument values
        // to the stack buffer at the offsets computed above.
        int FI = MF.getFrameInfo()->CreateStackObject(NumBytes,
                                                      Layout.getStackAlignment(),
                /*isSS=*/false);
        unsigned ValNo = 0;
        SmallVector<SDValue, 8> Chains;
        for (SDValue Arg :
                make_range(OutVals.begin() + NumFixedArgs, OutVals.end())) {
            assert(ArgLocs[ValNo].getValNo() == ValNo &&
                   "ArgLocs should remain in order and only hold varargs args");
            unsigned Offset = ArgLocs[ValNo++].getLocMemOffset();
            FINode = DAG.getFrameIndex(FI, getPointerTy(Layout));
            SDValue Add = DAG.getNode(ISD::ADD, DL, PtrVT, FINode,
                                      DAG.getConstant(Offset, DL, PtrVT));
            Chains.push_back(DAG.getStore(
                    Chain, DL, Arg, Add,
                    MachinePointerInfo::getFixedStack(MF, FI, Offset), 0));
        }
        if (!Chains.empty())
            Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, Chains);
    } else if (IsVarArg) {
        FINode = DAG.getIntPtrConstant(0, DL);
    }

    // Compute the operands for the CALLn node.
    SmallVector<SDValue, 16> Ops;
    Ops.push_back(Chain);
    Ops.push_back(Callee);

    // Add all fixed arguments. Note that for non-varargs calls, NumFixedArgs
    // isn't reliable.
    Ops.append(OutVals.begin(),
               IsVarArg ? OutVals.begin() + NumFixedArgs : OutVals.end());
    // Add a pointer to the vararg buffer.
    if (IsVarArg) Ops.push_back(FINode);

    SmallVector<EVT, 8> InTys;
    for (const auto &In : Ins) {
        assert(!In.Flags.isByVal() && "byval is not valid for return values");
        assert(!In.Flags.isNest() && "nest is not valid for return values");
        if (In.Flags.isInAlloca())
            fail(DL, DAG, "ZCPU hasn't implemented inalloca return values");
        if (In.Flags.isInConsecutiveRegs())
            fail(DL, DAG, "ZCPU hasn't implemented cons regs return values");
        if (In.Flags.isInConsecutiveRegsLast())
            fail(DL, DAG,
                 "ZCPU hasn't implemented cons regs last return values");
        // Ignore In.getOrigAlign() because all our arguments are passed in
        // registers.
        InTys.push_back(In.VT);
    }
    InTys.push_back(MVT::Other);
    SDVTList InTyList = DAG.getVTList(InTys);
    SDValue Res =
            DAG.getNode(Ins.empty(),
                        DL, InTyList, Ops);
    if (Ins.empty()) {
        Chain = Res;
    } else {
        InVals.push_back(Res);
        Chain = Res.getValue(1);
    }

    return Chain;
}

bool ZCPUTargetLowering::CanLowerReturn(
        CallingConv::ID /*CallConv*/, MachineFunction & /*MF*/, bool /*IsVarArg*/,
        const SmallVectorImpl<ISD::OutputArg> &Outs,
        LLVMContext & /*Context*/) const {
    // ZCPU can't currently handle returning tuples.
    return Outs.size() <= 1;
}

SDValue ZCPUTargetLowering::LowerReturn(
        SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
        const SmallVectorImpl<ISD::OutputArg> &Outs,
        const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
        SelectionDAG &DAG) const {
    MachineFunction &MF = DAG.getMachineFunction();
    ZCPUFunctionInfo *FuncInfo = MF.getInfo<ZCPUFunctionInfo>();

    if (CallConv == CallingConv::X86_INTR && !Outs.empty())
        report_fatal_error("X86 interrupts may not return any value");

    SmallVector<CCValAssign, 16> RVLocs;
    CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, *DAG.getContext());
    CCInfo.AnalyzeReturn(Outs, RetCC_ZCPU);

    SDValue Flag;
    SmallVector<SDValue, 6> RetOps;
    RetOps.push_back(Chain); // Operand #0 = Chain (updated below)
    // Operand #1 = Bytes To Pop
    RetOps.push_back(DAG.getTargetConstant(48, DL,
                                           MVT::i64));

    // Copy the result values into the output registers.
    for (unsigned i = 0, e = RVLocs.size(); i != e; ++i) {
        CCValAssign &VA = RVLocs[i];
        assert(VA.isRegLoc() && "Can only return in registers!");
        SDValue ValToCopy = OutVals[i];
        EVT ValVT = ValToCopy.getValueType();

        // Promote values to the appropriate types.
        if (VA.getLocInfo() == CCValAssign::SExt)
            ValToCopy = DAG.getNode(ISD::SIGN_EXTEND, DL, VA.getLocVT(), ValToCopy);
        else if (VA.getLocInfo() == CCValAssign::ZExt)
            ValToCopy = DAG.getNode(ISD::ZERO_EXTEND, DL, VA.getLocVT(), ValToCopy);
        else if (VA.getLocInfo() == CCValAssign::AExt) {
            if (ValVT.isVector() && ValVT.getVectorElementType() == MVT::i1)
                ValToCopy = DAG.getNode(ISD::SIGN_EXTEND, DL, VA.getLocVT(), ValToCopy);
            else
                ValToCopy = DAG.getNode(ISD::ANY_EXTEND, DL, VA.getLocVT(), ValToCopy);
        }
        else if (VA.getLocInfo() == CCValAssign::BCvt)
            ValToCopy = DAG.getBitcast(VA.getLocVT(), ValToCopy);

        assert(VA.getLocInfo() != CCValAssign::FPExt &&
               "Unexpected FP-extend for return value.");

        // If this is x86-64, and we disabled SSE, we can't return FP values,
        // or SSE or MMX vectors.
        // Likewise we can't return F64 values with SSE1 only.  gcc does so, but
        // llvm-gcc has never done it right and no one has noticed, so this
        // should be OK for now.

        // Returns in ST0/ST1 are handled specially: these are pushed as operands to
        // the RET instruction and handled by the FP Stackifier.

        Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), ValToCopy, Flag);
        Flag = Chain.getValue(1);
        RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
    }

    // Swift calling convention does not require we copy the sret argument
    // into %rax/%eax for the return, and SRetReturnReg is not set for Swift.

    // All x86 ABIs require that for returning structs by value we copy
    // the sret argument into %rax/%eax (depending on ABI) for the return.
    // We saved the argument into a virtual register in the entry block,
    // so now we copy the value out and into %rax/%eax.
    //
    // Checking Function.hasStructRetAttr() here is insufficient because the IR
    // may not have an explicit sret argument. If FuncInfo.CanLowerReturn is
    // false, then an sret argument may be implicitly inserted in the SelDAG. In
    // either case FuncInfo->setSRetReturnReg() will have been called.
    if (unsigned SRetReg = ZCPU::EAX) {
        // When we have both sret and another return value, we should use the
        // original Chain stored in RetOps[0], instead of the current Chain updated
        // in the above loop. If we only have sret, RetOps[0] equals to Chain.

        // For the case of sret and another return value, we have
        //   Chain_0 at the function entry
        //   Chain_1 = getCopyToReg(Chain_0) in the above loop
        // If we use Chain_1 in getCopyFromReg, we will have
        //   Val = getCopyFromReg(Chain_1)
        //   Chain_2 = getCopyToReg(Chain_1, Val) from below

        // getCopyToReg(Chain_0) will be glued together with
        // getCopyToReg(Chain_1, Val) into Unit A, getCopyFromReg(Chain_1) will be
        // in Unit B, and we will have cyclic dependency between Unit A and Unit B:
        //   Data dependency from Unit B to Unit A due to usage of Val in
        //     getCopyToReg(Chain_1, Val)
        //   Chain dependency from Unit A to Unit B

        // So here, we use RetOps[0] (i.e Chain_0) for getCopyFromReg.
        SDValue Val = DAG.getCopyFromReg(RetOps[0], DL, SRetReg,
                                         getPointerTy(MF.getDataLayout()));

        unsigned RetValReg
                = ZCPU::EAX;
        Chain = DAG.getCopyToReg(Chain, DL, RetValReg, Val, Flag);
        Flag = Chain.getValue(1);

        // RAX/EAX now acts like a return value.
        RetOps.push_back(
                DAG.getRegister(RetValReg, getPointerTy(DAG.getDataLayout())));
    }

    const ZCPURegisterInfo *TRI = Subtarget->getRegisterInfo();
    const MCPhysReg *I =
            TRI->getCalleeSavedRegsViaCopy(&DAG.getMachineFunction());
    if (I) {
        for (; *I; ++I) {
            if (ZCPU::BothNormAndExtendedRegClass.contains(*I))
                RetOps.push_back(DAG.getRegister(*I, MVT::i64));
            else
                llvm_unreachable("Unexpected register class in CSRsViaCopy!");
        }
    }

    RetOps[0] = Chain;  // Update chain.

    // Add the flag if we have it.
    if (Flag.getNode())
        RetOps.push_back(Flag);

   auto opcode = ZCPUISD::RET_FLAG;
    if (CallConv == CallingConv::X86_INTR)
        opcode = ZCPUISD::IRET;

    return DAG.getNode(opcode, DL, MVT::Other, RetOps);
}

SDValue ZCPUTargetLowering::LowerFormalArguments(
        SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
        const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
        SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
    MachineFunction &MF = DAG.getMachineFunction();
    auto *MFI = MF.getInfo<ZCPUFunctionInfo>();

    if (!CallingConvSupported(CallConv))
        fail(DL, DAG, "WebAssembly doesn't support non-C calling conventions");

    // Set up the incoming ARGUMENTS value, which serves to represent the liveness
    // of the incoming values before they're represented by virtual registers.
    MF.getRegInfo().addLiveIn(ZCPU::EAX);

    for (const ISD::InputArg &In : Ins) {
        if (In.Flags.isInAlloca())
            fail(DL, DAG, "WebAssembly hasn't implemented inalloca arguments");
        if (In.Flags.isNest())
            fail(DL, DAG, "WebAssembly hasn't implemented nest arguments");
        if (In.Flags.isInConsecutiveRegs())
            fail(DL, DAG, "WebAssembly hasn't implemented cons regs arguments");
        if (In.Flags.isInConsecutiveRegsLast())
            fail(DL, DAG, "WebAssembly hasn't implemented cons regs last arguments");
        // Ignore In.getOrigAlign() because all our arguments are passed in
        // registers.
        InVals.push_back(
                In.Used
                ? DAG.getNode(ZCPUISD::RET_FLAG, DL, In.VT,
                              DAG.getTargetConstant(InVals.size(), DL, MVT::i32))
                : DAG.getUNDEF(In.VT));

        // Record the number and types of arguments.
        MFI->addParam(In.VT);
    }

    // Varargs are copied into a buffer allocated by the caller, and a pointer to
    // the buffer is passed as an argument.
    if (IsVarArg) {
        MVT PtrVT = getPointerTy(MF.getDataLayout());
        unsigned VarargVreg =
                MF.getRegInfo().createVirtualRegister(getRegClassFor(PtrVT));
        MFI->setVarargBufferVreg(VarargVreg);
        Chain = DAG.getCopyToReg(
                Chain, DL, VarargVreg,
                DAG.getNode(ZCPUISD::RET_FLAG, DL, PtrVT,
                            DAG.getTargetConstant(Ins.size(), DL, MVT::i64)));
        MFI->addParam(PtrVT);
    }

    return Chain;
}

//===----------------------------------------------------------------------===//
//  Custom lowering hooks.
//===----------------------------------------------------------------------===//

SDValue ZCPUTargetLowering::LowerOperation(SDValue Op,
                                           SelectionDAG &DAG) const {
    SDLoc DL(Op);
    switch (Op.getOpcode()) {
        default:
            return SDValue();
    }
}
//===----------------------------------------------------------------------===//
//                          ZCPU Optimization Hooks
//===----------------------------------------------------------------------===//
SDValue ZCPUTargetLowering::LowerFRAMEADDR(SDValue Op,
                                                  SelectionDAG &DAG) const {
    // Non-zero depths are not supported by WebAssembly currently. Use the
    // legalizer's default expansion, which is to return 0 (what this function is
    // documented to do).
    if (Op.getConstantOperandVal(0) > 0)
        return SDValue();

    DAG.getMachineFunction().getFrameInfo()->setFrameAddressIsTaken(true);
    EVT VT = Op.getValueType();
    unsigned FP =
            Subtarget->getRegisterInfo()->getFrameRegister(DAG.getMachineFunction());
    return DAG.getCopyFromReg(DAG.getEntryNode(), SDLoc(Op), FP, VT);
}
SDValue ZCPUTargetLowering::LowerFrameIndex(SDValue Op,
                                                   SelectionDAG &DAG) const {
    int FI = cast<FrameIndexSDNode>(Op)->getIndex();
    return DAG.getTargetFrameIndex(FI, Op.getValueType());
}
void ZCPUTargetLowering::LowerAsmOperandForConstraint(SDValue Op,
                                                     std::string &Constraint,
                                                     std::vector<SDValue>&Ops,
                                                     SelectionDAG &DAG) const {
    SDValue Result;

    // Only support length 1 constraints for now.
    if (Constraint.length() > 1) return;

    char ConstraintLetter = Constraint[0];
    switch (ConstraintLetter) {
        default: break;
        case 'I':
            if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
                if (C->getZExtValue() <= 31) {
                    Result = DAG.getTargetConstant(C->getZExtValue(), SDLoc(Op),
                                                   Op.getValueType());
                    break;
                }
            }
            return;
        case 'J':
            if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
                if (C->getZExtValue() <= 63) {
                    Result = DAG.getTargetConstant(C->getZExtValue(), SDLoc(Op),
                                                   Op.getValueType());
                    break;
                }
            }
            return;
        case 'K':
            if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
                if (isInt<8>(C->getSExtValue())) {
                    Result = DAG.getTargetConstant(C->getZExtValue(), SDLoc(Op),
                                                   Op.getValueType());
                    break;
                }
            }
            return;
        case 'L':
            if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
                if (C->getZExtValue() == 0xff || C->getZExtValue() == 0xffff ) {
                    Result = DAG.getTargetConstant(C->getSExtValue(), SDLoc(Op),
                                                   Op.getValueType());
                    break;
                }
            }
            return;
        case 'M':
            if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
                if (C->getZExtValue() <= 3) {
                    Result = DAG.getTargetConstant(C->getZExtValue(), SDLoc(Op),
                                                   Op.getValueType());
                    break;
                }
            }
            return;
        case 'N':
            if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
                if (C->getZExtValue() <= 255) {
                    Result = DAG.getTargetConstant(C->getZExtValue(), SDLoc(Op),
                                                   Op.getValueType());
                    break;
                }
            }
            return;
        case 'O':
            if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
                if (C->getZExtValue() <= 127) {
                    Result = DAG.getTargetConstant(C->getZExtValue(), SDLoc(Op),
                                                   Op.getValueType());
                    break;
                }
            }
            return;
        case 'e': {
            // 32-bit signed value
            if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
                if (ConstantInt::isValueValidForType(Type::getInt32Ty(*DAG.getContext()),
                                                     C->getSExtValue())) {
                    // Widen to 64 bits here to get it sign extended.
                    Result = DAG.getTargetConstant(C->getSExtValue(), SDLoc(Op), MVT::i64);
                    break;
                }
                // FIXME gcc accepts some relocatable values here too, but only in certain
                // memory models; it's complicated.
            }
            return;
        }
        case 'Z': {
            // 32-bit unsigned value
            if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op)) {
                if (ConstantInt::isValueValidForType(Type::getInt32Ty(*DAG.getContext()),
                                                     C->getZExtValue())) {
                    Result = DAG.getTargetConstant(C->getZExtValue(), SDLoc(Op),
                                                   Op.getValueType());
                    break;
                }
            }
            // FIXME gcc accepts some relocatable values here too, but only in certain
            // memory models; it's complicated.
            return;
        }
        case 'i': {
            // Literal immediates are always ok.
            if (ConstantSDNode *CST = dyn_cast<ConstantSDNode>(Op)) {
                // Widen to 64 bits here to get it sign extended.
                Result = DAG.getTargetConstant(CST->getSExtValue(), SDLoc(Op), MVT::i64);
                break;
            }

            // If we are in non-pic codegen mode, we allow the address of a global (with
            // an optional displacement) to be used with 'i'.
            GlobalAddressSDNode *GA = nullptr;
            int64_t Offset = 0;

            // Match either (GA), (GA+C), (GA+C1+C2), etc.
            while (1) {
                if ((GA = dyn_cast<GlobalAddressSDNode>(Op))) {
                    Offset += GA->getOffset();
                    break;
                } else if (Op.getOpcode() == ISD::ADD) {
                    if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op.getOperand(1))) {
                        Offset += C->getZExtValue();
                        Op = Op.getOperand(0);
                        continue;
                    }
                } else if (Op.getOpcode() == ISD::SUB) {
                    if (ConstantSDNode *C = dyn_cast<ConstantSDNode>(Op.getOperand(1))) {
                        Offset += -C->getZExtValue();
                        Op = Op.getOperand(0);
                        continue;
                    }
                }

                // Otherwise, this isn't something we can handle, reject it.
                return;
            }

            const GlobalValue *GV = GA->getGlobal();

            Result = DAG.getTargetGlobalAddress(GV, SDLoc(Op),
                                                GA->getValueType(0), Offset);
            break;
        }
    }

    if (Result.getNode()) {
        Ops.push_back(Result);
        return;
    }
    return TargetLowering::LowerAsmOperandForConstraint(Op, Constraint, Ops, DAG);
}