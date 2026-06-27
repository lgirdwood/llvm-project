//===- XtensaAsmParser.cpp - Parse Xtensa assembly to MCInst instructions -===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/XtensaMCAsmInfo.h"
#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "MCTargetDesc/XtensaTargetStreamer.h"
#include "TargetInfo/XtensaTargetInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCParser/AsmLexer.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Casting.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Alignment.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCSectionELF.h"

using namespace llvm;

namespace llvm {
extern cl::opt<bool> AbsoluteLiterals;

cl::opt<bool> Longcalls(
    "longcalls",
    cl::desc("Enable longcall relaxation"),
    cl::init(true));

cl::opt<bool> TargetAlign(
    "target-align",
    cl::desc("Align branch targets"),
    cl::init(true));

cl::opt<bool> NoTargetAlign(
    "no-target-align",
    cl::desc("Do not align branch targets"),
    cl::init(false));

cl::opt<bool> Transform(
    "transform",
    cl::desc("Enable transform mode"),
    cl::init(true));

cl::opt<bool> NoTransform(
    "no-transform",
    cl::desc("Disable transform mode"),
    cl::init(false));

extern cl::opt<bool> Trampolines;

cl::opt<bool> NoTrampolines(
    "no-trampolines",
    cl::desc("Disable trampolines relaxation"),
    cl::init(false));

extern cl::opt<bool> AutoLitpools;

cl::opt<bool> NoAutoLitpools(
    "no-auto-litpools",
    cl::desc("Disable automatic literal pools"),
    cl::init(false));

extern cl::opt<int> AutoLitpoolLimit;
}

#define DEBUG_TYPE "xtensa-asm-parser"

struct XtensaOperand;

class XtensaAsmParser : public MCTargetAsmParser {
  const MCRegisterInfo &MRI;
  bool LongcallsEnabled;
  bool TargetAlignEnabled;
  unsigned StructUniqueID;
  bool InBrackets;
  MCInst CurrentBundle;
  bool TransformEnabled;
  std::vector<bool> AbsoluteLiteralsStack;
  std::vector<bool> TransformStack;
  bool TrampolinesEnabled;
  std::vector<bool> TrampolinesStack;
  bool AutoLitpoolsEnabled;
  std::vector<bool> AutoLitpoolsStack;

  enum XtensaRegisterType { Xtensa_Generic, Xtensa_SR, Xtensa_UR };
  SMLoc getLoc() const { return getParser().getTok().getLoc(); }

  XtensaTargetStreamer &getTargetStreamer() {
    MCTargetStreamer &TS = *getParser().getStreamer().getTargetStreamer();
    return static_cast<XtensaTargetStreamer &>(TS);
  }

  ParseStatus parseDirective(AsmToken DirectiveID) override;
  bool parseRegister(MCRegister &Reg, SMLoc &StartLoc, SMLoc &EndLoc) override;
  bool parseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;
  bool matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;
  unsigned validateTargetOperandClass(MCParsedAsmOperand &Op,
                                      unsigned Kind) override;
  void onLabelParsed(MCSymbol *Symbol) override;
  uint64_t getSectionCurrentOffset(const MCSection *Sec);

  bool processInstruction(MCInst &Inst, SMLoc IDLoc, MCStreamer &Out,
                          const MCSubtargetInfo *STI);

  bool tokenIsStartOfStatement(AsmToken::TokenKind Token) override {
    return Token == AsmToken::LCurly || Token == AsmToken::RCurly;
  }

// Auto-generated instruction matching functions
#define GET_ASSEMBLER_HEADER
#include "XtensaGenAsmMatcher.inc"

  ParseStatus parseImmediate(OperandVector &Operands);
  ParseStatus
  parseRegister(OperandVector &Operands, bool AllowParens = false,
                XtensaRegisterType SR = Xtensa_Generic,
                Xtensa::RegisterAccessType RAType = Xtensa::REGISTER_EXCHANGE);
  ParseStatus parseOperandWithModifier(OperandVector &Operands);
  bool
  parseOperand(OperandVector &Operands, StringRef Mnemonic,
               XtensaRegisterType SR = Xtensa_Generic,
               Xtensa::RegisterAccessType RAType = Xtensa::REGISTER_EXCHANGE);
  bool ParseInstructionWithSR(ParseInstructionInfo &Info, StringRef Name,
                              SMLoc NameLoc, OperandVector &Operands);
  ParseStatus tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,
                               SMLoc &EndLoc) override {
    return ParseStatus::NoMatch;
  }

  ParseStatus parsePCRelTarget(OperandVector &Operands);
  bool parseLiteralDirective(SMLoc L);

public:
  enum XtensaMatchResultTy {
    Match_Dummy = FIRST_TARGET_MATCH_RESULT_TY,
#define GET_OPERAND_DIAGNOSTIC_TYPES
#include "XtensaGenAsmMatcher.inc"
#undef GET_OPERAND_DIAGNOSTIC_TYPES
  };

  XtensaAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
                  const MCInstrInfo &MII)
      : MCTargetAsmParser(STI, MII),
        MRI(*Parser.getContext().getRegisterInfo()),
        LongcallsEnabled(Longcalls),
        TargetAlignEnabled(TargetAlign && !NoTargetAlign),
        StructUniqueID(0),
        InBrackets(false),
        TransformEnabled(Transform && !NoTransform),
        TrampolinesEnabled(Trampolines && !NoTrampolines),
        AutoLitpoolsEnabled(AutoLitpools && !NoAutoLitpools) {
    setAvailableFeatures(ComputeAvailableFeatures(STI.getFeatureBits()));
    if (auto *TS = Parser.getStreamer().getTargetStreamer()) {
      static_cast<XtensaTargetStreamer *>(TS)->setAbsoluteLiterals(AbsoluteLiterals);
    }
    Parser.addAliasForDirective(".word", ".4byte");
    Parser.addAliasForDirective(".short", ".2byte");
    Parser.addAliasForDirective(".hword", ".2byte");
    Parser.addAliasForDirective(".half", ".2byte");
  }

  bool hasWindowed() const {
    return getSTI().getFeatureBits()[Xtensa::FeatureWindowed];
  };
};

// Return true if Expr is in the range [MinValue, MaxValue].
static bool inRange(const MCExpr *Expr, int64_t MinValue, int64_t MaxValue) {
  if (auto *CE = dyn_cast<MCConstantExpr>(Expr)) {
    int64_t Value = CE->getValue();
    return Value >= MinValue && Value <= MaxValue;
  }
  return false;
}

struct XtensaOperand : public MCParsedAsmOperand {

  enum KindTy {
    Token,
    Register,
    Immediate,
  } Kind;

  struct RegOp {
    unsigned RegNum;
  };

  struct ImmOp {
    const MCExpr *Val;
  };

  SMLoc StartLoc, EndLoc;
  union {
    StringRef Tok;
    RegOp Reg;
    ImmOp Imm;
  };

  XtensaOperand(KindTy K) : MCParsedAsmOperand(), Kind(K) {}

public:
  XtensaOperand(const XtensaOperand &o) : MCParsedAsmOperand() {
    Kind = o.Kind;
    StartLoc = o.StartLoc;
    EndLoc = o.EndLoc;
    switch (Kind) {
    case Register:
      Reg = o.Reg;
      break;
    case Immediate:
      Imm = o.Imm;
      break;
    case Token:
      Tok = o.Tok;
      break;
    }
  }

  bool isToken() const override { return Kind == Token; }
  bool isReg() const override { return Kind == Register; }
  bool isImm() const override { return Kind == Immediate; }
  bool isMem() const override { return false; }

  bool isImm(int64_t MinValue, int64_t MaxValue) const {
    return Kind == Immediate && inRange(getImm(), MinValue, MaxValue);
  }

  bool isImm8() const { return isImm(-128, 127); }

