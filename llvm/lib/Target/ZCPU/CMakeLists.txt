set(LLVM_TARGET_DEFINITIONS ZCPUOther.td)

tablegen(LLVM ZCPUGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM ZCPUGenInstrInfo.inc -gen-instr-info)

tablegen(LLVM ZCPUGenSubtargetInfo.inc -gen-subtarget)


# ZCPUCommonTableGen must be defined
add_public_tablegen_target(ZCPUCommonTableGen)

# ZCPUCodeGen should match with LLVMBuild.txt ZCPUCodeGen
add_llvm_target(ZCPUCodeGen
  ZCPUTargetMachine.cpp
  )

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(InstPrinter)