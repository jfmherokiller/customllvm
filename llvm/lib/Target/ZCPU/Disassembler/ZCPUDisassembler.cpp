//===- ZCPUDisassembler.cpp - Disassembler for ZCPU -------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file is part of the ZCPU Disassembler.
//
//===----------------------------------------------------------------------===//

#include "ZCPU.h"
#if CH >= CH10_1

#include "ZCPURegisterInfo.h"
#include "ZCPUSubtarget.h"
#include "llvm/MC/MCDisassembler.h"
#include "llvm/MC/MCFixedLenDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "cpu0-disassembler"

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {

/// ZCPUDisassemblerBase - a disasembler class for ZCPU.
class ZCPUDisassemblerBase : public MCDisassembler {
public:
  /// Constructor     - Initializes the disassembler.
  ///
  ZCPUDisassemblerBase(const MCSubtargetInfo &STI, MCContext &Ctx,
                       bool bigEndian) :
    MCDisassembler(STI, Ctx),
    IsBigEndian(bigEndian) {}

  virtual ~ZCPUDisassemblerBase() {}

protected:
  bool IsBigEndian;
};

/// ZCPUDisassembler - a disasembler class for ZCPU32.
class ZCPUDisassembler : public ZCPUDisassemblerBase {
public:
  /// Constructor     - Initializes the disassembler.
  ///
  ZCPUDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx, bool bigEndian)
      : ZCPUDisassemblerBase(STI, Ctx, bigEndian) {
  }

  /// getInstruction - See MCDisassembler.
  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &VStream,
                              raw_ostream &CStream) const override;
};

} // end anonymous namespace

// Decoder tables for GPR register
static const unsigned CPURegsTable[] = {
  ZCPU::ZERO, ZCPU::AT, ZCPU::V0, ZCPU::V1,
  ZCPU::A0, ZCPU::A1, ZCPU::T9, ZCPU::T0, 
  ZCPU::T1, ZCPU::S0, ZCPU::S1, ZCPU::GP, 
  ZCPU::FP, ZCPU::SP, ZCPU::LR, ZCPU::SW
};

// Decoder tables for co-processor 0 register
static const unsigned C0RegsTable[] = {
  ZCPU::PC, ZCPU::EPC
};

static DecodeStatus DecodeCPURegsRegisterClass(MCInst &Inst,
                                               unsigned RegNo,
                                               uint64_t Address,
                                               const void *Decoder);
static DecodeStatus DecodeGPROutRegisterClass(MCInst &Inst,
                                               unsigned RegNo,
                                               uint64_t Address,
                                               const void *Decoder);
static DecodeStatus DecodeSRRegisterClass(MCInst &Inst,
                                               unsigned RegNo,
                                               uint64_t Address,
                                               const void *Decoder);
static DecodeStatus DecodeC0RegsRegisterClass(MCInst &Inst,
                                              unsigned RegNo,
                                              uint64_t Address,
                                              const void *Decoder);
static DecodeStatus DecodeBranch16Target(MCInst &Inst,
                                       unsigned Insn,
                                       uint64_t Address,
                                       const void *Decoder);
static DecodeStatus DecodeBranch24Target(MCInst &Inst,
                                       unsigned Insn,
                                       uint64_t Address,
                                       const void *Decoder);
static DecodeStatus DecodeJumpTarget(MCInst &Inst,
                                     unsigned Insn,
                                     uint64_t Address,
                                     const void *Decoder);
static DecodeStatus DecodeJumpFR(MCInst &Inst,
                                 unsigned Insn,
                                 uint64_t Address,
                                 const void *Decoder);

static DecodeStatus DecodeMem(MCInst &Inst,
                              unsigned Insn,
                              uint64_t Address,
                              const void *Decoder);
static DecodeStatus DecodeSimm16(MCInst &Inst,
                                 unsigned Insn,
                                 uint64_t Address,
                                 const void *Decoder);

