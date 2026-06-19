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
                                 MCContext &Ctx) {
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
    if (!isUInt<6>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    unsigned Hi2 = (Value >> 4) & 0x3;
    unsigned Lo4 = Value & 0xf;
    return (Hi2 << 4) | (Lo4 << 12);
  }
  case Xtensa::fixup_xtensa_branch_8:
    Value -= 4;
    if (!isInt<8>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    return (Value & 0xff);
  case Xtensa::fixup_xtensa_branch_12:
    Value -= 4;
    if (!isInt<12>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    return (Value & 0xfff);
  case Xtensa::fixup_xtensa_jump_18:
    Value -= 4;
    if (!isInt<18>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    return (Value & 0x3ffff);
  case Xtensa::fixup_xtensa_call_18:
    Value -= 4;
    if (!isInt<20>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    if (Value & 0x3)
      Ctx.reportError(Fixup.getLoc(), "fixup value must be 4-byte aligned");
    return (Value & 0xffffc) >> 2;
  case Xtensa::fixup_xtensa_loop_8:
    Value -= 4;
    if (!isUInt<8>(Value))
      Ctx.reportError(Fixup.getLoc(), "loop fixup value out of range");
    return (Value & 0xff);
  case Xtensa::fixup_xtensa_l32r_16: {
    if (!IsResolved)
      return 0;
    unsigned Offset = Fixup.getOffset();
    if (Offset & 0x3)
      Value -= 4;
    if (!isInt<18>(Value) && (Value & 0x20000))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    if (Value & 0x3)
      Ctx.reportError(Fixup.getLoc(), "fixup value must be 4-byte aligned");
    return (Value & 0x3fffc) >> 2;
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
                                                    MCFixup &Fixup, MCValue &,
                                                    uint64_t &Value) {
  // For a few PC-relative fixups, offsets need to be aligned down. We
  // compensate here because the default handler's `Value` decrement doesn't
  // account for this alignment.
  switch (Fixup.getKind()) {
  case Xtensa::fixup_xtensa_call_18:
  case Xtensa::fixup_xtensa_l32r_16:
    Value = (Asm->getFragmentOffset(F) + Fixup.getOffset()) % 4;
  }
  return {};
}

void XtensaAsmBackend::applyFixup(const MCFragment &F, const MCFixup &Fixup,
                                  const MCValue &Target, uint8_t *Data,
                                  uint64_t Value, bool IsResolved) {
  if (Fixup.getKind() == (MCFixupKind)Xtensa::fixup_xtensa_l32r_16) {
    IsResolved = false;
  }
  maybeAddReloc(F, Fixup, Target, Value, IsResolved);
  MCContext &Ctx = getContext();
  MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());

  Value = adjustFixupValue(Fixup, Value, Ctx);

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
  uint64_t NumNops24b = Count / 3;

  for (uint64_t i = 0; i != NumNops24b; ++i) {
    // Currently just little-endian machine supported,
    // but probably big-endian will be also implemented in future
    if (IsLittleEndian) {
      OS.write("\xf0", 1);
      OS.write("\x20", 1);
      OS.write("\0x00", 1);
    } else {
      report_fatal_error("Big-endian mode currently is not supported!");
    }
    Count -= 3;
  }

  // TODO maybe function should return error if (Count > 0)
  switch (Count) {
  default:
    break;
  case 1:
    OS.write("\0", 1);
    break;
  case 2:
    // NOP.N instruction
    OS.write("\x3d", 1);
    OS.write("\xf0", 1);
    break;
  }

  return true;
}

namespace llvm {
extern cl::opt<bool> Trampolines;
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

bool XtensaAsmBackend::mayNeedRelaxation(unsigned Opcode,
                                         ArrayRef<MCOperand> Operands,
                                         const MCSubtargetInfo &STI) const {
  if (Opcode == Xtensa::J) {
    return true;
  }
  if (Opcode == Xtensa::BEQZ_N || Opcode == Xtensa::BNEZ_N) {
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
      return false;
    int64_t SVal = (int64_t)Value;
    int64_t RelOffset = SVal - 4;
    if (RelOffset >= 0 && RelOffset <= 63) {
      return false;
    }
    return true;
  }
  return false;
}

void XtensaAsmBackend::relaxInstruction(MCInst &Inst,
                                        const MCSubtargetInfo &STI) const {
  unsigned Opcode = Inst.getOpcode();
  if (Opcode == Xtensa::BEQZ_N) {
    Inst.setOpcode(Xtensa::BEQZ);
  } else if (Opcode == Xtensa::BNEZ_N) {
    Inst.setOpcode(Xtensa::BNEZ);
  } else {
    llvm_unreachable("Unexpected instruction to relax");
  }
}
bool XtensaAsmBackend::finishLayout() const {
  MCContext &Ctx = getContext();
  bool Changed = false;

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