  bool isImm8_sh8() const {
    return isImm(-32768, 32512) &&
           ((cast<MCConstantExpr>(getImm())->getValue() & 0xFF) == 0);
  }

  bool isImm12() const { return isImm(-2048, 2047); }

  // Convert MOVI to literal load, when immediate is not in range (-2048, 2047)
  bool isImm12m() const { return Kind == Immediate; }

  bool isOffset4m32() const {
    return isImm(0, 60) &&
           ((cast<MCConstantExpr>(getImm())->getValue() & 0x3) == 0);
  }

  bool isOffset8m8() const { return isImm(0, 255); }

  bool isOffset8m16() const {
    return isImm(0, 510) &&
           ((cast<MCConstantExpr>(getImm())->getValue() & 0x1) == 0);
  }

  bool isOffset8m32() const {
    return isImm(0, 1020) &&
           ((cast<MCConstantExpr>(getImm())->getValue() & 0x3) == 0);
  }

  bool isentry_imm12() const {
    return isImm(0, 32760) &&
           ((cast<MCConstantExpr>(getImm())->getValue() % 8) == 0);
  }

  bool isUimm2() const { return isImm(0, 3); }
  bool isUimm4() const { return isImm(0, 15); }
  bool isUimm4_x8() const {
    return isImm(0, 120) &&
           ((cast<MCConstantExpr>(getImm())->getValue() % 8) == 0);
  }

  bool isUimm4_x16() const {
    return isImm(0, 240) &&
           ((cast<MCConstantExpr>(getImm())->getValue() % 16) == 0);
  }

  bool isUimm8_x4() const {
    return isImm(0, 1020) &&
           ((cast<MCConstantExpr>(getImm())->getValue() % 4) == 0);
  }



  bool isUimm5() const { return isImm(0, 31); }
  bool isUimm6() const { return isImm(0, 63); }
  bool isUimm8() const { return isImm(0, 255); }

  bool isImm8n_7() const { return isImm(-8, 7); }

  bool isShimm1_31() const { return isImm(1, 31); }

  bool isImm8_x4_add8() const {
    return isImm(-64, 56) &&
           ((cast<MCConstantExpr>(getImm())->getValue() % 8) == 0);
  }

  bool isImm8n_7_x2() const {
    return isImm(-16, 14) &&
           ((cast<MCConstantExpr>(getImm())->getValue() % 2) == 0);
  }

  bool isImm8n_7_x4() const {
    return isImm(-32, 28) &&
           ((cast<MCConstantExpr>(getImm())->getValue() % 4) == 0);
  }

  bool isImm8n_7_x8() const {
    return isImm(-64, 56) &&
           ((cast<MCConstantExpr>(getImm())->getValue() % 8) == 0);
  }

  bool isImm16_31() const { return isImm(16, 31); }

  bool isImm1_16() const { return isImm(1, 16); }

  // Check that value is either equals (-1) or from [1,15] range.
  bool isImm1n_15() const { return isImm(1, 15) || isImm(-1, -1); }

  bool isImm32n_95() const { return isImm(-32, 95); }

  bool isImm64n_4n() const {
    return isImm(-64, -4) &&
           ((cast<MCConstantExpr>(getImm())->getValue() & 0x3) == 0);
  }

  bool isB4const() const {
    if (Kind != Immediate)
      return false;
    if (auto *CE = dyn_cast<MCConstantExpr>(getImm())) {
      int64_t Value = CE->getValue();
      switch (Value) {
      case -1:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 10:
      case 12:
      case 16:
      case 32:
      case 64:
      case 128:
      case 256:
        return true;
      default:
        return false;
      }
    }
    return false;
  }

  bool isB4constu() const {
    if (Kind != Immediate)
      return false;
    if (auto *CE = dyn_cast<MCConstantExpr>(getImm())) {
      int64_t Value = CE->getValue();
      switch (Value) {
      case 32768:
      case 65536:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 10:
      case 12:
      case 16:
      case 32:
      case 64:
      case 128:
      case 256:
        return true;
      default:
        return false;
      }
    }
    return false;
  }

  bool isimm7_22() const { return isImm(7, 22); }

  /// getStartLoc - Gets location of the first token of this operand
  SMLoc getStartLoc() const override { return StartLoc; }
  /// getEndLoc - Gets location of the last token of this operand
  SMLoc getEndLoc() const override { return EndLoc; }

  MCRegister getReg() const override {
    assert(Kind == Register && "Invalid type access!");
    return Reg.RegNum;
  }

  const MCExpr *getImm() const {
    assert(Kind == Immediate && "Invalid type access!");
    return Imm.Val;
  }

  StringRef getToken() const {
    assert(Kind == Token && "Invalid type access!");
    return Tok;
  }

  void print(raw_ostream &OS, const MCAsmInfo &MAI) const override {
    switch (Kind) {
    case Immediate:
      MAI.printExpr(OS, *getImm());
      break;
    case Register:
      OS << "<register x";
      OS << getReg() << ">";
      break;
    case Token:
      OS << "'" << getToken() << "'";
      break;
    }
  }

  static std::unique_ptr<XtensaOperand> createToken(StringRef Str, SMLoc S) {
    auto Op = std::make_unique<XtensaOperand>(Token);
    Op->Tok = Str;
    Op->StartLoc = S;
    Op->EndLoc = S;
    return Op;
  }

  static std::unique_ptr<XtensaOperand> createReg(unsigned RegNo, SMLoc S,
                                                  SMLoc E) {
    auto Op = std::make_unique<XtensaOperand>(Register);
    Op->Reg.RegNum = RegNo;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<XtensaOperand> createImm(const MCExpr *Val, SMLoc S,
                                                  SMLoc E) {
    auto Op = std::make_unique<XtensaOperand>(Immediate);
    Op->Imm.Val = Val;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  void addExpr(MCInst &Inst, const MCExpr *Expr) const {
    assert(Expr && "Expr shouldn't be null!");
    int64_t Imm = 0;
    bool IsConstant = false;

    if (auto *CE = dyn_cast<MCConstantExpr>(Expr)) {
      IsConstant = true;
      Imm = CE->getValue();
    }

    if (IsConstant)
      Inst.addOperand(MCOperand::createImm(Imm));
    else
      Inst.addOperand(MCOperand::createExpr(Expr));
  }

  // Used by the TableGen Code
  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getReg()));
  }

  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    addExpr(Inst, getImm());
  }
};

#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "XtensaGenAsmMatcher.inc"

unsigned XtensaAsmParser::validateTargetOperandClass(MCParsedAsmOperand &AsmOp,
                                                     unsigned Kind) {
  return Match_InvalidOperand;
}

static SMLoc RefineErrorLoc(const SMLoc Loc, const OperandVector &Operands,
                            uint64_t ErrorInfo) {
  if (ErrorInfo != ~0ULL && ErrorInfo < Operands.size()) {
    SMLoc ErrorLoc = Operands[ErrorInfo]->getStartLoc();
    if (ErrorLoc == SMLoc())
      return Loc;
    return ErrorLoc;
  }
  return Loc;
}

