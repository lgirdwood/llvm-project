# RUN: not llvm-mc %s -triple=xtensa -filetype=obj --no-trampolines 2>&1 | FileCheck -check-prefix=ERROR %s
# RUN: not llvm-mc %s -triple=xtensa -filetype=obj 2>&1 | FileCheck -check-prefix=ERROR-DIR %s

# ERROR: :[[#@LINE+5]]:{{[0-9]+}}: error: fixup value out of range
# ERROR-DIR: :[[#@LINE+4]]:{{[0-9]+}}: error: fixup value out of range

.text
.begin no-trampolines
j target
ret
.rept 40000
nop
.align 4
.endr
target:
.end no-trampolines
