//===-- XtensaMCFixups.h - Xtensa-specific fixup entries --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_XTENSA_MCTARGETDESC_XTENSAMCFIXUPS_H
#define LLVM_LIB_TARGET_XTENSA_MCTARGETDESC_XTENSAMCFIXUPS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace Xtensa {
enum FixupKind {
  fixup_xtensa_branch_6 = FirstTargetFixupKind,
  fixup_xtensa_branch_8,
  fixup_xtensa_branch_12,
  fixup_xtensa_jump_18,
  fixup_xtensa_call_18,
  fixup_xtensa_l32r_16,
  fixup_xtensa_loop_8,
  fixup_xtensa_slot0,
  fixup_xtensa_slot1,
  fixup_xtensa_slot2,
  fixup_xtensa_slot3,
  fixup_xtensa_slot4,
  fixup_xtensa_slot5,
  fixup_xtensa_slot6,
  fixup_xtensa_slot7,
  fixup_xtensa_slot8,
  fixup_xtensa_slot9,
  fixup_xtensa_slot10,
  fixup_xtensa_slot11,
  fixup_xtensa_slot12,
  fixup_xtensa_slot13,
  fixup_xtensa_slot14,
  fixup_xtensa_invalid,
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};
} // end namespace Xtensa
} // end namespace llvm

#endif // LLVM_LIB_TARGET_XTENSA_MCTARGETDESC_XTENSAMCFIXUPS_H
