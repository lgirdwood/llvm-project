# Xtensa AE FLIX bundle encoding table generator

The per-core `<core>_enc.inc` tables (consumed by XtensaMCCodeEmitter.cpp) are
auto-generated from the Tensilica libisa config for each target core, so the
integrated assembler encodes the bundle-only Audio Engine (AE/HiFi) instructions
identically to GNU `as`. The emitter picks the table by `-mcpu` at encode time.
Currently shipped: `ace15_enc.inc` (ACE15 / HiFi4, `-mcpu=intel_ace15_adsp`),
`ace30_enc.inc` (ACE30 / HiFi4, `-mcpu=intel_ace30_adsp` / `intel_ace30_ptl`;
byte-for-byte identical to ace15_enc.inc — AE bundle encodings are the same),
`ace40_enc.inc` (ACE40 / HiFi5, `-mcpu=intel_ace40_adsp` / `intel_ace40_nvl`), and
`tgl_enc.inc` (Tiger Lake / cavs2.5 / HiFi3, `-mcpu=intel_tgl_adsp`).

## Inputs (external, not in-tree)
- Generic libisa: `xtensa-isa.c`, `xtensa-isa.h`, `xtensa-isa-internal.h`
  (e.g. from QEMU `target/xtensa/`).
- Per-config data module `xtensa-modules.c` for the target core, e.g. the Zephyr
  SDK overlay `overlays/xtensa_intel_ace15_mtpm/binutils/bfd/xtensa-modules.c`.

## Steps
1. Build the enumerator (stubs: `qemu/osdep.h` -> stdlib/string/stdint/stdio;
   `ansidecl.h` -> `#define ATTRIBUTE_UNUSED __attribute__((unused))` + stdint):
     gcc -w -I. xtensa-isa.c xtensa-modules.c gen-ace-enc.c -o gen
   It drives the libisa encode API (xtensa_opcode_encode / xtensa_operand_set_field /
   xtensa_format_set_slot / xtensa_format_slot_nop_opcode / xtensa_insnbuf_to_chars)
   and prints, per AE opcode: bundle length, NOP-filled template (operands zeroed),
   and for each operand a kind flag (1=reg, 0=identity-immediate, 2=non-identity)
   plus the input-bit -> output-bit permutation. Save as xt_table.tsv.
2. `python3 emit-inc.py` joins xt_table.tsv with the LLVM .td asm strings
   (def name -> mnemonic, outs/ins operand order) to key the table by LLVM opcode
   and map each xtensa operand to its MCInst operand index (handles tied operands).
   It writes ace15_enc.inc (bundle-only ops; standalone and non-identity-immediate
   ops are excluded and keep the normal TableGen / generic path).

To retarget: swap in the core-specific xtensa-modules.c and regenerate.

## Build integration (config-driven)

The two steps above are wired into CMake (`MCTargetDesc/CMakeLists.txt`). By
default the checked-in tables are compiled in and no external sources are
needed. To regenerate a core's table from its libisa at build time, configure
LLVM with the generic half once plus the per-core data module(s):

    -DXTENSA_LIBISA_DIR=<dir with xtensa-isa.c, xtensa-isa.h, xtensa-isa-internal.h>
    -DXTENSA_MODULES_C_ACE15=<ACE15 core xtensa-modules.c>
    -DXTENSA_MODULES_C_TGL=<Tiger Lake / cavs2.5 core xtensa-modules.c>

For each `-DXTENSA_MODULES_C_<CORE>` set, CMake builds `gen-ace-enc` with the
host C compiler, runs it to produce `<core>.tsv`, runs
`emit-inc.py <td-dir> <tsv> <out.inc>` to write `<core>_enc.inc` into the build
tree, and compiles `XtensaMCCodeEmitter.cpp` with
`-DXTENSA_ENC_INC_<CORE>="<build>/<core>_enc.inc"` so the generated table is used
instead of the checked-in one. `XTENSA_LIBISA_DIR` must hold all three
generic-half files together; in a QEMU checkout `xtensa-isa.h` lives under
`include/hw/xtensa/` while the other two are in `target/xtensa/`, so gather them
into one directory first.

The stub headers `ansidecl.h` and `qemu/osdep.h` in this directory are our own
shims (not third-party) that let the Tensilica libisa sources compile as a
standalone host tool.
