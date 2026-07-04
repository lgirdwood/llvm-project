# RUN: llvm-mc %s -triple=xtensa -show-encoding --mattr=+windowed,+hifi4 \
# RUN:     | FileCheck -check-prefixes=CHECK,CHECK-INST %s

# Tests for HiFi4 AE (Audio Engine) instruction assembly encoding.
# Covers: [Xtensa] HiFi DSP: intrinsics, register classes, instruction definitions,
# and builtins. All encodings verified against llvm-mc -show-encoding output.

.align 4

# AE_MOVI aed0, 0 -- zeroise AE register via sub-self pseudo
# (maps to AE_SUB64 aed0, aed0, aed0 in the assembler)
# CHECK-INST: ae_sub64 aed0, aed0, aed0
# CHECK: encoding: [0x04,0x00,0x4d]
ae_movi aed0, 0

# AE_SUB64 -- 64-bit subtract of two AE accumulator registers (3-byte encoding)
# CHECK-INST: ae_sub64 aed0, aed1, aed2
# CHECK: encoding: [0x24,0x01,0x4d]
ae_sub64 aed0, aed1, aed2

# AE_ADD64 -- 64-bit add
# CHECK-INST: ae_add64 aed3, aed0, aed1
# CHECK: encoding: [0x14,0x30,0x31]
ae_add64 aed3, aed0, aed1

# AE_ABS64 -- 64-bit absolute value
# CHECK-INST: ae_abs64 aed5, aed4
# CHECK: encoding: [0x44,0x58,0x27]
ae_abs64 aed5, aed4

# AE_SLAI64 -- 64-bit shift-left accumulate immediate
# CHECK-INST: ae_slai64 aed0, aed1, 3
# CHECK: encoding: [0x14,0x03,0x88]
ae_slai64 aed0, aed1, 3
