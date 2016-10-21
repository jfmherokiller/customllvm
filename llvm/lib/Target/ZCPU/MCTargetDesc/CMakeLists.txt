# MCTargetDesc/CMakeLists.txt
add_llvm_library(LLVMZCPUDesc
#if CH >= CH3_1
  ZCPUABIInfo.cpp
#endif
#if CH >= CH3_2
  ZCPUMCAsmInfo.cpp
#endif
#if CH >= CH5_1
  ZCPUAsmBackend.cpp
  ZCPUMCCodeEmitter.cpp
  ZCPUMCExpr.cpp
  ZCPUELFObjectWriter.cpp
  ZCPUTargetStreamer.cpp
#endif
  ZCPUMCTargetDesc.cpp
  )
