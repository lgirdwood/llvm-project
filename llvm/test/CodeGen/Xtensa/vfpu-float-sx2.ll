; RUN: llc -mtriple=xtensa -mcpu=intel_ace30_ptl < %s | FileCheck %s
; HiFi4 VFPU packed int32x2 <-> float32x2 conversions with a 2^imm scale.
declare <2 x float> @llvm.xtensa.float.sx2(<2 x i32>, i32)
declare <2 x float> @llvm.xtensa.ufloat.sx2(<2 x i32>, i32)
declare <2 x i32> @llvm.xtensa.trunc.sx2(<2 x float>, i32)
declare <2 x i32> @llvm.xtensa.utrunc.sx2(<2 x float>, i32)

; vt[i] = (float)(i32)vr[i] * 2^-imm
define i64 @float_sx2(i64 %x) {
; CHECK-LABEL: float_sx2:
; CHECK: float.sx2 {{aed[0-9]+}}, {{aed[0-9]+}}, 31
  %v = bitcast i64 %x to <2 x i32>
  %r = call <2 x float> @llvm.xtensa.float.sx2(<2 x i32> %v, i32 31)
  %o = bitcast <2 x float> %r to i64
  ret i64 %o
}

; vt[i] = (float)(u32)vr[i] * 2^-imm
define i64 @ufloat_sx2(i64 %x) {
; CHECK-LABEL: ufloat_sx2:
; CHECK: ufloat.sx2 {{aed[0-9]+}}, {{aed[0-9]+}}, 31
  %v = bitcast i64 %x to <2 x i32>
  %r = call <2 x float> @llvm.xtensa.ufloat.sx2(<2 x i32> %v, i32 31)
  %o = bitcast <2 x float> %r to i64
  ret i64 %o
}

; vt[i] = (i32)(vr[i] * 2^imm)
define i64 @trunc_sx2(i64 %x) {
; CHECK-LABEL: trunc_sx2:
; CHECK: trunc.sx2 {{aed[0-9]+}}, {{aed[0-9]+}}, 31
  %v = bitcast i64 %x to <2 x float>
  %r = call <2 x i32> @llvm.xtensa.trunc.sx2(<2 x float> %v, i32 31)
  %o = bitcast <2 x i32> %r to i64
  ret i64 %o
}

; vt[i] = (u32)(vr[i] * 2^imm)
define i64 @utrunc_sx2(i64 %x) {
; CHECK-LABEL: utrunc_sx2:
; CHECK: utrunc.sx2 {{aed[0-9]+}}, {{aed[0-9]+}}, 31
  %v = bitcast i64 %x to <2 x float>
  %r = call <2 x i32> @llvm.xtensa.utrunc.sx2(<2 x float> %v, i32 31)
  %o = bitcast <2 x i32> %r to i64
  ret i64 %o
}