namespace llvm {
extern Target TheZCPUelTarget, TheZCPUTarget, TheZCPU64Target,
              TheZCPU64elTarget;
}

static MCDisassembler *createZCPUDisassembler(
                       const Target &T,
                       const MCSubtargetInfo &STI,
                       MCContext &Ctx) {
  return new ZCPUDisassembler(STI, Ctx, true);
}

static MCDisassembler *createZCPUelDisassembler(
                       const Target &T,
                       const MCSubtargetInfo &STI,
                       MCContext &Ctx) {
  return new ZCPUDisassembler(STI, Ctx, false);
}

extern "C" void LLVMInitializeZCPUDisassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(TheZCPUTarget,
                                         createZCPUDisassembler);
  TargetRegistry::RegisterMCDisassembler(TheZCPUelTarget,
                                         createZCPUelDisassembler);
}


#include "ZCPUGenDisassemblerTables.inc"

/// Read four bytes from the ArrayRef and return 32 bit word sorted
/// according to the given endianess
static DecodeStatus readInstruction32(ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      uint64_t &Size, uint32_t &Insn,
                                      bool IsBigEndian) {
  // We want to read exactly 4 Bytes of data.
  if (Bytes.size() < 4) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  if (IsBigEndian) {
    // Encoded as a big-endian 32-bit word in the stream.
    Insn = (Bytes[3] <<  0) |
           (Bytes[2] <<  8) |
           (Bytes[1] << 16) |
           (Bytes[0] << 24);
  }
  else {
    // Encoded as a small-endian 32-bit word in the stream.
    Insn = (Bytes[0] <<  0) |
           (Bytes[1] <<  8) |
           (Bytes[2] << 16) |
           (Bytes[3] << 24);
  }

  return MCDisassembler::Success;
}

DecodeStatus
ZCPUDisassembler::getInstruction(MCInst &Instr, uint64_t &Size,
                                              ArrayRef<uint8_t> Bytes,
                                              uint64_t Address,
                                              raw_ostream &VStream,
                                              raw_ostream &CStream) const {
  uint32_t Insn;

  DecodeStatus Result;

  Result = readInstruction32(Bytes, Address, Size, Insn, IsBigEndian);

  if (Result == MCDisassembler::Fail)
    return MCDisassembler::Fail;

  // Calling the auto-generated decoder function.
  Result = decodeInstruction(DecoderTableZCPU32, Instr, Insn, Address,
                             this, STI);
  if (Result != MCDisassembler::Fail) {
    Size = 4;
    return Result;
  }

  return MCDisassembler::Fail;
}

