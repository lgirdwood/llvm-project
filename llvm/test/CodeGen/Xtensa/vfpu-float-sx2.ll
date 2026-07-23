; RUN: llc -mtriple=xtensa -mcpu=intel_ace30_ptl < %s | FileCheck %s
; float.sx2 : packed int32x2 -> float32x2 with a 2^-imm scale (HiFi4 VFPU).
declare <2 x float> @llvm.xtensa.float.sx2(<2 x i32>, i32)

define i64 @float_sx2(i64 %x) {
; CHECK-LABEL: float_sx2:
; CHECK: float.sx2 {{aed[0-9]+}}, {{aed[0-9]+}}, 31
  %v = bitcast i64 %x to <2 x i32>
  %r = call <2 x float> @llvm.xtensa.float.sx2(<2 x i32> %v, i32 31)
  %o = bitcast <2 x float> %r to i64
  ret i64 %o
}
