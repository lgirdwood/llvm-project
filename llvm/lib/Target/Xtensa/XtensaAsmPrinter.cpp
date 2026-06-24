//===- XtensaAsmPrinter.cpp Xtensa LLVM Assembly Printer ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format Xtensa assembly language.
//
//===----------------------------------------------------------------------===//

#include "XtensaAsmPrinter.h"
#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "MCTargetDesc/XtensaInstPrinter.h"
#include "MCTargetDesc/XtensaMCAsmInfo.h"
#include "MCTargetDesc/XtensaTargetStreamer.h"
#include "TargetInfo/XtensaTargetInfo.h"
#include "XtensaConstantPoolValue.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/CodeGen/MachineInstrBundle.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

namespace llvm {
extern cl::opt<bool> TextSectionLiterals;
}


static Xtensa::Specifier
getModifierSpecifier(XtensaCP::XtensaCPModifier Modifier) {
  switch (Modifier) {
  case XtensaCP::no_modifier:
    return Xtensa::S_None;
  case XtensaCP::TPOFF:
    return Xtensa::S_TPOFF;
  case XtensaCP::PLT:
    return Xtensa::S_PLT;
  }
  report_fatal_error("Invalid XtensaCPModifier!");
}

// Map a HiFi AE_ZERO* pseudo opcode to the equivalent encoded
// `ae_subNs` opcode. Returns 0 if the pseudo is not one we know how
// to expand here.
//
// The td file declares pseudos AE_ZERO16/24/32_Pseudo_HIFI3 and
// AE_ZEROP48/Q56/64_Pseudo_HIFI3 with AsmStrings of the form
// "ae_subNs $r, $r, $r". Those pseudos have no encoding of their own,
// so without expansion the MC code emitter aborts with
// "Unsupported instruction" the first time a v2i32 zero vector
// reaches isel (e.g. from compiler_builtins).
//
// Only the variants with an encoded AE_SUBxx_HIFI3 instruction in
// XtensaHIFIInstrInfo.td are handled here. AE_ZERO16/24 currently map
// to ae_sub16s/sub24s which are not (yet) defined as encoded
// instructions; those would need new td entries.
static unsigned getHIFIZeroPseudoExpansion(unsigned Opc) {
  switch (Opc) {
  case Xtensa::AE_ZERO16_Pseudo_HIFI3:
    return Xtensa::AE_SUB16_HIFI3;
  case Xtensa::AE_ZERO32_Pseudo_HIFI3:
    return Xtensa::AE_SUB32S_HIFI3;
  case Xtensa::AE_ZERO64_Pseudo_HIFI3:
  case Xtensa::AE_ZEROP48_Pseudo_HIFI3:
  case Xtensa::AE_ZEROQ56_Pseudo_HIFI3:
    return Xtensa::AE_SUB64_HIFI3;
  default:
    return 0;
  }
}

