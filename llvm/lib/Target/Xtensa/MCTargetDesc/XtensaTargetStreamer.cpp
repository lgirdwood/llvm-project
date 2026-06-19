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
#include "XtensaInstPrinter.h"
#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

namespace llvm {
cl::opt<bool> TextSectionLiterals(
    "text-section-literals",
    cl::desc("Put literal pools in a text section"),
    cl::init(false));

cl::opt<bool> AbsoluteLiterals(
    "absolute-literals",
    cl::desc("Enable absolute literal mode"),
    cl::init(false));

extern cl::opt<bool> AutoLitpools;
}

static std::string getLiteralSectionName(StringRef CSectionName, bool AbsoluteLiterals) {
  std::size_t Pos = CSectionName.find(".text");
  std::string SectionName;
  std::string Suffix = AbsoluteLiterals ? ".lit4" : ".literal";
  if (Pos != std::string::npos) {
    SectionName = CSectionName.substr(0, Pos);
 
    if (Pos > 0)
      SectionName += ".text";
 
    CSectionName = CSectionName.drop_front(Pos);
    CSectionName.consume_front(".text");
 
    SectionName += Suffix;
    SectionName += CSectionName;
  } else {
    SectionName = CSectionName;
    SectionName += Suffix;
  }
  return SectionName;
}

XtensaTargetStreamer::XtensaTargetStreamer(MCStreamer &S)
    : MCTargetStreamer(S), AbsoluteLiteralsEnabled(AbsoluteLiterals),
      AutoLitpoolsEnabled(AutoLitpools) {}

void XtensaTargetStreamer::emitLiteralPrefix(StringRef Prefix) {
  LiteralPrefixStack.push_back(Prefix.str());
}

void XtensaTargetStreamer::emitLiteralPrefixEnd() {
  if (!LiteralPrefixStack.empty())
    LiteralPrefixStack.pop_back();
}

void XtensaTargetStreamer::finish() {
  MCTargetStreamer::finish();
}

XtensaTargetAsmStreamer::XtensaTargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS)
    : XtensaTargetStreamer(S), OS(OS) {}

void XtensaTargetAsmStreamer::emitLiteral(MCSymbol *LblSym, const MCExpr *Value,
                                          bool SwitchLiteralSection, SMLoc L) {
  MCStreamer &OutStreamer = getStreamer();
  if ((TextSectionLiterals || AutoLitpoolsEnabled) && SwitchLiteralSection) {
    MCSection *CurSection = OutStreamer.getCurrentSectionOnly();
    LiteralMap[CurSection].push_back({LblSym, Value, L});
    return;
  }

  if (SwitchLiteralSection) {
    auto *CS = static_cast<MCSectionELF *>(OutStreamer.getCurrentSectionOnly());
    std::string SectionName;
    if (!LiteralPrefixStack.empty() && !LiteralPrefixStack.back().empty()) {
      SectionName = LiteralPrefixStack.back() + (AbsoluteLiteralsEnabled ? ".lit4" : ".literal");
    } else {
      SectionName = getLiteralSectionName(CS->getName(), AbsoluteLiteralsEnabled);
    }

    unsigned Flags = ELF::SHF_ALLOC;
    if (CS->getFlags() & ELF::SHF_EXECINSTR)
      Flags |= ELF::SHF_EXECINSTR;

    MCSection *ConstSection = OutStreamer.getContext().getELFSection(
        SectionName, ELF::SHT_PROGBITS, Flags);
    OutStreamer.pushSection();
    OutStreamer.switchSection(ConstSection);
    OutStreamer.emitValueToAlignment(Align(4));
  }

  SmallString<60> Str;
  raw_svector_ostream LiteralStr(Str);

  LiteralStr << "\t.literal " << LblSym->getName() << ", ";

  if (auto CE = dyn_cast<MCConstantExpr>(Value)) {
    LiteralStr << CE->getValue() << "\n";
  } else if (auto SRE = dyn_cast<MCSymbolRefExpr>(Value)) {
    const MCSymbol &Sym = SRE->getSymbol();
    LiteralStr << Sym.getName() << "\n";
  } else {
    llvm_unreachable("unexpected constant pool entry type");
  }

  OS << LiteralStr.str();

  if (SwitchLiteralSection) {
    OutStreamer.popSection();
  }
}

