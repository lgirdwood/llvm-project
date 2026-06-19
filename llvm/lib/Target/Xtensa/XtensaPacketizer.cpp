//===- XtensaPacketizer.cpp - Xtensa FLIX VLIW Packetizer -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This implements a greedy peephole FLIX packetizer for the Xtensa target.
// It bundles compatible adjacent instructions into BUNDLE pseudo-ops that
// the AsmPrinter emits using { op1; op2 } syntax for the external GCC
// assembler to encode as FLIX instruction words.
//
// Supported FLIX formats:
//   ae_format48:   2 slots, 6 bytes  (two scalar/HiFi ops)
//   ae_format48_2: 2 slots, 6 bytes  (branch + scalar)
//
// Dual HiFi bundles: multiple HiFi instructions can be packed when they
// use disjoint execution units (e.g., two loads in Slot 0 + Slot 1).
//
// The GCC assembler handles encoding validation, so we only need to emit
// syntactically correct bundles with compatible instruction pairs.
//
//===----------------------------------------------------------------------===//

#include "XtensaPacketizer.h"
#include "XtensaInstrInfo.h"
#include "XtensaSubtarget.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineInstrBundle.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"
#include "llvm/Support/Debug.h"

using namespace llvm;

#define DEBUG_TYPE "xtensa-packetizer"

