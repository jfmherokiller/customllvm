#if CH >= CH11_1 1
set(LLVM_TARGET_DEFINITIONS ZCPUAsm.td)
tablegen(LLVM ZCPUGenAsmMatcher.inc -gen-asm-matcher)
#endif

set(LLVM_TARGET_DEFINITIONS ZCPUOther.td)

# Generate ZCPUGenRegisterInfo.inc and ZCPUGenInstrInfo.inc which included by 
#  your hand code C++ files. 
# ZCPUGenRegisterInfo.inc came from ZCPURegisterInfo.td, ZCPUGenInstrInfo.inc 
#  came from ZCPUInstrInfo.td.
tablegen(LLVM ZCPUGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM ZCPUGenInstrInfo.inc -gen-instr-info)
#if CH >= CH10_1 1
tablegen(LLVM ZCPUGenDisassemblerTables.inc -gen-disassembler)
#endif
#if CH >= CH3_2 1
tablegen(LLVM ZCPUGenCodeEmitter.inc -gen-emitter)
tablegen(LLVM ZCPUGenMCCodeEmitter.inc -gen-emitter)

tablegen(LLVM ZCPUGenAsmWriter.inc -gen-asm-writer)
#endif
#if CH >= CH3_1 1
tablegen(LLVM ZCPUGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM ZCPUGenCallingConv.inc -gen-callingconv)
#endif
tablegen(LLVM ZCPUGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM ZCPUGenMCPseudoLowering.inc -gen-pseudo-lowering)

# ZCPUCommonTableGen must be defined
add_public_tablegen_target(ZCPUCommonTableGen)

# ZCPUCodeGen should match with LLVMBuild.txt ZCPUCodeGen
add_llvm_target(ZCPUCodeGen
#if CH >= CH3_5
  ZCPUAnalyzeImmediate.cpp
#endif
#if CH >= CH3_2 2
  ZCPUAsmPrinter.cpp
  ZCPUMCInstLower.cpp
#endif
#if CH >= CH8_2 //1
  ZCPUDelaySlotFiller.cpp
#endif
#if CH >= CH8_2 //2
  ZCPUDelUselessJMP.cpp
#endif
#if CH >= CH9_3
  ZCPUEmitGPRestore.cpp
#endif
#if CH >= CH3_1 2
  ZCPUFrameLowering.cpp
#endif
#if CH >= CH8_2 //3
  ZCPULongBranch.cpp
#endif
#if CH >= CH3_1 2.5
  ZCPUInstrInfo.cpp
  ZCPUISelLowering.cpp
  ZCPUMachineFunction.cpp
  ZCPURegisterInfo.cpp
  ZCPUSEFrameLowering.cpp
  ZCPUSEInstrInfo.cpp
  ZCPUSEISelLowering.cpp
  ZCPUSERegisterInfo.cpp
  ZCPUSubtarget.cpp
  ZCPUTargetObjectFile.cpp
#endif
#if CH >= CH3_3
  ZCPUISelDAGToDAG.cpp
  ZCPUSEISelDAGToDAG.cpp
#endif
  ZCPUTargetMachine.cpp
  )

# Should match with "subdirectories =  MCTargetDesc TargetInfo" in LLVMBuild.txt
#if CH >= CH3_2 3
add_subdirectory(InstPrinter)
#endif
#if CH >= CH10_1 2
add_subdirectory(Disassembler)
#endif
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
#if CH >= CH11_1 2
add_subdirectory(AsmParser)
#endif
