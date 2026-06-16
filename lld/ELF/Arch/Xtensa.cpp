//===- Xtensa.cpp ---------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "InputFiles.h"
#include "Symbols.h"
#include "Target.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/Support/Endian.h"

using namespace llvm;
using namespace llvm::object;
using namespace llvm::support::endian;
using namespace llvm::ELF;
using namespace lld;
using namespace lld::elf;

namespace {
class Xtensa final : public TargetInfo {
public:
  Xtensa(Ctx &ctx);
  RelExpr getRelExpr(RelType type, const Symbol &s,
                     const uint8_t *loc) const override;
  void relocate(uint8_t *loc, const Relocation &rel,
                uint64_t val) const override;
};
} // namespace

Xtensa::Xtensa(Ctx &ctx) : TargetInfo(ctx) {
  defaultImageBase = 0;
}

RelExpr Xtensa::getRelExpr(RelType type, const Symbol &s,
                           const uint8_t *loc) const {
  switch (type) {
  case R_XTENSA_NONE:
    return R_NONE;
  case R_XTENSA_32:
    return R_ABS;
  case R_XTENSA_32_PCREL:
    return R_PC;
  case R_XTENSA_SLOT0_OP:
  case R_XTENSA_SLOT1_OP:
  case R_XTENSA_SLOT2_OP:
  case R_XTENSA_SLOT3_OP:
  case R_XTENSA_SLOT4_OP:
  case R_XTENSA_SLOT5_OP:
  case R_XTENSA_SLOT6_OP:
  case R_XTENSA_SLOT7_OP:
  case R_XTENSA_SLOT8_OP:
  case R_XTENSA_SLOT9_OP:
  case R_XTENSA_SLOT10_OP:
  case R_XTENSA_SLOT11_OP:
  case R_XTENSA_SLOT12_OP:
  case R_XTENSA_SLOT13_OP:
  case R_XTENSA_SLOT14_OP:
    return R_ABS;
  default:
    Err(ctx) << getErrorLoc(ctx, loc) << "unknown relocation (" << type.v
             << ") against symbol " << &s;
    return R_NONE;
  }
}

void Xtensa::relocate(uint8_t *loc, const Relocation &rel, uint64_t val) const {
  switch (rel.type) {
  case R_XTENSA_NONE:
    break;
  case R_XTENSA_32:
  case R_XTENSA_32_PCREL:
    write32le(loc, val);
    break;
  case R_XTENSA_SLOT0_OP:
  case R_XTENSA_SLOT1_OP:
  case R_XTENSA_SLOT2_OP:
  case R_XTENSA_SLOT3_OP:
  case R_XTENSA_SLOT4_OP:
  case R_XTENSA_SLOT5_OP:
  case R_XTENSA_SLOT6_OP:
  case R_XTENSA_SLOT7_OP:
  case R_XTENSA_SLOT8_OP:
  case R_XTENSA_SLOT9_OP:
  case R_XTENSA_SLOT10_OP:
  case R_XTENSA_SLOT11_OP:
  case R_XTENSA_SLOT12_OP:
  case R_XTENSA_SLOT13_OP:
  case R_XTENSA_SLOT14_OP:
    break;
  default:
    llvm_unreachable("unknown relocation");
  }
}

void elf::setXtensaTargetInfo(Ctx &ctx) { ctx.target.reset(new Xtensa(ctx)); }