static unsigned getHIFIPseudoExpansion(unsigned Opc) {
  switch (Opc) {
  case Xtensa::AE_MOVDA32X2_PSEUDO: return Xtensa::AE_MOVDA32X2;




  case Xtensa::AE_MULF32S_LL: return Xtensa::AE_MULF32S_LL_REAL;
  case Xtensa::AE_MULF32S_LH: return Xtensa::AE_MULF32S_LH_REAL;
  case Xtensa::AE_MULF32S_HH: return Xtensa::AE_MULF32S_HH_REAL;
  case Xtensa::AE_MULF32R_LL: return Xtensa::AE_MULF32R_LL_REAL;
  case Xtensa::AE_MULF32R_LH: return Xtensa::AE_MULF32R_LH_REAL;
  case Xtensa::AE_MULF32R_HH: return Xtensa::AE_MULF32R_HH_REAL;
  case Xtensa::AE_MULF32RA_LL: return Xtensa::AE_MULF32RA_LL_REAL;
  case Xtensa::AE_MULF32RA_LH: return Xtensa::AE_MULF32RA_LH_REAL;
  case Xtensa::AE_MULF32RA_HH: return Xtensa::AE_MULF32RA_HH_REAL;
  case Xtensa::AE_MULSF32S_LL: return Xtensa::AE_MULSF32S_LL_REAL;
  case Xtensa::AE_MULSF32S_LH: return Xtensa::AE_MULSF32S_LH_REAL;
  case Xtensa::AE_MULSF32S_HH: return Xtensa::AE_MULSF32S_HH_REAL;
  case Xtensa::AE_MULSF32R_LL: return Xtensa::AE_MULSF32R_LL_REAL;
  case Xtensa::AE_MULSF32R_LH: return Xtensa::AE_MULSF32R_LH_REAL;
  case Xtensa::AE_MULSF32R_HH: return Xtensa::AE_MULSF32R_HH_REAL;
  case Xtensa::AE_MULSF32RA_LL: return Xtensa::AE_MULSF32RA_LL_REAL;
  case Xtensa::AE_MULSF32RA_LH: return Xtensa::AE_MULSF32RA_LH_REAL;
  case Xtensa::AE_MULSF32RA_HH: return Xtensa::AE_MULSF32RA_HH_REAL;

  case Xtensa::AE_S64_I_PSEUDO: return Xtensa::AE_S64_I_REAL;
  case Xtensa::AE_MULAF32S_HH: return Xtensa::AE_MULAF32S_HH_HIFI3;
  case Xtensa::AE_MULFP16X4RAS: return Xtensa::AE_MULFP16X4RAS_REAL;
  case Xtensa::AE_MULFP16X4S: return Xtensa::AE_MULFP16X4S_REAL;
  case Xtensa::AE_MULFP16X4RS: return Xtensa::AE_MULFP16X4RS_REAL;
  case Xtensa::AE_MULFP32X16X2RAS_H: return Xtensa::AE_MULFP32X16X2RAS_H_REAL;
  case Xtensa::AE_MULFP32X16X2RAS_L: return Xtensa::AE_MULFP32X16X2RAS_L_REAL;
  case Xtensa::AE_MULFP32X16X2RS_H: return Xtensa::AE_MULFP32X16X2RS_H_REAL;
  case Xtensa::AE_MULFP32X16X2RS_L: return Xtensa::AE_MULFP32X16X2RS_L_REAL;
  case Xtensa::AE_MULFP32X2RS: return Xtensa::AE_MULFP32X2RS_REAL;
  case Xtensa::AE_MULFP24X2R: return Xtensa::AE_MULFP24X2R_REAL;
  case Xtensa::AE_MULAFD32X16X2_FIR_HH_HIFI3: return Xtensa::AE_MULAFD32X16X2_FIR_HH;

  case Xtensa::AE_S16_0_X_PSEUDO: return Xtensa::AE_S16_0_X;
  case Xtensa::AE_MULAFD32X16X2_FIR_HL_HIFI3: return Xtensa::AE_MULAFD32X16X2_FIR_HL_REAL;
  case Xtensa::AE_MULAFD32X16X2_FIR_LH_HIFI3: return Xtensa::AE_MULAFD32X16X2_FIR_LH_REAL;
  case Xtensa::AE_MULAFD32X16X2_FIR_LL_HIFI3: return Xtensa::AE_MULAFD32X16X2_FIR_LL_REAL;
  case Xtensa::AE_MUL16X4_HIFI3: return Xtensa::AE_MUL16X4_REAL;
  case Xtensa::AE_SA64POS_FP_PSEUDO: return Xtensa::AE_SA64POS_FP_REAL;
  case Xtensa::AE_LA16X4POS_PC_PSEUDO: return Xtensa::AE_LA16X4POS_PC_REAL;
  case Xtensa::AE_LA32X2POS_PC_PSEUDO: return Xtensa::AE_LA32X2POS_PC_REAL;

  case Xtensa::AE_S32_L_X_PSEUDO: return Xtensa::AE_S32_L_X;

  case Xtensa::AE_L16X4_X_PSEUDO: return Xtensa::AE_L16X4_X;
  case Xtensa::AE_S32X2_X_PSEUDO: return Xtensa::AE_S32X2_X;
  case Xtensa::AE_S32X2_XC_PSEUDO: return Xtensa::AE_S32X2_XC;
  case Xtensa::AE_S32X2_XC1_PSEUDO: return Xtensa::AE_S32X2_XC1;
  case Xtensa::AE_S32X2_I_PSEUDO: return Xtensa::AE_S32X2_I;
  case Xtensa::AE_S32X2_IP_PSEUDO: return Xtensa::AE_S32X2_IP;
  case Xtensa::AE_S32X2F24_I_PSEUDO: return Xtensa::AE_S32X2F24_I;
  case Xtensa::AE_S32X2F24_IP_PSEUDO: return Xtensa::AE_S32X2F24_IP;




  default: return 0;
  }
}

