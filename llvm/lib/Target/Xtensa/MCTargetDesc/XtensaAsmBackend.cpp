//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/XtensaFixupKinds.h"
#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSection.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/Support/Debug.h"
#include "llvm/MC/MCRegisterInfo.h"

using namespace llvm;

namespace llvm {
class MCObjectTargetWriter;
}
namespace {
class XtensaAsmBackend : public MCAsmBackend {
  uint8_t OSABI;
  bool IsLittleEndian;

public:
  XtensaAsmBackend(uint8_t osABI, bool isLE)
      : MCAsmBackend(llvm::endianness::little), OSABI(osABI),
        IsLittleEndian(isLE) {}

  MCFixupKindInfo getFixupKindInfo(MCFixupKind Kind) const override;
  std::optional<bool> evaluateFixup(const MCFragment &, MCFixup &, MCValue &,
                                    uint64_t &) override;
  void applyFixup(const MCFragment &, const MCFixup &, const MCValue &Target,
                  uint8_t *Data, uint64_t Value, bool IsResolved) override;
  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;
  bool finishLayout() const override;
  bool mayNeedRelaxation(unsigned Opcode, ArrayRef<MCOperand> Operands,
                         const MCSubtargetInfo &STI) const override;
  bool fixupNeedsRelaxationAdvanced(const MCFragment &F, const MCFixup &Fixup,
                                    const MCValue &Target, uint64_t Value,
                                    bool Resolved) const override;
  void relaxInstruction(MCInst &Inst,
                        const MCSubtargetInfo &STI) const override;

  std::unique_ptr<MCObjectTargetWriter> createObjectTargetWriter() const override {
    return createXtensaObjectWriter(OSABI, IsLittleEndian);
  }
};
} // namespace

MCFixupKindInfo XtensaAsmBackend::getFixupKindInfo(MCFixupKind Kind) const {
  const static MCFixupKindInfo Infos[Xtensa::NumTargetFixupKinds] = {
      // name                     offset bits  flags
      {"fixup_xtensa_branch_6", 0, 16, 0},
      {"fixup_xtensa_branch_8", 16, 8, 0},
      {"fixup_xtensa_branch_12", 12, 12, 0},
      {"fixup_xtensa_jump_18", 6, 18, 0},
      {"fixup_xtensa_call_18", 6, 18, 0},
      {"fixup_xtensa_l32r_16", 8, 16, 0},
      {"fixup_xtensa_loop_8", 16, 8, 0},
      {"fixup_xtensa_slot0", 0, 32, 0},
      {"fixup_xtensa_slot1", 0, 32, 0},
      {"fixup_xtensa_slot2", 0, 32, 0},
      {"fixup_xtensa_slot3", 0, 32, 0},
      {"fixup_xtensa_slot4", 0, 32, 0},
      {"fixup_xtensa_slot5", 0, 32, 0},
      {"fixup_xtensa_slot6", 0, 32, 0},
      {"fixup_xtensa_slot7", 0, 32, 0},
      {"fixup_xtensa_slot8", 0, 32, 0},
      {"fixup_xtensa_slot9", 0, 32, 0},
      {"fixup_xtensa_slot10", 0, 32, 0},
      {"fixup_xtensa_slot11", 0, 32, 0},
      {"fixup_xtensa_slot12", 0, 32, 0},
      {"fixup_xtensa_slot13", 0, 32, 0},
      {"fixup_xtensa_slot14", 0, 32, 0},
  };

  if (Kind < FirstTargetFixupKind)
    return MCAsmBackend::getFixupKindInfo(Kind);
  assert(unsigned(Kind - FirstTargetFixupKind) < Xtensa::NumTargetFixupKinds &&
         "Invalid kind!");
  return Infos[Kind - FirstTargetFixupKind];
}

