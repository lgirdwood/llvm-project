# RUN: llvm-mc %s -triple=xtensa -filetype=obj -auto-litpools -auto-litpool-limit=1000 -o - | llvm-objdump -d --triple=xtensa - | FileCheck %s
# RUN: llvm-mc %s -triple=xtensa -filetype=obj -o - | llvm-objdump -d --triple=xtensa - | FileCheck -check-prefix=DIR-CHECK %s

# CHECK:       0: {{.*}} j       0x8
# CHECK-NEXT:  3: {{.*}} slli    a1, a1, 16
# CHECK-NEXT:  6: {{.*}} l32r    a1, 0x844c
# CHECK-NEXT:  9: {{.*}} <unknown>
# CHECK:     4bc: {{.*}} j       0x4c4
# CHECK-NEXT: 4bf: {{.*}} slli    a1, a1, 16
# CHECK-NEXT: 4c2: {{.*}} l32r    a1, 0x8908
# CHECK-NEXT: 4c5: {{.*}} <unknown>
# CHECK-NEXT: 4c8: {{.*}} l32r    a1, 0x490c
# CHECK-NEXT: 4cb: {{.*}} <unknown>

# DIR-CHECK:       0: {{.*}} j       0xc
# DIR-CHECK:       c: {{.*}} l32r    a2, 0x4

.text
.align 4

# Test directive scope
.begin auto-litpools
.literal .L1, 0x11111111
l32r a2, .L1
.fill 1200, 1, 0
.align 4
l32r a2, .L1
.end auto-litpools