namespace {

/// Classify instructions into FLIX slot categories.
enum FLIXSlot {
  FLIX_None = 0,     // Cannot be bundled
  FLIX_Mem,          // Scalar load/store (strict slot 0)
  FLIX_ALU,          // Scalar ALU (strict slot 1)
  FLIX_ScalarNarrow, // Narrow scalar (mov.n, addi.n, l32i.n, s32i.n, etc.)
  FLIX_Branch,       // Branch/loop instructions (slot 0 of ae_format48_2)
  FLIX_HiFiMem,      // HiFi loads (ae_l*) — can dual-issue in Slot 0 + Slot 1
  FLIX_HiFiMAC,      // HiFi MAC/MUL (ae_mul*, ae_mula*, ae_muls*) — Slot 2
  FLIX_HiFiFmt,      // HiFi formatting (ae_round*, ae_sat*, ae_cvt*, ae_slai*,
                     //   ae_srai*, ae_max*, ae_min*, ae_abs*, ae_neg*) — Slot 3
  FLIX_HiFiLS,       // HiFi stores and other mem ops (ae_s*, ae_la*, ae_sa*)
  FLIX_HiFiALU,      // HiFi ALU (ae_add*, ae_sub*, ae_sel*, etc.)
  FLIX_HiFiSetup,    // HiFi setup (ae_zalign*, ae_la64.pp, ae_sa64pos.fp, ae_cbegin/cend)
};

static bool isHiFiOpcode(unsigned Opc, const TargetInstrInfo &TII) {
  StringRef Name = TII.getName(Opc);
  return Name.starts_with("AE_");
}

/// Classify an Xtensa instruction for FLIX slot assignment.
/// We are very conservative here — only instructions that we have verified
/// work in FLIX bundles are classified as bundleable.
static FLIXSlot classifyInstr(const MachineInstr &MI,
                              const TargetInstrInfo &TII) {
  if (MI.isDebugInstr() || MI.isImplicitDef() || MI.isCFIInstruction())
    return FLIX_None;

  unsigned Opc = MI.getOpcode();

  // Never bundle these
  switch (Opc) {
  case TargetOpcode::BUNDLE:
  case TargetOpcode::DBG_VALUE:
  case TargetOpcode::DBG_LABEL:
  case TargetOpcode::IMPLICIT_DEF:
  case TargetOpcode::KILL:
  case TargetOpcode::COPY:
  case TargetOpcode::INLINEASM:
  case TargetOpcode::INLINEASM_BR:
  case TargetOpcode::EH_LABEL:
  case TargetOpcode::GC_LABEL:
  case Xtensa::BR_JT:
    return FLIX_None;
  default:
    break;
  }

  // Entry/return - never bundle
  if (Opc == Xtensa::ENTRY || Opc == Xtensa::RETW ||
      Opc == Xtensa::RETW_N || Opc == Xtensa::RET ||
      Opc == Xtensa::RET_N)
    return FLIX_None;

  // Call and unconditional branch instructions - never bundle
  // (unconditional jumps, loops, and all calls)
  if (MI.isCall() || MI.isUnconditionalBranch() || Opc == Xtensa::LOOP || Opc == Xtensa::LOOPNEZ || Opc == Xtensa::LOOPGTZ)
    return FLIX_None;

  // Conditional branches are allowed in slot 0 paired with a scalar in slot 1
  if (MI.isConditionalBranch())
    return FLIX_Branch;

  // HiFi instructions — classify by execution unit for multi-HiFi bundling
  if (isHiFiOpcode(Opc, TII)) {
    // Loads go to Slot 0/1 (can dual-issue)
    if (MI.mayLoad() && !MI.mayStore())
      return FLIX_HiFiMem;

    // MAC/MUL instructions go to Slot 2
    // Match opcode names starting with AE_MUL (covers AE_MUL*, AE_MULA*, AE_MULS*)
    StringRef Name = TII.getName(Opc);
    if (Name.starts_with("AE_MUL"))
      return FLIX_HiFiMAC;

    // Formatting/rounding/saturation instructions go to Slot 3
    if (Name.starts_with("AE_ROUND") || Name.starts_with("AE_SAT") ||
        Name.starts_with("AE_CVT") || Name.starts_with("AE_SLAI") ||
        Name.starts_with("AE_SRAI") || Name.starts_with("AE_MAX") ||
        Name.starts_with("AE_MIN") || Name.starts_with("AE_ABS") ||
        Name.starts_with("AE_NEG") || Name.starts_with("AE_TRUNC") ||
        Name.starts_with("AE_PKSR"))
      return FLIX_HiFiFmt;

    // HiFi ALU instructions go to Slot 2 (same unit as MAC per hardware spec)
    if (Name.starts_with("AE_ADD") || Name.starts_with("AE_SUB") ||
        Name.starts_with("AE_SEL") || Name.starts_with("AE_AND") ||
        Name.starts_with("AE_OR") || Name.starts_with("AE_XOR") ||
        Name.starts_with("AE_MOV") || Name.starts_with("AE_ZERO"))
      return FLIX_HiFiALU;

    return FLIX_HiFiLS;
  }

  // Scalar instructions categorization for strict Slot 0 / Slot 1 modeling
  switch (Opc) {
  // Memory (Strictly Slot 0)
  case Xtensa::L32I:
  case Xtensa::S32I:
  case Xtensa::L32R:
  case Xtensa::L8UI:
  case Xtensa::L16UI:
  case Xtensa::L16SI:
  case Xtensa::S8I:
  case Xtensa::S16I:
    return FLIX_Mem;

  // ALU / Setup (Strictly Slot 1 when paired with Memory)
  case Xtensa::ADD:
  case Xtensa::ADDI:
  case Xtensa::SUB:
  case Xtensa::AND:
  case Xtensa::OR:
  case Xtensa::XOR:
  case Xtensa::MOVI:
  case Xtensa::NOP:
  case Xtensa::NEG:
  case Xtensa::ABS:
  case Xtensa::EXTUI:
  case Xtensa::SEXT:
  case Xtensa::CLAMPS:
  case Xtensa::MOVEQZ:
  case Xtensa::MOVNEZ:
  case Xtensa::MOVLTZ:
  case Xtensa::MOVGEZ:
  case Xtensa::ADDX2:
  case Xtensa::ADDX4:
  case Xtensa::ADDX8:
  case Xtensa::MIN:
  case Xtensa::MAX:
  case Xtensa::MINU:
  case Xtensa::MAXU:
  case Xtensa::SLLI:
  case Xtensa::SRLI:
  case Xtensa::SRAI:
  case Xtensa::SRC:
  case Xtensa::SSA8L:
  case Xtensa::SSA8B:
  case Xtensa::SSAI:
  case Xtensa::SSL:
  case Xtensa::SSR:
    return FLIX_ALU;

  // Narrow instructions
  case Xtensa::MOV_N:
  case Xtensa::MOVI_N:
  case Xtensa::ADD_N:
  case Xtensa::ADDI_N:
  case Xtensa::L32I_N:
  case Xtensa::S32I_N:
    return FLIX_ScalarNarrow;
  default:
    break;
  }

  // Everything else (unrecognized instructions, etc.)
  // is NOT bundled — these have slot restrictions we don't fully model yet.
  return FLIX_None;
}

/// Check if two instructions have data dependencies (RAW, WAR, WAW).
static bool hasDataDep(const MachineInstr &A, const MachineInstr &B,
                       const TargetRegisterInfo &TRI) {
  // Check all defs of A against uses/defs of B (RAW, WAW)
  for (const MachineOperand &DefOp : A.operands()) {
    if (!DefOp.isReg() || !DefOp.isDef())
      continue;
    Register DefReg = DefOp.getReg();
    if (!DefReg.isPhysical())
      continue;

    for (const MachineOperand &BOp : B.operands()) {
      if (!BOp.isReg())
        continue;
      Register BReg = BOp.getReg();
      if (!BReg.isPhysical())
        continue;
      if (TRI.regsOverlap(DefReg, BReg))
        return true;
    }
  }

  // Check all defs of B against uses of A (WAR)
  for (const MachineOperand &DefOp : B.operands()) {
    if (!DefOp.isReg() || !DefOp.isDef())
      continue;
    Register DefReg = DefOp.getReg();
    if (!DefReg.isPhysical())
      continue;

    for (const MachineOperand &AOp : A.operands()) {
      if (!AOp.isReg() || !AOp.isUse())
        continue;
      Register AReg = AOp.getReg();
      if (!AReg.isPhysical())
        continue;
      if (TRI.regsOverlap(DefReg, AReg))
        return true;
    }
  }

  // Memory dependencies: don't bundle stores with other memory ops.
  // Two loads (including HiFi parallel loads) are safe to reorder.
  if (A.mayStore() && (B.mayLoad() || B.mayStore()))
    return true;
  if (B.mayStore() && A.mayLoad())
    return true;
  // Two loads with no register overlap are safe — no extra check needed.

  return false;
}

/// Check if two instructions can form an ae_format48 bundle.
static bool canBundle2(const MachineInstr &A, const MachineInstr &B,
                       const TargetRegisterInfo &TRI) {
  const TargetInstrInfo &TII =
      *A.getMF()->getSubtarget().getInstrInfo();
  FLIXSlot SlotA = classifyInstr(A, TII);
  FLIXSlot SlotB = classifyInstr(B, TII);

  if (SlotA == FLIX_None || SlotB == FLIX_None)
    return false;

  // Don't bundle two branches
  if (SlotA == FLIX_Branch && SlotB == FLIX_Branch)
    return false;

  // Check for data dependencies
  if (hasDataDep(A, B, TRI))
    return false;

  // Valid ae_format48 pairs:
  //   Scalar Mem + Scalar ALU    (Slot 0 + Slot 1)
  //   Branch + Scalar ALU        (Slot 0 + Slot 1)
  //   HiFi Load + Scalar ALU     (Slot 0 + Slot 1)
  //   Scalar Mem + HiFi Load     (Slot 0 + Slot 1)
  //   HiFi Load + HiFi Load      (Slot 0 + Slot 1)
  //   HiFi Load + HiFi MAC/ALU   (Slot 0 + Slot 2)
  //   HiFi Load + HiFi Fmt       (Slot 0 + Slot 3)
  //   HiFi MAC/ALU + HiFi Fmt    (Slot 2 + Slot 3)

  bool AisMem = (SlotA == FLIX_Mem);
  bool BisMem = (SlotB == FLIX_Mem);
  bool AisALU = (SlotA == FLIX_ALU || SlotA == FLIX_ScalarNarrow);
  bool BisALU = (SlotB == FLIX_ALU || SlotB == FLIX_ScalarNarrow);
  bool AisHiFiMem = (SlotA == FLIX_HiFiMem);
  bool BisHiFiMem = (SlotB == FLIX_HiFiMem);
  bool AisHiFiSlot2 = (SlotA == FLIX_HiFiMAC || SlotA == FLIX_HiFiALU);
  bool BisHiFiSlot2 = (SlotB == FLIX_HiFiMAC || SlotB == FLIX_HiFiALU);
  bool AisHiFiFmt = (SlotA == FLIX_HiFiFmt);
  bool BisHiFiFmt = (SlotB == FLIX_HiFiFmt);
  bool AisBranch = (SlotA == FLIX_Branch);

  // 1. Scalar Memory + Scalar ALU (Slot 0 = Mem, Slot 1 = ALU)
  if (AisMem && BisALU)
    return true;

  // 2. Branch + Scalar ALU (Slot 0 = Branch, Slot 1 = ALU)
  if (AisBranch && BisALU)
    return true;

  // 3. HiFi Load + Scalar ALU (Slot 0 = HiFi Mem, Slot 1 = ALU)
  if (AisHiFiMem && BisALU)
    return true;

  // 4. Scalar Memory + HiFi Load (Slot 0 = Mem, Slot 1 = HiFi Mem)
  if (AisMem && BisHiFiMem)
    return true;

  // 5. Dual HiFi Loads (Slot 0 + Slot 1, parallel loads)
  if (AisHiFiMem && BisHiFiMem)
    return true;

  // 6. HiFi Load + HiFi MAC/ALU (Slot 0 = Mem, Slot 2 = MAC/ALU)
  //    Load must precede MAC/ALU in the bundle.
  if (AisHiFiMem && BisHiFiSlot2)
    return true;

  // 7. HiFi Load + HiFi Fmt (Slot 0 = Mem, Slot 3 = Fmt)
  //    Load must precede Fmt in the bundle.
  if (AisHiFiMem && BisHiFiFmt)
    return true;

  // 8. HiFi MAC/ALU + HiFi Fmt (Slot 2 + Slot 3)
  if (AisHiFiSlot2 && BisHiFiFmt)
    return true;

  // 9. HiFi Fmt + HiFi Load (Slot 0 = Fmt, Slot 1 = Load)
  //    Only ae_cvt* instructions are confirmed Slot 0-capable.
  //    Other Fmt ops (ae_round*, ae_sat*) are strictly Slot 3.
  if (AisHiFiFmt && BisHiFiMem) {
    StringRef NameA = TII.getName(A.getOpcode());
    if (NameA.starts_with("AE_CVT"))
      return true;
  }

  return false;
}

/// Map a FLIXSlot category to a hardware slot number (0-3).
/// Returns -1 if the slot category can't be mapped to a single slot.
static int mapToSlot(FLIXSlot S) {
  switch (S) {
  case FLIX_Mem:
  case FLIX_Branch:
  case FLIX_HiFiMem:    // Primary memory in Slot 0 (or Slot 1 for dual loads)
    return 0;
  case FLIX_ALU:
  case FLIX_ScalarNarrow:
    return 1;
  case FLIX_HiFiMAC:
  case FLIX_HiFiALU:    // HiFi ALU shares Slot 2 with MAC
    return 2;
  case FLIX_HiFiFmt:
    return 3;
  default:
    return -1;
  }
}

/// Check if instruction C can be added to an existing bundle.
/// BundleMembers contains the instructions already in the bundle.
/// OccupiedSlots is a bitmask of slots already taken.
static bool canExtendBundle(const MachineInstr &C,
                            ArrayRef<MachineInstr *> BundleMembers,
                            unsigned OccupiedSlots,
                            const TargetInstrInfo &TII,
                            const TargetRegisterInfo &TRI) {
  FLIXSlot SlotC = classifyInstr(C, TII);
  if (SlotC == FLIX_None)
    return false;

  // Only extend to 3+ slots with HiFi instructions.
  // The GCC assembler's multi-slot formats (ae_format64) require HiFi ops.
  // Scalar-only bundles must stay at 2 slots (ae_format48).
  bool CisHiFi = (SlotC == FLIX_HiFiMem || SlotC == FLIX_HiFiMAC ||
                  SlotC == FLIX_HiFiFmt || SlotC == FLIX_HiFiALU);
  if (!CisHiFi)
    return false;

  // Verify existing bundle has at least one HiFi instruction
  bool BundleHasHiFi = false;
  for (MachineInstr *M : BundleMembers) {
    FLIXSlot S = classifyInstr(*M, TII);
    if (S == FLIX_HiFiMem || S == FLIX_HiFiMAC ||
        S == FLIX_HiFiFmt || S == FLIX_HiFiALU) {
      BundleHasHiFi = true;
      break;
    }
  }
  if (!BundleHasHiFi)
    return false;

  int HwSlot = mapToSlot(SlotC);
  if (HwSlot < 0)
    return false;

  // HiFiMem can go to Slot 0 or Slot 1. Try Slot 1 if Slot 0 is taken.
  if (SlotC == FLIX_HiFiMem && (OccupiedSlots & (1u << 0)))
    HwSlot = 1;

  // Only ae_cvt* Fmt ops can flex to Slot 0. Others are strictly Slot 3.
  if (SlotC == FLIX_HiFiFmt && (OccupiedSlots & (1u << 3)) &&
      !(OccupiedSlots & (1u << 0))) {
    StringRef Name = TII.getName(C.getOpcode());
    if (Name.starts_with("AE_CVT"))
      HwSlot = 0;
  }

  // Check slot conflict
  if (OccupiedSlots & (1u << HwSlot))
    return false;

  // Check data dependencies against all existing bundle members
  for (MachineInstr *M : BundleMembers) {
    if (hasDataDep(*M, C, TRI))
      return false;
  }

  return true;
}

/// Get the hardware slot for instruction C when extending a bundle.
static int getExtendSlot(const MachineInstr &C,
                         unsigned OccupiedSlots,
                         const TargetInstrInfo &TII) {
  FLIXSlot SlotC = classifyInstr(C, TII);
  int HwSlot = mapToSlot(SlotC);
  if (SlotC == FLIX_HiFiMem && (OccupiedSlots & (1u << 0)))
    HwSlot = 1;
  // Only ae_cvt* Fmt ops can flex to Slot 0
  if (SlotC == FLIX_HiFiFmt && (OccupiedSlots & (1u << 3)) &&
      !(OccupiedSlots & (1u << 0))) {
    StringRef Name = TII.getName(C.getOpcode());
    if (Name.starts_with("AE_CVT"))
      HwSlot = 0;
  }
  return HwSlot;
}

} // end anonymous namespace

