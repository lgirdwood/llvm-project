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
  int64_t getImplicitAddend(const uint8_t *buf, RelType type) const override;
  RelType getDynRel(RelType type) const override;
  bool needsThunk(RelExpr expr, RelType type, const InputFile *file,
                  uint64_t branchAddr, const Symbol &s,
                  int64_t a) const override;
  uint32_t getThunkSectionSpacing() const override;
  bool inBranchRange(RelType type, uint64_t src, uint64_t dst) const override;
};
} // namespace

Xtensa::Xtensa(Ctx &ctx) : TargetInfo(ctx) {
  defaultImageBase = 0;
  gotRel = R_XTENSA_GLOB_DAT;
  pltRel = R_XTENSA_JMP_SLOT;
  relativeRel = R_XTENSA_RELATIVE;
  symbolicRel = R_XTENSA_32;
  needsThunks = true;
}

RelExpr Xtensa::getRelExpr(RelType type, const Symbol &s,
                           const uint8_t *loc) const {
  switch (type) {
  case R_XTENSA_NONE:
  case R_XTENSA_DIFF8:
  case R_XTENSA_DIFF16:
  case R_XTENSA_DIFF32:
  case R_XTENSA_PDIFF8:
  case R_XTENSA_PDIFF16:
  case R_XTENSA_PDIFF32:
  case R_XTENSA_NDIFF8:
  case R_XTENSA_NDIFF16:
  case R_XTENSA_NDIFF32:
  case R_XTENSA_ASM_EXPAND:
  case R_XTENSA_ASM_SIMPLIFY:
    return R_NONE;
  case R_XTENSA_32:
    return R_ABS;
  case R_XTENSA_32_PCREL:
    return R_PC;
  case R_XTENSA_PLT:
    return R_PLT;
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
    return R_PC;
  default:
    Err(ctx) << getErrorLoc(ctx, loc) << "unknown relocation (" << type.v
             << ") against symbol " << &s;
    return R_NONE;
  }
}

