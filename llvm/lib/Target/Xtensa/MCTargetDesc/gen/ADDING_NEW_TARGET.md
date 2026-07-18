# Adding a New Xtensa Core with HiFi Bundle Encoding

This document is a step-by-step recipe for adding LLVM support for a new
Xtensa core that has a HiFi Audio Engine coprocessor.  It covers everything
from obtaining the core's libisa data through verifying the result and wiring
it into the build.

The two existing cores — `intel_ace15_adsp` (ACE 1.5 / HiFi4) and
`intel_tgl_adsp` (Tiger Lake / cavs2.5 / HiFi3) — were added following
exactly these steps and serve as reference examples throughout.

---

## Background: why per-core tables?

HiFi AE instructions that can only appear inside a FLIX bundle have no
standalone encoding.  LLVM emits them as lone instructions (no `BUNDLE`
wrapper), so `XtensaMCCodeEmitter` must wrap them in a NOP-padded FLIX bundle
before writing bytes.  The bundle format, template bytes, and operand
bit-positions differ between HiFi generations (HiFi3 / HiFi4 / HiFi5) **and
between cores within the same generation** — the only authoritative source is
the Tensilica libisa data module (`xtensa-modules.c`) shipped with each core's
toolchain.  The generator in this directory extracts the exact byte layout from
that data module and emits a C include table that the emitter uses directly.

---

## Step 1 — decide whether a new table is actually needed

A new core may be able to **reuse an existing table** if its AE instruction set
is identical in encoding to an already-supported core.  Two cores share the same
table if and only if:

- they use the same HiFi generation (HiFi3, HiFi4, or HiFi5), **and**
- both cores use the same Tensilica IP release (same `xtensa-modules.c` content
  for all AE opcodes of interest).

In practice, cores from the same Intel ACE generation have used the same table
(`ace15_enc.inc` covers both `intel_ace15_adsp` and `intel_ace15_mtpm`).  If
this applies, skip Steps 2–5 and go directly to Step 6, pointing `selectBundleTable`
at the existing table for the new CPU string.

If unsure, generate the table anyway and `diff` it against the candidate
existing table.  An empty diff means you can share.

---

## Step 2 — obtain the required source files

You need two parts of the Tensilica libisa: a generic algorithm half (three
files, same for every core) and a per-core data module (one file).

### Generic half (three files, core-agnostic)

| File | What it contains |
|------|-----------------|
| `xtensa-isa.c` | libisa algorithm implementation |
| `xtensa-isa.h` | public libisa API header |
| `xtensa-isa-internal.h` | internal structs used by `xtensa-isa.c` |

These three files are **identical across all Xtensa cores**.  Obtain them
from any of the following sources (pick whichever you already have):

**Option 1 — SOF QEMU fork (recommended for SOF/Zephyr work)**

```
git clone https://github.com/thesofproject/qemu.git
```

| File | Path inside the repo |
|------|---------------------|
| `xtensa-isa.c` | `target/xtensa/xtensa-isa.c` |
| `xtensa-isa-internal.h` | `target/xtensa/xtensa-isa-internal.h` |
| `xtensa-isa.h` | `include/hw/xtensa/xtensa-isa.h` |

Note that `xtensa-isa.h` is in a **different subtree** from the other two.
The generator needs all three in the same directory; copy or symlink them
together before pointing `XTENSA_LIBISA_DIR` at the result.

**Option 2 — Zephyr SDK `sdk-ng` sources**

```
git clone https://github.com/zephyrproject-rtos/sdk-ng.git
```

All three files appear under any core's binutils overlay (they are shared
source; the copy for any core will do):

```
sdk-ng/overlays/xtensa_intel_ace15_mtpm/binutils/bfd/xtensa-isa.c
sdk-ng/overlays/xtensa_intel_ace15_mtpm/binutils/bfd/xtensa-isa.h
sdk-ng/overlays/xtensa_intel_ace15_mtpm/binutils/bfd/xtensa-isa-internal.h
```

**Option 3 — Tensilica / Cadence SDK**

Found under `XtensaTools/xtensa-elf/src/binutils/bfd/` in a full Tensilica
installation.

### Per-core data module (one file, core-specific)

`xtensa-modules.c` defines the `xtensa_modules` global that describes the
instruction set, formats, and operand encodings for one specific core
configuration.  It is the **data** half of libisa; the generic half above is
the algorithm.  They must match: both must come from tools built for the same
core.

**From the Zephyr SDK `sdk-ng` sources** (most accessible for Zephyr targets):

```
sdk-ng/overlays/xtensa_<overlay-name>/binutils/bfd/xtensa-modules.c
```

The overlay name follows the pattern `xtensa_<vendor>_<core>` (optionally with
a `_zephyr` suffix).  Known Intel SOF targets:

