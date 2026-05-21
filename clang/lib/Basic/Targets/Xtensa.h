//===--- Xtensa.h - Declare Xtensa target feature support -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares Xtensa TargetInfo objects.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_BASIC_TARGETS_XTENSA_H
#define LLVM_CLANG_LIB_BASIC_TARGETS_XTENSA_H

#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/Support/Compiler.h"
#include "llvm/TargetParser/Triple.h"

#include "clang/Basic/Builtins.h"
#include "clang/Basic/MacroBuilder.h"
#include "clang/Basic/TargetBuiltins.h"

namespace clang {
namespace targets {

class LLVM_LIBRARY_VISIBILITY XtensaTargetInfo : public TargetInfo {
  static const Builtin::Info BuiltinInfo[];

protected:
  std::string CPU;

public:
  XtensaTargetInfo(const llvm::Triple &Triple, const TargetOptions &)
      : TargetInfo(Triple) {
    // no big-endianess support yet
    BigEndian = false;
    NoAsmVariants = true;
    LongLongAlign = 64;
    SuitableAlign = 32;
    DoubleAlign = LongDoubleAlign = 64;
    SizeType = UnsignedInt;
    PtrDiffType = SignedInt;
    IntPtrType = SignedInt;
    WCharType = SignedInt;
    WIntType = UnsignedInt;
    UseZeroLengthBitfieldAlignment = true;
    MaxAtomicPromoteWidth = MaxAtomicInlineWidth = 32;
    resetDataLayout();
  }

  void getTargetDefines(const LangOptions &Opts,
                        MacroBuilder &Builder) const override;

  llvm::SmallVector<Builtin::InfosShard> getTargetBuiltins() const override;

  BuiltinVaListKind getBuiltinVaListKind() const override {
    return TargetInfo::XtensaABIBuiltinVaList;
  }

  std::string_view getClobbers() const override { return ""; }

  ArrayRef<const char *> getGCCRegNames() const override {
    static const char *const GCCRegNames[] = {
        // General register name
        "a0", "sp", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "a10",
        "a11", "a12", "a13", "a14", "a15",
        // Special register name
        "sar",
        // Audio Engine Registers
        "aed0", "aed1", "aed2", "aed3", "aed4", "aed5", "aed6", "aed7",
        "aed8", "aed9", "aed10", "aed11", "aed12", "aed13", "aed14", "aed15",
        "ae0", "ae1", "ae2", "ae3", "ae4", "ae5", "ae6", "ae7",
        "ae8", "ae9", "ae10", "ae11", "ae12", "ae13", "ae14", "ae15",
        // Boolean registers
        "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7",
        "b8", "b9", "b10", "b11", "b12", "b13", "b14", "b15"};
    return llvm::ArrayRef(GCCRegNames);
  }

  ArrayRef<TargetInfo::GCCRegAlias> getGCCRegAliases() const override {
    return {};
  }

  bool validateAsmConstraint(const char *&Name,
                             TargetInfo::ConstraintInfo &Info) const override {
    switch (*Name) {
    default:
      return false;
    case 'a':
    case 'b': // Boolean register
    case 'd': // AE data register
      Info.setAllowsRegister();
      return true;
    }
    return false;
  }

  int getEHDataRegisterNumber(unsigned RegNo) const override {
    return (RegNo < 2) ? RegNo : -1;
  }

  bool isValidCPUName(StringRef Name) const override {
    return llvm::StringSwitch<bool>(Name)
        .Case("generic", true)
        // Cadence/Tensilica reference cores
        .Case("dc233c", true)
        .Case("sample_controller", true)
        .Case("sample_controller32", true)
        // Espressif ESP32 family
        .Case("esp32", true)
        .Case("espressif_esp32s2", true)
        .Case("espressif_esp32s3", true)
        .Case("esp8266", true)
        // Intel Audio DSP
        .Case("intel_tgl_adsp", true)
        .Case("intel_ace15_adsp", true)
        .Case("intel_ace30_adsp", true)
        .Case("intel_ace40_adsp", true)
        // AMD Audio Co-Processor
        .Case("amd_acp_6_0_adsp", true)
        .Case("amd_acp_7_0_adsp", true)
        .Case("amd_acp_7_3_adsp", true)
        // MediaTek Audio DSP
        .Case("mtk_mt818x_adsp", true)
        .Case("mtk_mt8195_adsp", true)
        .Case("mtk_mt8196_adsp", true)
        .Case("mtk_mt8365_adsp", true)
        // NXP Audio DSP
        .Case("nxp_imx_adsp", true)
        .Case("nxp_imx8m_adsp", true)
        .Case("nxp_imx8ulp_adsp", true)
        .Case("nxp_rt500_adsp", true)
        .Case("nxp_rt600_adsp", true)
        .Case("nxp_rt700_hifi1", true)
        .Case("nxp_rt700_hifi4", true)
        .Default(false);
  }

  bool setCPU(const std::string &Name) override {
    CPU = Name;
    return isValidCPUName(Name);
  }
};

} // namespace targets
} // namespace clang
#endif // LLVM_CLANG_LIB_BASIC_TARGETS_XTENSA_H
