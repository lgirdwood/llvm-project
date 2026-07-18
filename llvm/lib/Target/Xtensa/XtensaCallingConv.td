//===- XtensaCallingConv.td - Xtensa Calling Conventions -*- tablegen ---*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for the Xtensa ABI.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Xtensa base calling convention
//===----------------------------------------------------------------------===//
// Xtensa return value
def RetCC_Xtensa : CallingConv<[
  // First two return values go in a2, a3, a4, a5
  CCIfType<[i32], CCAssignToReg<[A2, A3, A4, A5]>>,
  CCIfType<[f32], CCAssignToReg<[A2, A3, A4, A5]>>,
  CCIfType<[i64], CCAssignToRegWithShadow<[A2, A4], [A3, A5]>>
]>;

// Callee-saved register lists
def CSR_Xtensa : CalleeSavedRegs<(add A0, A12, A13, A14, A15)>;

//===----------------------------------------------------------------------===//
// Xtensa windowed calling convention. Currently by default implemented
// rotation window by 8 registers.
//===----------------------------------------------------------------------===//
// Xtensa return value for 8 registers window
def RetCCW8_Xtensa : CallingConv<[
  //First two return values go in a10, a11, a12, a13
  CCIfType<[i32], CCAssignToReg<[A10, A11, A12, A13]>>,
  // f32 values are passed as integer bit-patterns in the same AR registers
  // (Xtensa windowed ABI: no FP registers for argument/return passing)
  CCIfType<[f32], CCAssignToReg<[A10, A11, A12, A13]>>,
  CCIfType<[i64], CCAssignToRegWithShadow<[A10, A12], [A11, A13]>>
]>;

// Callee-saved register lists for rotation window by 8 registers.
// AR registers: the windowed ABI saves/restores the caller's AR window
// implicitly via register rotation; only non-windowed regs need listing.
// FP registers: f0-f3 are caller-saved (scratch); f4-f15 are callee-saved.
// The callee must save and restore f4-f15 across function calls.
def CSRW8_Xtensa : CalleeSavedRegs<(add F4, F5, F6, F7, F8, F9, F10, F11,
                                        F12, F13, F14, F15)> {
  let OtherPreserved = (add A0, SP, A2, A3, A4, A5, A6, A7);
}
