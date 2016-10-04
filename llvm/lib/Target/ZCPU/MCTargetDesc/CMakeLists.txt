add_llvm_library(LLVMZCPUDesc
  ZCPUAsmBackend.cpp
  ZCPUMCAsmInfo.cpp
  ZCPUMCCodeEmitter.cpp
  ZCPUMCTargetDesc.cpp
)

add_dependencies(LLVMZCPUDesc LLVMCommonTableGen)