bool XtensaAsmParser::processInstruction(MCInst &Inst, SMLoc IDLoc,
                                         MCStreamer &Out,
                                         const MCSubtargetInfo *STI) {
  Inst.setLoc(IDLoc);
  const unsigned Opcode = Inst.getOpcode();
  switch (Opcode) {
  case Xtensa::J: {
    if (TrampolinesEnabled) {
      Inst.setFlags(Inst.getFlags() | Xtensa::XtensaJJumpTrampolinesEnabled);
    } else {
      Inst.setFlags(Inst.getFlags() | Xtensa::XtensaJJumpTrampolinesDisabled);
    }
    break;
  }
  case Xtensa::L32R: {
    if (AutoLitpoolsEnabled) {
      Inst.setFlags(Inst.getFlags() | Xtensa::XtensaL32RAutoLitpoolsEnabled);
    } else {
      Inst.setFlags(Inst.getFlags() | Xtensa::XtensaL32RAutoLitpoolsDisabled);
    }
    const MCSymbolRefExpr *OpExpr =
        static_cast<const MCSymbolRefExpr *>(Inst.getOperand(1).getExpr());
    Inst.getOperand(1).setExpr(OpExpr);
    break;
  }
  case Xtensa::MOVI: {
    XtensaTargetStreamer &TS = this->getTargetStreamer();

    // Expand MOVI operand
    if (TransformEnabled) {
      if (!Inst.getOperand(1).isExpr()) {
        uint64_t ImmOp64 = Inst.getOperand(1).getImm();
        int32_t Imm = ImmOp64;
        if (!isInt<12>(Imm)) {
          XtensaTargetStreamer &TS = this->getTargetStreamer();
          MCInst TmpInst;
          TmpInst.setLoc(IDLoc);
          TmpInst.setOpcode(Xtensa::L32R);
          if (AutoLitpoolsEnabled) {
            TmpInst.setFlags(TmpInst.getFlags() | Xtensa::XtensaL32RAutoLitpoolsEnabled);
          } else {
            TmpInst.setFlags(TmpInst.getFlags() | Xtensa::XtensaL32RAutoLitpoolsDisabled);
          }
          const MCExpr *Value = MCConstantExpr::create(ImmOp64, getContext());
          MCSymbol *Sym = getContext().createTempSymbol();
          const MCExpr *Expr = MCSymbolRefExpr::create(Sym, getContext());
          TmpInst.addOperand(Inst.getOperand(0));
          MCOperand Op1 = MCOperand::createExpr(Expr);
          TmpInst.addOperand(Op1);
          TS.emitLiteral(Sym, Value, true, IDLoc);
          Inst = TmpInst;
        }
      } else {
        MCInst TmpInst;
        TmpInst.setLoc(IDLoc);
        TmpInst.setOpcode(Xtensa::L32R);
        if (AutoLitpoolsEnabled) {
          TmpInst.setFlags(TmpInst.getFlags() | Xtensa::XtensaL32RAutoLitpoolsEnabled);
        } else {
          TmpInst.setFlags(TmpInst.getFlags() | Xtensa::XtensaL32RAutoLitpoolsDisabled);
        }
        const MCExpr *Value = Inst.getOperand(1).getExpr();
        MCSymbol *Sym = getContext().createTempSymbol();
        const MCExpr *Expr = MCSymbolRefExpr::create(Sym, getContext());
        TmpInst.addOperand(Inst.getOperand(0));
        MCOperand Op1 = MCOperand::createExpr(Expr);
        TmpInst.addOperand(Op1);
        Inst = TmpInst;
        TS.emitLiteral(Sym, Value, true, IDLoc);
      }
    } else {
      if (!Inst.getOperand(1).isExpr()) {
        uint64_t ImmOp64 = Inst.getOperand(1).getImm();
        int32_t Imm = ImmOp64;
        if (!isInt<12>(Imm)) {
          return Error(IDLoc, "operand out of range");
        }
      } else {
        return Error(IDLoc, "operand must be immediate when transform is disabled");
      }
    }
    break;
  }
  case Xtensa::CALL0:
  case Xtensa::CALL4:
  case Xtensa::CALL8:
  case Xtensa::CALL12: {
    if (LongcallsEnabled) {
      unsigned Reg;
      unsigned CallXOpcode;
      if (Opcode == Xtensa::CALL0) {
        Reg = Xtensa::A0;
        CallXOpcode = Xtensa::CALLX0;
      } else if (Opcode == Xtensa::CALL4) {
        Reg = Xtensa::A4;
        CallXOpcode = Xtensa::CALLX4;
      } else if (Opcode == Xtensa::CALL8) {
        Reg = Xtensa::A8;
        CallXOpcode = Xtensa::CALLX8;
      } else {
        Reg = Xtensa::A12;
        CallXOpcode = Xtensa::CALLX12;
      }

      XtensaTargetStreamer &TS = this->getTargetStreamer();
      const MCExpr *Value = Inst.getOperand(0).getExpr();
      MCSymbol *Sym = getContext().createTempSymbol();
      const MCExpr *Expr = MCSymbolRefExpr::create(Sym, getContext());

      MCInst L32RInst;
      L32RInst.setLoc(IDLoc);
      L32RInst.setOpcode(Xtensa::L32R);
      if (AutoLitpoolsEnabled) {
        L32RInst.setFlags(L32RInst.getFlags() | Xtensa::XtensaL32RAutoLitpoolsEnabled);
      } else {
        L32RInst.setFlags(L32RInst.getFlags() | Xtensa::XtensaL32RAutoLitpoolsDisabled);
      }
      L32RInst.addOperand(MCOperand::createReg(Reg));
      L32RInst.addOperand(MCOperand::createExpr(Expr));
      Out.emitInstruction(L32RInst, *STI);

      TS.emitLiteral(Sym, Value, true, IDLoc);

      MCInst CallXInst;
      CallXInst.setLoc(IDLoc);
      CallXInst.setOpcode(CallXOpcode);
      CallXInst.addOperand(MCOperand::createReg(Reg));
      Inst = CallXInst;
    }
    break;
  }
  case Xtensa::LOOP: {
    Out.emitValueToAlignment(Align(4));
    break;
  }
  default:
    break;
  }

  MCInst CInst;
  if (Xtensa::compress(CInst, Inst, *STI)) {
    Inst = CInst;
  }

  return true;
}