static bool getURPseudoExpansion(unsigned Opc, unsigned &RealOpc, unsigned &RegVal) {
  switch (Opc) {
  case Xtensa::AE_CBEGIN0_Pseudo: RealOpc = Xtensa::RUR; RegVal = Xtensa::AE_CBEGIN0; return true;
  case Xtensa::AE_CBEGIN1_Pseudo: RealOpc = Xtensa::RUR; RegVal = Xtensa::AE_CBEGIN1; return true;
  case Xtensa::AE_CEND0_Pseudo:   RealOpc = Xtensa::RUR; RegVal = Xtensa::AE_CEND0;   return true;
  case Xtensa::AE_CEND1_Pseudo:   RealOpc = Xtensa::RUR; RegVal = Xtensa::AE_CEND1;   return true;
  case Xtensa::AE_SETCBEGIN0_Pseudo: RealOpc = Xtensa::WUR; RegVal = Xtensa::AE_CBEGIN0; return true;
  case Xtensa::AE_SETCBEGIN1_Pseudo: RealOpc = Xtensa::WUR; RegVal = Xtensa::AE_CBEGIN1; return true;
  case Xtensa::AE_SETCEND0_Pseudo:   RealOpc = Xtensa::WUR; RegVal = Xtensa::AE_CEND0;   return true;
  case Xtensa::AE_SETCEND1_Pseudo:   RealOpc = Xtensa::WUR; RegVal = Xtensa::AE_CEND1;   return true;
  default: return false;
  }
}

