//===-- XtensaTargetTransformInfo.h - Xtensa specific TTI -------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines a TargetTransformInfo::Concept conforming object specific
// to the Xtensa target machine. It uses the target's detailed information to
// provide more precise answers to certain TTI queries, while letting the
// target independent and default TTI implementations handle the rest.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_XTENSA_XTENSATARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_XTENSA_XTENSATARGETTRANSFORMINFO_H

#include "XtensaSubtarget.h"
#include "XtensaTargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"

namespace llvm {

class XtensaTTIImpl final : public BasicTTIImplBase<XtensaTTIImpl> {
  using BaseT = BasicTTIImplBase<XtensaTTIImpl>;
  using TTI = TargetTransformInfo;
  friend BaseT;

  const XtensaSubtarget *ST;
  const XtensaTargetLowering *TLI;

  const XtensaSubtarget *getST() const { return ST; }
  const XtensaTargetLowering *getTLI() const { return TLI; }

public:
  explicit XtensaTTIImpl(const XtensaTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getDataLayout()), ST(TM->getSubtargetImpl(F)),
        TLI(ST->getTargetLowering()) {}

  // The Xtensa base ISA has no general-purpose vector registers.
  // HiFi AEDR registers exist but are only for use via intrinsics,
  // not for auto-vectorization. Returning 0 for Vector register width
  // prevents the loop vectorizer from creating vector operations like
  // <4 x i8> that the backend cannot correctly lower.
  TypeSize getRegisterBitWidth(TTI::RegisterKind K) const override {
    switch (K) {
    case TTI::RGK_Scalar:
      return TypeSize::getFixed(32);
    case TTI::RGK_FixedWidthVector:
      return TypeSize::getFixed(0);
    case TTI::RGK_ScalableVector:
      return TypeSize::getScalable(0);
    }
    llvm_unreachable("Unsupported register kind");
  }
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_XTENSA_XTENSATARGETTRANSFORMINFO_H