static bool isStandaloneHiFiInstr(StringRef Name) {
  return StringSwitch<bool>(Name)
      .StartsWith("AE_L32_I_HIFI4", true)
      .StartsWith("AE_L16_I_HIFI4", true)
      .StartsWith("AE_L32_X_HIFI4", true)
      .StartsWith("AE_L32_I_HIFI3", true)
      .StartsWith("AE_L16_I_HIFI3", true)
      .StartsWith("AE_L32_I_HIFI5", true)
      .StartsWith("AE_L16_I_HIFI5", true)
      .StartsWith("AE_L32_X_HIFI5", true)
      .StartsWith("AE_S32_L_I_HIFI4", true)
      .StartsWith("AE_S16_I_HIFI4", true)
      .StartsWith("AE_S32_X_HIFI4", true)
      .StartsWith("AE_S32_L_I_HIFI3", true)
      .StartsWith("AE_S32_L_X_HIFI4", true)
      .StartsWith("AE_L32_IP_HIFI4", true)
      .StartsWith("AE_L32_IP_HIFI3", true)
      .StartsWith("AE_S32_L_I_HIFI5", true)
      .StartsWith("AE_S16_I_HIFI5", true)
      .StartsWith("AE_S32_X_HIFI5", true)
      .StartsWith("AE_LA64_PP_HIFI4", true)
      .StartsWith("AE_LA64_PP_HIFI3", true)
      .StartsWith("AE_LA32X2_IP_HIFI4", true)
      .StartsWith("AE_LA32X2_IP_HIFI3", true)
      .StartsWith("AE_LA16X4_IP_HIFI4", true)
      .StartsWith("AE_LA16X4_IP_HIFI3", true)
      .StartsWith("AE_L16X4_IP_HIFI4", true)
      .StartsWith("AE_L16X4_IP_HIFI3", true)
      .StartsWith("AE_L16X4_XP_HIFI4", true)
      .StartsWith("AE_L16X4_XP_HIFI3", true)
      .StartsWith("AE_L32_XC_HIFI4", true)
      .StartsWith("AE_L32_XC_HIFI3", true)
      .StartsWith("AE_S16X4_IP_HIFI4", true)
      .StartsWith("AE_S16X4_IP_HIFI3", true)
      .StartsWith("AE_S16_0_IP_HIFI4", true)
      .StartsWith("AE_S16_0_IP_HIFI3", true)
      .StartsWith("AE_S32_L_IP_HIFI4", true)
      .StartsWith("AE_S32_L_IP_HIFI3", true)
      .StartsWith("AE_L16_IP_HIFI4", true)
      .StartsWith("AE_L16_IP_HIFI3", true)
      .StartsWith("AE_L16X4_I", true)
      .StartsWith("AE_L16X4_X", true)
      .StartsWith("AE_L16X4_XC", true)
      .StartsWith("AE_S16X4_I", true)
      .StartsWith("AE_S16X4_X", true)
      .StartsWith("AE_S16X4_XC", true)
      .StartsWith("AE_S16X4_XP", true)
      .StartsWith("AE_MUL32_LL_HIFI3", true)
      .StartsWith("AE_MULAFD32X16X2_FIR_HH", true)
      .StartsWith("AE_S32X2X2_XC1_HIFI5", true)
      .StartsWith("AE_ABS24S_HIFI3", true)
      .StartsWith("AE_ABS32_HIFI3", true)
      .StartsWith("AE_ABS64_HIFI3", true)
      .StartsWith("AE_ABS64S_HIFI3", true)
      .StartsWith("AE_ABSSQ56S_HIFI3", true)
      .StartsWith("AE_L16_X_HIFI3", true)
      .StartsWith("AE_L16M_I_HIFI3", true)
      .StartsWith("AE_MAX32_HIFI3", true)
      .StartsWith("AE_MIN32_HIFI3", true)
      .StartsWith("AE_ABS16S_HIFI3", true)
      .StartsWith("AE_ABS32S_HIFI3", true)
      .StartsWith("AE_NEG16S_HIFI3", true)
      .StartsWith("AE_NEG32S_HIFI3", true)
      .StartsWith("AE_SRAI32_HIFI3", true)
      .StartsWith("AE_SLAI32_HIFI3", true)
      .StartsWith("AE_SLAI32S_HIFI3", true)
      .StartsWith("AE_SRAI64_HIFI3", true)
      .StartsWith("AE_SLAI64S_HIFI3", true)
      .StartsWith("AE_SLAA64S_HIFI3", true)
      .StartsWith("AE_SRAI32R_HIFI3", true)
      .StartsWith("AE_ADD16_HIFI3", true)
      .StartsWith("AE_ADD32_HIFI3", true)
      .StartsWith("AE_ADD32S_HIFI3", true)
      .StartsWith("AE_ADD64_HIFI3", true)
      .StartsWith("AE_SUB16_HIFI3", true)
      .StartsWith("AE_SUB32_HIFI3", true)
      .StartsWith("AE_SUB32S_HIFI3", true)
      .StartsWith("AE_SUB64_HIFI3", true)
      .StartsWith("AE_ROUND32X2F48SASYM_HIFI3", true)
      .StartsWith("AE_ROUND32X2F48SSYM_HIFI3", true)
      .StartsWith("AE_NSAZ32_L", true)
      .StartsWith("AE_SEL16I", true)
      .StartsWith("AE_L64_I_HIFI3", true)
      .StartsWith("AE_L16M_X", true)
      .StartsWith("AE_L16M_XU", true)
      .StartsWith("AE_L32F24_XC", true)
      .StartsWith("AE_L32M_X", true)
      .StartsWith("AE_L32X2F24_IP", true)
      .StartsWith("AE_L32X2F24_XC", true)
      .StartsWith("AE_L32X2_I", true)
      .StartsWith("AE_L32X2_IP", true)
      .StartsWith("AE_L32X2_XC", true)
      .StartsWith("AE_L32X2_XC1", true)
      .StartsWith("AE_S64_I_HIFI3", true)
      .StartsWith("AE_S16_0_X", true)
      .StartsWith("AE_S16_0_XC", true)
      .StartsWith("AE_S16_0_XC1", true)
      .StartsWith("AE_S16_0_XP", true)
      .StartsWith("AE_L16_XP", true)
      .StartsWith("AE_L32_XP", true)
      .StartsWith("AE_S32_L_XP", true)
      .StartsWith("AE_S32_L_X", true)
      .StartsWith("AE_L32_X", true)
      .StartsWith("AE_L32_XC1", true)
      .StartsWith("AE_S32_L_XC1", true)
      .StartsWith("AE_L16_XC", true)
      .StartsWith("AE_S32_L_XC", true)
      .StartsWith("AE_S32X2_I", true)
      .StartsWith("AE_S16X2M_I", true)
      .StartsWith("AE_S32X2_IP", true)
      .StartsWith("AE_S32X2_X", true)
      .StartsWith("AE_S32X2_XC", true)
      .StartsWith("AE_S32X2_XC1", true)
      .StartsWith("AE_LA128_PP_HIFI5", true)
      .StartsWith("AE_SA128POS_FP_HIFI5", true)
      .StartsWith("AE_CVT48A32", true)
      .StartsWith("AE_MOVAD32_L", true)
      .StartsWith("AE_MOVAD32_H", true)
      .StartsWith("AE_MOVDA32", true)
      .StartsWith("AE_MOVAD16_0", true)
      .StartsWith("AE_MOVAD16_2", true)
      .StartsWith("AE_MOVAD16_3", true)
      .StartsWith("AE_MOVDA32X2", true)
      .StartsWith("AE_SEXT32X2D16_10", true)
      .StartsWith("AE_SEXT32X2D16_32", true)
      .StartsWith("AE_ROUND16X4F32SSYM", true)
      .StartsWith("AE_SRAA32", true)
      .StartsWith("AE_AND_HIFI3", true)
      .StartsWith("AE_OR_HIFI3", true)
      .StartsWith("AE_XOR_HIFI3", true)
      .StartsWith("AE_ADD16S", true)
      .StartsWith("AE_MULAAAAFQ32X16_HIFI5", true)
      .StartsWith("AE_MULAAF2D32RA_HH_LL_HIFI5", true)
      .StartsWith("AE_MULAAFP24S_HH_LL_HIFI3", true)
      .StartsWith("AE_MULF32RA_LL_REAL", true)
      .StartsWith("AE_MULF32RA_LH_REAL", true)
      .StartsWith("AE_MULF32RA_HH_REAL", true)
      .StartsWith("AE_MULSF32RA_LL_REAL", true)
      .StartsWith("AE_MULSF32RA_LH_REAL", true)
      .StartsWith("AE_MULSF32RA_HH_REAL", true)
      .StartsWith("AE_L64_I_REAL", true)
      .StartsWith("AE_S64_I_REAL", true)
      .StartsWith("AE_S32X2F24_I", true)
      .StartsWith("AE_S32X2F24_IP", true)
      .StartsWith("AE_LA32X2X2_IP_HIFI5", true)
      .StartsWith("AE_LA16X4X2_IP_HIFI5", true)
      .StartsWith("AE_L32X2X2_XC_HIFI5", true)
      .StartsWith("AE_SA32X2X2_IP_HIFI5", true)
      .StartsWith("AE_SA16X4X2_IP_HIFI5", true)
      .StartsWith("AE_MULA2Q32X16_FIR_H5_HIFI5", true)
      .StartsWith("AE_MOVAE", true)
      .StartsWith("AE_MOVEA", true)
      .StartsWith("AE_MOVFCRFSRV", true)
      .StartsWith("AE_MOVVFCRFSR", true)
      .StartsWith("AE_ZALIGN64", true)
      .StartsWith("AE_LALIGN64_I", true)
      .StartsWith("AE_SALIGN64_I", true)
      .StartsWith("AE_SB", true)
      .StartsWith("AE_SBI", true)
      .StartsWith("AE_SEXT16", true)
      .StartsWith("AE_ZEXT16", true)
      .StartsWith("AE_MOVDA16", true)
      .StartsWith("AE_MOVAD16_1", true)
      .StartsWith("AE_MOVAB2", true)
      .StartsWith("AE_CVTP24A16X2_LL", true)
      .StartsWith("AE_ROUND16X4F32SASYM", true)
      .StartsWith("AE_MOV", true)
      .StartsWith("AE_SLAA16S", true)
      .StartsWith("AE_SLAA32", true)
      .StartsWith("AE_SLAA32S", true)
      .StartsWith("AE_SLAA64", true)
      .StartsWith("AE_SRAA32S", true)
      .StartsWith("AE_SRAA64", true)
      .StartsWith("AE_SRAAQ56", true)
      .StartsWith("AE_SLAI24S", true)
      .StartsWith("AE_SLAI64", true)
      .StartsWith("AE_SRAA16RS", true)
      .StartsWith("AE_SRAA32RS", true)
      .StartsWith("AE_SLAI16S", true)
      .StartsWith("AE_ADD24S", true)
      .StartsWith("AE_SUB16S", true)
      .StartsWith("AE_PKSR32_REAL", true)
      .StartsWith("AE_MULAFD32X16X2_FIR_HL_REAL", true)
      .StartsWith("AE_MULAFD32X16X2_FIR_LH_REAL", true)
      .StartsWith("AE_MULAFD32X16X2_FIR_LL_REAL", true)
      .StartsWith("AE_MUL16X4_REAL", true)
      .StartsWith("AE_EQ16_REAL", true)
      .StartsWith("AE_LT16_REAL", true)
      .StartsWith("AE_LE16_REAL", true)
      .StartsWith("AE_EQ32_REAL", true)
      .StartsWith("AE_LT32_REAL", true)
      .StartsWith("AE_LE32_REAL", true)
      .StartsWith("AE_ROUND32X2F64SSYM_REAL", true)
      .StartsWith("AE_ROUND32X2F64SASYM_REAL", true)
      .StartsWith("AE_MULAF16SS_11_REAL", true)
      .StartsWith("AE_MULAF16SS_22_REAL", true)
      .StartsWith("AE_MULAF16SS_33_REAL", true)
      .StartsWith("AE_MULF16SS_11_REAL", true)
      .StartsWith("AE_MULF16SS_22_REAL", true)
      .StartsWith("AE_MULF16SS_33_REAL", true)
      .StartsWith("AE_LA24_IP_REAL", true)
      .StartsWith("AE_LA24X2_IP_REAL", true)
      .StartsWith("AE_SA16X4_IP_REAL", true)
      .StartsWith("AE_SA16X4_IC_REAL", true)
      .StartsWith("AE_ROUND24X2F48SSYM_REAL", true)
      .StartsWith("AE_SA32X2_IP_REAL", true)
      .StartsWith("AE_SA32X2_IC_REAL", true)
      .StartsWith("AE_SA24X2_IP_REAL", true)
      .StartsWith("AE_SA64POS_FP_REAL", true)
      .StartsWith("AE_LA16X4POS_PC_REAL", true)
      .StartsWith("AE_LA32X2POS_PC_REAL", true)
      .StartsWith("AE_LALIGN128_I", true)
      .StartsWith("AE_SALIGN128_I", true)
      .StartsWith("AE_MOVDRZBVC", true)
      .StartsWith("AE_MOVZBVCDR", true)
      .StartsWith("AE_S32_H_I", true)
      .StartsWith("AE_MULF2P32X16X4RS", true)
      .StartsWith("AE_MULF2P32X4RS", true)
      .StartsWith("AE_MULF32X2R_HH_LL", true)
      .StartsWith("AE_MOVAD16_1", true)
      .StartsWith("AE_ROUND16X4F32SASYM", true)
      .StartsWith("AE_MULAFD32X16X2_FIR", true)
      .StartsWith("AE_ROUND16X4F32SSYM", true)
      .StartsWith("AE_ROUND32X2F64SSYM", true)
      .StartsWith("AE_ROUND32X2F64SASYM", true)
      .StartsWith("AE_MUL16X4", true)
      .StartsWith("AE_MULAF16SS", true)
      .StartsWith("AE_MULF16SS", true)
      .StartsWith("AE_EQ16", true)
      .StartsWith("AE_LT16", true)
      .StartsWith("AE_LE16", true)
      .StartsWith("AE_LE32", true)
      .StartsWith("AE_SA16X4_IC", true)
      .StartsWith("AE_SA24X2_IP", true)
      .StartsWith("AE_LA16X4POS_PC", true)
      .StartsWith("AE_ROUND24X2F48SSYM", true)
      .StartsWith("AE_SA32X2_IC", true)
      .StartsWith("AE_S16_0_X", true)
      .StartsWith("AE_S32_L_X", true)
      .StartsWith("AE_L32_X", true)
      .StartsWith("AE_L32X2_XC1", true)
      .StartsWith("AE_MOVVFCRFSR", true)
      .StartsWith("AE_MOVFCRFSRV", true)
      .StartsWith("AE_MOVDRZBVC", true)
      .StartsWith("AE_MOVZBVCDR", true)
      .Default(false);
}