void Xtensa::relocate(uint8_t *loc, const Relocation &rel, uint64_t val) const {
  switch (rel.type) {
  case R_XTENSA_NONE:
  case R_XTENSA_DIFF8:
  case R_XTENSA_DIFF16:
  case R_XTENSA_DIFF32:
  case R_XTENSA_PDIFF8:
  case R_XTENSA_PDIFF16:
  case R_XTENSA_PDIFF32:
  case R_XTENSA_NDIFF8:
  case R_XTENSA_NDIFF16:
  case R_XTENSA_NDIFF32:
  case R_XTENSA_ASM_EXPAND:
  case R_XTENSA_ASM_SIMPLIFY:
    break;
  case R_XTENSA_32:
  case R_XTENSA_32_PCREL:
  case R_XTENSA_PLT:
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
  case R_XTENSA_SLOT14_OP: {
    uint32_t inst = read32le(loc);
    uint8_t op = inst & 0xf;
    if (op == 6) {
      uint8_t t = (inst >> 4) & 0xf;
      uint8_t t_low = t & 0x3;
      if (t_low == 0) { // J
        inst &= ~(0x3ffff << 6);
        inst |= (((val - 4) & 0x3ffff) << 6);
      } else if (t_low == 1) { // br12 branch (beqz, bnez, etc.)
        inst &= ~(0xfff << 12);
        inst |= (((val - 4) & 0xfff) << 12);
      } else { // RRI8 branch (beqi, bgeui, etc.)
        inst &= ~(0xff << 16);
        inst |= (((val - 4) & 0xff) << 16);
      }
    } else if (op == 7) { // RRI8 branch (beqi, bgeui, bbci, bbsi, etc.)
      inst &= ~(0xff << 16);
      inst |= (((val - 4) & 0xff) << 16);
    } else if (op == 5) { // CALLx
      uint64_t target = rel.sym->getVA(ctx) + rel.addend;
      uint64_t PC = target - val;
      int64_t offset = ((int64_t)target - (int64_t)((PC & ~3) + 4)) >> 2;
      inst &= ~(0x3ffff << 6);
      inst |= ((offset & 0x3ffff) << 6);
    } else if (op == 1) { // L32R
      uint64_t target = rel.sym->getVA(ctx) + rel.addend;
      uint64_t PC = target - val;
      bool relaxed = false;
      if (ctx.arg.relax) {
        uint8_t t = (inst >> 4) & 0xf;
        uint32_t inst_callx = read32le(loc + 3);
        if ((inst_callx & 0xf) == 0 &&
            ((inst_callx >> 16) & 0xf) == 0 &&
            ((inst_callx >> 20) & 0xf) == 0 &&
            ((inst_callx >> 12) & 0xf) == 0) {
          uint8_t s = (inst_callx >> 8) & 0xf;
          uint8_t m = (inst_callx >> 6) & 0x3;
          uint8_t n = (inst_callx >> 4) & 0x3;
          if (t == s) {
            auto *d = dyn_cast_or_null<Defined>(rel.sym);
            if (d && d->section) {
              if (auto *sec = dyn_cast<InputSectionBase>(d->section)) {
                uint64_t litOffset = d->value + rel.addend;
                Symbol *targetSym = nullptr;
                int64_t targetAddend = 0;
                for (const Relocation &litRel : sec->relocs()) {
                  if (litRel.offset == litOffset) {
                    targetSym = litRel.sym;
                    targetAddend = litRel.addend;
                    break;
                  }
                }
                if (targetSym) {
                uint64_t targetVA = targetSym->getVA(ctx) + targetAddend;
                if (m == 3) { // CALLX
                  int64_t distance = (int64_t)targetVA - (int64_t)((PC & ~3) + 4);
                  if (distance % 4 == 0) {
                    int64_t offset = distance >> 2;
                    if (offset >= -131072 && offset <= 131071) {
                      uint32_t call_inst = ((offset & 0x3ffff) << 6) | ((n & 3) << 4) | 0x05;
                      inst = call_inst;
                      loc[3] = 0xf0;
                      loc[4] = 0x20;
                      loc[5] = 0x00;
                      relaxed = true;
                    }
                  }
                } else if (m == 2 && n == 0) { // JX
                  int64_t distance = (int64_t)targetVA - (int64_t)(PC + 4);
                  if (distance >= -131072 && distance <= 131071) {
                    uint32_t j_inst = ((distance & 0x3ffff) << 6) | 0x06;
                    inst = j_inst;
                    loc[3] = 0xf0;
                    loc[4] = 0x20;
                    loc[5] = 0x00;
                    relaxed = true;
                  }
                }
              }
            }
          }
        }
      }
    }
    if (!relaxed) {
        int64_t offset = ((int64_t)target - (int64_t)((PC + 3) & ~3)) >> 2;
        inst &= ~(0xffff << 8);
        inst |= ((offset & 0xffff) << 8);
      }
      loc[0] = inst & 0xff;
      loc[1] = (inst >> 8) & 0xff;
      loc[2] = (inst >> 16) & 0xff;
      break;
    } else if (op == 4) { // LOOP
      inst &= ~(0xff << 16);
      inst |= (((val - 4) & 0xff) << 16);
    }
    loc[0] = inst & 0xff;
    loc[1] = (inst >> 8) & 0xff;
    loc[2] = (inst >> 16) & 0xff;
    break;
  }
  default:
    llvm_unreachable("unknown relocation");
  }
}

int64_t Xtensa::getImplicitAddend(const uint8_t *buf, RelType type) const {
  switch (type) {
  case R_XTENSA_32:
  case R_XTENSA_32_PCREL:
  case R_XTENSA_PLT:
    return read32le(buf);
  default:
    return 0;
  }
}

RelType Xtensa::getDynRel(RelType type) const {
  if (type == symbolicRel)
    return type;
  return R_XTENSA_NONE;
}

bool Xtensa::needsThunk(RelExpr expr, RelType type, const InputFile *file,
                        uint64_t branchAddr, const Symbol &s,
                        int64_t a) const {
  if (type < R_XTENSA_SLOT0_OP || type > R_XTENSA_SLOT14_OP)
    return false;
  if (s.isUndefined() && !s.isInPlt(ctx))
    return true;
  StringRef name = s.getName();
  if (name.starts_with(".literal") || name.starts_with(".L") || name.empty())
    return false;
  uint64_t dst = s.getVA(ctx, a);
  int64_t offset = (int64_t)dst - (int64_t)branchAddr;
  return offset < -524288 || offset > 524287;
}

uint32_t Xtensa::getThunkSectionSpacing() const {
  return 0x70000;
}

bool Xtensa::inBranchRange(RelType type, uint64_t src, uint64_t dst) const {
  if (type < R_XTENSA_SLOT0_OP || type > R_XTENSA_SLOT14_OP)
    return true;
  int64_t offset = (int64_t)dst - (int64_t)src;
  return offset >= -524288 && offset <= 524287;
}

void elf::setXtensaTargetInfo(Ctx &ctx) { ctx.target.reset(new Xtensa(ctx)); }