static uint64_t adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                                 MCContext &Ctx, bool IsResolved) {
  unsigned Kind = Fixup.getKind();
  switch (Kind) {
  default:
    llvm_unreachable("Unknown fixup kind!");
  case Xtensa::fixup_xtensa_slot0:
  case Xtensa::fixup_xtensa_slot1:
  case Xtensa::fixup_xtensa_slot2:
  case Xtensa::fixup_xtensa_slot3:
  case Xtensa::fixup_xtensa_slot4:
  case Xtensa::fixup_xtensa_slot5:
  case Xtensa::fixup_xtensa_slot6:
  case Xtensa::fixup_xtensa_slot7:
  case Xtensa::fixup_xtensa_slot8:
  case Xtensa::fixup_xtensa_slot9:
  case Xtensa::fixup_xtensa_slot10:
  case Xtensa::fixup_xtensa_slot11:
  case Xtensa::fixup_xtensa_slot12:
  case Xtensa::fixup_xtensa_slot13:
  case Xtensa::fixup_xtensa_slot14:
    return 0;
  case FK_Data_1:
  case FK_Data_2:
  case FK_Data_4:
  case FK_Data_8:
    return Value;
  case Xtensa::fixup_xtensa_branch_6: {
    if (!Value)
      return 0;
    Value -= 4;
    if (IsResolved && !isUInt<6>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value " + Twine(Value) + " out of range for branch_6! Opcode is unknown");
    unsigned Hi2 = (Value >> 4) & 0x3;
    unsigned Lo4 = Value & 0xf;
    return (Hi2 << 4) | (Lo4 << 12);
  }
  case Xtensa::fixup_xtensa_branch_8:
    Value -= 4;
    if (IsResolved && !isInt<8>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value " + Twine(Value) + " out of range for branch_8");
    return (Value & 0xff);
  case Xtensa::fixup_xtensa_branch_12:
    Value -= 4;
    if (IsResolved && !isInt<12>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value " + Twine(Value) + " out of range for branch_12");
    return (Value & 0xfff);
  case Xtensa::fixup_xtensa_jump_18:
    Value -= 4;
    if (IsResolved && !isInt<18>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value " + Twine(Value) + " out of range for jump_18");
    return (Value & 0x3ffff);
  case Xtensa::fixup_xtensa_call_18:
    Value -= 4;
    if (IsResolved && !isInt<20>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value " + Twine(Value) + " out of range for call_18");
    if (IsResolved && (Value & 0x3))
      Ctx.reportError(Fixup.getLoc(), "fixup value must be 4-byte aligned");
    return (Value & 0xffffc) >> 2;
  case Xtensa::fixup_xtensa_loop_8:
    Value -= 4;
    if (IsResolved && !isUInt<8>(Value))
      Ctx.reportError(Fixup.getLoc(), "loop fixup value " + Twine(Value) + " out of range");
    return (Value & 0xff);
  case Xtensa::fixup_xtensa_l32r_16: {
    if (!IsResolved)
      return 0;
    int64_t SVal = (int64_t)Value;
    if (IsResolved && (SVal > -4 || SVal < -262144))
      Ctx.reportError(Fixup.getLoc(), "fixup value " + Twine(SVal) + " out of range for l32r_16");
    return (SVal >> 2) & 0xffff;
  }
  }
}

static unsigned getSize(unsigned Kind) {
  switch (Kind) {
  default:
    return 3;
  case FK_Data_4:
    return 4;
  case Xtensa::fixup_xtensa_branch_6:
    return 2;
  }
}

std::optional<bool> XtensaAsmBackend::evaluateFixup(const MCFragment &F,
                                                    MCFixup &Fixup, MCValue &Target,
                                                    uint64_t &Value) {
  // For a few PC-relative fixups, offsets need to be aligned down. We
  switch (Fixup.getKind()) {
  case Xtensa::fixup_xtensa_slot0:
  case Xtensa::fixup_xtensa_slot1:
  case Xtensa::fixup_xtensa_slot2:
  case Xtensa::fixup_xtensa_slot3:
  case Xtensa::fixup_xtensa_slot4:
  case Xtensa::fixup_xtensa_slot5:
  case Xtensa::fixup_xtensa_slot6:
  case Xtensa::fixup_xtensa_slot7:
  case Xtensa::fixup_xtensa_slot8:
  case Xtensa::fixup_xtensa_slot9:
  case Xtensa::fixup_xtensa_slot10:
  case Xtensa::fixup_xtensa_slot11:
  case Xtensa::fixup_xtensa_slot12:
  case Xtensa::fixup_xtensa_slot13:
  case Xtensa::fixup_xtensa_slot14:
    return false;
  case Xtensa::fixup_xtensa_call_18:
    Value -= (Asm->getFragmentOffset(F) + Fixup.getOffset()) % 4;
    break;
  case Xtensa::fixup_xtensa_l32r_16: {
    if (!Target.isAbsolute() && Target.getAddSym() && !Target.getSubSym()) {
      const MCSymbol &Sym = *Target.getAddSym();
      if (Sym.isDefined() && Sym.isInSection() &&
          &Sym.getSection() == F.getParent()) {
        uint64_t TargetOffset = Asm->getSymbolOffset(Sym) + Target.getConstant();
        uint64_t SectionOffset = Asm->getFragmentOffset(F) + Fixup.getOffset();
        Value = TargetOffset - ((SectionOffset + 3) & ~3);
        return true;
      }
    }
    return false;
  }
  }
  return {};
}

void XtensaAsmBackend::applyFixup(const MCFragment &F, const MCFixup &Fixup,
                                  const MCValue &Target, uint8_t *Data,
                                  uint64_t OrigValue, bool IsResolved) {
  if (Fixup.getKind() == (MCFixupKind)Xtensa::fixup_xtensa_jump_18) {
    if (IsResolved) {
      int64_t RelOffset = (int64_t)OrigValue - 4;
      if (RelOffset == -1) {
        if (IsLittleEndian) {
          Data[0] = 0xf0;
          Data[1] = 0x20;
          Data[2] = 0x00;
        } else {
          Data[0] = 0x00;
          Data[1] = 0x20;
          Data[2] = 0xf0;
        }
        return;
      }
    }
  }

  bool WasResolved = IsResolved;
  uint64_t Value = OrigValue;
  if (Fixup.getKind() >= (MCFixupKind)Xtensa::fixup_xtensa_slot0 &&
      Fixup.getKind() <= (MCFixupKind)Xtensa::fixup_xtensa_slot14) {
    // IsResolved = false; // Intentionally commented out.
  }
  maybeAddReloc(F, Fixup, Target, Value, IsResolved);

  if (!WasResolved)
    OrigValue = 0;

  MCContext &Ctx = getContext();
  MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());

  Value = adjustFixupValue(Fixup, OrigValue, Ctx, WasResolved);

  // Shift the value into position.
  Value <<= Info.TargetOffset;

  if (!Value)
    return; // Doesn't change encoding.

  unsigned FullSize = getSize(Fixup.getKind());

  for (unsigned i = 0; i != FullSize; ++i) {
    Data[i] |= uint8_t((Value >> (i * 8)) & 0xff);
  }
}

bool XtensaAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                    const MCSubtargetInfo *STI) const {
  if (Count == 0) return true;
  if (Count == 1) {
    // If we reach here, it means we failed to widen an instruction before
    // a 1-byte padding boundary. Write a 0 byte, which will cause an
    // Illegal Instruction exception if executed, but keeps offsets correct.
    OS.write("\0", 1);
    return true;
  }

  uint64_t Num2s = 0;
  uint64_t Num3s = 0;

  if (Count % 3 == 0) {
    Num3s = Count / 3;
  } else if (Count % 3 == 2) {
    Num3s = (Count - 2) / 3;
    Num2s = 1;
  } else if (Count % 3 == 1) {
    Num3s = (Count - 4) / 3;
    Num2s = 2;
  }

  for (uint64_t i = 0; i != Num3s; ++i) {
    if (IsLittleEndian) {
      OS.write("\xf0\x20\x00", 3);
    } else {
      OS.write("\x00\x20\xf0", 3);
    }
  }
  for (uint64_t i = 0; i != Num2s; ++i) {
    if (IsLittleEndian) {
      OS.write("\x3d\xf0", 2);
    } else {
      OS.write("\xf0\x3d", 2);
    }
  }

  return true;
}

namespace llvm {
extern cl::opt<bool> Trampolines;
extern cl::opt<bool> AutoLitpools;
extern cl::opt<int> AutoLitpoolLimit;
}

static bool isUnconditionalBranchOrReturn(const MCFragment &F) {
  if (F.getKind() != MCFragment::FT_Relaxable)
    return false;
  unsigned Opcode = F.getInst().getOpcode();
  return Opcode == Xtensa::J || Opcode == Xtensa::JX ||
         Opcode == Xtensa::RET || Opcode == Xtensa::RETW ||
         Opcode == Xtensa::RET_N || Opcode == Xtensa::RETW_N;
}

static bool isTrampolineEnabled(const MCInst &Inst) {
  unsigned Flags = Inst.getFlags();
  if (Flags & Xtensa::XtensaJJumpTrampolinesEnabled)
    return true;
  if (Flags & Xtensa::XtensaJJumpTrampolinesDisabled)
    return false;
  return Trampolines;
}

static bool isAutoLitpoolsEnabled(const MCInst &Inst) {
  unsigned Flags = Inst.getFlags();
  if (Flags & Xtensa::XtensaL32RAutoLitpoolsEnabled)
    return true;
  if (Flags & Xtensa::XtensaL32RAutoLitpoolsDisabled)
    return false;
  return AutoLitpools;
}

static unsigned getInvertedBranchOpcode(unsigned Opcode) {
  switch (Opcode) {
  case Xtensa::BEQ: return Xtensa::BNE;
  case Xtensa::BNE: return Xtensa::BEQ;
  case Xtensa::BLT: return Xtensa::BGE;
  case Xtensa::BGE: return Xtensa::BLT;
  case Xtensa::BLTU: return Xtensa::BGEU;
  case Xtensa::BGEU: return Xtensa::BLTU;
  case Xtensa::BEQI: return Xtensa::BNEI;
  case Xtensa::BNEI: return Xtensa::BEQI;
  case Xtensa::BLTI: return Xtensa::BGEI;
  case Xtensa::BGEI: return Xtensa::BLTI;
  case Xtensa::BLTUI: return Xtensa::BGEUI;
  case Xtensa::BGEUI: return Xtensa::BLTUI;
  case Xtensa::BEQZ: return Xtensa::BNEZ;
  case Xtensa::BNEZ: return Xtensa::BEQZ;
  case Xtensa::BGEZ: return Xtensa::BLTZ;
  case Xtensa::BLTZ: return Xtensa::BGEZ;
  case Xtensa::BEQZ_N: return Xtensa::BNEZ_N;
  case Xtensa::BNEZ_N: return Xtensa::BEQZ_N;
  case Xtensa::BANY: return Xtensa::BNONE;
  case Xtensa::BNONE: return Xtensa::BANY;
  case Xtensa::BALL: return Xtensa::BNALL;
  case Xtensa::BNALL: return Xtensa::BALL;
  case Xtensa::BBC: return Xtensa::BBS;
  case Xtensa::BBS: return Xtensa::BBC;
  case Xtensa::BBCI: return Xtensa::BBSI;
  case Xtensa::BBSI: return Xtensa::BBCI;
  case Xtensa::BT: return Xtensa::BF;
  case Xtensa::BF: return Xtensa::BT;
  default: return 0;
  }
}

static unsigned getBundleBranchOpcode(const MCInst &BundleInst) {
  for (unsigned i = 0; i < BundleInst.getNumOperands(); ++i) {
    const MCOperand &Op = BundleInst.getOperand(i);
    if (Op.isInst()) {
      unsigned Opc = Op.getInst()->getOpcode();
      if (getInvertedBranchOpcode(Opc) != 0) {
        return Opc;
      }
    }
  }
  return 0;
}

static MCInst *getBundleBranchInst(MCInst &BundleInst) {
  for (unsigned i = 0; i < BundleInst.getNumOperands(); ++i) {
    MCOperand &Op = BundleInst.getOperand(i);
    if (Op.isInst()) {
      unsigned Opc = const_cast<MCInst *>(Op.getInst())->getOpcode();
      if (getInvertedBranchOpcode(Opc) != 0) {
        return const_cast<MCInst *>(Op.getInst());
      }
    }
  }
  return nullptr;
}

bool XtensaAsmBackend::mayNeedRelaxation(unsigned Opcode,
                                         ArrayRef<MCOperand> Operands,
                                         const MCSubtargetInfo &STI) const {
  bool Res = false;
  if (Opcode == Xtensa::BUNDLE) {
    for (unsigned i = 0; i < Operands.size(); ++i) {
      const MCOperand &Op = Operands[i];
      if (Op.isInst()) {
        unsigned SubOpc = Op.getInst()->getOpcode();
        if (SubOpc == Xtensa::J || SubOpc == Xtensa::L32R ||
            SubOpc == Xtensa::LOOP || SubOpc == Xtensa::LOOPNEZ || SubOpc == Xtensa::LOOPGTZ ||
            getInvertedBranchOpcode(SubOpc) != 0) {
          Res = true;
          break;
        }
      }
    }
  } else if (getInvertedBranchOpcode(Opcode) != 0) {
    Res = true;
  } else if (Opcode == Xtensa::LOOP || Opcode == Xtensa::LOOPNEZ || Opcode == Xtensa::LOOPGTZ) {
    Res = true;
  } else if (Opcode == Xtensa::J || Opcode == Xtensa::L32R) {
    Res = true;
  } else if (Opcode == Xtensa::BEQZ_N || Opcode == Xtensa::BNEZ_N) {
    Res = true;
  } else if (Opcode == Xtensa::MOV_N || Opcode == Xtensa::MOVI_N || Opcode == Xtensa::L32I_N) {
    Res = true;
  } else if (Opcode == Xtensa::CALL0 || Opcode == Xtensa::CALL4 ||
      Opcode == Xtensa::CALL8 || Opcode == Xtensa::CALL12 ||
      Opcode == Xtensa::CALLX0 || Opcode == Xtensa::CALLX4 ||
      Opcode == Xtensa::CALLX8 || Opcode == Xtensa::CALLX12) {
    Res = true;
  }
  // No debug prints in production
  return Res;
}

bool XtensaAsmBackend::fixupNeedsRelaxationAdvanced(const MCFragment &F,
                                                    const MCFixup &Fixup,
                                                    const MCValue &Target,
                                                    uint64_t Value,
                                                    bool Resolved) const {
  if (Fixup.getKind() == (MCFixupKind)Xtensa::fixup_xtensa_branch_6) {
    if (!Resolved)
      return true;
    int64_t SVal = (int64_t)Value;
    int64_t RelOffset = SVal - 4;
    // Use a conservative threshold (48 instead of 63) to prevent infinite
    // layout loops caused by interactions with alignment fragments.
    if (RelOffset >= 0 && RelOffset <= 48) {
      return false;
    }
    return true;
  }
  // fixup_xtensa_branch_8: used by all 2-register conditional branches
  // (BEQ, BNE, BLT, BGE, BLTU, BGEU, BEQI, BNEI, BLTI, BGEI, BLTUI, BGEUI,
  //  BANY, BNONE, BALL, BNALL, BBC, BBS, BBCI, BBSI, BT, BF).
  // These instructions have an 8-bit signed PC-relative offset field giving a
  // hardware range of -128..+127 bytes relative to the instruction end (i.e.
  // RelOffset = Value - 4 must fit in int8_t).
  // When out of range, return true so the MC relaxation engine triggers a
  // re-layout, which causes finishLayout()'s branch trampoline pass to insert
  // an inverted-branch + unconditional J trampoline, exactly as GAS does.
  if (Fixup.getKind() == (MCFixupKind)Xtensa::fixup_xtensa_branch_8) {
    if (!Resolved)
      return true;
    int64_t SVal = (int64_t)Value;
    int64_t RelOffset = SVal - 4;
    return !isInt<8>(RelOffset);
  }
  // fixup_xtensa_branch_12: used by BEQZ, BNEZ, BGEZ, BLTZ.
  // These have a 12-bit signed offset giving a range of -2048..+2047 bytes
  // relative to the instruction end.
  if (Fixup.getKind() == (MCFixupKind)Xtensa::fixup_xtensa_branch_12) {
    if (!Resolved)
      return true;
    int64_t SVal = (int64_t)Value;
    int64_t RelOffset = SVal - 4;
    return !isInt<12>(RelOffset);
  }
  return false;
}

void XtensaAsmBackend::relaxInstruction(MCInst &Inst,
                                        const MCSubtargetInfo &STI) const {
  unsigned Opcode = Inst.getOpcode();
  if (Opcode == Xtensa::BUNDLE) {
    for (unsigned i = 0; i < Inst.getNumOperands(); ++i) {
      MCOperand &Op = Inst.getOperand(i);
      if (Op.isInst()) {
        MCInst *SubInst = const_cast<MCInst *>(Op.getInst());
        relaxInstruction(*SubInst, STI);
      }
    }
    return;
  }
  if (Opcode == Xtensa::BEQZ_N) {
    Inst.setOpcode(Xtensa::BEQZ);
  } else if (Opcode == Xtensa::BNEZ_N) {
    Inst.setOpcode(Xtensa::BNEZ);
  } else if (Opcode == Xtensa::MOV_N) {
    Inst.setOpcode(Xtensa::OR);
    Inst.addOperand(Inst.getOperand(1)); // OR a11, a5, a5
  } else if (Opcode == Xtensa::MOVI_N) {
    Inst.setOpcode(Xtensa::MOVI);
  } else if (Opcode == Xtensa::L32I_N) {
    Inst.setOpcode(Xtensa::L32I);
  } else if (Opcode == Xtensa::CALL0 || Opcode == Xtensa::CALL4 ||
             Opcode == Xtensa::CALL8 || Opcode == Xtensa::CALL12 ||
             Opcode == Xtensa::CALLX0 || Opcode == Xtensa::CALLX4 ||
             Opcode == Xtensa::CALLX8 || Opcode == Xtensa::CALLX12) {
    // CALL instructions don't actually change opcode when relaxing, they just trigger the widening pass
  } else if (getInvertedBranchOpcode(Opcode) != 0) {
    // Conditional branches with fixup_xtensa_branch_8 or fixup_xtensa_branch_12
    // that are out of their encoding range are handled by finishLayout()'s branch
    // trampoline pass, which inserts an inverted-condition branch over an
    // unconditional J to the original target. This relaxInstruction call is a
    // deliberate no-op: we leave the opcode unchanged so the fragment's fixup
    // survives to the next layout iteration, at which point finishLayout() will
    // detect the out-of-range condition and insert the trampoline.
  } else {
    llvm_unreachable("Unexpected instruction to relax");
  }
}

bool XtensaAsmBackend::finishLayout() const {
  MCContext &Ctx = getContext();
  bool Changed = false;
  DenseSet<const MCFragment *> AutoLitpools;

  for (unsigned Iter = 0; Iter < 1000; ++Iter) {
    bool IterChanged = false;
    DenseSet<const MCFragment *> LabeledFragments;
    for (const MCSymbol &S : Asm->symbols()) {
      if (S.isDefined() && !S.isVariable()) {
        if (const MCFragment *F = S.getFragment()) {
          LabeledFragments.insert(F);
        }
      }
    }

    for (MCSection &Sec : *Asm) {
      if (!Sec.isText() && !Sec.getName().starts_with(".text") && !Sec.getName().starts_with(".cold"))
        continue;

      // ---- Target Alignment Widening Pass ----
      SmallVector<MCFragment *, 8> PrevFragments;
      for (MCSection::iterator I = Sec.begin(), E = Sec.end(); I != E; ++I) {
        MCFragment &F = *I;
        
        if (F.getKind() == MCFragment::FT_Relaxable) {
           unsigned Opcode = F.getInst().getOpcode();
           if (Opcode == Xtensa::MOV_N || Opcode == Xtensa::MOVI_N || Opcode == Xtensa::L32I_N || Opcode == Xtensa::BEQZ_N || Opcode == Xtensa::BNEZ_N || Opcode == Xtensa::RET_N || Opcode == Xtensa::RETW_N) {
             PrevFragments.push_back(&F);
           } else if (Opcode == Xtensa::CALL0 || Opcode == Xtensa::CALL4 ||
                      Opcode == Xtensa::CALL8 || Opcode == Xtensa::CALL12 ||
                      Opcode == Xtensa::CALLX0 || Opcode == Xtensa::CALLX4 ||
                      Opcode == Xtensa::CALLX8 || Opcode == Xtensa::CALLX12) {
             uint64_t Offset = Asm->getFragmentOffset(F);
             // Xtensa hardware requires CALL instructions to NOT cross a 4-byte
             // fetch boundary. Since CALL is 3 bytes, it must be placed at offset
             // 4n or 4n+1. GAS specifically aligns return addresses to 4-byte boundaries,
             // which means the CALL must end at 4n, so it must start at 4n+1.
             // We align it to 4n+1 to perfectly match GAS output density.
             uint64_t Mod = Offset % 4;
             uint64_t Padding = 0;
             if (Mod != 1) {
               Padding = (5 - Mod) % 4;
             }
             
             if (Padding > 0) {
               uint64_t BytesToWiden = Padding;
               while (BytesToWiden > 0 && !PrevFragments.empty()) {
                 MCFragment *PF = PrevFragments.pop_back_val();
                 MCInst Inst = PF->getInst();
                 if (Inst.getOpcode() == Xtensa::MOV_N) {
                   Inst.setOpcode(Xtensa::OR);
                   Inst.addOperand(Inst.getOperand(1));
                 } else if (Inst.getOpcode() == Xtensa::MOVI_N) { Inst.setOpcode(Xtensa::MOVI); }
                 else if (Inst.getOpcode() == Xtensa::L32I_N) { Inst.setOpcode(Xtensa::L32I); }
                 else if (Inst.getOpcode() == Xtensa::BEQZ_N) { Inst.setOpcode(Xtensa::BEQZ); }
                 else if (Inst.getOpcode() == Xtensa::BNEZ_N) { Inst.setOpcode(Xtensa::BNEZ); }
                 else if (Inst.getOpcode() == Xtensa::RET_N) { Inst.setOpcode(Xtensa::RET); }
                 else if (Inst.getOpcode() == Xtensa::RETW_N) { Inst.setOpcode(Xtensa::RETW); }
                 PF->setInst(Inst);
                 
                 SmallVector<char, 16> Data;
                 SmallVector<MCFixup, 1> Fixups;
                 Asm->getEmitter().encodeInstruction(Inst, Data, Fixups, *PF->getSubtargetInfo());
                 PF->setVarContents(Data);
                 PF->setVarFixups(Fixups);
                 
                 BytesToWiden -= 1; // All these narrow->wide expansions are 1 byte
                 IterChanged = true;
                 Changed = true;
               }
             }
             PrevFragments.clear();
           }
        } else if (F.getKind() == MCFragment::FT_Align) {
           uint64_t Offset = Asm->getFragmentOffset(F);
           uint64_t Alignment = F.getAlignment().value();
           uint64_t Padding = (Alignment - (Offset % Alignment)) % Alignment;
           
           if (Padding == 1 && !PrevFragments.empty()) {
                 MCFragment *PF = PrevFragments.pop_back_val();
                 MCInst Inst = PF->getInst();
                 if (Inst.getOpcode() == Xtensa::MOV_N) {
                   Inst.setOpcode(Xtensa::OR);
                   Inst.addOperand(Inst.getOperand(1));
                 } else if (Inst.getOpcode() == Xtensa::MOVI_N) { Inst.setOpcode(Xtensa::MOVI); }
                 else if (Inst.getOpcode() == Xtensa::L32I_N) { Inst.setOpcode(Xtensa::L32I); }
                 else if (Inst.getOpcode() == Xtensa::BEQZ_N) { Inst.setOpcode(Xtensa::BEQZ); }
                 else if (Inst.getOpcode() == Xtensa::BNEZ_N) { Inst.setOpcode(Xtensa::BNEZ); }
                 else if (Inst.getOpcode() == Xtensa::RET_N) { Inst.setOpcode(Xtensa::RET); }
                 else if (Inst.getOpcode() == Xtensa::RETW_N) { Inst.setOpcode(Xtensa::RETW); }
                 PF->setInst(Inst);
                 
                 SmallVector<char, 16> Data;
                 SmallVector<MCFixup, 1> Fixups;
                 Asm->getEmitter().encodeInstruction(Inst, Data, Fixups, *PF->getSubtargetInfo());
                 PF->setVarContents(Data);
                 PF->setVarFixups(Fixups);
                 IterChanged = true;
                 Changed = true;
           }
           PrevFragments.clear();
        }
      }

      if (!IterChanged) {
        // ---- Loop Relaxation Pass ----
        for (MCSection::iterator I = Sec.begin(), E = Sec.end(); I != E; ++I) {
          MCFragment &F = *I;
          if (F.getKind() == MCFragment::FT_Relaxable) {
            MCInst Inst = F.getInst();
            unsigned Opcode = Inst.getOpcode();
            const MCInst *RealInst = &Inst;
            if (Opcode == Xtensa::BUNDLE) {
              for (unsigned i = 0; i < Inst.getNumOperands(); ++i) {
                const MCOperand &Op = Inst.getOperand(i);
                if (Op.isInst()) {
                  unsigned SubOpc = Op.getInst()->getOpcode();
                  if (SubOpc == Xtensa::LOOP || SubOpc == Xtensa::LOOPNEZ || SubOpc == Xtensa::LOOPGTZ) {
                    RealInst = Op.getInst();
                    Opcode = SubOpc;
                    break;
                  }
                }
              }
            }

            if (Opcode == Xtensa::LOOP || Opcode == Xtensa::LOOPNEZ || Opcode == Xtensa::LOOPGTZ) {
              auto Fixups = F.getVarFixups();
              if (!Fixups.empty()) {
                const MCFixup &Fixup = Fixups[0];
                const MCExpr *Expr = Fixup.getValue();
                if (Expr) {
                  MCValue TargetVal;
                  if (Expr->evaluateAsRelocatable(TargetVal, Asm)) {
                    const MCSymbol *TargetSym = TargetVal.getAddSym();
                    if (TargetSym && TargetSym->isDefined() && TargetSym->getFragment()) {
                      if (TargetSym->getFragment()->getParent() == &Sec) {
                        uint64_t TargetOffset = Asm->getSymbolOffset(*TargetSym) + TargetVal.getConstant();
                        uint64_t SourceOffset = Asm->getFragmentOffset(F) + Fixup.getOffset();
                        int64_t OffsetVal = (int64_t)TargetOffset - (int64_t)SourceOffset;
                        int64_t RelOffset = OffsetVal - 4;

                        if (RelOffset < 0 || RelOffset > 255) {
                          unsigned Reg = RealInst->getOperand(0).getReg();
                          unsigned RegNum = Ctx.getRegisterInfo()->getEncodingValue(Reg);

                          uint8_t LoopByte0 = 0x76;
                          uint8_t LoopByte1 = 0x80 | RegNum;
                          if (Opcode == Xtensa::LOOPNEZ) {
                            LoopByte1 = 0x90 | RegNum;
                          } else if (Opcode == Xtensa::LOOPGTZ) {
                            LoopByte1 = 0xa0 | RegNum;
                          }
                          uint8_t LoopByte2 = 20;

                          int64_t TargetRelOffset = OffsetVal - 24;
                          int64_t Low = TargetRelOffset & 0xff;
                          if (Low >= 128) Low -= 256;
                          int64_t High = (TargetRelOffset - Low) >> 8;

                          SmallVector<char, 24> Data;
                          Data.push_back(LoopByte0);
                          Data.push_back(LoopByte1);
                          Data.push_back(LoopByte2);

                          Data.push_back(RegNum << 4);
                          Data.push_back(1);
                          Data.push_back(3);

                          Data.push_back(RegNum << 4);
                          Data.push_back(0);
                          Data.push_back(0x13);

                          Data.push_back(RegNum << 4);
                          Data.push_back(0);
                          Data.push_back(3);

                          Data.push_back((RegNum << 4) | 2);
                          Data.push_back(0xd0 | RegNum);
                          Data.push_back(High);

                          Data.push_back((RegNum << 4) | 2);
                          Data.push_back(0xc0 | RegNum);
                          Data.push_back(Low);

                          Data.push_back(RegNum << 4);
                          Data.push_back(1);
                          Data.push_back(0x13);

                          Data.push_back(0);
                          Data.push_back(0x20);
                          Data.push_back(0);

                          F.setVarContents(Data);
                          SmallVector<MCFixup, 1> EmptyFixups;
                          F.setVarFixups(EmptyFixups);

                          const_cast<MCAssembler *>(Asm)->layoutSection(Sec);
                          IterChanged = true;
                          Changed = true;
                          break;
                        }
                      }
                    }
                  }
                }
              }
            }
          } else if (F.getKind() == MCFragment::FT_Data) {
            MCFragment &DF = F;
            auto Fixups = DF.getFixups();
            for (size_t fixup_idx = 0; fixup_idx < Fixups.size(); ++fixup_idx) {
              MCFixup Fixup = Fixups[fixup_idx];
              unsigned FixupKind = Fixup.getKind();
              if (FixupKind == (MCFixupKind)Xtensa::fixup_xtensa_loop_8) {
                uint32_t FixupOffset = Fixup.getOffset();
                size_t InstOffset = FixupOffset;
                if (InstOffset + 3 <= DF.getContents().size()) {
                  uint8_t Byte0 = DF.getContents()[InstOffset];
                  uint8_t Byte1 = DF.getContents()[InstOffset + 1];
                  unsigned s = Byte1 & 0x0f;
                  unsigned r = Byte1 >> 4;
                  if (Byte0 == 0x76 && (r == 8 || r == 9 || r == 10)) {
                    const MCExpr *Expr = Fixup.getValue();
                    if (Expr) {
                      MCValue TargetVal;
                      if (Expr->evaluateAsRelocatable(TargetVal, Asm)) {
                        const MCSymbol *TargetSym = TargetVal.getAddSym();
                        if (TargetSym && TargetSym->isDefined() && TargetSym->getFragment()) {
                          if (TargetSym->getFragment()->getParent() == &Sec) {
                            uint64_t TargetOffset = Asm->getSymbolOffset(*TargetSym) + TargetVal.getConstant();
                            uint64_t SourceOffset = Asm->getFragmentOffset(DF) + InstOffset;
                            int64_t OffsetVal = (int64_t)TargetOffset - (int64_t)SourceOffset;
                            int64_t RelOffset = OffsetVal - 4;

                            if (RelOffset < 0 || RelOffset > 255) {
                              unsigned RegNum = s;

                              uint8_t LoopByte0 = 0x76;
                              uint8_t LoopByte1 = Byte1;
                              uint8_t LoopByte2 = 20;

                              int64_t TargetRelOffset = OffsetVal - 24;
                              int64_t Low = TargetRelOffset & 0xff;
                              if (Low >= 128) Low -= 256;
                              int64_t High = (TargetRelOffset - Low) >> 8;

                              SmallVector<char, 24> RelaxedData;
                              RelaxedData.push_back(LoopByte0);
                              RelaxedData.push_back(LoopByte1);
                              RelaxedData.push_back(LoopByte2);

                              RelaxedData.push_back(RegNum << 4);
                              RelaxedData.push_back(1);
                              RelaxedData.push_back(3);

                              RelaxedData.push_back(RegNum << 4);
                              RelaxedData.push_back(0);
                              RelaxedData.push_back(0x13);

                              RelaxedData.push_back(RegNum << 4);
                              RelaxedData.push_back(0);
                              RelaxedData.push_back(3);

                              RelaxedData.push_back((RegNum << 4) | 2);
                              RelaxedData.push_back(0xd0 | RegNum);
                              RelaxedData.push_back(High);

                              RelaxedData.push_back((RegNum << 4) | 2);
                              RelaxedData.push_back(0xc0 | RegNum);
                              RelaxedData.push_back(Low);

                              RelaxedData.push_back(RegNum << 4);
                              RelaxedData.push_back(1);
                              RelaxedData.push_back(0x13);

                              RelaxedData.push_back(0);
                              RelaxedData.push_back(0x20);
                              RelaxedData.push_back(0);

                              SmallVector<MCFixup, 8> NewFixedFixups;
                              SmallVector<MCFixup, 8> NewVarFixups;

                              auto AdjustAndAddFixup = [&](MCFixup Fxp) {
                                uint32_t Offset = Fxp.getOffset();
                                if (Fxp.getKind() == (MCFixupKind)Xtensa::fixup_xtensa_loop_8 && Offset == FixupOffset) {
                                  return;
                                }
                                if (Offset > FixupOffset) {
                                  Offset += 21;
                                }
                                Fxp.setOffset(Offset);

                                if (Offset < InstOffset) {
                                  NewFixedFixups.push_back(Fxp);
                                } else {
                                  Fxp.setOffset(Offset - InstOffset);
                                  NewVarFixups.push_back(Fxp);
                                }
                              };

                              for (const MCFixup &Fxp : DF.getFixups()) {
                                AdjustAndAddFixup(Fxp);
                              }
                              for (const MCFixup &Fxp : DF.getVarFixups()) {
                                AdjustAndAddFixup(Fxp);
                              }

                              SmallVector<char, 128> NewVarContents;
                              NewVarContents.append(RelaxedData.begin(), RelaxedData.end());

                              ArrayRef<char> FixedContents = DF.getContents();
                              if (FixedContents.size() > InstOffset + 3) {
                                NewVarContents.append(FixedContents.begin() + InstOffset + 3, FixedContents.end());
                              }

                              ArrayRef<char> OldVarContents = DF.getVarContents();
                              NewVarContents.append(OldVarContents.begin(), OldVarContents.end());

                              DF.setFixedSize(InstOffset);

                              DF.clearFixups();
                              DF.clearVarFixups();

                              DF.setFixupStart(DF.getParent()->getFixupStorage().size());
                              DF.setFixupEnd(DF.getParent()->getFixupStorage().size());

                              DF.appendFixups(NewFixedFixups);
                              DF.setVarContents(NewVarContents);
                              DF.setVarFixups(NewVarFixups);

                              const_cast<MCAssembler *>(Asm)->layoutSection(Sec);
                              IterChanged = true;
                              Changed = true;
                              break;
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
              if (IterChanged)
                break;
            }
          }
          if (IterChanged)
            break;
        }
      }
      if (!IterChanged) {
        // ---- Branch Trampoline Relaxation Pass ----
        for (MCSection::iterator I = Sec.begin(), E = Sec.end(); I != E; ++I) {
          MCFragment &F = *I;
          if (F.getKind() != MCFragment::FT_Relaxable)
            continue;

          MCInst Inst = F.getInst();
          unsigned Opc = Inst.getOpcode();
          if (Opc == Xtensa::BUNDLE) {
            Opc = getBundleBranchOpcode(Inst);
          }
          unsigned InvOpc = getInvertedBranchOpcode(Opc);
          if (InvOpc == 0)
            continue;


          auto Fixups = F.getVarFixups();
          if (Fixups.empty())
            continue;

          const MCFixup &Fixup = Fixups[0];
          const MCExpr *Expr = Fixup.getValue();
          if (!Expr)
            continue;

          MCValue TargetVal;
          if (!Expr->evaluateAsRelocatable(TargetVal, Asm))
            continue;

          const MCSymbol *TargetSym = TargetVal.getAddSym();
          if (!TargetSym || !TargetSym->isDefined() || !TargetSym->getFragment())
            continue;

          if (TargetSym->getFragment()->getParent() != &Sec)
            continue;

          uint64_t TargetOffset = Asm->getSymbolOffset(*TargetSym) + TargetVal.getConstant();
          uint64_t SourceOffset = Asm->getFragmentOffset(F) + Fixup.getOffset();
          int64_t OffsetVal = (int64_t)TargetOffset - (int64_t)SourceOffset;
          int64_t RelOffset = OffsetVal - 4;

          bool OutOfRange = false;
          if (Opc == Xtensa::BEQZ || Opc == Xtensa::BNEZ || Opc == Xtensa::BGEZ || Opc == Xtensa::BLTZ) {
            OutOfRange = (RelOffset < -2000 || RelOffset > 2000);
          } else if (Opc == Xtensa::BEQZ_N || Opc == Xtensa::BNEZ_N) {
            OutOfRange = (RelOffset < 0 || RelOffset > 46);
          } else {
            OutOfRange = (RelOffset < -120 || RelOffset > 120);
          }

          if (!OutOfRange)
            continue;

          const MCSubtargetInfo &STI = *F.getSubtargetInfo();
          MCSymbol *ResumeSym = Ctx.createTempSymbol();

          MCFragment *Tramp = new (Ctx.allocate(sizeof(MCFragment))) MCFragment(MCFragment::FT_Relaxable);
          Tramp->setHasInstructions(STI);
          Tramp->setParent(&Sec);

          MCInst TrampInst;
          TrampInst.setOpcode(Xtensa::J);
          TrampInst.addOperand(MCOperand::createExpr(MCSymbolRefExpr::create(TargetSym, Ctx)));
          TrampInst.setFlags(Xtensa::XtensaJJumpTrampolinesEnabled);

          SmallVector<char, 16> DataTramp;
          SmallVector<MCFixup, 1> FixupsTramp;
          Asm->getEmitter().encodeInstruction(TrampInst, DataTramp, FixupsTramp, STI);
          Tramp->setVarContents(DataTramp);
          Tramp->setInst(TrampInst);
          Tramp->setVarFixups(FixupsTramp);

          MCFragment *Resume = new (Ctx.allocate(sizeof(MCFragment))) MCFragment();
          Resume->setParent(&Sec);

          Resume->setNext(F.getNext());
          Tramp->setNext(Resume);
          F.setNext(Tramp);

          if (Sec.curFragList()->Tail == &F) {
            Sec.curFragList()->Tail = Resume;
          }

          ResumeSym->setFragment(Resume);
          ResumeSym->setOffset(0);
          const_cast<MCAssembler *>(Asm)->registerSymbol(*ResumeSym);

          unsigned LayoutOrder = 0;
          for (MCFragment &Frag : Sec) {
            Frag.setLayoutOrder(LayoutOrder++);
          }

          MCInst NewInst = F.getInst();
          if (Inst.getOpcode() == Xtensa::BUNDLE) {
            MCInst *SubInst = getBundleBranchInst(NewInst);
            SubInst->setOpcode(InvOpc);
            unsigned TargetOpIdx = SubInst->getNumOperands() - 1;
            SubInst->getOperand(TargetOpIdx).setExpr(MCSymbolRefExpr::create(ResumeSym, Ctx));
          } else {
            NewInst.setOpcode(InvOpc);
            unsigned TargetOpIdx = NewInst.getNumOperands() - 1;
            NewInst.getOperand(TargetOpIdx).setExpr(MCSymbolRefExpr::create(ResumeSym, Ctx));
          }
          F.setInst(NewInst);

          SmallVector<char, 16> DataF;
          SmallVector<MCFixup, 1> FixupsF;
          Asm->getEmitter().encodeInstruction(NewInst, DataF, FixupsF, STI);
          F.setVarContents(DataF);
          F.setVarFixups(FixupsF);

          const_cast<MCAssembler *>(Asm)->layoutSection(Sec);

          IterChanged = true;
          Changed = true;
          break;
        }
      }

      for (MCSection::iterator I = Sec.begin(), E = Sec.end(); I != E; ++I) {
        MCFragment &F = *I;
        if (F.getKind() != MCFragment::FT_Relaxable)
          continue;

        MCInst Inst = F.getInst();
        if (Inst.getOpcode() != Xtensa::J)
          continue;

        LLVM_DEBUG(dbgs() << "DEBUG: Found J instruction at offset "
                          << Asm->getFragmentOffset(F) << "\n");

        if (!isTrampolineEnabled(Inst)) {
          LLVM_DEBUG(dbgs() << "DEBUG: Trampolines disabled for this jump\n");
          continue;
        }

        auto Fixups = F.getVarFixups();
        if (Fixups.empty()) {
          LLVM_DEBUG(dbgs() << "DEBUG: Fixups empty\n");
          continue;
        }

        const MCFixup &Fixup = Fixups[0];
        LLVM_DEBUG(dbgs() << "DEBUG: Fixup kind = " << (unsigned)Fixup.getKind() << "\n");
        if (Fixup.getKind() != (MCFixupKind)Xtensa::fixup_xtensa_jump_18)
          continue;

        const MCExpr *Expr = Fixup.getValue();
        if (!Expr)
          continue;

        MCValue TargetVal;
        if (!Expr->evaluateAsRelocatable(TargetVal, Asm)) {
          LLVM_DEBUG(dbgs() << "DEBUG: Failed to evaluate expression as relocatable\n");
          continue;
        }

        const MCSymbol *TargetSym = TargetVal.getAddSym();
        if (!TargetSym) {
          LLVM_DEBUG(dbgs() << "DEBUG: TargetSym is null\n");
          continue;
        }

        LLVM_DEBUG(dbgs() << "DEBUG: TargetSym = " << TargetSym->getName() << "\n");
        if (!TargetSym->isDefined()) {
          LLVM_DEBUG(dbgs() << "DEBUG: TargetSym is undefined\n");
          continue;
        }

        if (!TargetSym->getFragment()) {
          LLVM_DEBUG(dbgs() << "DEBUG: TargetSym has no fragment\n");
          continue;
        }
        if (TargetSym->getFragment()->getParent() != &Sec) {
          LLVM_DEBUG(dbgs() << "DEBUG: TargetSym parent section is different\n");
          continue;
        }

        uint64_t TargetOffset = Asm->getSymbolOffset(*TargetSym) + TargetVal.getConstant();
        uint64_t SourceOffset = Asm->getFragmentOffset(F) + Fixup.getOffset();
        int64_t OffsetVal = (int64_t)TargetOffset - (int64_t)SourceOffset;
        int64_t RelOffset = OffsetVal - 4;

        LLVM_DEBUG(dbgs() << "DEBUG: TargetOffset=" << TargetOffset << " SourceOffset=" << SourceOffset << " RelOffset=" << RelOffset << "\n");

        if (isInt<18>(RelOffset)) {
          LLVM_DEBUG(dbgs() << "DEBUG: Jump is in range\n");
          continue;
        }

        LLVM_DEBUG(dbgs() << "DEBUG: Out of range jump found!\n");

        bool Forward = (RelOffset > 0);
        MCFragment *TrampolineInsertAfter = nullptr;
        bool UseUnreachable = false;

        if (Forward) {
          uint64_t FOffset = Asm->getFragmentOffset(F);
          MCFragment *G = &F;
          MCFragment *FurthestUnreachable = nullptr;
          MCFragment *FurthestPossible = &F;

          while (G->getNext()) {
            MCFragment *H = G->getNext();
            uint64_t HOffset = Asm->getFragmentOffset(*H);
            if (HOffset - FOffset > 120000)
              break;

            if (isUnconditionalBranchOrReturn(*G) && !LabeledFragments.count(H)) {
              FurthestUnreachable = G;
            }
            FurthestPossible = H;
            G = H;
          }

          if (FurthestUnreachable) {
            uint64_t UnreachOffset = Asm->getFragmentOffset(*FurthestUnreachable);
            int64_t DistToTarget = (int64_t)TargetOffset - (int64_t)UnreachOffset;
            if ((DistToTarget < 120000 && DistToTarget > -120000) || (UnreachOffset - FOffset) > 80000) {
              TrampolineInsertAfter = FurthestUnreachable;
              UseUnreachable = true;
            }
          }

          if (!TrampolineInsertAfter) {
            TrampolineInsertAfter = FurthestPossible;
            UseUnreachable = false;
          }
        } else {
          uint64_t FOffset = Asm->getFragmentOffset(F);
          MCFragment *Curr = Sec.curFragList()->Head;
          MCFragment *FurthestUnreachable = nullptr;
          MCFragment *FurthestPossible = nullptr;

          while (Curr && Curr != &F) {
            uint64_t CurrOffset = Asm->getFragmentOffset(*Curr);
            if (FOffset - CurrOffset < 120000) {
              if (!FurthestPossible) {
                FurthestPossible = Curr;
              }
              MCFragment *Next = Curr->getNext();
              if (Next && Next != &F) {
                if (isUnconditionalBranchOrReturn(*Curr) && !LabeledFragments.count(Next)) {
                  if (!FurthestUnreachable) {
                    FurthestUnreachable = Curr;
                  }
                }
              }
            }
            Curr = Curr->getNext();
          }

          if (FurthestUnreachable) {
            uint64_t UnreachOffset = Asm->getFragmentOffset(*FurthestUnreachable);
            int64_t DistToTarget = (int64_t)UnreachOffset - (int64_t)TargetOffset;
            if ((DistToTarget < 120000 && DistToTarget > -120000) || (FOffset - UnreachOffset) > 80000) {
              TrampolineInsertAfter = FurthestUnreachable;
              UseUnreachable = true;
            }
          }

          if (!TrampolineInsertAfter) {
            TrampolineInsertAfter = FurthestPossible;
            UseUnreachable = false;
          }
        }

        if (!TrampolineInsertAfter) {
          TrampolineInsertAfter = &F;
        }

        MCSymbol *TrampSym = Ctx.createTempSymbol();
        const MCSubtargetInfo &STI = *F.getSubtargetInfo();

        if (UseUnreachable) {
          MCFragment *T = new (Ctx.allocate(sizeof(MCFragment))) MCFragment(MCFragment::FT_Relaxable);
          T->setHasInstructions(STI);
          T->setParent(&Sec);

          MCInst TrampInst;
          TrampInst.setOpcode(Xtensa::J);
          TrampInst.addOperand(MCOperand::createExpr(MCSymbolRefExpr::create(TargetSym, Ctx)));
          if (Inst.getFlags() & Xtensa::XtensaJJumpTrampolinesEnabled)
            TrampInst.setFlags(Xtensa::XtensaJJumpTrampolinesEnabled);
          else if (Inst.getFlags() & Xtensa::XtensaJJumpTrampolinesDisabled)
            TrampInst.setFlags(Xtensa::XtensaJJumpTrampolinesDisabled);

          SmallVector<char, 16> Data;
          SmallVector<MCFixup, 1> Fixups;
          Asm->getEmitter().encodeInstruction(TrampInst, Data, Fixups, STI);
          T->setVarContents(Data);
          T->setInst(TrampInst);
          T->setVarFixups(Fixups);

          T->setNext(TrampolineInsertAfter->getNext());
          TrampolineInsertAfter->setNext(T);
          if (Sec.curFragList()->Tail == TrampolineInsertAfter) {
            Sec.curFragList()->Tail = T;
          }

          TrampSym->setFragment(T);
          TrampSym->setOffset(0);
          const_cast<MCAssembler *>(Asm)->registerSymbol(*TrampSym);
        } else {
          MCSymbol *ResumeSym = Ctx.createTempSymbol();

          MCFragment *JmpResume = new (Ctx.allocate(sizeof(MCFragment))) MCFragment(MCFragment::FT_Relaxable);
          JmpResume->setHasInstructions(STI);
          JmpResume->setParent(&Sec);

          MCInst JmpResumeInst;
          JmpResumeInst.setOpcode(Xtensa::J);
          JmpResumeInst.addOperand(MCOperand::createExpr(MCSymbolRefExpr::create(ResumeSym, Ctx)));
          JmpResumeInst.setFlags(Xtensa::XtensaJJumpTrampolinesDisabled);

          SmallVector<char, 16> Data1;
          SmallVector<MCFixup, 1> Fixups1;
          Asm->getEmitter().encodeInstruction(JmpResumeInst, Data1, Fixups1, STI);
          JmpResume->setVarContents(Data1);
          JmpResume->setInst(JmpResumeInst);
          JmpResume->setVarFixups(Fixups1);

          MCFragment *Tramp = new (Ctx.allocate(sizeof(MCFragment))) MCFragment(MCFragment::FT_Relaxable);
          Tramp->setHasInstructions(STI);
          Tramp->setParent(&Sec);

          MCInst TrampInst;
          TrampInst.setOpcode(Xtensa::J);
          TrampInst.addOperand(MCOperand::createExpr(MCSymbolRefExpr::create(TargetSym, Ctx)));
          if (Inst.getFlags() & Xtensa::XtensaJJumpTrampolinesEnabled)
            TrampInst.setFlags(Xtensa::XtensaJJumpTrampolinesEnabled);
          else if (Inst.getFlags() & Xtensa::XtensaJJumpTrampolinesDisabled)
            TrampInst.setFlags(Xtensa::XtensaJJumpTrampolinesDisabled);

          SmallVector<char, 16> Data2;
          SmallVector<MCFixup, 1> Fixups2;
          Asm->getEmitter().encodeInstruction(TrampInst, Data2, Fixups2, STI);
          Tramp->setVarContents(Data2);
          Tramp->setInst(TrampInst);
          Tramp->setVarFixups(Fixups2);

          MCFragment *Resume = new (Ctx.allocate(sizeof(MCFragment))) MCFragment();
          Resume->setParent(&Sec);

          Resume->setNext(TrampolineInsertAfter->getNext());
          Tramp->setNext(Resume);
          JmpResume->setNext(Tramp);
          TrampolineInsertAfter->setNext(JmpResume);

          if (Sec.curFragList()->Tail == TrampolineInsertAfter) {
            Sec.curFragList()->Tail = Resume;
          }

          TrampSym->setFragment(Tramp);
          TrampSym->setOffset(0);
          const_cast<MCAssembler *>(Asm)->registerSymbol(*TrampSym);

          ResumeSym->setFragment(Resume);
          ResumeSym->setOffset(0);
          const_cast<MCAssembler *>(Asm)->registerSymbol(*ResumeSym);
        }

        unsigned LayoutOrder = 0;
        for (MCFragment &Frag : Sec) {
          Frag.setLayoutOrder(LayoutOrder++);
        }

        MCInst NewInst = F.getInst();
        NewInst.getOperand(0).setExpr(MCSymbolRefExpr::create(TrampSym, Ctx));
        F.setInst(NewInst);

        SmallVector<char, 16> DataF;
        SmallVector<MCFixup, 1> FixupsF;
        Asm->getEmitter().encodeInstruction(NewInst, DataF, FixupsF, STI);
        F.setVarContents(DataF);
        F.setVarFixups(FixupsF);

        const_cast<MCAssembler *>(Asm)->layoutSection(Sec);

        IterChanged = true;
        Changed = true;
        break;
      }
      if (IterChanged)
        break;
    }

    if (!IterChanged) {
      // Check automatic literal pools
      for (MCSection &Sec : *Asm) {
        if (!Sec.isText() && !Sec.getName().starts_with(".text") && !Sec.getName().starts_with(".cold"))
          continue;

        for (MCSection::iterator I = Sec.begin(), E = Sec.end(); I != E; ++I) {
          MCFragment &F = *I;
          if (F.getKind() != MCFragment::FT_Relaxable)
            continue;

          MCInst Inst = F.getInst();
          if (Inst.getOpcode() != Xtensa::L32R)
            continue;

          if (!isAutoLitpoolsEnabled(Inst))
            continue;

          auto Fixups = F.getVarFixups();
          if (Fixups.empty())
            continue;

          const MCFixup &Fixup = Fixups[0];
          if (Fixup.getKind() != (MCFixupKind)Xtensa::fixup_xtensa_l32r_16)
            continue;

          const MCExpr *Expr = Fixup.getValue();
          if (!Expr)
            continue;

          MCValue TargetVal;
          if (!Expr->evaluateAsRelocatable(TargetVal, Asm))
            continue;

          const MCSymbol *TargetSym = TargetVal.getAddSym();
          if (!TargetSym)
            continue;

          if (!TargetSym->isDefined() || !TargetSym->getFragment())
            continue;

          if (TargetSym->getFragment()->getParent() != &Sec)
            continue;

          uint64_t TargetOffset = Asm->getSymbolOffset(*TargetSym) + TargetVal.getConstant();
          uint64_t SourceOffset = Asm->getFragmentOffset(F) + Fixup.getOffset();
          uint64_t BaseOffset = (SourceOffset + 3) & ~3;
          int64_t RelOffset = (int64_t)TargetOffset - (int64_t)BaseOffset;

          int64_t Limit = AutoLitpoolLimit;
          if (RelOffset >= -Limit && RelOffset <= -4)
            continue;

          // Out of range L32R! Let's insert/reuse a literal pool.
          MCFragment *PrevFrag = nullptr;
          for (MCFragment &Frag : Sec) {
            if (Frag.getNext() == &F) {
              PrevFrag = &Frag;
              break;
            }
          }

          MCFragment *TrampolineInsertAfter = nullptr;
          bool UseUnreachable = false;

          uint64_t FOffset = Asm->getFragmentOffset(F);
          MCFragment *Curr = Sec.curFragList()->Head;
          MCFragment *FurthestUnreachable = nullptr;
          MCFragment *FurthestPossible = nullptr;

          while (Curr && Curr != &F) {
            uint64_t CurrOffset = Asm->getFragmentOffset(*Curr);
            int64_t Dist = (int64_t)FOffset - (int64_t)CurrOffset;
            if (Dist <= Limit - 100) {
              if (!FurthestPossible) {
                FurthestPossible = Curr;
              }
              MCFragment *Next = Curr->getNext();
              if (Next && Next != &F) {
                if (isUnconditionalBranchOrReturn(*Curr) && !LabeledFragments.count(Next)) {
                  if (!FurthestUnreachable) {
                    FurthestUnreachable = Curr;
                  }
                }
              }
            }
            Curr = Curr->getNext();
          }

          if (FurthestUnreachable) {
            TrampolineInsertAfter = FurthestUnreachable;
            UseUnreachable = true;
          } else if (FurthestPossible) {
            TrampolineInsertAfter = FurthestPossible;
            UseUnreachable = false;
          } else {
            TrampolineInsertAfter = PrevFrag;
            UseUnreachable = false;
          }
          if (!TrampolineInsertAfter) {
            TrampolineInsertAfter = &F;
          }



          MCFragment *PoolData = nullptr;
          MCFragment *Next = TrampolineInsertAfter->getNext();

          if (Next && Next->getKind() == MCFragment::FT_Align) {
            MCFragment *P = Next->getNext();
            if (P && AutoLitpools.count(P)) {
              PoolData = P;
            }
          } else if (Next && Next->getKind() == MCFragment::FT_Relaxable && Next->getInst().getOpcode() == Xtensa::J) {
            MCFragment *AlignFrag = Next->getNext();
            if (AlignFrag && AlignFrag->getKind() == MCFragment::FT_Align) {
              MCFragment *P = AlignFrag->getNext();
              if (P && AutoLitpools.count(P)) {
                PoolData = P;
              }
            }
          }

          if (PoolData) {
            ArrayRef<char> LitContents = TargetSym->getFragment()->getContents();
            uint64_t LitOffset = TargetSym->getOffset();
            StringRef LitBytes(LitContents.data() + LitOffset, 4);

            ArrayRef<char> CurrContents = PoolData->getVarContents();
            ArrayRef<MCFixup> CurrFixups = PoolData->getVarFixups();

            std::vector<char> NewContents(CurrContents.begin(), CurrContents.end());
            std::vector<MCFixup> NewFixups(CurrFixups.begin(), CurrFixups.end());

            uint64_t OldSize = NewContents.size();
            NewContents.insert(NewContents.end(), LitBytes.begin(), LitBytes.end());

            for (const MCFixup &Flf : TargetSym->getFragment()->getFixups()) {
              if (Flf.getOffset() >= LitOffset && Flf.getOffset() < LitOffset + 4) {
                MCFixup NewF = MCFixup::create(Flf.getOffset() - LitOffset + OldSize, Flf.getValue(), Flf.getKind(), Flf.isPCRel());
                NewFixups.push_back(NewF);
              }
            }

            PoolData->setVarContents(NewContents);
            PoolData->setVarFixups(NewFixups);

            MCSymbol *TrampSym = Ctx.createTempSymbol();
            TrampSym->setFragment(PoolData);
            TrampSym->setOffset(OldSize);
            const_cast<MCAssembler *>(Asm)->registerSymbol(*TrampSym);

            MCInst NewInst = F.getInst();
            NewInst.getOperand(1).setExpr(MCSymbolRefExpr::create(TrampSym, Ctx));
            F.setInst(NewInst);

            const MCSubtargetInfo &STI = *F.getSubtargetInfo();
            SmallVector<char, 16> DataF;
            SmallVector<MCFixup, 1> FixupsF;
            Asm->getEmitter().encodeInstruction(NewInst, DataF, FixupsF, STI);
            F.setVarContents(DataF);
            F.setVarFixups(FixupsF);
          } else {
            MCSymbol *TrampSym = Ctx.createTempSymbol();
            const MCSubtargetInfo &STI = *F.getSubtargetInfo();

            ArrayRef<char> LitContents = TargetSym->getFragment()->getContents();
            uint64_t LitOffset = TargetSym->getOffset();
            StringRef LitBytes(LitContents.data() + LitOffset, 4);

            std::vector<char> NewContents(LitBytes.begin(), LitBytes.end());
            std::vector<MCFixup> NewFixups;
            for (const MCFixup &Flf : TargetSym->getFragment()->getFixups()) {
              if (Flf.getOffset() >= LitOffset && Flf.getOffset() < LitOffset + 4) {
                MCFixup NewF = MCFixup::create(Flf.getOffset() - LitOffset, Flf.getValue(), Flf.getKind(), Flf.isPCRel());
                NewFixups.push_back(NewF);
              }
            }

            PoolData = new (Ctx.allocate(sizeof(MCFragment))) MCFragment(MCFragment::FT_Data);
            PoolData->setParent(&Sec);
            PoolData->setVarContents(NewContents);
            PoolData->setVarFixups(NewFixups);
            AutoLitpools.insert(PoolData);

            MCFragment *AlignFrag = new (Ctx.allocate(sizeof(MCFragment))) MCFragment(MCFragment::FT_Align);
            AlignFrag->setParent(&Sec);
            AlignFrag->makeAlign(Align(4), 0, 1, 4);
            Sec.ensureMinAlignment(Align(4));

            if (UseUnreachable) {
              PoolData->setNext(TrampolineInsertAfter->getNext());
              AlignFrag->setNext(PoolData);
              TrampolineInsertAfter->setNext(AlignFrag);

              if (Sec.curFragList()->Tail == TrampolineInsertAfter) {
                Sec.curFragList()->Tail = PoolData;
              }
            } else {
              MCSymbol *ResumeSym = Ctx.createTempSymbol();

              MCFragment *JmpResume = new (Ctx.allocate(sizeof(MCFragment))) MCFragment(MCFragment::FT_Relaxable);
              JmpResume->setHasInstructions(STI);
              JmpResume->setParent(&Sec);

              MCInst JmpResumeInst;
              JmpResumeInst.setOpcode(Xtensa::J);
              JmpResumeInst.addOperand(MCOperand::createExpr(MCSymbolRefExpr::create(ResumeSym, Ctx)));
              JmpResumeInst.setFlags(Xtensa::XtensaJJumpTrampolinesDisabled);

              SmallVector<char, 16> Data1;
              SmallVector<MCFixup, 1> Fixups1;
              Asm->getEmitter().encodeInstruction(JmpResumeInst, Data1, Fixups1, STI);
              JmpResume->setVarContents(Data1);
              JmpResume->setInst(JmpResumeInst);
              JmpResume->setVarFixups(Fixups1);

              MCFragment *Resume = new (Ctx.allocate(sizeof(MCFragment))) MCFragment();
              Resume->setParent(&Sec);

              Resume->setNext(TrampolineInsertAfter->getNext());
              PoolData->setNext(Resume);
              AlignFrag->setNext(PoolData);
              JmpResume->setNext(AlignFrag);
              TrampolineInsertAfter->setNext(JmpResume);

              if (Sec.curFragList()->Tail == TrampolineInsertAfter) {
                Sec.curFragList()->Tail = Resume;
              }

              ResumeSym->setFragment(Resume);
              ResumeSym->setOffset(0);
              const_cast<MCAssembler *>(Asm)->registerSymbol(*ResumeSym);
            }

            TrampSym->setFragment(PoolData);
            TrampSym->setOffset(0);
            const_cast<MCAssembler *>(Asm)->registerSymbol(*TrampSym);

            MCInst NewInst = F.getInst();
            NewInst.getOperand(1).setExpr(MCSymbolRefExpr::create(TrampSym, Ctx));
            F.setInst(NewInst);

            SmallVector<char, 16> DataF;
            SmallVector<MCFixup, 1> FixupsF;
            Asm->getEmitter().encodeInstruction(NewInst, DataF, FixupsF, STI);
            F.setVarContents(DataF);
            F.setVarFixups(FixupsF);
          }

          unsigned LayoutOrder = 0;
          for (MCFragment &Frag : Sec) {
            Frag.setLayoutOrder(LayoutOrder++);
          }

          const_cast<MCAssembler *>(Asm)->layoutSection(Sec);

          IterChanged = true;
          Changed = true;
          break;
        }
        if (IterChanged)
          break;
      }
    }

    if (!IterChanged)
      break;
  }
  return Changed;
}

MCAsmBackend *llvm::createXtensaAsmBackend(const Target &T,
                                           const MCSubtargetInfo &STI,
                                           const MCRegisterInfo &MRI,
                                           const MCTargetOptions &Options) {
  uint8_t OSABI =
      MCELFObjectTargetWriter::getOSABI(STI.getTargetTriple().getOS());
  return new XtensaAsmBackend(OSABI, true);
}
