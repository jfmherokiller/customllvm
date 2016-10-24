set(LLVM_TARGET_DEFINITIONS ZCPUOther.td)

# Generate ZCPUGenRegisterInfo.inc and ZCPUGenInstrInfo.inc which included by 
#  your hand code C++ files. 
# ZCPUGenRegisterInfo.inc came from ZCPURegisterInfo.td, ZCPUGenInstrInfo.inc 
#  came from ZCPUInstrInfo.td.
tablegen(LLVM ZCPUGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM ZCPUGenInstrInfo.inc -gen-instr-info)

tablegen(LLVM ZCPUGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM ZCPUGenMCPseudoLowering.inc -gen-pseudo-lowering)

# ZCPUCommonTableGen must be defined
add_public_tablegen_target(ZCPUCommonTableGen)

# ZCPUCodeGen should match with LLVMBuild.txt ZCPUCodeGen
add_llvm_target(ZCPUCodeGen
  ZCPUTargetMachine.cpp
  )
