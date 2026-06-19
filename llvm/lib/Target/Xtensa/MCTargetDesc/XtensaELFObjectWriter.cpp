//===-- XtensaMCObjectWriter.cpp - Xtensa ELF writer ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/XtensaFixupKinds.h"
#include "MCTargetDesc/XtensaMCAsmInfo.h"
#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include <cassert>
#include <cstdint>

using namespace llvm;

namespace {
class XtensaObjectWriter : public MCELFObjectTargetWriter {
public:
  XtensaObjectWriter(uint8_t OSABI);

  virtual ~XtensaObjectWriter();

protected:
  unsigned getRelocType(const MCFixup &, const MCValue &,
                        bool IsPCRel) const override;
  bool needsRelocateWithSymbol(const MCValue &, unsigned Type) const override;
};
} // namespace

XtensaObjectWriter::XtensaObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(false, OSABI, ELF::EM_XTENSA,
                              /*HasRelocationAddend=*/true) {}

XtensaObjectWriter::~XtensaObjectWriter() {}

unsigned XtensaObjectWriter::getRelocType(const MCFixup &Fixup,
                                          const MCValue &Target,
                                          bool IsPCRel) const {
  uint8_t Specifier = Target.getSpecifier();

  switch ((unsigned)Fixup.getKind()) {
  case FK_Data_4:
    if (IsPCRel)
      return ELF::R_XTENSA_32_PCREL;
    if (Specifier == Xtensa::S_PLT)
      return ELF::R_XTENSA_PLT;
    return Specifier == Xtensa::S_TPOFF ? ELF::R_XTENSA_TLS_TPOFF
                                        : ELF::R_XTENSA_32;
  case Xtensa::fixup_xtensa_slot0:
    return ELF::R_XTENSA_SLOT0_OP;
  case Xtensa::fixup_xtensa_slot1:
    return ELF::R_XTENSA_SLOT1_OP;
  case Xtensa::fixup_xtensa_slot2:
    return ELF::R_XTENSA_SLOT2_OP;
  case Xtensa::fixup_xtensa_slot3:
    return ELF::R_XTENSA_SLOT3_OP;
  case Xtensa::fixup_xtensa_slot4:
    return ELF::R_XTENSA_SLOT4_OP;
  case Xtensa::fixup_xtensa_slot5:
    return ELF::R_XTENSA_SLOT5_OP;
  case Xtensa::fixup_xtensa_slot6:
    return ELF::R_XTENSA_SLOT6_OP;
  case Xtensa::fixup_xtensa_slot7:
    return ELF::R_XTENSA_SLOT7_OP;
  case Xtensa::fixup_xtensa_slot8:
    return ELF::R_XTENSA_SLOT8_OP;
  case Xtensa::fixup_xtensa_slot9:
    return ELF::R_XTENSA_SLOT9_OP;
  case Xtensa::fixup_xtensa_slot10:
    return ELF::R_XTENSA_SLOT10_OP;
  case Xtensa::fixup_xtensa_slot11:
    return ELF::R_XTENSA_SLOT11_OP;
  case Xtensa::fixup_xtensa_slot12:
    return ELF::R_XTENSA_SLOT12_OP;
  case Xtensa::fixup_xtensa_slot13:
    return ELF::R_XTENSA_SLOT13_OP;
  case Xtensa::fixup_xtensa_slot14:
    return ELF::R_XTENSA_SLOT14_OP;
  default:
    return ELF::R_XTENSA_SLOT0_OP;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createXtensaObjectWriter(uint8_t OSABI, bool IsLittleEndian) {
  return std::make_unique<XtensaObjectWriter>(OSABI);
}

bool XtensaObjectWriter::needsRelocateWithSymbol(const MCValue &,
                                                 unsigned Type) const {
  return false;
}
