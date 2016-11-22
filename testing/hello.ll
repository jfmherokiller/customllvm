; ModuleID = 'hello.c'
source_filename = "hello.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-darwin16.1.0"

; Function Attrs: nounwind
define i32 @main() #0 {
  %retval = alloca i32, align 4
  %weed = alloca i32, align 4
  %themeltingpointOFStealBeamsViaJetfuel = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 420, i32* %weed, align 4
  store i32 99999, i32* %themeltingpointOFStealBeamsViaJetfuel, align 4
  %1 = load i32, i32* %themeltingpointOFStealBeamsViaJetfuel, align 4
  %add = add nsw i32 %1, 1
  store i32 %add, i32* %weed, align 4
  ret i32 0
}

attributes #0 = { nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-features"="+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"Apple LLVM version 8.0.0 (clang-800.0.42.1)"}
