; RUN: llc -mtriple=xtensa -mattr=+windowed -verify-machineinstrs < %s \
; RUN:   | FileCheck -check-prefix=XTENSA-WINDOWED %s

define void @test_alloca(i32 %n) {
; XTENSA-WINDOWED-LABEL: test_alloca:
; XTENSA-WINDOWED:       # %bb.0:
; XTENSA-WINDOWED-NEXT:    entry a1, 48
; XTENSA-WINDOWED-NEXT:    or a7, a1, a1
; XTENSA-WINDOWED-NEXT:    addi a8, a2, 15
; XTENSA-WINDOWED-NEXT:    movi a9, -16
; XTENSA-WINDOWED-NEXT:    and a8, a8, a9
; XTENSA-WINDOWED-NEXT:    sub a8, a1, a8
; XTENSA-WINDOWED-NEXT:    movsp a1, a8
; XTENSA-WINDOWED-NEXT:    or a10, a1, a1
; XTENSA-WINDOWED-NEXT:    l32r a8, .LCPI
; XTENSA-WINDOWED-NEXT:    callx8 a8
; XTENSA-WINDOWED-NEXT:    retw

entry:
  %p = alloca i8, i32 %n
  call void @consume(ptr %p)
  ret void
}

declare void @consume(ptr)
