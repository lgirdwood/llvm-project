# The LLVM Compiler Infrastructure

[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/llvm/llvm-project/badge)](https://securityscorecards.dev/viewer/?uri=github.com/llvm/llvm-project)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/8273/badge)](https://www.bestpractices.dev/projects/8273)
[![libc++](https://github.com/llvm/llvm-project/actions/workflows/libcxx-build-and-test.yaml/badge.svg?branch=main&event=schedule)](https://github.com/llvm/llvm-project/actions/workflows/libcxx-build-and-test.yaml?query=event%3Aschedule)

# Xtensa Clang Development Branch for Zephyr and SOF on Intel ADSP

This is the Xtensa development branch of LLVM, designed to enable Clang for Zephyr and Sound Open Firmware (SOF) on Intel ADSP targets.

---

## Building Clang for Xtensa ADSP Target

### 1. Build LLVM/Clang Compiler
From the root of this `llvm-project` repository, configure and build LLVM and Clang:

```bash
# Configure the build
cmake -G Ninja -S llvm -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_PROJECTS="clang;lld" \
  -DLLVM_TARGETS_TO_BUILD="host" \
  -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD="Xtensa" \
  -DLLVM_ENABLE_ASSERTIONS=OFF \
  -DLLVM_OPTIMIZED_TABLEGEN=ON

# Run the build
ninja -C build
```

### 2. Build compiler-rt Builtins
Intel ADSP targets require specific builtins to be compiled with correct target features (e.g., windowed ABI, HiFi coprocessor disabled to prevent boot-time exceptions).

Run the pre-configured scripts in the repository root to compile and install `compiler-rt` builtins for the Xtensa targets:

- For the Windowed ABI (Standard for Intel ADSP targets):
  ```bash
  ./build_windowed_rt.sh
  ```
- For the Call0 ABI (if needed):
  ```bash
  ./build_call0_rt.sh
  ```

---

## Building SOF using this Clang

To build Sound Open Firmware (SOF) using the newly built Xtensa Clang:

### 1. Integrate Fork Branches into Developer Workspace
Ensure your SOF workspace is initialized. Pull the required development branches into your local repository:

- **SOF Repository**:
  Pull the [llvm-stable](https://github.com/lgirdwood/sof/tree/llvm-stable) branch from the [SOF fork](https://github.com/lgirdwood/sof.git):
  ```bash
  cd sof
  git remote add lgirdwood https://github.com/lgirdwood/sof.git
  git fetch lgirdwood llvm-stable
  git checkout -b my-working-branch # or checkout your current working branch
  git pull lgirdwood llvm-stable
  cd ..
  ```

- **Zephyr Repository**:
  Pull the [llvm-stable](https://github.com/lgirdwood/zephyr/tree/llvm-stable) branch from the [Zephyr fork](https://github.com/lgirdwood/zephyr.git):
  ```bash
  cd zephyr
  git remote add lgirdwood https://github.com/lgirdwood/zephyr.git
  git fetch lgirdwood llvm-stable
  git checkout -b my-working-branch # or checkout your current working branch
  git pull lgirdwood llvm-stable
  cd ..
  ```

### 2. Activate the Environment
Activate your Zephyr/SOF Python virtual environment:
```bash
source .venv/bin/activate
```

### 3. Build SOF for Intel ADSP Platforms
Use the SOF build script `xtensa-build-zephyr.py` located in `sof/scripts/` to build the firmware for the targets, pointing to your LLVM build directory:

- **Meteor Lake / Arrow Lake (mtl / arl)**:
  ```bash
  ./sof/scripts/xtensa-build-zephyr.py -p mtl --llvm-clang /path/to/llvm-project/build --build-dir-suffix -llvm
  ```
- **Tiger Lake (tgl)**:
  ```bash
  ./sof/scripts/xtensa-build-zephyr.py -p tgl --llvm-clang /path/to/llvm-project/build --build-dir-suffix -llvm
  ```
- **Panther Lake (ptl)**:
  ```bash
  ./sof/scripts/xtensa-build-zephyr.py -p ptl --llvm-clang /path/to/llvm-project/build --build-dir-suffix -llvm
  ```

---

Welcome to the LLVM project!

This repository contains the source code for LLVM, a toolkit for the
construction of highly optimized compilers, optimizers, and run-time
environments.

The LLVM project has multiple components. The core of the project is
itself called "LLVM". This contains all of the tools, libraries, and header
files needed to process intermediate representations and convert them into
object files. Tools include an assembler, disassembler, bitcode analyzer, and
bitcode optimizer.

C-like languages use the [Clang](https://clang.llvm.org/) frontend. This
component compiles C, C++, Objective-C, and Objective-C++ code into LLVM bitcode
-- and from there into object files, using LLVM.

Other components include:
the [libc++ C++ standard library](https://libcxx.llvm.org),
the [LLD linker](https://lld.llvm.org), and more.

## Getting the Source Code and Building LLVM

Consult the
[Getting Started with LLVM](https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm)
page for information on building and running LLVM.

For information on how to contribute to the LLVM project, please take a look at
the [Contributing to LLVM](https://llvm.org/docs/Contributing.html) guide.

## Getting in touch

Join the [LLVM Discourse forums](https://discourse.llvm.org/), [Discord
chat](https://discord.gg/xS7Z362),
[LLVM Office Hours](https://llvm.org/docs/GettingInvolved.html#office-hours) or
[Regular sync-ups](https://llvm.org/docs/GettingInvolved.html#online-sync-ups).

The LLVM project has adopted a [code of conduct](https://llvm.org/docs/CodeOfConduct.html) for
participants to all modes of communication within the project.
