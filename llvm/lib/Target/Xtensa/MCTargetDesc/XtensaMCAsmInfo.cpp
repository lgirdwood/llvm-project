//===-- XtensaMCAsmInfo.cpp - Xtensa Asm Properties -----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the XtensaMCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "XtensaMCAsmInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/TargetParser/Triple.h"

using namespace llvm;

XtensaMCAsmInfo::XtensaMCAsmInfo(const Triple &TT,
                                 const MCTargetOptions &Options)
    : MCAsmInfoELF(Options) {
  CodePointerSize = 4;
  CalleeSaveStackSlotSize = 4;
  InternalSymbolPrefix = ".L";
  CommentString = "#";
  ZeroDirective = "\t.space\t";
  Data64bitsDirective = "\t.quad\t";
  GlobalDirective = "\t.global\t";
  UsesELFSectionDirectiveForBSS = true;
  SupportsDebugInformation = true;
  // Don't emit .cfi_* directives — the Xtensa GCC assembler doesn't
  // support them, and they cause errors with -fno-integrated-as.
  ExceptionsType = ExceptionHandling::None;
  AlignmentIsInBytes = false;
}

void XtensaMCAsmInfo::printSpecifierExpr(raw_ostream &OS,
                                         const MCSpecifierExpr &Expr) const {
  StringRef S = Xtensa::getSpecifierName(Expr.getSpecifier());
  if (!S.empty())
    OS << '%' << S << '(';
  printExpr(OS, *Expr.getSubExpr());
  if (!S.empty())
    OS << ')';
}

uint8_t Xtensa::parseSpecifier(StringRef name) { return 0; }

StringRef Xtensa::getSpecifierName(uint8_t S) {
  switch (S) {
  default:
    llvm_unreachable("Invalid ELF symbol kind");
  }
}