namespace llvm {

class XtensaPacketizer : public MachineFunctionPass {
public:
  static char ID;
  XtensaPacketizer() : MachineFunctionPass(ID) {
    initializeXtensaPacketizerPass(*PassRegistry::getPassRegistry());
  }

  StringRef getPassName() const override {
    return "Xtensa FLIX VLIW Packetizer";
  }

  bool runOnMachineFunction(MachineFunction &MF) override;

private:
  bool packetizeMBB(MachineBasicBlock &MBB, const TargetInstrInfo &TII,
                    const TargetRegisterInfo &TRI);
};

} // end namespace llvm

char llvm::XtensaPacketizer::ID = 0;

INITIALIZE_PASS(XtensaPacketizer, "xtensa-packetizer",
                "Xtensa FLIX VLIW Packetizer", false, false)

bool XtensaPacketizer::runOnMachineFunction(MachineFunction &MF) {
  const XtensaSubtarget &ST = MF.getSubtarget<XtensaSubtarget>();
  if (!ST.hasFLIX())
    return false;

  const TargetInstrInfo &TII = *ST.getInstrInfo();
  const TargetRegisterInfo &TRI = *ST.getRegisterInfo();

  bool Changed = false;
  for (MachineBasicBlock &MBB : MF)
    Changed |= packetizeMBB(MBB, TII, TRI);

  return Changed;
}

