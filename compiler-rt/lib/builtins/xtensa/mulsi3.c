//===-- mulsi3.c - Implement __mulsi3 -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "../int_lib.h"

// Returns: a * b
// This uses the peasant shift-and-add algorithm to avoid calling itself recursively
// (since the core lacks a 32-bit hardware multiplier).
COMPILER_RT_ABI su_int __mulsi3(su_int a, su_int b) {
  su_int r = 0;
  while (b > 0) {
    if (b & 1) {
      r += a;
    }
    a <<= 1;
    b >>= 1;
  }
  return r;
}
