# RUN: llvm-mc %s -triple=xtensa -filetype=obj -o - | llvm-objdump -d --triple=xtensa - | FileCheck %s

# CHECK:       0: {{.*}} j 0x1d4c7
# CHECK-NEXT:  3: {{.*}} ret
# CHECK-NEXT:  6: {{.*}} nop
# ...
# CHECK:   1d4c4: {{.*}} j 0x1d4ca
# CHECK-NEXT: 1d4c7: {{.*}} j 0x27110
# CHECK-NEXT: 1d4ca: {{.*}} nop

.text
.begin trampolines
j target
ret
.rept 40000
nop
.align 4
.endr
target:
.end trampolines