static DecodeStatus DecodeCPURegsRegisterClass(MCInst &Inst,
                                               unsigned RegNo,
                                               uint64_t Address,
                                               const void *Decoder) {
  if (RegNo > 15)
    return MCDisassembler::Fail;

  Inst.addOperand(MCOperand::createReg(CPURegsTable[RegNo]));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeGPROutRegisterClass(MCInst &Inst,
                                               unsigned RegNo,
                                               uint64_t Address,
                                               const void *Decoder) {
  return DecodeCPURegsRegisterClass(Inst, RegNo, Address, Decoder);
}

static DecodeStatus DecodeSRRegisterClass(MCInst &Inst,
                                               unsigned RegNo,
                                               uint64_t Address,
                                               const void *Decoder) {
  return DecodeCPURegsRegisterClass(Inst, RegNo, Address, Decoder);
}

static DecodeStatus DecodeC0RegsRegisterClass(MCInst &Inst,
                                              unsigned RegNo,
                                              uint64_t Address,
                                              const void *Decoder) {
  if (RegNo > 1)
    return MCDisassembler::Fail;

  Inst.addOperand(MCOperand::createReg(C0RegsTable[RegNo]));
  return MCDisassembler::Success;
}

//@DecodeMem {
static DecodeStatus DecodeMem(MCInst &Inst,
                              unsigned Insn,
                              uint64_t Address,
                              const void *Decoder) {
//@DecodeMem body {
  int Offset = SignExtend32<16>(Insn & 0xffff);
  int Reg = (int)fieldFromInstruction(Insn, 20, 4);
  int Base = (int)fieldFromInstruction(Insn, 16, 4);

#if CH >= CH12_1 //1
  if(Inst.getOpcode() == ZCPU::SC){
    Inst.addOperand(MCOperand::createReg(Reg));
  }
#endif

  Inst.addOperand(MCOperand::createReg(CPURegsTable[Reg]));
  Inst.addOperand(MCOperand::createReg(CPURegsTable[Base]));
  Inst.addOperand(MCOperand::createImm(Offset));

  return MCDisassembler::Success;
}

static DecodeStatus DecodeBranch16Target(MCInst &Inst,
                                       unsigned Insn,
                                       uint64_t Address,
                                       const void *Decoder) {
  int BranchOffset = fieldFromInstruction(Insn, 0, 16);
  if (BranchOffset > 0x8fff)
  	BranchOffset = -1*(0x10000 - BranchOffset);
  Inst.addOperand(MCOperand::createImm(BranchOffset));
  return MCDisassembler::Success;
}

/* CBranch instruction define $ra and then imm24; The printOperand() print 
operand 1 (operand 0 is $ra and operand 1 is imm24), so we Create register 
operand first and create imm24 next, as follows,

// ZCPUInstrInfo.td
class CBranch<bits<8> op, string instr_asm, RegisterClass RC,
                   list<Register> UseRegs>:
  FJ<op, (outs), (ins RC:$ra, brtarget:$addr),
             !strconcat(instr_asm, "\t$addr"),
             [(brcond RC:$ra, bb:$addr)], IIBranch> {

// ZCPUAsmWriter.inc
void ZCPUInstPrinter::printInstruction(const MCInst *MI, raw_ostream &O) {
...
  case 3:
    // CMP, JEQ, JGE, JGT, JLE, JLT, JNE
    printOperand(MI, 1, O); 
    break;
*/
static DecodeStatus DecodeBranch24Target(MCInst &Inst,
                                       unsigned Insn,
                                       uint64_t Address,
                                       const void *Decoder) {
  int BranchOffset = fieldFromInstruction(Insn, 0, 24);
  if (BranchOffset > 0x8fffff)
  	BranchOffset = -1*(0x1000000 - BranchOffset);
  Inst.addOperand(MCOperand::createReg(ZCPU::SW));
  Inst.addOperand(MCOperand::createImm(BranchOffset));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeJumpTarget(MCInst &Inst,
                                     unsigned Insn,
                                     uint64_t Address,
                                     const void *Decoder) {

  unsigned JumpOffset = fieldFromInstruction(Insn, 0, 24);
  Inst.addOperand(MCOperand::createImm(JumpOffset));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeJumpFR(MCInst &Inst,
                                     unsigned Insn,
                                     uint64_t Address,
                                     const void *Decoder) {
  int Reg_a = (int)fieldFromInstruction(Insn, 20, 4);
  Inst.addOperand(MCOperand::createReg(CPURegsTable[Reg_a]));
// exapin in http://jonathan2251.github.io/lbd/llvmstructure.html#jr-note
  if (CPURegsTable[Reg_a] == ZCPU::LR)
    Inst.setOpcode(ZCPU::RET);
  else
    Inst.setOpcode(ZCPU::JR);
  return MCDisassembler::Success;
}

static DecodeStatus DecodeSimm16(MCInst &Inst,
                                 unsigned Insn,
                                 uint64_t Address,
                                 const void *Decoder) {
  Inst.addOperand(MCOperand::createImm(SignExtend32<16>(Insn)));
  return MCDisassembler::Success;
}

#else // #if CH >= CH11_1
extern "C" void LLVMInitializeZCPUDisassembler() {}
#endif // #if CH >= CH10_1