void XtensaTargetAsmStreamer::emitLiteralPosition() {
  MCStreamer &OutStreamer = getStreamer();
  MCSection *CurSection = OutStreamer.getCurrentSectionOnly();
  auto it = LiteralMap.find(CurSection);
  bool HasLiterals = (it != LiteralMap.end() && !it->second.empty());
  if (TextSectionLiterals || AutoLitpoolsEnabled || HasLiterals) {
    if (HasLiterals) {
      for (const auto &Lit : it->second) {
        emitLiteral(Lit.Sym, Lit.Value, false, Lit.Loc);
      }
      it->second.clear();
    }
  } else {
    OS << "\t.literal_position\n";
  }
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

void XtensaTargetAsmStreamer::emitLiteralPrefix(StringRef Prefix) {
  XtensaTargetStreamer::emitLiteralPrefix(Prefix);
  if (!Prefix.empty()) {
    OS << "\t.begin\tliteral_prefix\t" << Prefix << "\n";
  } else {
    OS << "\t.begin\tliteral_prefix\n";
  }
}

void XtensaTargetAsmStreamer::emitLiteralPrefixEnd() {
  XtensaTargetStreamer::emitLiteralPrefixEnd();
  OS << "\t.end\tliteral_prefix\n";
}

void XtensaTargetAsmStreamer::finish() {
  MCStreamer &OutStreamer = getStreamer();
  for (auto &Pair : LiteralMap) {
    MCSection *Sec = Pair.first;
    if (!Pair.second.empty()) {
      OutStreamer.pushSection();
      OutStreamer.switchSection(Sec);
      emitLiteralPosition();
      OutStreamer.popSection();
    }
  }
  XtensaTargetStreamer::finish();
}

XtensaTargetELFStreamer::XtensaTargetELFStreamer(MCStreamer &S, const MCSubtargetInfo &STI)
    : XtensaTargetStreamer(S), STI(STI) {}

void XtensaTargetELFStreamer::emitLiteral(MCSymbol *LblSym, const MCExpr *Value,
                                          bool SwitchLiteralSection, SMLoc L) {
  MCStreamer &OutStreamer = getStreamer();
  if ((TextSectionLiterals || AutoLitpoolsEnabled) && SwitchLiteralSection) {
    MCSection *CurSection = OutStreamer.getCurrentSectionOnly();
    LiteralMap[CurSection].push_back({LblSym, Value, L});
    return;
  }

  if (SwitchLiteralSection) {
    MCContext &Context = OutStreamer.getContext();
    auto *CS = static_cast<MCSectionELF *>(OutStreamer.getCurrentSectionOnly());
    std::string SectionName;
    if (!LiteralPrefixStack.empty() && !LiteralPrefixStack.back().empty()) {
      SectionName = LiteralPrefixStack.back() + (AbsoluteLiteralsEnabled ? ".lit4" : ".literal");
    } else {
      SectionName = getLiteralSectionName(CS->getName(), AbsoluteLiteralsEnabled);
    }

    unsigned Flags = ELF::SHF_ALLOC;
    if (CS->getFlags() & ELF::SHF_EXECINSTR)
      Flags |= ELF::SHF_EXECINSTR;

    MCSection *ConstSection = Context.getELFSection(
        SectionName, ELF::SHT_PROGBITS, Flags);
    ConstSection->setAlignment(Align(4));

    OutStreamer.pushSection();
    OutStreamer.switchSection(ConstSection);
  }

  OutStreamer.emitLabel(LblSym, L);
  OutStreamer.emitValue(Value, 4, L);

  if (SwitchLiteralSection) {
    OutStreamer.popSection();
  }
}

void XtensaTargetELFStreamer::emitLiteralPosition() {
  MCStreamer &OutStreamer = getStreamer();
  MCSection *CurSection = OutStreamer.getCurrentSectionOnly();
  auto it = LiteralMap.find(CurSection);
  if (it != LiteralMap.end() && !it->second.empty()) {
    OutStreamer.emitValueToAlignment(Align(4));
    for (const auto &Lit : it->second) {
      OutStreamer.emitLabel(Lit.Sym, Lit.Loc);
      OutStreamer.emitValue(Lit.Value, 4, Lit.Loc);
    }
    it->second.clear();
  }
}

void XtensaTargetELFStreamer::startLiteralSection(MCSection *BaseSection) {
  MCContext &Context = getStreamer().getContext();

  std::string SectionName;
  if (!LiteralPrefixStack.empty() && !LiteralPrefixStack.back().empty()) {
    SectionName = LiteralPrefixStack.back() + (AbsoluteLiteralsEnabled ? ".lit4" : ".literal");
  } else {
    SectionName = getLiteralSectionName(BaseSection->getName(), AbsoluteLiteralsEnabled);
  }

  auto *BS = static_cast<MCSectionELF *>(BaseSection);
  unsigned Flags = ELF::SHF_ALLOC;
  if (BS->getFlags() & ELF::SHF_EXECINSTR)
    Flags |= ELF::SHF_EXECINSTR;

  MCSection *ConstSection = Context.getELFSection(
      SectionName, ELF::SHT_PROGBITS, Flags);

  ConstSection->setAlignment(Align(4));
}

void XtensaTargetELFStreamer::emitLiteralPrefix(StringRef Prefix) {
  XtensaTargetStreamer::emitLiteralPrefix(Prefix);
}

void XtensaTargetELFStreamer::emitLiteralPrefixEnd() {
  XtensaTargetStreamer::emitLiteralPrefixEnd();
}

void XtensaTargetELFStreamer::finish() {
  MCStreamer &OutStreamer = getStreamer();
  MCContext &Context = OutStreamer.getContext();
  for (auto &Pair : LiteralMap) {
    MCSection *Sec = Pair.first;
    if (!Pair.second.empty()) {
      OutStreamer.pushSection();
      OutStreamer.switchSection(Sec);
      
      bool IsText = Sec->isText();
      MCSymbol *PostLabel = nullptr;
      if (IsText) {
        PostLabel = Context.createTempSymbol();
        MCInst Jmp;
        Jmp.setOpcode(Xtensa::J);
        Jmp.addOperand(MCOperand::createExpr(MCSymbolRefExpr::create(PostLabel, Context)));
        OutStreamer.emitInstruction(Jmp, STI);
      }
      
      emitLiteralPosition();
      
      if (IsText) {
        OutStreamer.emitLabel(PostLabel);
      }
      
      OutStreamer.popSection();
    }
  }
  XtensaTargetStreamer::finish();
}

MCELFStreamer &XtensaTargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}
