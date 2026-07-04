# RUN: llvm-mc -filetype=obj -triple=xtensa \
# RUN:     --mattr=+windowed,+hifi4 %s -o - \
# RUN:     | llvm-objdump --mattr=+windowed,+hifi4 -d - \
# RUN:     | FileCheck --check-prefix=CHECK-DISASM %s
# RUN: llvm-mc -triple=xtensa --mattr=+windowed,+hifi4 -show-encoding %s \
# RUN:     | FileCheck --check-prefix=CHECK-ENC %s

# Test VLIW FLIX bundle assembly encoding.
# Covers: [Xtensa] VLIW: Packetizer, fixup kinds, MCTargetDesc, InstPrinter
# and [Xtensa] MCCodeEmitter: scalar operand encoding and 128-bit APInt support

.text
.align 4

# A NOP inside { } brackets forces a 2-slot FLIX bundle (Format 0, 48-bit).
# Slot 0: the NOP instruction, Slot 1: implicit NOP filler.
# CHECK-ENC: { nop }
# CHECK-ENC: encoding: [0x7e,0xb4,0x06,0x01,0xe6,0xfc]
{
  nop
}

# A standalone NOP outside brackets emits as density NOP.N (2-byte).
# CHECK-ENC: nop.n
# CHECK-ENC: encoding: [0x3d,0xf0]
nop