static bool lowerXtensaHIFIPseudo(const MachineInstr *MI, MCInst &OutMI, XtensaAsmPrinter &AP, MCContext &OutContext) {
  unsigned Opc = MI->getOpcode();

  // 1. Zero pseudos
  if (Opc == Xtensa::AE_ZERO16_Pseudo_HIFI3 || Opc == Xtensa::AE_ZERO24_Pseudo_HIFI3 ||
      Opc == Xtensa::AE_ZERO32_Pseudo_HIFI3 || Opc == Xtensa::AE_ZERO64_Pseudo_HIFI3 ||
      Opc == Xtensa::AE_ZEROP48_Pseudo_HIFI3 || Opc == Xtensa::AE_ZEROQ56_Pseudo_HIFI3) {
    unsigned RealOpc = getHIFIZeroPseudoExpansion(Opc);
    unsigned Dst = MI->getOperand(0).getReg();
    OutMI = MCInstBuilder(RealOpc).addReg(Dst).addReg(Dst).addReg(Dst);
    return true;
  }

  // 2. User register pseudos
  unsigned RealOpc, RegVal;
  if (getURPseudoExpansion(Opc, RealOpc, RegVal)) {
    unsigned SrcDst = MI->getOperand(0).getReg();
    if (RealOpc == Xtensa::WUR) {
      OutMI = MCInstBuilder(RealOpc).addReg(RegVal).addReg(SrcDst);
    } else {
      OutMI = MCInstBuilder(RealOpc).addReg(SrcDst).addReg(RegVal);
    }
    return true;
  }

  // 3. ZALIGN pseudos
  if (Opc == Xtensa::AE_ZALIGN64_PSEUDO_HIFI3 || Opc == Xtensa::AE_ZALIGN64_PSEUDO_HIFI4) {
    OutMI = MCInstBuilder(Xtensa::AE_ZALIGN64).addReg(Xtensa::U1);
    return true;
  }

  // 4. Select pseudos (with immediate)
  if (Opc == Xtensa::AE_SEL32_HH_Pseudo_HIFI3 || Opc == Xtensa::AE_SELP24_HH_Pseudo_HIFI3 ||
      Opc == Xtensa::AE_SEL32_HL_Pseudo_HIFI3 ||
      Opc == Xtensa::AE_SEL32_LH_Pseudo_HIFI3 || Opc == Xtensa::AE_SELP24_LH_Pseudo_HIFI3 ||
      Opc == Xtensa::AE_SEL32_LL_Pseudo_HIFI3) {
    AP.lowerToMCInst(MI, OutMI);
    unsigned SelVal = 0;
    if (Opc == Xtensa::AE_SEL32_HH_Pseudo_HIFI3 || Opc == Xtensa::AE_SELP24_HH_Pseudo_HIFI3)
      SelVal = 3;
    else if (Opc == Xtensa::AE_SEL32_HL_Pseudo_HIFI3)
      SelVal = 2;
    else if (Opc == Xtensa::AE_SEL32_LH_Pseudo_HIFI3 || Opc == Xtensa::AE_SELP24_LH_Pseudo_HIFI3)
      SelVal = 1;
    OutMI.setOpcode(Xtensa::AE_SEL16I);
    OutMI.addOperand(MCOperand::createImm(SelVal));
    return true;
  }

  // 4b. Custom Group 2 pseudos
  if (Opc == Xtensa::AE_MOVAB2_PSEUDO) {
    unsigned Dst = MI->getOperand(0).getReg();
    OutMI = MCInstBuilder(Xtensa::AE_MOVAB2).addReg(Dst);
    return true;
  }
  if (Opc == Xtensa::AE_MOVINT32_FROMINT16_PSEUDO) {
    unsigned Dst = MI->getOperand(0).getReg();
    unsigned Src = MI->getOperand(1).getReg();
    OutMI = MCInstBuilder(Xtensa::OR).addReg(Dst).addReg(Src).addReg(Src);
    return true;
  }
  if (Opc == Xtensa::AE_MOVINT32_FROMINT24X2_PSEUDO ||
      Opc == Xtensa::AE_MOVINT32_FROMINT64_PSEUDO) {
    unsigned Dst = MI->getOperand(0).getReg();
    unsigned Src = MI->getOperand(1).getReg();
    OutMI = MCInstBuilder(Xtensa::AE_MOVAD32_L).addReg(Dst).addReg(Src);
    return true;
  }

  // 5. General 1-to-1 mapped pseudos
  unsigned TargetOpc = getHIFIPseudoExpansion(Opc);
  if (TargetOpc != 0) {
    AP.lowerToMCInst(MI, OutMI);
    OutMI.setOpcode(TargetOpc);
    return true;
  }

  return false;
}