| Target CPU string | `sdk-ng` overlay directory |
|-------------------|---------------------------|
| `intel_tgl_adsp` (Tiger Lake / cavs2.5 / HiFi3) | `xtensa_intel_tgl_adsp` |
| `intel_ace15_adsp` / `intel_ace15_mtpm` (ACE 1.5 / HiFi4) | `xtensa_intel_ace15_mtpm` |
| `intel_ace30_adsp` / `intel_ace30_ptl` (ACE 3.0 / HiFi4) | `xtensa_intel_ace30_ptl` |
| `intel_ace40_adsp` / `intel_ace40_nvl` (ACE 4.0 / HiFi5) | `xtensa_intel_ace40` |

To discover the overlay name for any other core, list the overlays directory:

```sh
ls sdk-ng/overlays/ | grep <keyword>
```

**From a Tensilica / Cadence SDK:**

```
XtensaTools/xtensa-elf/src/binutils/bfd/xtensa-modules.c
```

Note: this path contains the modules for whichever core the SDK was configured
for.  A multi-core Tensilica installation will have separate SDK trees per core.

**Quick sanity check** — confirm the data module matches the GNU assembler you
will use as the oracle in Step 4.  The assembler binary and `xtensa-modules.c`
must come from the same toolchain release; mismatched versions will produce
encoding differences that are impossible to resolve by inspection alone.

The oracle assembler binary for Intel SOF targets ships with the prebuilt Zephyr
SDK (even though the SDK does not include `xtensa-modules.c` source):

```
<sdk-root>/gnu/xtensa-intel_ace15_mtpm_zephyr-elf/bin/xtensa-intel_ace15_mtpm_zephyr-elf-as
<sdk-root>/gnu/xtensa-intel_tgl_adsp_zephyr-elf/bin/xtensa-intel_tgl_adsp_zephyr-elf-as
```

The `xtensa-modules.c` in `sdk-ng` was built from the same upstream data that
produced those assembler binaries, so the two are always in sync.

---

## Step 3 — generate the encoding table

With both source parts in hand, use the CMake build-time path (recommended)
or the manual path.

### Option A — CMake (recommended)

Configure LLVM with the extra variables and rebuild `LLVMXtensaDesc`:

```sh
cmake -DXTENSA_LIBISA_DIR=/path/to/libisa-generic \
      -DXTENSA_MODULES_C_<NEWCORE>=/path/to/xtensa-modules.c \
      ...existing LLVM cmake flags... \
      ../llvm

cmake --build . --target LLVMXtensaDesc
```

`<NEWCORE>` is the macro suffix you will use throughout (e.g. `TGL`, `ACE15`,
`ACE30`).  CMake builds the enumerator tool, runs it to produce a `.tsv`, then
runs `emit-inc.py` to produce `<newcore>_enc.inc` in the build tree.

### Option B — manual

```sh
# 1. Compile the enumerator (uses only stdlib; -w silences vendor warnings)
gcc -w -I gen/ -I /path/to/libisa-generic \
    /path/to/libisa-generic/xtensa-isa.c \
    /path/to/xtensa-modules.c \
    gen/gen-ace-enc.c \
    -o gen-<newcore>

# 2. Enumerate encodings
./gen-<newcore> > <newcore>.tsv

# 3. Join with LLVM .td operand info
python3 gen/emit-inc.py \
    llvm/lib/Target/Xtensa \
    <newcore>.tsv \
    <newcore>_enc.inc
```

The generator only records bundle-only AE opcodes (those with no valid 8-byte
or 4-byte standalone form).  Instructions that have a valid standalone encoding
are excluded and follow the normal TableGen path.

---

## Step 4 — verify byte-exact against GNU as

Run a quick sanity check before committing anything.  The generated table must
produce bytes that match the target core's GNU assembler for every opcode in the
table.

A convenient test: compile a small snippet that exercises a variety of AE
instructions using both clang (with the new table) and the SDK GNU as, then
compare the object bytes:

```sh
# With the new table compiled in (add -DXTENSA_ENC_INC_<NEWCORE>="<newcore>_enc.inc"
# to XtensaMCCodeEmitter.cpp manually if testing outside the CMake build):
clang --target=xtensa-<newcore>_zephyr-elf -mcpu=<new-cpu-name> \
      -S -emit-llvm test.c -o test.ll
llc -mtriple=xtensa-<newcore>_zephyr-elf -mcpu=<new-cpu-name> test.ll -o test.s
clang --target=xtensa-<newcore>_zephyr-elf -mcpu=<new-cpu-name> test.s -c -o clang.o

# With the SDK GNU assembler:
<sdk>/xtensa-<core>-elf-as test.s -o gas.o

# Compare text sections byte-for-byte:
<sdk>/xtensa-<core>-elf-objdump -d clang.o > clang.txt
<sdk>/xtensa-<core>-elf-objdump -d gas.o   > gas.txt
diff clang.txt gas.txt
```

Acceptable divergence: the operand registers in the dump may differ from the
source if `llc` chose different registers than the assembler example; what must
match is the **bundle template bytes** (the NOP-filled slots and format bytes).
Run with `--regalloc=fast` and forced register assignments (inline asm) to get
a true byte-exact test.

For the `tgl` and `ace15` tables every opcode in the table was verified this
way before being committed.

---

