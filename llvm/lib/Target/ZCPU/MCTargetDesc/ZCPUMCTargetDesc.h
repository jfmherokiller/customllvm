//==- ZCPUMCTargetDesc.h - ZCPU Target Descriptions -*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file provides ZCPU-specific target descriptions.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUMCTARGETDESC_H
#define LLVM_LIB_TARGET_ZCPU_MCTARGETDESC_ZCPUMCTARGETDESC_H

#include "llvm/MC/MCInstrDesc.h"
#include "llvm/Support/DataTypes.h"

namespace llvm {

    class MCAsmBackend;

    class MCCodeEmitter;

    class MCContext;

    class MCInstrInfo;

    class MCObjectWriter;

    class MCSubtargetInfo;

    class Target;

    class Triple;

    class raw_pwrite_stream;

    extern Target TheZCPUTarget32;

    MCCodeEmitter *createZCPUMCCodeEmitter(const MCInstrInfo &MCII);

    MCAsmBackend *createZCPUAsmBackend(const Triple &TT);

    MCObjectWriter *createZCPUELFObjectWriter(raw_pwrite_stream &OS,
                                              bool Is64Bit, uint8_t OSABI);

    namespace ZCPU {
        enum OperandType {
            /// Basic block label in a branch construct.
                    OPERAND_BASIC_BLOCK = MCOI::OPERAND_FIRST_TARGET,
            /// 64-bit floating-point immediates.
                    OPERAND_FP64IMM,
            /// p2align immediate for load and store address alignment.
                    OPERAND_P2ALIGN
        };

/// ZCPU-specific directive identifiers.
        enum Directive {
            // FIXME: This is not the real binary encoding.
                    DotParam = UINT64_MAX - 0,   ///< .param
            DotResult = UINT64_MAX - 1,  ///< .result
            DotLocal = UINT64_MAX - 2,   ///< .local
            DotEndFunc = UINT64_MAX - 3, ///< .endfunc
        };

    } // end namespace ZCPU

    namespace ZCPUII {
        enum {
            // For variadic instructions, this flag indicates whether an operand
            // in the variable_ops range is an immediate value.
                    VariableOpIsImmediate = (1 << 0),
            // For immediate values in the variable_ops range, this flag indicates
            // whether the value represents a control-flow label.
                    VariableOpImmediateIsLabel = (1 << 1),
        };
    } // end namespace ZCPUII

} // end namespace llvm

// Defines symbolic names for ZCPU registers. This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM

#include "ZCPUGenRegisterInfo.inc"

// Defines symbolic names for the ZCPU instructions.
//
#define GET_INSTRINFO_ENUM

#include "ZCPUGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM

#include "ZCPUGenSubtargetInfo.inc"

namespace llvm {
    namespace ZCPU {


/// The operand number of the load or store address in load/store instructions.
        static const unsigned MemOpAddressOperandNo = 2;
/// The operand number of the stored value in a store instruction.
        static const unsigned StoreValueOperandNo = 4;

    } // end namespace ZCPU
} // end namespace llvm

#endif