bool XtensaAsmParser::matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                              OperandVector &Operands,
                                              MCStreamer &Out,
                                              uint64_t &ErrorInfo,
                                              bool MatchingInlineAsm) {
  XtensaOperand &FirstOperand = static_cast<XtensaOperand &>(*Operands[0]);
  if (FirstOperand.isToken() && FirstOperand.getToken() == "{") {
    if (InBrackets) {
      return Error(IDLoc, "Already in a bundle");
    }
    InBrackets = true;
    CurrentBundle.clear();
    CurrentBundle.setOpcode(Xtensa::BUNDLE);
    return false;
  }
  if (FirstOperand.isToken() && FirstOperand.getToken() == "}") {
    if (!InBrackets) {
      return Error(IDLoc, "Not in a bundle");
    }
    InBrackets = false;
    CurrentBundle.setLoc(IDLoc);
    Out.emitInstruction(CurrentBundle, getSTI());
    CurrentBundle.clear();
    return false;
  }

  if (FirstOperand.isToken() && FirstOperand.getToken() == "addi" && Operands.size() == 4) {
    XtensaOperand &Op1 = static_cast<XtensaOperand &>(*Operands[1]);
    XtensaOperand &Op2 = static_cast<XtensaOperand &>(*Operands[2]);
    XtensaOperand &Op3 = static_cast<XtensaOperand &>(*Operands[3]);
    if (Op1.isReg() && Op2.isReg() && Op3.isImm()) {
      const MCExpr *Expr = Op3.getImm();
      int64_t Value;
      if (Expr->evaluateAsAbsolute(Value)) {
        if (Value < -128 || Value > 127) {
          int64_t ValSh8 = ((Value + 128) >> 8) << 8;
          int64_t ValRem = Value - ValSh8;
          if (ValSh8 >= -32768 && ValSh8 <= 32512 && ValRem >= -128 && ValRem <= 127) {
            MCInst Inst1;
            Inst1.setOpcode(Xtensa::ADDMI);
            Inst1.addOperand(MCOperand::createReg(Op1.getReg()));
            Inst1.addOperand(MCOperand::createReg(Op2.getReg()));
            Inst1.addOperand(MCOperand::createImm(ValSh8));
            Inst1.setLoc(IDLoc);
            if (InBrackets) {
              MCInst *SubInst = getParser().getContext().createMCInst();
              *SubInst = Inst1;
              CurrentBundle.addOperand(MCOperand::createInst(SubInst));
            } else {
              Out.emitInstruction(Inst1, getSTI());
            }

            if (ValRem != 0) {
              MCInst Inst2;
              Inst2.setOpcode(Xtensa::ADDI);
              Inst2.addOperand(MCOperand::createReg(Op1.getReg()));
              Inst2.addOperand(MCOperand::createReg(Op1.getReg()));
              Inst2.addOperand(MCOperand::createImm(ValRem));
              Inst2.setLoc(IDLoc);
              if (InBrackets) {
                MCInst *SubInst = getParser().getContext().createMCInst();
                *SubInst = Inst2;
                CurrentBundle.addOperand(MCOperand::createInst(SubInst));
              } else {
                Out.emitInstruction(Inst2, getSTI());
              }
            }
            return false;
          }
        }
      }
    }
  }

  MCInst Inst;
  auto Result =
      MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm);

  switch (Result) {
  default:
    break;
  case Match_Success: {
    processInstruction(Inst, IDLoc, Out, STI);
    Inst.setLoc(IDLoc);
    if (InBrackets) {
      MCInst *SubInst = getParser().getContext().createMCInst();
      *SubInst = Inst;
      CurrentBundle.addOperand(MCOperand::createInst(SubInst));
    } else {
      if (MII.getName(Inst.getOpcode()).starts_with("AE_") &&
          !isStandaloneHiFiInstr(MII.getName(Inst.getOpcode()))) {
        MCInst StandaloneBundle;
        StandaloneBundle.setOpcode(Xtensa::BUNDLE);
        StandaloneBundle.setLoc(IDLoc);
        MCInst *SubInst = getParser().getContext().createMCInst();
        *SubInst = Inst;
        StandaloneBundle.addOperand(MCOperand::createInst(SubInst));
        Out.emitInstruction(StandaloneBundle, getSTI());
      } else {
        Out.emitInstruction(Inst, getSTI());
      }
    }
    return false;
  }
  case Match_MissingFeature:
    return Error(IDLoc, "instruction use requires an option to be enabled");
  case Match_MnemonicFail:
    return Error(IDLoc, "unrecognized instruction mnemonic");
  case Match_InvalidOperand: {
    SMLoc ErrorLoc = IDLoc;
    if (ErrorInfo != ~0U) {
      if (ErrorInfo >= Operands.size())
        return Error(ErrorLoc, "too few operands for instruction");

      ErrorLoc = ((XtensaOperand &)*Operands[ErrorInfo]).getStartLoc();
      if (ErrorLoc == SMLoc())
        ErrorLoc = IDLoc;
    }
    return Error(ErrorLoc, "invalid operand for instruction");
  }
  case Match_InvalidImm8:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [-128, 127]");
  case Match_InvalidImm8_sh8:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [-32768, 32512], first 8 bits "
                 "should be zero");
  case Match_InvalidB4const:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected b4const immediate");
  case Match_InvalidB4constu:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected b4constu immediate");
  case Match_InvalidImm12:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [-2048, 2047]");
  case Match_InvalidImm12m:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [-2048, 2047]");
  case Match_InvalidImm1_16:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [1, 16]");
  case Match_InvalidImm1n_15:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [-1, 15] except 0");
  case Match_InvalidImm32n_95:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [-32, 95]");
  case Match_InvalidImm64n_4n:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [-64, -4]");
  case Match_InvalidImm8n_7:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [-8, 7]");
  case Match_InvalidShimm1_31:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [1, 31]");
  case Match_InvalidUimm4:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [0, 15]");
  case Match_InvalidUimm4_x16:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [0, 240] and multiple of 16");
  case Match_InvalidUimm8_x4:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [0, 1020] and multiple of 4");
  case Match_InvalidUimm5:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [0, 31]");
  case Match_InvalidUimm6:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [0, 63]");
  case Match_InvalidUimm8:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [0, 255]");
  case Match_InvalidOffset8m8:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [0, 255]");
  case Match_InvalidOffset8m16:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [0, 510], first bit "
                 "should be zero");
  case Match_InvalidOffset8m32:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [0, 1020], first 2 bits "
                 "should be zero");
  case Match_InvalidOffset4m32:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [0, 60], first 2 bits "
                 "should be zero");
  case Match_Invalidentry_imm12:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [0, 32760], first 3 bits "
                 "should be zero");
  case Match_Invalidimm7_22:
    return Error(RefineErrorLoc(IDLoc, Operands, ErrorInfo),
                 "expected immediate in range [7, 22]");
  }

  report_fatal_error("Unknown match type detected!");
}