void XtensaAsmPrinter::emitInstruction(const MachineInstr *MI) {
  unsigned Opc = MI->getOpcode();

  switch (Opc) {
  case Xtensa::AE_EQ16_Pseudo:
  case Xtensa::AE_LT16_Pseudo:
  case Xtensa::AE_LE16_Pseudo:
  case Xtensa::AE_EQ32_Pseudo:
  case Xtensa::AE_LT32_Pseudo:
  case Xtensa::AE_LE32_Pseudo: {
    unsigned RealOpc = 0;
    if (Opc == Xtensa::AE_EQ16_Pseudo) RealOpc = Xtensa::AE_EQ16_REAL;
    else if (Opc == Xtensa::AE_LT16_Pseudo) RealOpc = Xtensa::AE_LT16_REAL;
    else if (Opc == Xtensa::AE_LE16_Pseudo) RealOpc = Xtensa::AE_LE16_REAL;
    else if (Opc == Xtensa::AE_EQ32_Pseudo) RealOpc = Xtensa::AE_EQ32_REAL;
    else if (Opc == Xtensa::AE_LT32_Pseudo) RealOpc = Xtensa::AE_LT32_REAL;
    else if (Opc == Xtensa::AE_LE32_Pseudo) RealOpc = Xtensa::AE_LE32_REAL;

    unsigned DstReg = MI->getOperand(0).getReg();
    unsigned SrcRegS = MI->getOperand(1).getReg();
    unsigned SrcRegT = MI->getOperand(2).getReg();

    MCInst CompareInst;
    CompareInst.setOpcode(RealOpc);
    CompareInst.addOperand(MCOperand::createReg(Xtensa::B0));
    CompareInst.addOperand(MCOperand::createReg(SrcRegS));
    CompareInst.addOperand(MCOperand::createReg(SrcRegT));
    EmitToStreamer(*OutStreamer, CompareInst);

    MCInst RsrInst;
    RsrInst.setOpcode(Xtensa::RSR);
    RsrInst.addOperand(MCOperand::createReg(DstReg));
    RsrInst.addOperand(MCOperand::createReg(Xtensa::BREG));
    EmitToStreamer(*OutStreamer, RsrInst);
    return;
  }
  case TargetOpcode::BUNDLE: {
    if (OutStreamer->hasRawTextSupport()) {
      // Emit FLIX bundle using { op1; op2 } syntax for the external assembler
      OutStreamer->emitRawText(" {");
      const MachineBasicBlock *MBB = MI->getParent();
      MachineBasicBlock::const_instr_iterator MII = MI->getIterator();
      for (++MII; MII != MBB->instr_end() && MII->isInsideBundle(); ++MII) {
        if (MII->isDebugInstr() || MII->isImplicitDef())
          continue;
        MCInst LoweredMI;
        if (lowerXtensaHIFIPseudo(&*MII, LoweredMI, *this, OutContext)) {
          EmitToStreamer(*OutStreamer, LoweredMI);
          continue;
        }
        lowerToMCInst(&*MII, LoweredMI);
        EmitToStreamer(*OutStreamer, LoweredMI);
      }
      OutStreamer->emitRawText(" }");
    } else {
      MCInst BundleInst;
      BundleInst.setOpcode(Xtensa::BUNDLE);
      const MachineBasicBlock *MBB = MI->getParent();
      MachineBasicBlock::const_instr_iterator MII = MI->getIterator();
      for (++MII; MII != MBB->instr_end() && MII->isInsideBundle(); ++MII) {
        if (MII->isDebugInstr() || MII->isImplicitDef())
          continue;
        MCInst *SubInst = OutContext.createMCInst();
        if (lowerXtensaHIFIPseudo(&*MII, *SubInst, *this, OutContext)) {
          // Lowered successfully
        } else {
          lowerToMCInst(&*MII, *SubInst);
        }
        BundleInst.addOperand(MCOperand::createInst(SubInst));
      }
      EmitToStreamer(*OutStreamer, BundleInst);
    }
    return;
  }
  case Xtensa::BR_JT:
    EmitToStreamer(
        *OutStreamer,
        MCInstBuilder(Xtensa::JX).addReg(MI->getOperand(0).getReg()));
    return;
  }

  MCInst LoweredMI;
  if (lowerXtensaHIFIPseudo(MI, LoweredMI, *this, OutContext)) {
    EmitToStreamer(*OutStreamer, LoweredMI);
    return;
  }

  lowerToMCInst(MI, LoweredMI);
  EmitToStreamer(*OutStreamer, LoweredMI);
}

