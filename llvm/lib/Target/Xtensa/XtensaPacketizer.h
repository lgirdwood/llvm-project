//===- XtensaPacketizer.h - Xtensa FLIX VLIW Packetizer ---------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_XTENSA_XTENSAPACKETIZER_H
#define LLVM_LIB_TARGET_XTENSA_XTENSAPACKETIZER_H

#include "llvm/PassRegistry.h"

namespace llvm {
class FunctionPass;
class PassRegistry;

FunctionPass *createXtensaPacketizerPass();
void initializeXtensaPacketizerPass(PassRegistry &);
} // end namespace llvm

#endif