ParseStatus XtensaAsmParser::parsePCRelTarget(OperandVector &Operands) {
  MCAsmParser &Parser = getParser();
  LLVM_DEBUG(dbgs() << "parsePCRelTarget\n");

  SMLoc S = getLexer().getLoc();

  // Expressions are acceptable
  const MCExpr *Expr = nullptr;
  if (Parser.parseExpression(Expr)) {
    // We have no way of knowing if a symbol was consumed so we must ParseFail
    return ParseStatus::Failure;
  }

  // Currently not support constants
  if (Expr->getKind() == MCExpr::ExprKind::Constant)
    return Error(getLoc(), "unknown operand");

  Operands.push_back(XtensaOperand::createImm(Expr, S, getLexer().getLoc()));
  return ParseStatus::Success;
}

bool XtensaAsmParser::parseRegister(MCRegister &Reg, SMLoc &StartLoc,
                                    SMLoc &EndLoc) {
  const AsmToken &Tok = getParser().getTok();
  StartLoc = Tok.getLoc();
  EndLoc = Tok.getEndLoc();
  Reg = Xtensa::NoRegister;
  StringRef Name = getLexer().getTok().getIdentifier();

  if (!MatchRegisterName(Name) && !MatchRegisterAltName(Name)) {
    getParser().Lex(); // Eat identifier token.
    return false;
  }

  return Error(StartLoc, "invalid register name");
}

ParseStatus XtensaAsmParser::parseRegister(OperandVector &Operands,
                                           bool AllowParens,
                                           XtensaRegisterType RegType,
                                           Xtensa::RegisterAccessType RAType) {
  SMLoc FirstS = getLoc();
  bool HadParens = false;
  AsmToken Buf[2];
  StringRef RegName;

  // If this a parenthesised register name is allowed, parse it atomically
  if (AllowParens && getLexer().is(AsmToken::LParen)) {
    size_t ReadCount = getLexer().peekTokens(Buf);
    if (ReadCount == 2 && Buf[1].getKind() == AsmToken::RParen) {
      if (Buf[0].getKind() == AsmToken::Integer && RegType == Xtensa_Generic)
        return ParseStatus::NoMatch;
      HadParens = true;
      getParser().Lex(); // Eat '('
    }
  }

  MCRegister RegNo = 0;

  switch (getLexer().getKind()) {
  default:
    return ParseStatus::NoMatch;
  case AsmToken::Integer:
    if (RegType == Xtensa_Generic)
      return ParseStatus::NoMatch;

    // Parse case when we expect UR register code as special case,
    // because SR and UR registers may have the same number
    // and such situation may lead to confilct
    if (RegType == Xtensa_UR) {
      int64_t RegCode = getLexer().getTok().getIntVal();
      RegNo = Xtensa::getUserRegister(RegCode, MRI);
    } else {
      RegName = getLexer().getTok().getString();
      RegNo = MatchRegisterAltName(RegName);
    }
    break;
  case AsmToken::Identifier:
    RegName = getLexer().getTok().getIdentifier();
    RegNo = MatchRegisterName(RegName);
    if (RegNo == 0)
      RegNo = MatchRegisterAltName(RegName);
    break;
  }

  if (RegNo == 0) {
    if (HadParens)
      getLexer().UnLex(Buf[0]);
    return ParseStatus::NoMatch;
  }

  if (!Xtensa::checkRegister(RegNo, getSTI().getFeatureBits(), RAType))
    return ParseStatus::NoMatch;

  if (HadParens)
    Operands.push_back(XtensaOperand::createToken("(", FirstS));
  SMLoc S = getLoc();
  SMLoc E = getParser().getTok().getEndLoc();
  getLexer().Lex();
  Operands.push_back(XtensaOperand::createReg(RegNo, S, E));

  if (HadParens) {
    getParser().Lex(); // Eat ')'
    Operands.push_back(XtensaOperand::createToken(")", getLoc()));
  }

  return ParseStatus::Success;
}