void XtensaAsmPrinter::emitMachineConstantPoolValue(
    MachineConstantPoolValue *MCPV) {
  XtensaConstantPoolValue *XtensaCPV =
      static_cast<XtensaConstantPoolValue *>(MCPV);
  MCSymbol *MCSym;

  if (XtensaCPV->isBlockAddress()) {
    const BlockAddress *BA =
        cast<XtensaConstantPoolConstant>(XtensaCPV)->getBlockAddress();
    MCSym = GetBlockAddressSymbol(BA);
  } else if (XtensaCPV->isMachineBasicBlock()) {
    const MachineBasicBlock *MBB =
        cast<XtensaConstantPoolMBB>(XtensaCPV)->getMBB();
    MCSym = MBB->getSymbol();
  } else if (XtensaCPV->isJumpTable()) {
    unsigned Idx = cast<XtensaConstantPoolJumpTable>(XtensaCPV)->getIndex();
    MCSym = this->GetJTISymbol(Idx, false);
  } else {
    assert(XtensaCPV->isExtSymbol() && "unrecognized constant pool value");
    XtensaConstantPoolSymbol *XtensaSym =
        cast<XtensaConstantPoolSymbol>(XtensaCPV);
    const char *SymName = XtensaSym->getSymbol();

    if (XtensaSym->isPrivateLinkage()) {
      const DataLayout &DL = getDataLayout();
      MCSym = OutContext.getOrCreateSymbol(Twine(DL.getInternalSymbolPrefix()) +
                                           SymName);
    } else {
      MCSym = OutContext.getOrCreateSymbol(SymName);
    }
  }

  MCSymbol *LblSym = GetCPISymbol(XtensaCPV->getLabelId());
  auto *TS =
      static_cast<XtensaTargetStreamer *>(OutStreamer->getTargetStreamer());
  auto Spec = getModifierSpecifier(XtensaCPV->getModifier());

  if (XtensaCPV->getModifier() != XtensaCP::no_modifier) {
    std::string SymName(MCSym->getName());
    StringRef Modifier = XtensaCPV->getModifierText();
    SymName += Modifier;
    MCSym = OutContext.getOrCreateSymbol(SymName);
  }

  const MCExpr *Expr = MCSymbolRefExpr::create(MCSym, Spec, OutContext);
  TS->emitLiteral(LblSym, Expr, true);
}

void XtensaAsmPrinter::emitMachineConstantPoolEntry(
    const MachineConstantPoolEntry &CPE, int i) {
  if (CPE.isMachineConstantPoolEntry()) {
    XtensaConstantPoolValue *XtensaCPV =
        static_cast<XtensaConstantPoolValue *>(CPE.Val.MachineCPVal);
    XtensaCPV->setLabelId(i);
    emitMachineConstantPoolValue(CPE.Val.MachineCPVal);
  } else {
    MCSymbol *LblSym = GetCPISymbol(i);
    auto *TS =
        static_cast<XtensaTargetStreamer *>(OutStreamer->getTargetStreamer());
    const Constant *C = CPE.Val.ConstVal;
    const MCExpr *Value = nullptr;

    Type *Ty = C->getType();
    if (const auto *CFP = dyn_cast<ConstantFP>(C)) {
      Value = MCConstantExpr::create(
          CFP->getValueAPF().bitcastToAPInt().getSExtValue(), OutContext);
    } else if (const auto *CI = dyn_cast<ConstantInt>(C)) {
      Value = MCConstantExpr::create(CI->getValue().getSExtValue(), OutContext);
    } else if (isa<PointerType>(Ty)) {
      Value = lowerConstant(C);
    }

    if (Value) {
      TS->emitLiteral(LblSym, Value, true);
    } else {
      OutStreamer->emitLabel(LblSym);
      emitGlobalConstant(getDataLayout(), C);
    }
  }
}

