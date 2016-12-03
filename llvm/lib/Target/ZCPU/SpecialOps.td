def SDT_ZCPUAdjDynAlloc  : SDTypeProfile<1,  1, [SDTCisVT<0, i64>,SDTCisVT<1, i64>]>;
def ZCPUAdjDynAlloc : SDNode<"ZCPUISD::ADJDYNALLOC", SDT_ZCPUAdjDynAlloc>;

let Defs = [ESP], Uses = [ESP] in {
  def ADJDYNALLOC : IPsudo<(outs BothNormAndExtendedInt:$dst), (ins BothNormAndExtendedInt:$src),[(set BothNormAndExtendedInt:$dst, (ZCPUAdjDynAlloc BothNormAndExtendedInt:$src))],"#ADJDYNALLOC $dst $src">;
}