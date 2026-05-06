//===- Xtensa.cpp ---------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ABIInfoImpl.h"
#include "TargetInfo.h"

using namespace clang;
using namespace clang::CodeGen;

//===----------------------------------------------------------------------===//
// Xtensa ABI Implementation
//===----------------------------------------------------------------------===//

namespace {

class XtensaABIInfo : public DefaultABIInfo {
private:
  static const int MaxNumArgGPRs = 6; // a2 - a7

public:
  XtensaABIInfo(CodeGenTypes &CGT) : DefaultABIInfo(CGT) {}

  ABIArgInfo classifyArgumentType(QualType Ty, int &ArgGPRsLeft) const {
    Ty = useFirstFieldIfTransparentUnion(Ty);

    if (isAggregateTypeForABI(Ty)) {
      // Records with non-trivial destructors/constructors should not be passed
      // by value.
      if (CGCXXABI::RecordArgABI RAA = getRecordArgABI(Ty, getCXXABI()))
        return getNaturalAlignIndirect(Ty, getDataLayout().getAllocaAddrSpace(),
                                       RAA == CGCXXABI::RAA_DirectInMemory);

      uint64_t Size = getContext().getTypeSize(Ty);

      // Ignore empty structs/unions.
      if (isEmptyRecord(getContext(), Ty, true) && Size == 0)
        return ABIArgInfo::getIgnore();

      int NeededArgGPRs = (Size + 31) / 32;

      if (NeededArgGPRs > 0 && NeededArgGPRs <= ArgGPRsLeft) {
        ArgGPRsLeft -= NeededArgGPRs;
        return ABIArgInfo::getDirect();
      }

      // If there are not enough registers, fall back to indirect (byval on stack).
      ArgGPRsLeft = 0;
      return DefaultABIInfo::classifyArgumentType(Ty);
    }

    // Treat an enum type as its underlying type.
    if (const auto *ED = Ty->getAsEnumDecl())
      Ty = ED->getIntegerType();

    uint64_t Size = getContext().getTypeSize(Ty);
    int NeededArgGPRs = (Size + 31) / 32;

    if (NeededArgGPRs > 0 && NeededArgGPRs <= ArgGPRsLeft)
      ArgGPRsLeft -= NeededArgGPRs;

    return DefaultABIInfo::classifyArgumentType(Ty);
  }

  ABIArgInfo classifyReturnType(QualType RetTy) const {
    if (RetTy->isVoidType())
      return ABIArgInfo::getIgnore();

    if (isAggregateTypeForABI(RetTy)) {
      uint64_t Size = getContext().getTypeSize(RetTy);
      if (Size <= 16 * 8) // max 4 registers: a2, a3, a4, a5 (16 bytes)
        return ABIArgInfo::getDirect();

      return getNaturalAlignIndirect(RetTy,
                                     getDataLayout().getAllocaAddrSpace());
    }

    return DefaultABIInfo::classifyReturnType(RetTy);
  }

  void computeInfo(CGFunctionInfo &FI) const override {
    if (!getCXXABI().classifyReturnType(FI))
      FI.getReturnInfo() = classifyReturnType(FI.getReturnType());

    int ArgGPRsLeft = MaxNumArgGPRs;

    // Xtensa uses a2 as a hidden struct pointer for indirect return values.
    if (FI.getReturnInfo().isIndirect())
      ArgGPRsLeft -= 1;

    for (auto &ArgInfo : FI.arguments()) {
      ArgInfo.info = classifyArgumentType(ArgInfo.type, ArgGPRsLeft);
    }
  }
};

class XtensaTargetCodeGenInfo : public TargetCodeGenInfo {
public:
  XtensaTargetCodeGenInfo(CodeGenTypes &CGT)
      : TargetCodeGenInfo(std::make_unique<XtensaABIInfo>(CGT)) {}
};

} // namespace

std::unique_ptr<TargetCodeGenInfo>
CodeGen::createXtensaTargetCodeGenInfo(CodeGenModule &CGM) {
  return std::make_unique<XtensaTargetCodeGenInfo>(CGM.getTypes());
}
