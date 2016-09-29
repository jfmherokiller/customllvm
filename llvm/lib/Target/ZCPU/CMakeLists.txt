set(LLVM_TARGET_DEFINITIONS ZCPU.td)

tablegen(LLVM X86GenRegisterInfo.inc -gen-register-info)
#tablegen(LLVM X86GenDisassemblerTables.inc -gen-disassembler)
#tablegen(LLVM X86GenInstrInfo.inc -gen-instr-info)
#tablegen(LLVM X86GenAsmWriter.inc -gen-asm-writer)
#tablegen(LLVM X86GenAsmWriter1.inc -gen-asm-writer -asmwriternum=1)
#tablegen(LLVM X86GenAsmMatcher.inc -gen-asm-matcher)
#tablegen(LLVM X86GenDAGISel.inc -gen-dag-isel)
#tablegen(LLVM X86GenFastISel.inc -gen-fast-isel)
tablegen(LLVM X86GenCallingConv.inc -gen-callingconv)
#tablegen(LLVM X86GenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(ZCPUCommonTableGen)

set(sources
ZCPUTargetMachine.cpp
)
add_llvm_target(ZCPUCodeGen ${sources})
