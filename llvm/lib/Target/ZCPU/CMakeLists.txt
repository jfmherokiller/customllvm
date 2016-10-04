set(LLVM_TARGET_DEFINITIONS ZCPU.td)

tablegen(LLVM ZCPUGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM ZCPUGenCallingConv.inc -gen-callingconv)
tablegen(LLVM ZCPUGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM ZCPUGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM ZCPUGenMCCodeEmitter.inc -gen-emitter -mc-emitter)
tablegen(LLVM ZCPUGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM ZCPUGenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(ZCPUCommonTableGen)

add_llvm_target(ZCPUCodeGen
  ZCPUAsmPrinter.cpp
  ZCPUFrameLowering.cpp
  ZCPUISelDAGToDAG.cpp
  ZCPUISelLowering.cpp
  ZCPUInstrInfo.cpp
  ZCPUMachineFunctionInfo.cpp
  ZCPUMCInstLower.cpp
  ZCPURegisterInfo.cpp
  ZCPUSelectionDAGInfo.cpp
  ZCPUSubtarget.cpp
  ZCPUTargetMachine.cpp
)

add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
