//===-- ZCPUTargetTransformInfo.cpp - ZCPU-specific TTI -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file defines the ZCPU-specific TargetTransformInfo
/// implementation.
///
//===----------------------------------------------------------------------===//

#include "ZCPUTargetTransformInfo.h"
#include "llvm/Support/Debug.h"
#include "llvm/Target/CostTable.h"
using namespace llvm;

#define DEBUG_TYPE "zcputti"

TargetTransformInfo::PopcntSupportKind
ZCPUTTIImpl::getPopcntSupport(unsigned TyWidth) const {
  assert(isPowerOf2_32(TyWidth) && "Ty width must be power of 2");
  return TargetTransformInfo::PSK_FastHardware;
}

unsigned ZCPUTTIImpl::getNumberOfRegisters(bool Vector) {
  unsigned Result = BaseT::getNumberOfRegisters(Vector);

  // For SIMD, use at least 16 registers, as a rough guess.
  if (Vector)
    Result = std::max(Result, 16u);

  return Result;
}

unsigned ZCPUTTIImpl::getRegisterBitWidth(bool Vector) {
  if (Vector && getST()->hasSIMD128())
    return 128;

  return 64;
}

unsigned ZCPUTTIImpl::getArithmeticInstrCost(
    unsigned Opcode, Type *Ty, TTI::OperandValueKind Opd1Info,
    TTI::OperandValueKind Opd2Info, TTI::OperandValueProperties Opd1PropInfo,
    TTI::OperandValueProperties Opd2PropInfo) {

  unsigned Cost = BasicTTIImplBase<ZCPUTTIImpl>::getArithmeticInstrCost(
      Opcode, Ty, Opd1Info, Opd2Info, Opd1PropInfo, Opd2PropInfo);

  if (VectorType *VTy = dyn_cast<VectorType>(Ty)) {
    switch (Opcode) {
    case Instruction::LShr:
    case Instruction::AShr:
    case Instruction::Shl:
      // SIMD128's shifts currently only accept a scalar shift count. For each
      // element, we'll need to extract, op, insert. The following is a rough
      // approxmation.
      if (Opd2Info != TTI::OK_UniformValue &&
          Opd2Info != TTI::OK_UniformConstantValue)
        Cost = VTy->getNumElements() *
               (TargetTransformInfo::TCC_Basic +
                getArithmeticInstrCost(Opcode, VTy->getElementType()) +
                TargetTransformInfo::TCC_Basic);
      break;
    }
  }
  return Cost;
}

unsigned ZCPUTTIImpl::getVectorInstrCost(unsigned Opcode, Type *Val,
                                                unsigned Index) {
  unsigned Cost = BasicTTIImplBase::getVectorInstrCost(Opcode, Val, Index);

  // SIMD128's insert/extract currently only take constant indices.
  if (Index == -1u)
    return Cost + 25 * TargetTransformInfo::TCC_Expensive;

  return Cost;
}
