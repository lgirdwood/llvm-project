//===-- XtensaTargetStreamer.h - Xtensa Target Streamer --------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_XTENSA_XTENSATARGETSTREAMER_H
#define LLVM_LIB_TARGET_XTENSA_XTENSATARGETSTREAMER_H

#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Support/SMLoc.h"
#include <map>
#include <vector>
#include <string>

namespace llvm {
class formatted_raw_ostream;

struct XtensaLiteral {
  MCSymbol *Sym;
  const MCExpr *Value;
  SMLoc Loc;
};

class XtensaTargetStreamer : public MCTargetStreamer {
protected:
  std::map<MCSection *, std::vector<XtensaLiteral>> LiteralMap;
  std::vector<std::string> LiteralPrefixStack;
  bool AbsoluteLiteralsEnabled = false;
  bool AutoLitpoolsEnabled = false;

public:
  XtensaTargetStreamer(MCStreamer &S);

  bool isAbsoluteLiteralsEnabled() const { return AbsoluteLiteralsEnabled; }
  void setAbsoluteLiterals(bool Val) { AbsoluteLiteralsEnabled = Val; }
  bool isAutoLitpoolsEnabled() const { return AutoLitpoolsEnabled; }
  void setAutoLitpools(bool Val) { AutoLitpoolsEnabled = Val; }

  // Emit literal label and literal Value to the literal section. If literal
  // section is not switched yet (SwitchLiteralSection is true) then switch to
  // literal section.
  virtual void emitLiteral(MCSymbol *LblSym, const MCExpr *Value,
                           bool SwitchLiteralSection, SMLoc L = SMLoc()) = 0;

  virtual void emitLiteralPosition() = 0;

  // Switch to the literal section. The BaseSection name is used to construct
  // literal section name.
  virtual void startLiteralSection(MCSection *BaseSection) = 0;

  virtual void emitLiteralPrefix(StringRef Prefix);
  virtual void emitLiteralPrefixEnd();

  void finish() override;
};

class XtensaTargetAsmStreamer : public XtensaTargetStreamer {
  formatted_raw_ostream &OS;

public:
  XtensaTargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);
  void emitLiteral(MCSymbol *LblSym, const MCExpr *Value,
                   bool SwitchLiteralSection, SMLoc L) override;
  void emitLiteralPosition() override;
  void startLiteralSection(MCSection *Section) override;
  void emitLiteralPrefix(StringRef Prefix) override;
  void emitLiteralPrefixEnd() override;
  void finish() override;
};

class XtensaTargetELFStreamer : public XtensaTargetStreamer {
  const MCSubtargetInfo &STI;
public:
  XtensaTargetELFStreamer(MCStreamer &S, const MCSubtargetInfo &STI);
  MCELFStreamer &getStreamer();
  void emitLiteral(MCSymbol *LblSym, const MCExpr *Value,
                   bool SwitchLiteralSection, SMLoc L) override;
  void emitLiteralPosition() override;
  void startLiteralSection(MCSection *Section) override;
  void emitLiteralPrefix(StringRef Prefix) override;
  void emitLiteralPrefixEnd() override;
  void finish() override;
};
} // end namespace llvm

#endif // LLVM_LIB_TARGET_XTENSA_XTENSATARGETSTREAMER_H
