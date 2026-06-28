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

bool XtensaAsmBackend::mayNeedRelaxation(unsigned Opcode,
                                         ArrayRef<MCOperand> Operands,
                                         const MCSubtargetInfo &STI) const {
  if (Opcode == Xtensa::BUNDLE) {
    return false;
  }
  if (Opcode == Xtensa::J || Opcode == Xtensa::L32R) {
    return true;
  }
  if (Opcode == Xtensa::BEQZ_N || Opcode == Xtensa::BNEZ_N) {
    return true;
  }
  if (Opcode == Xtensa::MOV_N || Opcode == Xtensa::MOVI_N || Opcode == Xtensa::L32I_N) {
    return true;
  }
  if (Opcode == Xtensa::CALL0 || Opcode == Xtensa::CALL4 ||
      Opcode == Xtensa::CALL8 || Opcode == Xtensa::CALL12 ||
      Opcode == Xtensa::CALLX0 || Opcode == Xtensa::CALLX4 ||
      Opcode == Xtensa::CALLX8 || Opcode == Xtensa::CALLX12) {
    return true;
  }
  return false;
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
  } else {
    llvm_unreachable("Unexpected instruction to relax");
  }
}

bool XtensaAsmBackend::finishLayout() const {
  MCContext &Ctx = getContext();
  bool Changed = false;
  DenseSet<const MCFragment *> AutoLitpools;

  for (unsigned Iter = 0; Iter < 100; ++Iter) {
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
      if (!Sec.isText())
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
        if (!Sec.isText())
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
