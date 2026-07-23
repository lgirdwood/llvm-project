; RUN: llc -mtriple=xtensa -mcpu=intel_ace40_adsp < %s | FileCheck %s
; Object emission must succeed too: these are FLIX bundle-only ops whose bytes
; come from the per-core enc table (ace40_enc.inc), not a TableGen encoding.
; Without a table entry the MC emitter aborts with "Unsupported instruction",
; so a bare -filetype=obj run is the regression guard the asm check above misses.
; Cover both the bare core name and the _adsp spelling (they must route to the
; same ACE40 bundle table).
; RUN: llc -mtriple=xtensa -mcpu=intel_ace40 -filetype=obj < %s -o /dev/null
; RUN: llc -mtriple=xtensa -mcpu=intel_ace40_adsp -filetype=obj < %s -o /dev/null
; HiFi5 VFPU 4-wide packed single-float arithmetic (dual-output register pair).
; (a,b) = op((c,d),(e,f)); values live in the 64-bit AEDR class, moved as i64.
; madd/msub read-modify-write the (a,b) accumulator pair (tied operands).
declare {<2 x float>, <2 x float>} @llvm.xtensa.add.sx2x2(<2 x float>, <2 x float>, <2 x float>, <2 x float>)
declare {<2 x float>, <2 x float>} @llvm.xtensa.sub.sx2x2(<2 x float>, <2 x float>, <2 x float>, <2 x float>)
declare {<2 x float>, <2 x float>} @llvm.xtensa.mul.sx2x2(<2 x float>, <2 x float>, <2 x float>, <2 x float>)
declare {<2 x float>, <2 x float>} @llvm.xtensa.madd.sx2x2(<2 x float>, <2 x float>, <2 x float>, <2 x float>, <2 x float>, <2 x float>)
declare {<2 x float>, <2 x float>} @llvm.xtensa.msub.sx2x2(<2 x float>, <2 x float>, <2 x float>, <2 x float>, <2 x float>, <2 x float>)

define void @add_sx2x2(i64 %c, i64 %d, i64 %e, i64 %f, ptr %pa, ptr %pb) {
; CHECK-LABEL: add_sx2x2:
; CHECK: add.sx2x2 {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}
  %vc = bitcast i64 %c to <2 x float>
  %vd = bitcast i64 %d to <2 x float>
  %ve = bitcast i64 %e to <2 x float>
  %vf = bitcast i64 %f to <2 x float>
  %r = call {<2 x float>, <2 x float>} @llvm.xtensa.add.sx2x2(<2 x float> %vc, <2 x float> %vd, <2 x float> %ve, <2 x float> %vf)
  %a = extractvalue {<2 x float>, <2 x float>} %r, 0
  %b = extractvalue {<2 x float>, <2 x float>} %r, 1
  %ia = bitcast <2 x float> %a to i64
  %ib = bitcast <2 x float> %b to i64
  store i64 %ia, ptr %pa
  store i64 %ib, ptr %pb
  ret void
}

define void @sub_sx2x2(i64 %c, i64 %d, i64 %e, i64 %f, ptr %pa, ptr %pb) {
; CHECK-LABEL: sub_sx2x2:
; CHECK: sub.sx2x2 {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}
  %vc = bitcast i64 %c to <2 x float>
  %vd = bitcast i64 %d to <2 x float>
  %ve = bitcast i64 %e to <2 x float>
  %vf = bitcast i64 %f to <2 x float>
  %r = call {<2 x float>, <2 x float>} @llvm.xtensa.sub.sx2x2(<2 x float> %vc, <2 x float> %vd, <2 x float> %ve, <2 x float> %vf)
  %a = extractvalue {<2 x float>, <2 x float>} %r, 0
  %b = extractvalue {<2 x float>, <2 x float>} %r, 1
  %ia = bitcast <2 x float> %a to i64
  %ib = bitcast <2 x float> %b to i64
  store i64 %ia, ptr %pa
  store i64 %ib, ptr %pb
  ret void
}

define void @mul_sx2x2(i64 %c, i64 %d, i64 %e, i64 %f, ptr %pa, ptr %pb) {
; CHECK-LABEL: mul_sx2x2:
; CHECK: mul.sx2x2 {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}
  %vc = bitcast i64 %c to <2 x float>
  %vd = bitcast i64 %d to <2 x float>
  %ve = bitcast i64 %e to <2 x float>
  %vf = bitcast i64 %f to <2 x float>
  %r = call {<2 x float>, <2 x float>} @llvm.xtensa.mul.sx2x2(<2 x float> %vc, <2 x float> %vd, <2 x float> %ve, <2 x float> %vf)
  %a = extractvalue {<2 x float>, <2 x float>} %r, 0
  %b = extractvalue {<2 x float>, <2 x float>} %r, 1
  %ia = bitcast <2 x float> %a to i64
  %ib = bitcast <2 x float> %b to i64
  store i64 %ia, ptr %pa
  store i64 %ib, ptr %pb
  ret void
}

define void @madd_sx2x2(i64 %a0, i64 %b0, i64 %c, i64 %d, i64 %e, i64 %f, ptr %pa, ptr %pb) {
; CHECK-LABEL: madd_sx2x2:
; CHECK: madd.sx2x2 {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}
  %va0 = bitcast i64 %a0 to <2 x float>
  %vb0 = bitcast i64 %b0 to <2 x float>
  %vc = bitcast i64 %c to <2 x float>
  %vd = bitcast i64 %d to <2 x float>
  %ve = bitcast i64 %e to <2 x float>
  %vf = bitcast i64 %f to <2 x float>
  %r = call {<2 x float>, <2 x float>} @llvm.xtensa.madd.sx2x2(<2 x float> %va0, <2 x float> %vb0, <2 x float> %vc, <2 x float> %vd, <2 x float> %ve, <2 x float> %vf)
  %a = extractvalue {<2 x float>, <2 x float>} %r, 0
  %b = extractvalue {<2 x float>, <2 x float>} %r, 1
  %ia = bitcast <2 x float> %a to i64
  %ib = bitcast <2 x float> %b to i64
  store i64 %ia, ptr %pa
  store i64 %ib, ptr %pb
  ret void
}

define void @msub_sx2x2(i64 %a0, i64 %b0, i64 %c, i64 %d, i64 %e, i64 %f, ptr %pa, ptr %pb) {
; CHECK-LABEL: msub_sx2x2:
; CHECK: msub.sx2x2 {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}, {{aed[0-9]+}}
  %va0 = bitcast i64 %a0 to <2 x float>
  %vb0 = bitcast i64 %b0 to <2 x float>
  %vc = bitcast i64 %c to <2 x float>
  %vd = bitcast i64 %d to <2 x float>
  %ve = bitcast i64 %e to <2 x float>
  %vf = bitcast i64 %f to <2 x float>
  %r = call {<2 x float>, <2 x float>} @llvm.xtensa.msub.sx2x2(<2 x float> %va0, <2 x float> %vb0, <2 x float> %vc, <2 x float> %vd, <2 x float> %ve, <2 x float> %vf)
  %a = extractvalue {<2 x float>, <2 x float>} %r, 0
  %b = extractvalue {<2 x float>, <2 x float>} %r, 1
  %ia = bitcast <2 x float> %a to i64
  %ib = bitcast <2 x float> %b to i64
  store i64 %ia, ptr %pa
  store i64 %ib, ptr %pb
  ret void
}
