#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "${REPO_ROOT}"

LLVM_BUILD="${REPO_ROOT}/build"

echo "=== Rebuilding compiler-rt with Windowed ABI (+windowed -hifi3 -hifi4 -hifi5) for xtensa-zephyr-elf ==="
rm -rf build-rt
cmake -G Ninja -B build-rt -S runtimes \
  -DCMAKE_C_COMPILER="${LLVM_BUILD}/bin/clang" \
  -DCMAKE_CXX_COMPILER="${LLVM_BUILD}/bin/clang++" \
  -DCMAKE_ASM_COMPILER="${LLVM_BUILD}/bin/clang" \
  -DCMAKE_AR="${LLVM_BUILD}/bin/llvm-ar" \
  -DCMAKE_RANLIB="${LLVM_BUILD}/bin/llvm-ranlib" \
  -DLLVM_ENABLE_RUNTIMES="compiler-rt" \
  -DCMAKE_SYSTEM_NAME=Generic \
  -DCMAKE_SYSTEM_PROCESSOR=xtensa \
  -DCMAKE_C_COMPILER_TARGET="xtensa-zephyr-elf" \
  -DCMAKE_CXX_COMPILER_TARGET="xtensa-zephyr-elf" \
  -DCMAKE_ASM_COMPILER_TARGET="xtensa-zephyr-elf" \
  -DCOMPILER_RT_BUILD_BUILTINS=ON \
  -DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
  -DCOMPILER_RT_BUILD_MEMPROF=OFF \
  -DCOMPILER_RT_BUILD_PROFILE=OFF \
  -DCOMPILER_RT_BUILD_SANITIZERS=OFF \
  -DCOMPILER_RT_BUILD_XRAY=OFF \
  -DCOMPILER_RT_BAREMETAL_BUILD=ON \
  -DCOMPILER_RT_DEFAULT_TARGET_ONLY=ON \
  -DCOMPILER_RT_OS_DIR="baremetal" \
  -DCMAKE_C_COMPILER_WORKS=ON \
  -DCMAKE_CXX_COMPILER_WORKS=ON \
  -DCMAKE_ASM_COMPILER_WORKS=ON \
  -DCMAKE_C_FLAGS="-Xclang -target-feature -Xclang +windowed -Xclang -target-feature -Xclang -hifi3 -Xclang -target-feature -Xclang -hifi4 -Xclang -target-feature -Xclang -hifi5" \
  -DCMAKE_CXX_FLAGS="-Xclang -target-feature -Xclang +windowed -Xclang -target-feature -Xclang -hifi3 -Xclang -target-feature -Xclang -hifi4 -Xclang -target-feature -Xclang -hifi5" \
  -DCMAKE_ASM_FLAGS="-Xclang -target-feature -Xclang +windowed -Xclang -target-feature -Xclang -hifi3 -Xclang -target-feature -Xclang -hifi4 -Xclang -target-feature -Xclang -hifi5"

ninja -C build-rt

echo "=== Copying builtins for xtensa-zephyr-elf ==="
cp build-rt/compiler-rt/lib/baremetal/libclang_rt.builtins-xtensa.a \
   build/lib/clang/23/lib/xtensa-zephyr-unknown-elf/libclang_rt.builtins.a

echo "=== Rebuilding compiler-rt with Windowed ABI (+windowed -hifi3 -hifi4 -hifi5) for xtensa-elf ==="
rm -rf build-compiler-rt
cmake -G Ninja -B build-compiler-rt -S runtimes \
  -DCMAKE_C_COMPILER="${LLVM_BUILD}/bin/clang" \
  -DCMAKE_CXX_COMPILER="${LLVM_BUILD}/bin/clang++" \
  -DCMAKE_ASM_COMPILER="${LLVM_BUILD}/bin/clang" \
  -DCMAKE_AR="${LLVM_BUILD}/bin/llvm-ar" \
  -DCMAKE_RANLIB="${LLVM_BUILD}/bin/llvm-ranlib" \
  -DLLVM_ENABLE_RUNTIMES="compiler-rt" \
  -DCMAKE_SYSTEM_NAME=Generic \
  -DCMAKE_SYSTEM_PROCESSOR=xtensa \
  -DCMAKE_C_COMPILER_TARGET="xtensa-elf" \
  -DCMAKE_CXX_COMPILER_TARGET="xtensa-elf" \
  -DCMAKE_ASM_COMPILER_TARGET="xtensa-elf" \
  -DCOMPILER_RT_BUILD_BUILTINS=ON \
  -DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
  -DCOMPILER_RT_BUILD_MEMPROF=OFF \
  -DCOMPILER_RT_BUILD_PROFILE=OFF \
  -DCOMPILER_RT_BUILD_SANITIZERS=OFF \
  -DCOMPILER_RT_BUILD_XRAY=OFF \
  -DCOMPILER_RT_BAREMETAL_BUILD=ON \
  -DCOMPILER_RT_DEFAULT_TARGET_ONLY=ON \
  -DCOMPILER_RT_OS_DIR="baremetal" \
  -DCMAKE_C_COMPILER_WORKS=ON \
  -DCMAKE_CXX_COMPILER_WORKS=ON \
  -DCMAKE_ASM_COMPILER_WORKS=ON \
  -DCMAKE_C_FLAGS="-Xclang -target-feature -Xclang +windowed -Xclang -target-feature -Xclang -hifi3 -Xclang -target-feature -Xclang -hifi4 -Xclang -target-feature -Xclang -hifi5" \
  -DCMAKE_CXX_FLAGS="-Xclang -target-feature -Xclang +windowed -Xclang -target-feature -Xclang -hifi3 -Xclang -target-feature -Xclang -hifi4 -Xclang -target-feature -Xclang -hifi5" \
  -DCMAKE_ASM_FLAGS="-Xclang -target-feature -Xclang +windowed -Xclang -target-feature -Xclang -hifi3 -Xclang -target-feature -Xclang -hifi4 -Xclang -target-feature -Xclang -hifi5"

ninja -C build-compiler-rt

echo "=== Copying builtins for xtensa-elf ==="
cp build-compiler-rt/compiler-rt/lib/baremetal/libclang_rt.builtins-xtensa.a \
   build/lib/clang/23/lib/xtensa-unknown-unknown-elf/libclang_rt.builtins.a

echo "=== Rebuild completed successfully ==="
