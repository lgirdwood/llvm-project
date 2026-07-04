# RUN: llvm-mc %s -triple=xtensa -show-encoding --mattr=+windowed,+bool \
# RUN:     | FileCheck -check-prefixes=CHECK,CHECK-INST %s

# Test SALT (Set if Less Than, signed) and SALTU (unsigned) instructions
# Added in [Xtensa] TableGen: add SALT and SALTU instructions

.align 4

# Instruction format RRR
# CHECK-INST: salt a3, a4, a5
# CHECK: encoding: [0x50,0x34,0x72]
salt a3, a4, a5

# Instruction format RRR
# CHECK-INST: saltu a3, a4, a5
# CHECK: encoding: [0x50,0x34,0x62]
saltu a3, a4, a5
