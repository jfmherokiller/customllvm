; ModuleID = 'testing/fib.c'
source_filename = "testing/fib.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin16.1.0"

declare void @llvm.trap() noreturn nounwind


; Function Attrs: nounwind
define i32 @main() #0 {
  %retval = alloca i32, align 4
  %n = alloca i32, align 4
  %first = alloca i32, align 4
  %second = alloca i32, align 4
  %next = alloca i32, align 4
  %c = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 100, i32* %n, align 4
  store i32 0, i32* %first, align 4
  store i32 1, i32* %second, align 4
  store i32 0, i32* %c, align 4
  br label %1

; <label>:1                                       ; preds = %14, %0
  %2 = load i32, i32* %c, align 4
  %3 = load i32, i32* %n, align 4
  %cmp = icmp slt i32 %2, %3
  br i1 %cmp, label %4, label %16

; <label>:4                                       ; preds = %1
  %5 = load i32, i32* %c, align 4
  %cmp1 = icmp sle i32 %5, 1
  br i1 %cmp1, label %6, label %8

; <label>:6                                       ; preds = %4
  %7 = load i32, i32* %c, align 4
  store i32 %7, i32* %next, align 4
  br label %13

; <label>:8                                       ; preds = %4
  %9 = load i32, i32* %first, align 4
  %10 = load i32, i32* %second, align 4
  %add = add nsw i32 %9, %10
  store i32 %add, i32* %next, align 4
  %11 = load i32, i32* %second, align 4
  store i32 %11, i32* %first, align 4
  %12 = load i32, i32* %next, align 4
  store i32 %12, i32* %second, align 4
  br label %13

; <label>:13                                      ; preds = %8, %6
  br label %14

; <label>:14                                      ; preds = %13
  %15 = load i32, i32* %c, align 4
  %inc = add nsw i32 %15, 1
  store i32 %inc, i32* %c, align 4
  br label %1

; <label>:16                                      ; preds = %1
  ret i32 0
}

attributes #0 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-features"="+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"Apple LLVM version 8.0.0 (clang-800.0.42.1)"}