## Step 5 — check in the generated .inc file

Copy the generated `<newcore>_enc.inc` into this directory:

```
llvm/lib/Target/Xtensa/MCTargetDesc/gen/<newcore>_enc.inc
```

The checked-in file is the authoritative source used by default builds (no
`XTENSA_LIBISA_DIR` needed).  The CMake regeneration path is for maintainers
who have the Tensilica sources and want to verify or update the table.

---

## Step 6 — wire up the C++ emitter

Three edits to `XtensaMCCodeEmitter.cpp` (all grouped near line 720):

### 6a. Add the fallback include guard

```cpp
#ifndef XTENSA_ENC_INC_<NEWCORE>
#define XTENSA_ENC_INC_<NEWCORE> "<newcore>_enc.inc"
#endif
```

This lets the build-time CMake path override the checked-in table by defining
the macro with a build-tree path.

### 6b. Declare the table

```cpp
static const XtBundleEnc XtBundleTable<NewCore>[] = {
#define XTENSA_BUNDLE_ENC(name, len, tmpl, nops, ...) {name, len, tmpl, nops, {__VA_ARGS__}},
#include XTENSA_ENC_INC_<NEWCORE>
};
#undef XTENSA_BUNDLE_ENC
```

### 6c. Add the CPU to `selectBundleTable`

```cpp
static XtBundleTableRef selectBundleTable(StringRef CPU) {
  if (CPU == "<new-cpu-name>")
    return {XtBundleTable<NewCore>,
            sizeof(XtBundleTable<NewCore>) / sizeof(XtBundleEnc)};
  if (CPU == "intel_tgl_adsp")
    return {XtBundleTableTgl, sizeof(XtBundleTableTgl) / sizeof(XtBundleEnc)};
  // Default: ACE15/HiFi4 table.
  return {XtBundleTableAce15, sizeof(XtBundleTableAce15) / sizeof(XtBundleEnc)};
}
```

If the new core shares a table with an existing one (Step 1), just add an
additional `if (CPU == "...")` pointing at the existing table — no new table
declaration needed.

---

## Step 7 — register the CPU in XtensaProcessors.td

Add a `Proc<>` entry in `llvm/lib/Target/Xtensa/XtensaProcessors.td` with the
correct feature flags for the new core.  The HiFi generation determines which
`FeatureHIFIx` to include:

| Coprocessor | Feature flag | Implies |
|-------------|-------------|---------|
| HiFi 3 | `FeatureHIFI3` | — |
| HiFi 4 | `FeatureHIFI4` | `FeatureHIFI3` |
| HiFi 5 | `FeatureHIFI5` | `FeatureHIFI4` → `FeatureHIFI3` |

Example (modelled on the TGL entry):

```tablegen
// my_new_adsp — MyPlatform Audio DSP (HiFi4)
def : Proc<"my_new_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean,
                            FeatureLoop, FeatureSEXT, FeatureNSA,
                            FeatureMul16, FeatureMul32, FeatureMul32High,
                            FeatureDiv32, FeatureS32C1I, FeatureSingleFloat,
                            FeatureTHREADPTR, FeatureDebug, FeatureException,
                            FeatureHighPriInterrupts, FeatureCoprocessor,
                            FeatureInterrupt, FeatureDataCache,
                            FeatureRelocatableVector, FeatureTimers3,
                            FeaturePRID, FeatureRegionProtection,
                            FeatureMINMAX, FeatureCLAMPS, FeatureHIFI4]>;
```

The exact feature list must match the core's `core-isa.h`
(`XCHAL_HAVE_*` macros) from the Zephyr SDK or Tensilica SDK.

---

## Step 8 — add CMake regeneration support

In `MCTargetDesc/CMakeLists.txt`, add one block after the existing `TGL` block:

```cmake
if(XTENSA_MODULES_C_<NEWCORE>)
  xtensa_gen_enc(<NEWCORE> <newcore>_enc.inc ${XTENSA_MODULES_C_<NEWCORE>})
endif()
```

This is optional for users who only consume the checked-in table, but it lets
anyone with the Tensilica sources reproduce or update the table.

---

## Step 9 — update this README

Add the new core to the "Currently shipped" line in `README.md` and note any
platform-specific quirks discovered during the porting (e.g. unsupported GAS
macro forms in tie-asm.h that affect the Zephyr firmware build side).

---

## Checklist

- [ ] `xtensa-modules.c` obtained for the new core
- [ ] Table generated and verified byte-exact against SDK GNU as
- [ ] `<newcore>_enc.inc` checked into `MCTargetDesc/gen/`
- [ ] `XtensaMCCodeEmitter.cpp`: include guard, table, `selectBundleTable` case
- [ ] `XtensaProcessors.td`: `Proc<>` entry with correct feature flags
- [ ] `MCTargetDesc/CMakeLists.txt`: `xtensa_gen_enc(...)` block
- [ ] `MCTargetDesc/gen/README.md`: "Currently shipped" line updated
- [ ] Build succeeds and `llvm-lit` Xtensa tests pass
