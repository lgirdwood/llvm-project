//===-- XtensaTargetStreamer.cpp - Xtensa Target Streamer Methods ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides Xtensa specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "XtensaTargetStreamer.h"
#include "MCTargetDesc/XtensaMCAsmInfo.h"
#include "XtensaInstPrinter.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

static std::string getLiteralSectionName(StringRef CSectionName) {
  std::size_t Pos = CSectionName.find(".text");
  std::string SectionName;
  if (Pos != std::string::npos) {
    SectionName = CSectionName.substr(0, Pos);

    if (Pos > 0)
      SectionName += ".text";

    CSectionName = CSectionName.drop_front(Pos);
    CSectionName.consume_front(".text");

    SectionName += ".literal";
    SectionName += CSectionName;
  } else {
    SectionName = CSectionName;
    SectionName += ".literal";
  }
  return SectionName;
}

XtensaTargetStreamer::XtensaTargetStreamer(MCStreamer &S)
    : MCTargetStreamer(S) {}

XtensaTargetAsmStreamer::XtensaTargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS)
    : XtensaTargetStreamer(S), OS(OS) {}

void XtensaTargetAsmStreamer::emitLiteral(MCSymbol *LblSym, const MCExpr *Value,
                                          bool SwitchLiteralSection, SMLoc L) {
  SmallString<60> Str;
  raw_svector_ostream LiteralStr(Str);

  LiteralStr << "\t.literal " << LblSym->getName() << ", ";

  if (auto CE = dyn_cast<MCConstantExpr>(Value)) {
    LiteralStr << CE->getValue() << "\n";
  } else if (auto SRE = dyn_cast<MCSymbolRefExpr>(Value)) {
    const MCSymbol &Sym = SRE->getSymbol();
    Xtensa::Specifier Spec =
        static_cast<Xtensa::Specifier>(SRE->getSpecifier());
    LiteralStr << Sym.getName();
    if (Spec == Xtensa::S_TPOFF)
      LiteralStr << "@TPOFF";
    LiteralStr << '\n';
  } else {
    llvm_unreachable("unexpected constant pool entry type");
  }

  OS << LiteralStr.str();
}

void XtensaTargetAsmStreamer::emitLiteralPosition() {
  OS << "\t.literal_position\n";
}

void XtensaTargetAsmStreamer::startLiteralSection(MCSection *BaseSection) {
  // When using --text-section-literals with the GNU assembler, literal pool
  // entries are placed into a section derived from the *current* section
  // (e.g., .imr.0 -> .imr.0.literal, .text -> merged into .text).
  // We must always switch to the function's actual section before emitting
  // .literal_position so that GAS creates the correct literal section.
  // Without this, literals can cross section boundaries causing l32r
  // out-of-range errors between different MEMORY regions.
  if (BaseSection) {
    StringRef SecName = BaseSection->getName();
    if (!SecName.empty()) {
      if (SecName == ".text") {
        OS << "\t.text\n";
      } else {
        OS << "\t.section\t" << SecName << ",\"ax\",@progbits\n";
      }
    }
  }
  emitLiteralPosition();
}

XtensaTargetELFStreamer::XtensaTargetELFStreamer(MCStreamer &S)
    : XtensaTargetStreamer(S) {}

void XtensaTargetELFStreamer::emitLiteral(MCSymbol *LblSym, const MCExpr *Value,
                                          bool SwitchLiteralSection, SMLoc L) {
  MCStreamer &OutStreamer = getStreamer();
  if (SwitchLiteralSection) {
    MCContext &Context = OutStreamer.getContext();
    auto *CS = static_cast<MCSectionELF *>(OutStreamer.getCurrentSectionOnly());
    std::string SectionName = getLiteralSectionName(CS->getName());

    MCSection *ConstSection = Context.getELFSection(
        SectionName, ELF::SHT_PROGBITS, ELF::SHF_EXECINSTR | ELF::SHF_ALLOC);

    OutStreamer.pushSection();
    OutStreamer.switchSection(ConstSection);
  }

  OutStreamer.emitLabel(LblSym, L);
  OutStreamer.emitValue(Value, 4, L);

  if (SwitchLiteralSection) {
    OutStreamer.popSection();
  }
}

void XtensaTargetELFStreamer::startLiteralSection(MCSection *BaseSection) {
  MCContext &Context = getStreamer().getContext();

  std::string SectionName = getLiteralSectionName(BaseSection->getName());

  MCSection *ConstSection = Context.getELFSection(
      SectionName, ELF::SHT_PROGBITS, ELF::SHF_EXECINSTR | ELF::SHF_ALLOC);

  ConstSection->setAlignment(Align(4));
  MCStreamer &OutStreamer = getStreamer();
  OutStreamer.switchSection(ConstSection);
}

MCELFStreamer &XtensaTargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}