bool XtensaPacketizer::packetizeMBB(MachineBasicBlock &MBB,
                                    const TargetInstrInfo &TII,
                                    const TargetRegisterInfo &TRI) {
  if (MBB.empty())
    return false;

  bool Changed = false;

  // Use instr_iterator to walk individual instructions (not bundles)
  auto I = MBB.instr_begin();
  auto E = MBB.instr_end();

  while (I != E) {
    MachineInstr &First = *I;

    // Skip debug/meta/already-bundled instructions
    if (First.isDebugInstr() || First.isImplicitDef() ||
        First.isCFIInstruction() ||
        First.getOpcode() == TargetOpcode::BUNDLE ||
        First.isInsideBundle()) {
      ++I;
      continue;
    }

    // Find the next non-debug real instruction
    auto Next = std::next(I);
    while (Next != E && (Next->isDebugInstr() || Next->isImplicitDef()))
      ++Next;

    // Try to bundle with the next instruction
    if (Next != E && !Next->isInsideBundle() &&
        Next->getOpcode() != TargetOpcode::BUNDLE) {
      MachineInstr &Second = *Next;

      if (canBundle2(First, Second, TRI)) {
        LLVM_DEBUG(dbgs() << "FLIX bundle: " << First << "  with: " << Second);

        // Track bundle members and occupied slots for multi-slot extension
        SmallVector<MachineInstr *, 4> BundleMembers = {&First, &Second};
        unsigned OccupiedSlots = 0;

        // Special case: Fmt + Load → Fmt goes to Slot 0, Load to Slot 1.
        // This enables extending with another Fmt in Slot 3.
        FLIXSlot SlotFirst = classifyInstr(First, TII);
        FLIXSlot SlotSecond = classifyInstr(Second, TII);
        if (SlotFirst == FLIX_HiFiFmt && SlotSecond == FLIX_HiFiMem) {
          OccupiedSlots |= (1u << 0);  // Fmt in Slot 0
          OccupiedSlots |= (1u << 1);  // Load in Slot 1
        } else {
          OccupiedSlots |= (1u << getExtendSlot(First, 0, TII));
          OccupiedSlots |= (1u << getExtendSlot(Second, OccupiedSlots, TII));
        }

        // Greedily try to extend to 3 or 4 slots
        auto BundleEnd = std::next(Next);
        while (BundleMembers.size() < 4 && BundleEnd != E) {
          // Skip debug instructions
          if (BundleEnd->isDebugInstr() || BundleEnd->isImplicitDef()) {
            ++BundleEnd;
            continue;
          }
          if (BundleEnd->isInsideBundle() ||
              BundleEnd->getOpcode() == TargetOpcode::BUNDLE)
            break;

          if (canExtendBundle(*BundleEnd, BundleMembers, OccupiedSlots,
                              TII, TRI)) {
            LLVM_DEBUG(dbgs() << "  extend: " << *BundleEnd);
            int Slot = getExtendSlot(*BundleEnd, OccupiedSlots, TII);
            OccupiedSlots |= (1u << Slot);
            BundleMembers.push_back(&*BundleEnd);
            ++BundleEnd;
          } else {
            break;
          }
        }

        // Create the bundle
        MIBundleBuilder(MBB, I, BundleEnd);
        finalizeBundle(MBB, I);

        Changed = true;
        I = BundleEnd;
        continue;
      }
    }

    ++I;
  }

  return Changed;
}

FunctionPass *llvm::createXtensaPacketizerPass() {
  return new XtensaPacketizer();
}
