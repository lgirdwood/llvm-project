# RUN: llvm-mc %s -triple=xtensa -filetype=obj -o - | llvm-objdump -d --triple=xtensa - | FileCheck %s

# Test: conditional branches (fixup_xtensa_branch_8) with targets beyond the
# 8-bit signed PC-relative range (±127 bytes) are automatically relaxed by the
# assembler into an inverted-condition branch over an unconditional J trampoline.
# This matches the behaviour of GAS, which relaxes such branches silently.
#
# The pattern for a forward out-of-range "bgeu a2, a0, far":
#   bltu  a2, a0, <resume>   ; inverted condition — skip the J if a2 < a0
#   j     far                ; unconditional jump to original far target
#   <resume>:
#   ...
#
# CHECK-LABEL: <test_bgeu_forward>:
# CHECK:       {{.*}} bltu  {{.*}}
# CHECK-NEXT:  {{.*}} j     {{.*}} <far_label>
# CHECK-NOT:   {{.*}} bgeu

.text
.global test_bgeu_forward
test_bgeu_forward:
    movi    a0, 42
    bgeu    a2, a0, far_label    # forward branch >127 bytes — must be trampolined
    .rept 70
    nop                          # 70 x 3-byte nop = 210 bytes padding
    .endr
far_label:
    movi    a2, 0
    ret.n