// EmitConstantPool - Print to the current output stream assembly
// representations of the constants in the constant pool MCP. This is
// used to print out constants which have been "spilled to memory" by
// the code generator.
void XtensaAsmPrinter::emitConstantPool() {
  const Function &F = MF->getFunction();
  const MachineConstantPool *MCP = MF->getConstantPool();
  const std::vector<MachineConstantPoolEntry> &CP = MCP->getConstants();
  if (CP.empty())
    return;

  OutStreamer->pushSection();
  MCSection *CS = getObjFileLowering().SectionForGlobal(&F, TM);
  OutStreamer->switchSection(CS);

  auto *TS =
      static_cast<XtensaTargetStreamer *>(OutStreamer->getTargetStreamer());
  TS->startLiteralSection(CS);

  // The ELF streamer's startLiteralSection() only registers an aligned
  // .literal section as metadata; it does not switch to it, and the
  // following emitMachineConstantPoolEntry() calls run with
  // SwitchLiteralSection=false. As a result, the literal pool entries
  // are emitted into the function's own .text.<func> subsection,
  // packed against the last instruction at whatever offset the code
  // happens to end at. Because each l32r relocation requires a
  // 4-byte-aligned literal target in the final image, an unaligned
  // pool start (e.g., function size of 14 bytes -> pool at +0xe)
  // makes BFD reject the link with
  //   "dangerous relocation: l32r: misaligned literal target".
  // Insert an explicit 4-byte alignment before the pool to fix that.
  OutStreamer->emitValueToAlignment(Align(4));

  int CPIdx = 0;
  for (const MachineConstantPoolEntry &CPE : CP) {
    emitMachineConstantPoolEntry(CPE, CPIdx++);
  }

  if (TextSectionLiterals)
    TS->emitLiteralPosition();

  OutStreamer->popSection();
}

void XtensaAsmPrinter::printOperand(const MachineInstr *MI, int OpNo,
                                    raw_ostream &O) {
  const MachineOperand &MO = MI->getOperand(OpNo);

  switch (MO.getType()) {
  case MachineOperand::MO_Register:
  case MachineOperand::MO_Immediate: {
    MCOperand MC = lowerOperand(MI->getOperand(OpNo));
    XtensaInstPrinter::printOperand(MC, O);
    break;
  }
  default:
    llvm_unreachable("unknown operand type");
  }
}

bool XtensaAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                                       const char *ExtraCode, raw_ostream &O) {
  // Print the operand if there is no operand modifier.
  if (!ExtraCode || !ExtraCode[0]) {
    printOperand(MI, OpNo, O);
    return false;
  }

  // Fallback to the default implementation.
  return AsmPrinter::PrintAsmOperand(MI, OpNo, ExtraCode, O);
}

bool XtensaAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
                                             unsigned OpNo,
                                             const char *ExtraCode,
                                             raw_ostream &OS) {
  if (ExtraCode && ExtraCode[0])
    return true; // Unknown modifier.

  assert(OpNo + 1 < MI->getNumOperands() && "Insufficient operands");

  const MachineOperand &Base = MI->getOperand(OpNo);
  const MachineOperand &Offset = MI->getOperand(OpNo + 1);

  assert(Base.isReg() &&
         "Unexpected base pointer for inline asm memory operand.");
  assert(Offset.isImm() && "Unexpected offset for inline asm memory operand.");

  OS << XtensaInstPrinter::getRegisterName(Base.getReg());
  OS << ", ";
  OS << Offset.getImm();

  return false;
}

MCSymbol *
XtensaAsmPrinter::GetConstantPoolIndexSymbol(const MachineOperand &MO) const {
  // Create a symbol for the name.
  return GetCPISymbol(MO.getIndex());
}