ParseStatus XtensaAsmParser::parseImmediate(OperandVector &Operands) {
  SMLoc S = getLoc();
  SMLoc E;
  const MCExpr *Res;

  switch (getLexer().getKind()) {
  default:
    return ParseStatus::NoMatch;
  case AsmToken::LParen:
  case AsmToken::Minus:
  case AsmToken::Plus:
  case AsmToken::Tilde:
  case AsmToken::Integer:
  case AsmToken::String:
  case AsmToken::Identifier:
    if (getParser().parseExpression(Res))
      return ParseStatus::Failure;
    break;
  case AsmToken::Percent:
    return parseOperandWithModifier(Operands);
  }

  E = SMLoc::getFromPointer(S.getPointer() - 1);
  Operands.push_back(XtensaOperand::createImm(Res, S, E));
  return ParseStatus::Success;
}

ParseStatus XtensaAsmParser::parseOperandWithModifier(OperandVector &Operands) {
  return ParseStatus::Failure;
}

/// Looks at a token type and creates the relevant operand
/// from this information, adding to Operands.
/// If operand was parsed, returns false, else true.
bool XtensaAsmParser::parseOperand(OperandVector &Operands, StringRef Mnemonic,
                                   XtensaRegisterType RegType,
                                   Xtensa::RegisterAccessType RAType) {
  // Check if the current operand has a custom associated parser, if so, try to
  // custom parse the operand, or fallback to the general approach.
  ParseStatus Res = MatchOperandParserImpl(Operands, Mnemonic);
  if (Res.isSuccess())
    return false;

  // If there wasn't a custom match, try the generic matcher below. Otherwise,
  // there was a match, but an error occurred, in which case, just return that
  // the operand parsing failed.
  if (Res.isFailure())
    return true;

  // Attempt to parse token as register
  if (parseRegister(Operands, true, RegType, RAType).isSuccess())
    return false;

  // Attempt to parse token as an immediate
  if (parseImmediate(Operands).isSuccess())
    return false;

  // Finally we have exhausted all options and must declare defeat.
  return Error(getLoc(), "unknown operand");
}

bool XtensaAsmParser::ParseInstructionWithSR(ParseInstructionInfo &Info,
                                             StringRef Name, SMLoc NameLoc,
                                             OperandVector &Operands) {
  Xtensa::RegisterAccessType RAType =
      Name[0] == 'w' ? Xtensa::REGISTER_WRITE
                     : (Name[0] == 'r' ? Xtensa::REGISTER_READ
                                       : Xtensa::REGISTER_EXCHANGE);

  if ((Name.size() > 4) && Name[3] == '.') {
    // Parse case when instruction name is concatenated with SR/UR register
    // name, like "wsr.sar a1" or "wur.fcr a1"

    // First operand is token for instruction
    Operands.push_back(XtensaOperand::createToken(Name.take_front(3), NameLoc));

    StringRef RegName = Name.drop_front(4);
    unsigned RegNo = MatchRegisterName(RegName);

    if (RegNo == 0)
      RegNo = MatchRegisterAltName(RegName);

    if (!Xtensa::checkRegister(RegNo, getSTI().getFeatureBits(), RAType))
      return Error(NameLoc, "invalid register name");

    // Parse operand
    if (parseOperand(Operands, Name))
      return true;

    SMLoc S = getLoc();
    SMLoc E = SMLoc::getFromPointer(S.getPointer() - 1);
    Operands.push_back(XtensaOperand::createReg(RegNo, S, E));
  } else {
    // First operand is token for instruction
    Operands.push_back(XtensaOperand::createToken(Name, NameLoc));

    // Parse first operand
    if (parseOperand(Operands, Name))
      return true;

    if (!parseOptionalToken(AsmToken::Comma)) {
      SMLoc Loc = getLexer().getLoc();
      getParser().eatToEndOfStatement();
      return Error(Loc, "unexpected token");
    }

    // Parse second operand
    if (parseOperand(Operands, Name, Name[1] == 's' ? Xtensa_SR : Xtensa_UR,
                     RAType))
      return true;
  }

  if (getLexer().isNot(AsmToken::EndOfStatement)) {
    SMLoc Loc = getLexer().getLoc();
    getParser().eatToEndOfStatement();
    return Error(Loc, "unexpected token");
  }

  getParser().Lex(); // Consume the EndOfStatement.
  return false;
}

bool XtensaAsmParser::parseInstruction(ParseInstructionInfo &Info,
                                       StringRef Name, SMLoc NameLoc,
                                       OperandVector &Operands) {
  if (Name.starts_with("_"))
    Name = Name.drop_front(1);
  if (Name.ends_with(".l") || Name.ends_with(".w"))
    Name = Name.drop_back(2);

  if (Name == "{" || Name == "}") {
    Operands.push_back(XtensaOperand::createToken(Name, NameLoc));
    return false;
  }

  if (Name.starts_with("wsr") || Name.starts_with("rsr") ||
      Name.starts_with("xsr") || Name.starts_with("rur") ||
      Name.starts_with("wur")) {
    return ParseInstructionWithSR(Info, Name, NameLoc, Operands);
  }

  // First operand is token for instruction
  Operands.push_back(XtensaOperand::createToken(Name, NameLoc));

  // If there are no more operands, then finish
  if (getLexer().is(AsmToken::EndOfStatement))
    return false;

  // Parse first operand
  if (parseOperand(Operands, Name))
    return true;

  // Parse until end of statement, consuming commas between operands
  while (parseOptionalToken(AsmToken::Comma))
    if (parseOperand(Operands, Name))
      return true;

  if (getLexer().is(AsmToken::RCurly)) {
    return false;
  }

  if (getLexer().isNot(AsmToken::EndOfStatement)) {
    SMLoc Loc = getLexer().getLoc();
    getParser().eatToEndOfStatement();
    return Error(Loc, "unexpected token");
  }

  getParser().Lex(); // Consume the EndOfStatement.
  return false;
}

bool XtensaAsmParser::parseLiteralDirective(SMLoc L) {
  MCAsmParser &Parser = getParser();
  const MCExpr *Value;
  SMLoc LiteralLoc = getLexer().getLoc();
  XtensaTargetStreamer &TS = this->getTargetStreamer();

  if (Parser.parseExpression(Value))
    return true;

  const MCSymbolRefExpr *SE = dyn_cast<MCSymbolRefExpr>(Value);

  if (!SE)
    return Error(LiteralLoc, "literal label must be a symbol");

  if (Parser.parseComma())
    return true;

  SMLoc OpcodeLoc = getLexer().getLoc();
  if (parseOptionalToken(AsmToken::EndOfStatement))
    return Error(OpcodeLoc, "expected value");

  if (Parser.parseExpression(Value))
    return true;

  if (parseEOL())
    return true;

  MCSymbol *Sym = getContext().getOrCreateSymbol(SE->getSymbol().getName());

  TS.emitLiteral(Sym, Value, true, LiteralLoc);

  return false;
}

