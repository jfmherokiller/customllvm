; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -instcombine -S | FileCheck %s

define <4 x i32> @psignd_3(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: @psignd_3(
; CHECK-NEXT:    [[SUB:%.*]] = sub nsw <4 x i32> zeroinitializer, %a
; CHECK-NEXT:    [[B_LOBIT:%.*]] = ashr <4 x i32> %b, <i32 31, i32 31, i32 31, i32 31>
; CHECK-NEXT:    [[T1:%.*]] = xor <4 x i32> [[B_LOBIT]], <i32 -1, i32 -1, i32 -1, i32 -1>
; CHECK-NEXT:    [[T2:%.*]] = and <4 x i32> %a, [[T1]]
; CHECK-NEXT:    [[T3:%.*]] = and <4 x i32> [[B_LOBIT]], [[SUB]]
; CHECK-NEXT:    [[COND:%.*]] = or <4 x i32> [[T2]], [[T3]]
; CHECK-NEXT:    ret <4 x i32> [[COND]]
;
  %cmp = icmp slt <4 x i32> %b, zeroinitializer
  %sext = sext <4 x i1> %cmp to <4 x i32>
  %sub = sub nsw <4 x i32> zeroinitializer, %a
  %t0 = icmp slt <4 x i32> %sext, zeroinitializer
  %sext3 = sext <4 x i1> %t0 to <4 x i32>
  %t1 = xor <4 x i32> %sext3, <i32 -1, i32 -1, i32 -1, i32 -1>
  %t2 = and <4 x i32> %a, %t1
  %t3 = and <4 x i32> %sext3, %sub
  %cond = or <4 x i32> %t2, %t3
  ret <4 x i32> %cond
}

define <4 x i32> @test1(<4 x i32> %a, <4 x i32> %b) {
; CHECK-LABEL: @test1(
; CHECK-NEXT:    [[SUB:%.*]] = sub nsw <4 x i32> zeroinitializer, %a
; CHECK-NEXT:    [[B_LOBIT:%.*]] = ashr <4 x i32> %b, <i32 31, i32 31, i32 31, i32 31>
; CHECK-NEXT:    [[B_LOBIT_NOT:%.*]] = xor <4 x i32> [[B_LOBIT]], <i32 -1, i32 -1, i32 -1, i32 -1>
; CHECK-NEXT:    [[T2:%.*]] = and <4 x i32> [[B_LOBIT]], %a
; CHECK-NEXT:    [[T3:%.*]] = and <4 x i32> [[B_LOBIT_NOT]], [[SUB]]
; CHECK-NEXT:    [[COND:%.*]] = or <4 x i32> [[T2]], [[T3]]
; CHECK-NEXT:    ret <4 x i32> [[COND]]
;
  %cmp = icmp sgt <4 x i32> %b, <i32 -1, i32 -1, i32 -1, i32 -1>
  %sext = sext <4 x i1> %cmp to <4 x i32>
  %sub = sub nsw <4 x i32> zeroinitializer, %a
  %t0 = icmp slt <4 x i32> %sext, zeroinitializer
  %sext3 = sext <4 x i1> %t0 to <4 x i32>
  %t1 = xor <4 x i32> %sext3, <i32 -1, i32 -1, i32 -1, i32 -1>
  %t2 = and <4 x i32> %a, %t1
  %t3 = and <4 x i32> %sext3, %sub
  %cond = or <4 x i32> %t2, %t3
  ret <4 x i32> %cond
}