MCSymbol *XtensaAsmPrinter::GetJumpTableSymbol(const MachineOperand &MO) const {
  return GetJTISymbol(MO.getIndex());
}

MCOperand
XtensaAsmPrinter::LowerSymbolOperand(const MachineOperand &MO,
                                     MachineOperand::MachineOperandType MOTy,
                                     unsigned Offset) const {
  const MCSymbol *Symbol;
  switch (MOTy) {
  case MachineOperand::MO_GlobalAddress:
    Symbol = getSymbol(MO.getGlobal());
    Offset += MO.getOffset();
    break;
  case MachineOperand::MO_MachineBasicBlock:
    Symbol = MO.getMBB()->getSymbol();
    break;
  case MachineOperand::MO_BlockAddress:
    Symbol = GetBlockAddressSymbol(MO.getBlockAddress());
    Offset += MO.getOffset();
    break;
  case MachineOperand::MO_ExternalSymbol:
    Symbol = GetExternalSymbolSymbol(MO.getSymbolName());
    Offset += MO.getOffset();
    break;
  case MachineOperand::MO_JumpTableIndex:
    Symbol = GetJumpTableSymbol(MO);
    break;
  case MachineOperand::MO_ConstantPoolIndex:
    Symbol = GetConstantPoolIndexSymbol(MO);
    Offset += MO.getOffset();
    break;
  default:
    report_fatal_error("<unknown operand type>");
  }

  const MCExpr *ME = MCSymbolRefExpr::create(Symbol, OutContext);
  if (Offset) {
    // Assume offset is never negative.
    assert(Offset > 0);

    const MCConstantExpr *OffsetExpr =
        MCConstantExpr::create(Offset, OutContext);
    ME = MCBinaryExpr::createAdd(ME, OffsetExpr, OutContext);
  }

  return MCOperand::createExpr(ME);
}

MCOperand XtensaAsmPrinter::lowerOperand(const MachineOperand &MO,
                                         unsigned Offset) const {
  MachineOperand::MachineOperandType MOTy = MO.getType();

  switch (MOTy) {
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit())
      break;
    return MCOperand::createReg(MO.getReg());
  case MachineOperand::MO_Immediate:
    return MCOperand::createImm(MO.getImm() + Offset);
  case MachineOperand::MO_RegisterMask:
    break;
  case MachineOperand::MO_GlobalAddress:
  case MachineOperand::MO_MachineBasicBlock:
  case MachineOperand::MO_BlockAddress:
  case MachineOperand::MO_ExternalSymbol:
  case MachineOperand::MO_JumpTableIndex:
  case MachineOperand::MO_ConstantPoolIndex:
    return LowerSymbolOperand(MO, MOTy, Offset);
  default:
    report_fatal_error("unknown operand type");
  }

  return MCOperand();
}

void XtensaAsmPrinter::lowerToMCInst(const MachineInstr *MI,
                                     MCInst &OutMI) const {
  OutMI.setOpcode(MI->getOpcode());

  for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
    const MachineOperand &MO = MI->getOperand(i);
    MCOperand MCOp = lowerOperand(MO);

    if (MCOp.isValid())
      OutMI.addOperand(MCOp);
  }
}

void XtensaAsmPrinter::EmitToStreamer(MCStreamer &S, const MCInst &Inst) {
  MCInst CInst;
  if (Xtensa::compress(CInst, Inst, *STI)) {
    AsmPrinter::EmitToStreamer(S, CInst);
  } else {
    AsmPrinter::EmitToStreamer(S, Inst);
  }
}

char XtensaAsmPrinter::ID = 0;

INITIALIZE_PASS(XtensaAsmPrinter, "xtensa-asm-printer",
                "Xtensa Assembly Printer", false, false)

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeXtensaAsmPrinter() {
  RegisterAsmPrinter<XtensaAsmPrinter> A(getTheXtensaTarget());
}
