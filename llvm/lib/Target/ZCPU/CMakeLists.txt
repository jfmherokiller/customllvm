set(LLVM_TARGET_DEFINITIONS ZCPU.td)

add_llvm_target(ZCPUCodeGen
        ZCPUTargetMachine.cpp
        )