ParseStatus XtensaAsmParser::parseDirective(AsmToken DirectiveID) {
  StringRef IDVal = DirectiveID.getString();
  SMLoc Loc = getLexer().getLoc();

  if (IDVal == ".literal_position") {
    XtensaTargetStreamer &TS = this->getTargetStreamer();
    TS.emitLiteralPosition();
    return parseEOL();
  }

  if (IDVal == ".literal") {
    return parseLiteralDirective(Loc);
  }

  if (IDVal == ".struct") {
    MCAsmParser &Parser = getParser();
    const MCExpr *ExprVal;
    if (Parser.parseExpression(ExprVal))
      return ParseStatus::Failure;
    if (parseEOL())
      return ParseStatus::Failure;

    int64_t OffsetVal = 0;
    if (!ExprVal->evaluateAsAbsolute(OffsetVal))
      return Error(Loc, "expected absolute expression");

    MCContext &Ctx = getContext();
    unsigned StructID = ++StructUniqueID;
    MCSectionELF *StructSec = Ctx.getELFSection(".struct", ELF::SHT_NOBITS, 0, 0, "", false, StructID, nullptr);
    getParser().getStreamer().switchSection(StructSec);

    if (OffsetVal > 0) {
      getParser().getStreamer().emitZeros(OffsetVal);
    }
    return false;
  }

  if (IDVal == ".begin") {
    AsmToken ArgTok = getLexer().getTok();
    if (ArgTok.is(AsmToken::Identifier)) {
      StringRef FirstArg = ArgTok.getString();
      if (FirstArg == "longcalls") {
        LongcallsEnabled = true;
        getParser().Lex(); // consume 'longcalls' token
        return parseEOL();
      }
      if (FirstArg == "literal_prefix") {
        getParser().Lex(); // consume 'literal_prefix' token
        StringRef Prefix = "";
        if (getLexer().isNot(AsmToken::EndOfStatement)) {
          if (getParser().parseIdentifier(Prefix)) {
            return Error(getLexer().getLoc(), "expected literal prefix name");
          }
        }
        XtensaTargetStreamer &TS = this->getTargetStreamer();
        TS.emitLiteralPrefix(Prefix);
        return parseEOL();
      }
      std::string FullArg = "";
      SMLoc StartLoc = getLexer().getLoc();
      while (getLexer().isNot(AsmToken::EndOfStatement) &&
             getLexer().isNot(AsmToken::Eof)) {
        FullArg += getLexer().getTok().getString().str();
        getParser().Lex();
      }
      if (FullArg == "absolute-literals") {
        AbsoluteLiteralsStack.push_back(getTargetStreamer().isAbsoluteLiteralsEnabled());
        getTargetStreamer().setAbsoluteLiterals(true);
        return parseEOL();
      }
      if (FullArg == "no-absolute-literals") {
        AbsoluteLiteralsStack.push_back(getTargetStreamer().isAbsoluteLiteralsEnabled());
        getTargetStreamer().setAbsoluteLiterals(false);
        return parseEOL();
      }
      if (FullArg == "transform") {
        TransformStack.push_back(TransformEnabled);
        TransformEnabled = true;
        return parseEOL();
      }
      if (FullArg == "no-transform") {
        TransformStack.push_back(TransformEnabled);
        TransformEnabled = false;
        return parseEOL();
      }
      if (FullArg == "trampolines") {
        TrampolinesStack.push_back(TrampolinesEnabled);
        TrampolinesEnabled = true;
        return parseEOL();
      }
      if (FullArg == "no-trampolines") {
        TrampolinesStack.push_back(TrampolinesEnabled);
        TrampolinesEnabled = false;
        return parseEOL();
      }
      if (FullArg == "auto-litpools") {
        AutoLitpoolsStack.push_back(AutoLitpoolsEnabled);
        AutoLitpoolsEnabled = true;
        getTargetStreamer().setAutoLitpools(true);
        return parseEOL();
      }
      if (FullArg == "no-auto-litpools") {
        AutoLitpoolsStack.push_back(AutoLitpoolsEnabled);
        AutoLitpoolsEnabled = false;
        getTargetStreamer().setAutoLitpools(false);
        return parseEOL();
      }
      if (FullArg == "schedule") {
        return parseEOL();
      }
      return Error(StartLoc, "unrecognized argument to .begin directive");
    }
    return ParseStatus::NoMatch;
  }

  if (IDVal == ".end") {
    AsmToken ArgTok = getLexer().getTok();
    if (ArgTok.is(AsmToken::Identifier)) {
      StringRef FirstArg = ArgTok.getString();
      if (FirstArg == "longcalls") {
        LongcallsEnabled = false;
        getParser().Lex(); // consume 'longcalls' token
        return parseEOL();
      }
      if (FirstArg == "literal_prefix") {
        getParser().Lex(); // consume 'literal_prefix' token
        XtensaTargetStreamer &TS = this->getTargetStreamer();
        TS.emitLiteralPrefixEnd();
        return parseEOL();
      }
      std::string FullArg = "";
      SMLoc StartLoc = getLexer().getLoc();
      while (getLexer().isNot(AsmToken::EndOfStatement) &&
             getLexer().isNot(AsmToken::Eof)) {
        FullArg += getLexer().getTok().getString().str();
        getParser().Lex();
      }
      if (FullArg == "absolute-literals" || FullArg == "no-absolute-literals") {
        if (!AbsoluteLiteralsStack.empty()) {
          bool Prev = AbsoluteLiteralsStack.back();
          AbsoluteLiteralsStack.pop_back();
          getTargetStreamer().setAbsoluteLiterals(Prev);
        }
        return parseEOL();
      }
      if (FullArg == "transform" || FullArg == "no-transform") {
        if (!TransformStack.empty()) {
          bool Prev = TransformStack.back();
          TransformStack.pop_back();
          TransformEnabled = Prev;
        }
        return parseEOL();
      }
      if (FullArg == "trampolines" || FullArg == "no-trampolines") {
        if (!TrampolinesStack.empty()) {
          bool Prev = TrampolinesStack.back();
          TrampolinesStack.pop_back();
          TrampolinesEnabled = Prev;
        }
        return parseEOL();
      }
      if (FullArg == "auto-litpools" || FullArg == "no-auto-litpools") {
        if (!AutoLitpoolsStack.empty()) {
          bool Prev = AutoLitpoolsStack.back();
          AutoLitpoolsStack.pop_back();
          AutoLitpoolsEnabled = Prev;
          getTargetStreamer().setAutoLitpools(Prev);
        }
        return parseEOL();
      }
      if (FullArg == "schedule") {
        return parseEOL();
      }
      return Error(StartLoc, "unrecognized argument to .end directive");
    }
    return ParseStatus::NoMatch;
  }

  return ParseStatus::NoMatch;
}
void XtensaAsmParser::onLabelParsed(MCSymbol *Symbol) {
  MCSection *CurSec = getParser().getStreamer().getCurrentSectionOnly();
  if (CurSec && CurSec->getName() == ".struct") {
    uint64_t Offset = getSectionCurrentOffset(CurSec);
    Symbol->setVariableValue(MCConstantExpr::create(Offset, getContext()));
  }
}

uint64_t XtensaAsmParser::getSectionCurrentOffset(const MCSection *Sec) {
  uint64_t Offset = 0;
  for (const MCFragment &F : *Sec) {
    switch (F.getKind()) {
    case MCFragment::FT_Data:
    case MCFragment::FT_Relaxable:
    case MCFragment::FT_LEB:
    case MCFragment::FT_Dwarf:
    case MCFragment::FT_DwarfFrame:
    case MCFragment::FT_SFrame:
      Offset += F.getContents().size() + F.getVarContents().size();
      break;
    case MCFragment::FT_Align: {
      Align Alignment = F.getAlignment();
      uint64_t Padding = offsetToAlignment(Offset, Alignment);
      if (Padding > F.getAlignMaxBytesToEmit())
        Padding = 0;
      Offset += Padding;
      break;
    }
    case MCFragment::FT_Fill: {
      const MCFillFragment &FF = cast<MCFillFragment>(F);
      int64_t NumValues = 0;
      if (FF.getNumValues().evaluateAsAbsolute(NumValues)) {
        Offset += NumValues * FF.getValueSize();
      }
      break;
    }
    default:
      break;
    }
  }
  return Offset;
}

// Force static initialization.
extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeXtensaAsmParser() {
  RegisterMCAsmParser<XtensaAsmParser> X(getTheXtensaTarget());
}
