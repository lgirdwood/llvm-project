# RUN: not llvm-mc %s -triple=xtensa -filetype=obj --no-auto-litpools 2>&1 | FileCheck -check-prefix=ERROR %s
# RUN: not llvm-mc %s -triple=xtensa -filetype=obj 2>&1 | FileCheck -check-prefix=ERROR-DIR %s

# ERROR: :[[#@LINE+8]]:{{[0-9]+}}: error: fixup value out of range
# ERROR-DIR: :[[#@LINE+7]]:{{[0-9]+}}: error: fixup value out of range

.text
.align 4
.literal .L1, 0x11111111

.begin no-auto-litpools
l32r a2, .L1
.fill 300000, 1, 0
.align 4
l32r a2, .L1
.end no-auto-litpools
