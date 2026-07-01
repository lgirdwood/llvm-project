//===-- XtensaDisassembler.cpp - Disassembler for Xtensa ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the XtensaDisassembler class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/XtensaMCTargetDesc.h"
#include "TargetInfo/XtensaTargetInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDecoder.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Endian.h"
#include "llvm/MC/MCInstrInfo.h"
#include <memory>
#include <set>

using namespace llvm;
using namespace llvm::MCD;

#define DEBUG_TYPE "Xtensa-disassembler"

using DecodeStatus = MCDisassembler::DecodeStatus;

namespace {

class XtensaDisassembler : public MCDisassembler {
  bool IsLittleEndian;
  std::unique_ptr<const MCInstrInfo> MCII;
  mutable std::set<uint64_t> LiteralAddresses;
  mutable std::set<uint64_t> ScannedSections;

public:
  XtensaDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx, bool isLE,
                     std::unique_ptr<const MCInstrInfo> MCII)
      : MCDisassembler(STI, Ctx), IsLittleEndian(isLE), MCII(std::move(MCII)) {}

  bool hasDensity() const { return STI.hasFeature(Xtensa::FeatureDensity); }
  bool hasHIFI3() const { return STI.hasFeature(Xtensa::FeatureHIFI3); }
  bool hasHIFI4() const { return STI.hasFeature(Xtensa::FeatureHIFI4); }
  bool hasHIFI5() const { return STI.hasFeature(Xtensa::FeatureHIFI5); }
  bool hasFLIX() const { return STI.hasFeature(Xtensa::FeatureFLIX); }

  bool isVLIWOnlyInstruction(unsigned Opcode) const {
    if (!MCII) return false;
    StringRef Name = MCII->getName(Opcode);
    if (Name.ends_with("_REAL"))
      return true;
    if (Name.starts_with("AE_SAT") || Name.starts_with("AE_MAXABS") || 
        Name.starts_with("AE_ROUND") || Name.starts_with("AE_MUL"))
      return true;
    return false;
  }

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;

  bool decodeSlotVal(MCInst &MI, uint32_t Val, uint64_t Address) const;
};
} // end anonymous namespace

static MCDisassembler *createXtensaDisassembler(const Target &T,
                                                const MCSubtargetInfo &STI,
                                                MCContext &Ctx) {
  std::unique_ptr<const MCInstrInfo> MCII(T.createMCInstrInfo());
  return new XtensaDisassembler(STI, Ctx, true, std::move(MCII));
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeXtensaDisassembler() {
  TargetRegistry::RegisterMCDisassembler(getTheXtensaTarget(),
                                         createXtensaDisassembler);
}

const MCPhysReg ARDecoderTable[] = {
    Xtensa::A0,  Xtensa::SP,  Xtensa::A2,  Xtensa::A3, Xtensa::A4,  Xtensa::A5,
    Xtensa::A6,  Xtensa::A7,  Xtensa::A8,  Xtensa::A9, Xtensa::A10, Xtensa::A11,
    Xtensa::A12, Xtensa::A13, Xtensa::A14, Xtensa::A15};

static DecodeStatus DecodeARRegisterClass(MCInst &Inst, uint64_t RegNo,
                                          uint64_t Address,
                                          const void *Decoder) {
  if (RegNo >= std::size(ARDecoderTable))
    return MCDisassembler::Fail;

  MCPhysReg Reg = ARDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

const MCPhysReg AEDRDecoderTable[] = {
    Xtensa::AED0,  Xtensa::AED1,  Xtensa::AED2,  Xtensa::AED3,
    Xtensa::AED4,  Xtensa::AED5,  Xtensa::AED6,  Xtensa::AED7,
    Xtensa::AED8,  Xtensa::AED9,  Xtensa::AED10, Xtensa::AED11,
    Xtensa::AED12, Xtensa::AED13, Xtensa::AED14, Xtensa::AED15};

static DecodeStatus DecodeAEDRRegisterClass(MCInst &Inst, uint64_t RegNo,
                                            uint64_t Address,
                                            const void *Decoder) {
  if (RegNo >= std::size(AEDRDecoderTable))
    return MCDisassembler::Fail;

  MCPhysReg Reg = AEDRDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeMRRegisterClass(MCInst &Inst, uint64_t RegNo,
                                          uint64_t Address,
                                          const void *Decoder) {
  if (RegNo > 3)
    return MCDisassembler::Fail;

  MCPhysReg Reg = Xtensa::M0 + RegNo;
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeMR01RegisterClass(MCInst &Inst, uint64_t RegNo,
                                            uint64_t Address,
                                            const void *Decoder) {
  if (RegNo > 1)
    return MCDisassembler::Fail;

  MCPhysReg Reg = Xtensa::M0 + RegNo;
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeMR23RegisterClass(MCInst &Inst, uint64_t RegNo,
                                            uint64_t Address,
                                            const void *Decoder) {
  if (RegNo > 1)
    return MCDisassembler::Fail;

  MCPhysReg Reg = Xtensa::M2 + RegNo;
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeFPRRegisterClass(MCInst &Inst, uint64_t RegNo,
                                           uint64_t Address,
                                           const void *Decoder) {
  if (RegNo > 15)
    return MCDisassembler::Fail;

  MCPhysReg Reg = Xtensa::F0 + RegNo;
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeURRegisterClass(MCInst &Inst, uint64_t RegNo,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  if (RegNo > 255)
    return MCDisassembler::Fail;

  Xtensa::RegisterAccessType RAType = Inst.getOpcode() == Xtensa::WUR
                                          ? Xtensa::REGISTER_WRITE
                                          : Xtensa::REGISTER_READ;

  const XtensaDisassembler *Dis =
      static_cast<const XtensaDisassembler *>(Decoder);
  const MCRegisterInfo *MRI = Dis->getContext().getRegisterInfo();
  MCPhysReg Reg = Xtensa::getUserRegister(RegNo, *MRI);
  if (!Xtensa::checkRegister(Reg, Decoder->getSubtargetInfo().getFeatureBits(),
                             RAType))
    return MCDisassembler::Fail;

  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

struct DecodeRegister {
  MCPhysReg Reg;
  uint32_t RegNo;
};

const DecodeRegister SRDecoderTable[] = {
    {Xtensa::LBEG, 0},         {Xtensa::LEND, 1},
    {Xtensa::LCOUNT, 2},       {Xtensa::SAR, 3},
    {Xtensa::BREG, 4},         {Xtensa::LITBASE, 5},
    {Xtensa::SCOMPARE1, 12},   {Xtensa::ACCLO, 16},
    {Xtensa::ACCHI, 17},       {Xtensa::M0, 32},
    {Xtensa::M1, 33},          {Xtensa::M2, 34},
    {Xtensa::M3, 35},          {Xtensa::PREFCTL, 40},
    {Xtensa::WINDOWBASE, 72},  {Xtensa::WINDOWSTART, 73},
    {Xtensa::IBREAKENABLE, 96}, {Xtensa::MEMCTL, 97},
    {Xtensa::ATOMCTL, 99},     {Xtensa::DDR, 104},
    {Xtensa::MESAVE, 108},     {Xtensa::IBREAKA0, 128},
    {Xtensa::IBREAKA1, 129},   {Xtensa::DBREAKA0, 144},
    {Xtensa::DBREAKA1, 145},   {Xtensa::DBREAKC0, 160},
    {Xtensa::DBREAKC1, 161},   {Xtensa::CONFIGID0, 176},
    {Xtensa::EPC1, 177},       {Xtensa::EPC2, 178},
    {Xtensa::EPC3, 179},       {Xtensa::EPC4, 180},
    {Xtensa::EPC5, 181},       {Xtensa::EPC6, 182},
    {Xtensa::EPC7, 183},       {Xtensa::DEPC, 192},
    {Xtensa::EPS2, 194},       {Xtensa::EPS3, 195},
    {Xtensa::EPS4, 196},       {Xtensa::EPS5, 197},
    {Xtensa::EPS6, 198},       {Xtensa::EPS7, 199},
    {Xtensa::CONFIGID1, 208},  {Xtensa::EXCSAVE1, 209},
    {Xtensa::EXCSAVE2, 210},   {Xtensa::EXCSAVE3, 211},
    {Xtensa::EXCSAVE4, 212},   {Xtensa::EXCSAVE5, 213},
    {Xtensa::EXCSAVE6, 214},   {Xtensa::EXCSAVE7, 215},
    {Xtensa::CPENABLE, 224},   {Xtensa::INTERRUPT, 226},
    {Xtensa::INTCLEAR, 227},   {Xtensa::INTENABLE, 228},
    {Xtensa::PS, 230},         {Xtensa::VECBASE, 231},
    {Xtensa::EXCCAUSE, 232},   {Xtensa::DEBUGCAUSE, 233},
    {Xtensa::CCOUNT, 234},     {Xtensa::PRID, 235},
    {Xtensa::ICOUNT, 236},     {Xtensa::ICOUNTLEVEL, 237},
    {Xtensa::EXCVADDR, 238},   {Xtensa::CCOMPARE0, 240},
    {Xtensa::CCOMPARE1, 241},  {Xtensa::CCOMPARE2, 242},
    {Xtensa::MISC0, 244},      {Xtensa::MISC1, 245},
    {Xtensa::MISC2, 246},      {Xtensa::MISC3, 247}};

static DecodeStatus DecodeSRRegisterClass(MCInst &Inst, uint64_t RegNo,
                                          uint64_t Address,
                                          const MCDisassembler *Decoder) {
  if (RegNo > 255)
    return MCDisassembler::Fail;

  Xtensa::RegisterAccessType RAType =
      Inst.getOpcode() == Xtensa::WSR
          ? Xtensa::REGISTER_WRITE
          : (Inst.getOpcode() == Xtensa::RSR ? Xtensa::REGISTER_READ
                                             : Xtensa::REGISTER_EXCHANGE);

  for (unsigned i = 0; i < std::size(SRDecoderTable); i++) {
    if (SRDecoderTable[i].RegNo == RegNo) {
      MCPhysReg Reg = SRDecoderTable[i].Reg;

      // Handle special case. The INTERRUPT/INTSET registers use the same
      // encoding, but INTERRUPT used for read and INTSET for write.
      if (Reg == Xtensa::INTERRUPT && RAType == Xtensa::REGISTER_WRITE) {
        Reg = Xtensa::INTSET;
      }

      if (!Xtensa::checkRegister(
              Reg, Decoder->getSubtargetInfo().getFeatureBits(), RAType))
        return MCDisassembler::Fail;

      Inst.addOperand(MCOperand::createReg(Reg));
      return MCDisassembler::Success;
    }
  }

  return MCDisassembler::Fail;
}

static DecodeStatus DecodeBRRegisterClass(MCInst &Inst, uint64_t RegNo,
                                          uint64_t Address,
                                          const void *Decoder) {
  if (RegNo > 15)
    return MCDisassembler::Fail;

  MCPhysReg Reg = Xtensa::B0 + RegNo;
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static const MCPhysReg VALIGNDecoderTable[] = {
    Xtensa::A0, Xtensa::SP, Xtensa::A2, Xtensa::A3};

static DecodeStatus DecodeVALIGNRegisterClass(MCInst &Inst, uint64_t RegNo,
                                           uint64_t Address,
                                           const void *Decoder) {
  if (RegNo >= 4)
    return MCDisassembler::Fail;
  Inst.addOperand(MCOperand::createReg(VALIGNDecoderTable[RegNo]));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeURAlignRegisterClass(MCInst &Inst, uint64_t RegNo,
                                               uint64_t Address,
                                               const void *Decoder) {
  if (RegNo >= 4)
    return MCDisassembler::Fail;
  Inst.addOperand(MCOperand::createReg(VALIGNDecoderTable[RegNo]));
  return MCDisassembler::Success;
}

static DecodeStatus DecodeBR_AEPRegisterClass(MCInst &Inst, uint64_t RegNo,
                                              uint64_t Address,
                                              const void *Decoder) {
  if (RegNo > 3)
    return MCDisassembler::Fail;

  MCPhysReg Reg = Xtensa::B0 + RegNo;
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static bool tryAddingSymbolicOperand(int64_t Value, bool isBranch,
                                     uint64_t Address, uint64_t Offset,
                                     uint64_t InstSize, MCInst &MI,
                                     const void *Decoder) {
  const MCDisassembler *Dis = static_cast<const MCDisassembler *>(Decoder);
  return Dis->tryAddingSymbolicOperand(MI, Value, Address, isBranch, Offset,
                                       /*OpSize=*/0, InstSize);
}

static DecodeStatus decodeCallOperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<18>(Imm) && "Invalid immediate");
  Inst.addOperand(
      MCOperand::createImm(SignExtend64<20>(Imm << 2) + (Address & 0x3)));
  return MCDisassembler::Success;
}

static DecodeStatus decodeJumpOperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<18>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(SignExtend64<18>(Imm)));
  return MCDisassembler::Success;
}

static DecodeStatus decodeBranchOperand(MCInst &Inst, uint64_t Imm,
                                        int64_t Address, const void *Decoder) {
  switch (Inst.getOpcode()) {
  case Xtensa::BEQZ:
  case Xtensa::BGEZ:
  case Xtensa::BLTZ:
  case Xtensa::BNEZ:
    assert(isUInt<12>(Imm) && "Invalid immediate");
    if (!tryAddingSymbolicOperand(SignExtend64<12>(Imm) + 4 + Address, true,
                                  Address, 0, 3, Inst, Decoder))
      Inst.addOperand(MCOperand::createImm(SignExtend64<12>(Imm)));
    break;
  default:
    assert(isUInt<8>(Imm) && "Invalid immediate");
    if (!tryAddingSymbolicOperand(SignExtend64<8>(Imm) + 4 + Address, true,
                                  Address, 0, 3, Inst, Decoder))
      Inst.addOperand(MCOperand::createImm(SignExtend64<8>(Imm)));
  }
  return MCDisassembler::Success;
}

static DecodeStatus decodeLoopOperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {

  assert(isUInt<8>(Imm) && "Invalid immediate");
  if (!tryAddingSymbolicOperand(Imm + 4 + Address, true, Address, 0, 3, Inst,
                                Decoder))
    Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeL32ROperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {

  assert(isUInt<16>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(
      SignExtend64<17>((Imm << 2) + 0x40000 + (Address & 0x3))));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm8Operand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<8>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(SignExtend64<8>(Imm)));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm8_sh8Operand(MCInst &Inst, uint64_t Imm,
                                          int64_t Address,
                                          const void *Decoder) {
  assert(isUInt<8>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(SignExtend64<16>(Imm << 8)));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm12Operand(MCInst &Inst, uint64_t Imm,
                                       int64_t Address, const void *Decoder) {
  assert(isUInt<12>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(SignExtend64<12>(Imm)));
  return MCDisassembler::Success;
}

static DecodeStatus decodeUimm4Operand(MCInst &Inst, uint64_t Imm,
                                       int64_t Address, const void *Decoder) {
  assert(isUInt<4>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeUimm4_x8Operand(MCInst &Inst, uint64_t Imm,
                                          int64_t Address, const void *Decoder) {
  assert(isUInt<4>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm * 8));
  return MCDisassembler::Success;
}

static DecodeStatus decodeUimm4_x16Operand(MCInst &Inst, uint64_t Imm,
                                           int64_t Address, const void *Decoder) {
  assert(isUInt<4>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm * 16));
  return MCDisassembler::Success;
}

static DecodeStatus decodeUimm8_x4Operand(MCInst &Inst, uint64_t Imm,
                                          int64_t Address, const void *Decoder) {
  assert(isUInt<8>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm * 4));
  return MCDisassembler::Success;
}

static DecodeStatus decodeUimm8Operand(MCInst &Inst, uint64_t Imm,
                                       int64_t Address, const void *Decoder) {
  assert(isUInt<8>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}



static DecodeStatus decodeUimm5Operand(MCInst &Inst, uint64_t Imm,
                                       int64_t Address, const void *Decoder) {
  assert(isUInt<5>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeUimm2Operand(MCInst &Inst, uint64_t Imm,
                                       int64_t Address, const void *Decoder) {
  assert(isUInt<2>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeUimm6Operand(MCInst &Inst, uint64_t Imm,
                                       int64_t Address, const void *Decoder) {
  assert(isUInt<6>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm1_16Operand(MCInst &Inst, uint64_t Imm,
                                         int64_t Address, const void *Decoder) {
  assert(isUInt<4>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm + 1));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm1n_15Operand(MCInst &Inst, uint64_t Imm,
                                          int64_t Address,
                                          const void *Decoder) {
  assert(isUInt<4>(Imm) && "Invalid immediate");
  if (!Imm)
    Inst.addOperand(MCOperand::createImm(-1));
  else
    Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm32n_95Operand(MCInst &Inst, uint64_t Imm,
                                           int64_t Address,
                                           const void *Decoder) {
  assert(isUInt<7>(Imm) && "Invalid immediate");
  if ((Imm & 0x60) == 0x60)
    Inst.addOperand(MCOperand::createImm((~0x1f) | Imm));
  else
    Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm8n_7Operand(MCInst &Inst, uint64_t Imm,
                                         int64_t Address, const void *Decoder) {
  assert(isUInt<4>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm > 7 ? Imm - 16 : Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm64n_4nOperand(MCInst &Inst, uint64_t Imm,
                                           int64_t Address,
                                           const void *Decoder) {
  assert(isUInt<6>(Imm) && ((Imm & 0x3) == 0) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm((~0x3f) | (Imm)));
  return MCDisassembler::Success;
}

static DecodeStatus decodeEntry_Imm12OpValue(MCInst &Inst, uint64_t Imm,
                                             int64_t Address,
                                             const void *Decoder) {
  assert(isUInt<15>(Imm) && ((Imm & 0x7) == 0) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeShimm1_31Operand(MCInst &Inst, uint64_t Imm,
                                           int64_t Address,
                                           const void *Decoder) {
  assert(isUInt<5>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(32 - Imm));
  return MCDisassembler::Success;
}

static int64_t TableB4const[16] = {-1, 1,  2,  3,  4,  5,  6,   7,
                                   8,  10, 12, 16, 32, 64, 128, 256};
static DecodeStatus decodeB4constOperand(MCInst &Inst, uint64_t Imm,
                                         int64_t Address, const void *Decoder) {
  assert(isUInt<4>(Imm) && "Invalid immediate");

  Inst.addOperand(MCOperand::createImm(TableB4const[Imm]));
  return MCDisassembler::Success;
}

static int64_t TableB4constu[16] = {32768, 65536, 2,  3,  4,  5,  6,   7,
                                    8,     10,    12, 16, 32, 64, 128, 256};
static DecodeStatus decodeB4constuOperand(MCInst &Inst, uint64_t Imm,
                                          int64_t Address,
                                          const void *Decoder) {
  assert(isUInt<4>(Imm) && "Invalid immediate");

  Inst.addOperand(MCOperand::createImm(TableB4constu[Imm]));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm7_22Operand(MCInst &Inst, uint64_t Imm,
                                         int64_t Address, const void *Decoder) {
  assert(isUInt<4>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm + 7));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm8_x4_add8Operand(MCInst &Inst, uint64_t Imm,
                                              int64_t Address,
                                              const void *Decoder) {
  assert(isUInt<4>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(((int64_t)Imm - 8) * 8));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm8n_7_x2Operand(MCInst &Inst, uint64_t Imm,
                                          int64_t Address, const void *Decoder) {
  int64_t SignedImm = SignExtend64<4>(Imm);
  assert(isUInt<4>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(SignedImm * 2));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm8n_7_x4Operand(MCInst &Inst, uint64_t Imm,
                                          int64_t Address, const void *Decoder) {
  int64_t SignedImm = SignExtend64<4>(Imm);
  assert(isUInt<4>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(SignedImm * 4));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm8n_7_x8Operand(MCInst &Inst, uint64_t Imm,
                                          int64_t Address, const void *Decoder) {
  int64_t SignedImm = SignExtend64<4>(Imm);
  assert(isUInt<4>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(SignedImm * 8));
  return MCDisassembler::Success;
}

static DecodeStatus decodeMem8Operand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<12>(Imm) && "Invalid immediate");
  DecodeARRegisterClass(Inst, Imm & 0xf, Address, Decoder);
  Inst.addOperand(MCOperand::createImm((Imm >> 4) & 0xff));
  return MCDisassembler::Success;
}

static DecodeStatus decodeMem16Operand(MCInst &Inst, uint64_t Imm,
                                       int64_t Address, const void *Decoder) {
  assert(isUInt<12>(Imm) && "Invalid immediate");
  DecodeARRegisterClass(Inst, Imm & 0xf, Address, Decoder);
  Inst.addOperand(MCOperand::createImm((Imm >> 3) & 0x1fe));
  return MCDisassembler::Success;
}

static DecodeStatus decodeMem32Operand(MCInst &Inst, uint64_t Imm,
                                       int64_t Address, const void *Decoder) {
  assert(isUInt<12>(Imm) && "Invalid immediate");
  DecodeARRegisterClass(Inst, Imm & 0xf, Address, Decoder);
  Inst.addOperand(MCOperand::createImm((Imm >> 2) & 0x3fc));
  return MCDisassembler::Success;
}

static DecodeStatus decodeMem32nOperand(MCInst &Inst, uint64_t Imm,
                                        int64_t Address, const void *Decoder) {
  assert(isUInt<8>(Imm) && "Invalid immediate");
  DecodeARRegisterClass(Inst, Imm & 0xf, Address, Decoder);
  Inst.addOperand(MCOperand::createImm((Imm >> 2) & 0x3c));
  return MCDisassembler::Success;
}

/// Read two bytes from the ArrayRef and return 16 bit data sorted
/// according to the given endianness.
static DecodeStatus readInstruction16(ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      uint64_t &Size, uint64_t &Insn,
                                      bool IsLittleEndian) {
  // We want to read exactly 2 Bytes of data.
  if (Bytes.size() < 2) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  if (!IsLittleEndian) {
    report_fatal_error("Big-endian mode currently is not supported!");
  } else {
    Insn = (Bytes[1] << 8) | Bytes[0];
  }

  return MCDisassembler::Success;
}

/// Read three bytes from the ArrayRef and return 24 bit data
static DecodeStatus readInstruction24(ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      uint64_t &Size, uint64_t &Insn,
                                      bool IsLittleEndian) {
  // We want to read exactly 3 Bytes of data.
  if (Bytes.size() < 3) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  if (!IsLittleEndian) {
    report_fatal_error("Big-endian mode currently is not supported!");
  } else {
    Insn = (Bytes[2] << 16) | (Bytes[1] << 8) | (Bytes[0] << 0);
  }

  return MCDisassembler::Success;
}

static DecodeStatus readInstruction48(ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      uint64_t &Size, uint64_t &Insn,
                                      bool IsLittleEndian) {
  if (Bytes.size() < 6) {
    Size = 0;
    return MCDisassembler::Fail;
  }
  if (!IsLittleEndian) {
    report_fatal_error("Big-endian mode currently is not supported!");
  } else {
    Insn = 0;
    for (unsigned i = 0; i < 6; ++i) {
      Insn |= (uint64_t(Bytes[i]) << (i * 8));
    }
  }
  return MCDisassembler::Success;
}

static DecodeStatus readInstruction64(ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      uint64_t &Size, uint64_t &Insn,
                                      bool IsLittleEndian) {
  if (Bytes.size() < 8) {
    Size = 0;
    return MCDisassembler::Fail;
  }
  if (!IsLittleEndian) {
    report_fatal_error("Big-endian mode currently is not supported!");
  } else {
    Insn = 0;
    for (unsigned i = 0; i < 8; ++i) {
      Insn |= (uint64_t(Bytes[i]) << (i * 8));
    }
  }
  return MCDisassembler::Success;
}

#include "XtensaGenDisassemblerTables.inc"

static bool isLiteralAddress(uint64_t Addr, const std::set<uint64_t>& CandidateLiterals) {
  auto it = CandidateLiterals.upper_bound(Addr);
  if (it != CandidateLiterals.begin()) {
    --it;
    if (Addr < *it + 4)
      return true;
  }
  return false;
}

DecodeStatus XtensaDisassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                                ArrayRef<uint8_t> Bytes,
                                                uint64_t Address,
                                                raw_ostream &CS) const {
  static thread_local bool IsScanning = false;
  static thread_local uint64_t MaxScannedAddress = 0;
  if (Address >= 0x1000 && !IsScanning && Address >= MaxScannedAddress) {
    IsScanning = true;
    MaxScannedAddress = Address + Bytes.size();
    uint64_t ScanOffset = 0;
    std::set<uint64_t> CandidateLiterals;
    // Pass 1: Scan for Candidate Literals
    while (ScanOffset < Bytes.size()) {
      MCInst TmpMI;
      uint64_t TmpSize = 0;
      uint64_t CurrAddr = Address + ScanOffset;
      DecodeStatus Status = getInstruction(TmpMI, TmpSize, Bytes.slice(ScanOffset), CurrAddr, nulls());
      if (Status == MCDisassembler::Fail || TmpSize == 0) {
        ScanOffset += 1;
        continue;
      }
      if (TmpMI.getOpcode() == Xtensa::L32R) {
        if (TmpMI.getNumOperands() > 1 && TmpMI.getOperand(1).isImm()) {
          int64_t TargetVal = TmpMI.getOperand(1).getImm();
          TargetVal &= ~0x3;
          uint64_t PC = CurrAddr;
          uint64_t TargetAddr = ((PC + 3) & ~3) + TargetVal;
          uint64_t Diff = (PC > TargetAddr) ? (PC - TargetAddr) : (TargetAddr - PC);
          if (Diff <= 0x40000 && (TargetAddr % 4 == 0)) {
            CandidateLiterals.insert(TargetAddr);
          }
        }
      }
      ScanOffset += TmpSize;
    }

    // Pass 2: Scan for valid Code Addresses, avoiding Candidate Literals
    ScanOffset = 0;
    std::set<uint64_t> CodeAddresses;
    while (ScanOffset < Bytes.size()) {
      uint64_t CurrAddr = Address + ScanOffset;
      if (isLiteralAddress(CurrAddr, CandidateLiterals)) {
        ScanOffset += 1;
        continue;
      }
      MCInst TmpMI;
      uint64_t TmpSize = 0;
      DecodeStatus Status = getInstruction(TmpMI, TmpSize, Bytes.slice(ScanOffset), CurrAddr, nulls());
      if (Status == MCDisassembler::Fail || TmpSize == 0) {
        ScanOffset += 1;
        continue;
      }
      CodeAddresses.insert(CurrAddr);
      ScanOffset += TmpSize;
    }

    for (uint64_t LitAddr : CandidateLiterals) {
      if (!CodeAddresses.count(LitAddr)) {
        for (unsigned i = 0; i < 4; ++i) {
          LiteralAddresses.insert(LitAddr + i);
        }
      }
    }
    IsScanning = false;
  }

  if (Address >= 0x1000 && !IsScanning && LiteralAddresses.count(Address)) {
    Size = std::min<uint64_t>(4, Bytes.size());
    return MCDisassembler::Fail;
  }

  Size = 0;
  uint64_t Insn;
  DecodeStatus Result;

  // Manual disassembly of 128-bit instructions (AE_MOVDRZBVC, AE_MOVZBVCDR)
  if (hasHIFI5() && Bytes.size() >= 16) {
    if (Bytes[0] == 0x4f && Bytes[1] == 0x61 && Bytes[2] == 0x08 && Bytes[3] == 0xbb &&
        Bytes[4] == 0x53 && (Bytes[5] & 0x03) == 0x01 && Bytes[5] <= 0x3d &&
        (Bytes[6] == 0x0e || Bytes[6] == 0x1e) && Bytes[7] == 0x60 && Bytes[8] == 0x09 &&
        Bytes[9] == 0x99 && Bytes[10] == 0x00 && Bytes[11] == 0x5e && Bytes[12] == 0x80 &&
        Bytes[13] == 0xa4 && Bytes[14] == 0x01 && Bytes[15] == 0x00) {
      unsigned Opcode = (Bytes[6] == 0x0e) ? Xtensa::AE_MOVDRZBVC : Xtensa::AE_MOVZBVCDR;
      unsigned RegVal = (Bytes[5] - 1) >> 2;
      MI.setOpcode(Opcode);
      unsigned Reg = Xtensa::AED0 + RegVal;
      MI.addOperand(MCOperand::createReg(Reg));
      Size = 16;
      return MCDisassembler::Success;
    }
  }

  // Manual disassembly of custom 11-byte VLIW pseudo-instructions
  if (hasFLIX() && Bytes.size() >= 11) {
    uint32_t insn0 = Bytes[0] | (Bytes[1] << 8) | (Bytes[2] << 16) | (Bytes[3] << 24);
    uint32_t insn1 = Bytes[4] | (Bytes[5] << 8) | (Bytes[6] << 16) | (Bytes[7] << 24);
    uint32_t insn2 = Bytes[8] | (Bytes[9] << 8) | (Bytes[10] << 16);

    // 1. AE_MULAFD32X16X2_FIR_HH, HL, LH, LL
    if ((insn0 == 0x0d2c0700 || insn0 == 0x0d2c0f00 || insn0 == 0x0d2c1700 || insn0 == 0x0d2c1f00) &&
        insn2 == 0x0f3570) {
      if (insn0 == 0x0d2c0700) MI.setOpcode(Xtensa::AE_MULAFD32X16X2_FIR_HH);
      else if (insn0 == 0x0d2c0f00) MI.setOpcode(Xtensa::AE_MULAFD32X16X2_FIR_HL_REAL);
      else if (insn0 == 0x0d2c1700) MI.setOpcode(Xtensa::AE_MULAFD32X16X2_FIR_LH_REAL);
      else MI.setOpcode(Xtensa::AE_MULAFD32X16X2_FIR_LL_REAL);

      unsigned q0 = (insn1 >> 28) & 0xf;
      unsigned q1 = (insn1 >> 16) & 0xf;
      unsigned c  = (insn1 >> 2) & 0xf;
      unsigned d1 = (insn1 >> 9) & 0xf;
      unsigned d0 = (insn1 & 1) | (((insn1 >> 8) & 1) << 1) | (((insn1 >> 14) & 1) << 2) | (((insn1 >> 13) & 1) << 3);

      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[q0])); // t1
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[q1])); // t2
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[q0])); // q0
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[q1])); // q1
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[d0])); // d0
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[d1])); // d1
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[c]));  // c
      Size = 11;
      return MCDisassembler::Success;
    }

    // 2. AE_ROUND16X4F32SSYM
    if (Bytes[0] == 0x1f && Bytes[1] == 0x15 && Bytes[2] == 0x70 &&
        Bytes[8] == 0xc7 && Bytes[9] == 0x77 && Bytes[10] == 0xcb && (Bytes[7] & 0xfc) == 0xc4) {
      MI.setOpcode(Xtensa::AE_ROUND16X4F32SSYM);
      unsigned t = (insn1 >> 16) & 0xf;
      unsigned s = (insn1 >> 2) & 0xf;
      unsigned r_bit0 = (insn1 >> 6) & 1;
      unsigned r_bit1 = (insn1 >> 7) & 1;
      unsigned r_bit2 = Bytes[7] & 1;
      unsigned r_bit3 = (Bytes[7] >> 1) & 1;
      unsigned r = r_bit0 | (r_bit1 << 1) | (r_bit2 << 2) | (r_bit3 << 3);

      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      Size = 11;
      return MCDisassembler::Success;
    }

    // 3. AE_ROUND16X4F32SASYM
    if ((insn0 & 0xfeffffff) == 0xdcc777c9 && insn2 == 0x1f1570) {
      MI.setOpcode(Xtensa::AE_ROUND16X4F32SASYM);
      unsigned t = (insn1 >> 16) & 0xf;
      unsigned s = (insn1 >> 2) & 0xf;
      unsigned r_bit0 = (insn0 >> 24) & 1;
      unsigned r_bit1 = (insn1 >> 7) & 1;
      unsigned r_bit2 = (insn1 >> 5) & 1;
      unsigned r_bit3 = (insn1 >> 6) & 1;
      unsigned r = r_bit0 | (r_bit1 << 1) | (r_bit2 << 2) | (r_bit3 << 3);

      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      Size = 11;
      return MCDisassembler::Success;
    }

    // 4. AE_ROUND32X2F64SSYM_REAL
    if (Bytes[0] == 0xcd && Bytes[1] == 0x77 && Bytes[2] == 0xc7 && Bytes[3] == 0xd4 &&
        Bytes[8] == 0x70 && Bytes[9] == 0x15 && Bytes[10] == 0x1f) {
      MI.setOpcode(Xtensa::AE_ROUND32X2F64SSYM_REAL);
      unsigned t = (insn1 >> 16) & 0xf;
      unsigned s = (insn1 >> 6) & 0xf;
      unsigned r = (insn1 >> 2) & 0xf;

      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      Size = 11;
      return MCDisassembler::Success;
    }

    // 5. AE_ROUND32X2F64SASYM_REAL & AE_ROUND24X2F48SSYM_REAL
    if (Bytes[0] == 0x39 && Bytes[1] == 0x77 && Bytes[2] == 0xc6 && Bytes[3] == 0xc4 &&
        Bytes[8] == 0x70 && Bytes[9] == 0x00 && Bytes[10] == 0x1f) {
      unsigned sub = (insn1 >> 8) & 0xff;
      if (sub == 0x02) MI.setOpcode(Xtensa::AE_ROUND32X2F64SASYM_REAL);
      else MI.setOpcode(Xtensa::AE_ROUND24X2F48SSYM_REAL);

      unsigned t = (insn2 >> 12) & 0xf;
      unsigned s = (insn1 >> 20) & 0xf;
      unsigned r = Bytes[8] & 0xf;

      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      Size = 11;
      return MCDisassembler::Success;
    }
  }

  if (hasFLIX() && Bytes.size() >= 6 && (Bytes[0] & 0x0f) == 0x0e && Bytes[2] == 0x00 && Bytes[5] == 0x7c) {
    if (Bytes[3] == 0x60 && Bytes[4] == 0x9f) { // AE_L32X2_XC1
      unsigned t = Bytes[0] >> 4;
      unsigned r = Bytes[1] >> 4;
      unsigned s = Bytes[1] & 0xf;
      MI.setOpcode(Xtensa::AE_L32X2_XC1);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s_out
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      Size = 6;
      return MCDisassembler::Success;
    }
    if (Bytes[3] == 0xb0 && Bytes[4] == 0xa2) { // AE_S32_L_XC1
      unsigned t = Bytes[0] >> 4;
      unsigned r = Bytes[1] >> 4;
      unsigned s = Bytes[1] & 0xf;
      MI.setOpcode(Xtensa::AE_S32_L_XC1);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s_out
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      Size = 6;
      return MCDisassembler::Success;
    }
    if (Bytes[3] == 0x70 && Bytes[4] == 0xa2) { // AE_S32X2_XC1
      unsigned t = Bytes[0] >> 4;
      unsigned r = Bytes[1] >> 4;
      unsigned s = Bytes[1] & 0xf;
      MI.setOpcode(Xtensa::AE_S32X2_XC1);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s_out
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      Size = 6;
      return MCDisassembler::Success;
    }
  }

  // VLIW / FLIX bundle decoding
  if (hasFLIX() && Bytes.size() >= 6 && (Bytes[0] & 0x0f) == 0x0e) {
    // 48-bit (6-byte) bundle: Format 0, 1, or 2
    uint32_t insn0 = Bytes[0] | (Bytes[1] << 8) | (Bytes[2] << 16) | (Bytes[3] << 24);
    uint32_t insn1 = Bytes[4] | (Bytes[5] << 8);
    unsigned fmt_bits = Bytes[5] & 0xc0;

    if (fmt_bits == 0xc0) {
      // Format 0: 2 slots
      uint32_t val0 = ((insn0 >> 8) & 0xf) |
                      (((insn0 >> 4) & 0xf) << 4) |
                      (((insn0 >> 12) & 0xf) << 8) |
                      (((insn0 >> 28) & 0xf) << 12) |
                      ((insn1 & 0x3f) << 16);

      uint32_t val1 = ((insn0 >> 16) & 0xf) |
                      (((insn0 >> 24) & 0xf) << 4) |
                      (((insn0 >> 20) & 0xf) << 8) |
                      (((insn1 >> 6) & 0xff) << 12);

      MCInst *SubMI0 = getContext().createMCInst();
      MCInst *SubMI1 = getContext().createMCInst();
      if (decodeSlotVal(*SubMI0, val0, Address) && decodeSlotVal(*SubMI1, val1, Address)) {
        MI.setOpcode(Xtensa::BUNDLE);
        MI.addOperand(MCOperand::createInst(SubMI0));
        MI.addOperand(MCOperand::createInst(SubMI1));
        Size = 6;
        return MCDisassembler::Success;
      }
    } else if (fmt_bits == 0x80) {
      // Format 1: 2 slots
      uint32_t val0 = ((insn0 >> 8) & 0xf) |
                      (((insn0 >> 4) & 0xf) << 4) |
                      (((insn0 >> 12) & 0xf) << 8) |
                      (((insn0 >> 28) & 0xf) << 12) |
                      ((insn1 & 0xfff) << 16);

      uint32_t val1 = ((insn0 >> 16) & 0xf) |
                      (((insn0 >> 24) & 0xf) << 4) |
                      (((insn0 >> 20) & 0xf) << 8) |
                      (((insn1 >> 12) & 0x3) << 12);

      MCInst *SubMI0 = getContext().createMCInst();
      MCInst *SubMI1 = getContext().createMCInst();
      if (decodeSlotVal(*SubMI0, val0, Address) && decodeSlotVal(*SubMI1, val1, Address)) {
        MI.setOpcode(Xtensa::BUNDLE);
        MI.addOperand(MCOperand::createInst(SubMI0));
        MI.addOperand(MCOperand::createInst(SubMI1));
        Size = 6;
        return MCDisassembler::Success;
      }
    } else if (fmt_bits == 0x00) {
      // Format 2: 3 slots
      uint32_t val0 = ((insn0 >> 8) & 0xf) |
                      (((insn0 >> 4) & 0xf) << 4) |
                      (((insn0 >> 12) & 0xf) << 8) |
                      (((insn0 >> 28) & 0xf) << 12) |
                      ((insn1 & 0x3f) << 16);

      uint32_t val1 = (insn1 >> 6) & 0x1;

      uint32_t val2 = ((insn0 >> 16) & 0xfff) |
                      (((insn1 >> 7) & 0xff) << 12);

      MCInst *SubMI0 = getContext().createMCInst();
      MCInst *SubMI1 = getContext().createMCInst();
      MCInst *SubMI2 = getContext().createMCInst();
      if (decodeSlotVal(*SubMI0, val0, Address) &&
          decodeSlotVal(*SubMI1, val1, Address) &&
          decodeSlotVal(*SubMI2, val2, Address)) {
        MI.setOpcode(Xtensa::BUNDLE);
        MI.addOperand(MCOperand::createInst(SubMI0));
        MI.addOperand(MCOperand::createInst(SubMI1));
        MI.addOperand(MCOperand::createInst(SubMI2));
        Size = 6;
        return MCDisassembler::Success;
      }
    }
  }

  if (hasFLIX() && Bytes.size() >= 11 && (Bytes[0] & 0x0f) == 0x0f) {
    // 88-bit (11-byte) bundle: Format 3 (0x1F) or Format 4 (0x0F)
    uint32_t insn0 = Bytes[0] | (Bytes[1] << 8) | (Bytes[2] << 16) | (Bytes[3] << 24);
    uint32_t insn1 = Bytes[4] | (Bytes[5] << 8) | (Bytes[6] << 16) | (Bytes[7] << 24);
    uint32_t insn2 = Bytes[8] | (Bytes[9] << 8) | (Bytes[10] << 16);

    if (Bytes[0] == 0x1f) {
      // Format 3: 4 slots
      uint32_t val0 = ((insn0 >> 8) & 0xff) |
                      (((insn1 >> 6) & 0xf) << 8) |
                      (((insn1 >> 4) & 0x1) << 12) |
                      (((insn0 >> 5) & 0x7) << 13) |
                      (((insn1 >> 29) & 0x7) << 16) |
                      ((insn2 & 0x3) << 19);

      uint32_t val1 = ((insn0 >> 16) & 0xfff) |
                      (((insn2 >> 2) & 0xff) << 12);

      uint32_t val2 = ((insn0 >> 28) & 0xf) |
                      (((insn1 >> 5) & 0x1) << 4) |
                      (((insn1 >> 10) & 0x7) << 5) |
                      (((insn1 >> 14) & 0xf) << 8) |
                      (((insn1 >> 13) & 0x1) << 12) |
                      (((insn1 >> 26) & 0x1) << 13) |
                      (((insn2 >> 10) & 0x7f) << 14);

      uint32_t val3 = (insn1 & 0xf) |
                      (((insn1 >> 22) & 0xf) << 4) |
                      (((insn1 >> 18) & 0xf) << 8) |
                      (((insn1 >> 27) & 0x3) << 12) |
                      (((insn2 >> 17) & 0x7f) << 14);

      MCInst *SubMI0 = getContext().createMCInst();
      MCInst *SubMI1 = getContext().createMCInst();
      MCInst *SubMI2 = getContext().createMCInst();
      MCInst *SubMI3 = getContext().createMCInst();
      if (decodeSlotVal(*SubMI0, val0, Address) &&
          decodeSlotVal(*SubMI1, val1, Address) &&
          decodeSlotVal(*SubMI2, val2, Address) &&
          decodeSlotVal(*SubMI3, val3, Address)) {
        MI.setOpcode(Xtensa::BUNDLE);
        MI.addOperand(MCOperand::createInst(SubMI0));
        MI.addOperand(MCOperand::createInst(SubMI1));
        MI.addOperand(MCOperand::createInst(SubMI2));
        MI.addOperand(MCOperand::createInst(SubMI3));
        Size = 11;
        return MCDisassembler::Success;
      }
    } else if (Bytes[0] == 0x0f) {
      // Format 4: 3 slots
      uint32_t val0 = ((insn0 >> 8) & 0xff) |
                      (((insn1 >> 6) & 0xf) << 8) |
                      (((insn1 >> 4) & 0x1) << 12) |
                      (((insn1 >> 13) & 0x1) << 13) |
                      (((insn0 >> 6) & 0x3) << 14) |
                      (((insn1 >> 22) & 0x3ff) << 16) |
                      ((insn2 & 0x7) << 26);

      uint32_t val1 = ((insn0 >> 16) & 0xf) |
                      (((insn0 >> 24) & 0xf) << 4) |
                      (((insn0 >> 20) & 0xf) << 8) |
                      (((insn2 >> 3) & 0xff) << 12);

      uint32_t val2 = (insn1 & 0xf) |
                      (((insn0 >> 28) & 0xf) << 4) |
                      (((insn1 >> 14) & 0xf) << 8) |
                      (((insn1 >> 5) & 0x1) << 12) |
                      (((insn1 >> 10) & 0x7) << 13) |
                      (((insn1 >> 18) & 0xf) << 16) |
                      (((insn2 >> 11) & 0x1f) << 20);

      MCInst *SubMI0 = getContext().createMCInst();
      MCInst *SubMI1 = getContext().createMCInst();
      MCInst *SubMI2 = getContext().createMCInst();
      if (decodeSlotVal(*SubMI0, val0, Address) &&
          decodeSlotVal(*SubMI1, val1, Address) &&
          decodeSlotVal(*SubMI2, val2, Address)) {
        MI.setOpcode(Xtensa::BUNDLE);
        MI.addOperand(MCOperand::createInst(SubMI0));
        MI.addOperand(MCOperand::createInst(SubMI1));
        MI.addOperand(MCOperand::createInst(SubMI2));
        Size = 11;
        return MCDisassembler::Success;
      }
    }
  }

  // Parse 64-bit instructions
  if (hasHIFI3()) {
    Result = readInstruction64(Bytes, Address, Size, Insn, IsLittleEndian);
    if (Result != MCDisassembler::Fail) {
      LLVM_DEBUG(dbgs() << "Trying Xtensa HIFI3 64-bit instruction table :\n");
      Result = decodeInstruction(DecoderTableXtensaHIFI364, MI, Insn, Address, this, STI);
      if (Result != MCDisassembler::Fail) {
        Size = 8;
        return Result;
      }
    }
  }

  // Parse 48-bit instructions
  if (hasHIFI5()) {
    Result = readInstruction48(Bytes, Address, Size, Insn, IsLittleEndian);
    if (Result != MCDisassembler::Fail) {
      LLVM_DEBUG(dbgs() << "Trying Xtensa HIFI5 48-bit instruction table :\n");
      Result = decodeInstruction(DecoderTableXtensaHIFI548, MI, Insn, Address, this, STI);
      if (Result != MCDisassembler::Fail) {
        Size = 6;
        return Result;
      }
    }
  }

  // Parse 16-bit instructions
  if (hasDensity()) {
    Result = readInstruction16(Bytes, Address, Size, Insn, IsLittleEndian);
    if (Result == MCDisassembler::Fail)
      return MCDisassembler::Fail;
    LLVM_DEBUG(dbgs() << "Trying Xtensa 16-bit instruction table :\n");
    Result = decodeInstruction(DecoderTable16, MI, Insn, Address, this, STI);
    if (Result != MCDisassembler::Fail) {
      Size = 2;
      return Result;
    }
  }

  // Parse 24-bit instructions
  Result = readInstruction24(Bytes, Address, Size, Insn, IsLittleEndian);
  if (Result == MCDisassembler::Fail)
    return MCDisassembler::Fail;

  // Try decoding with SwappedInsn first for the byte-swapped instructions
  if (hasHIFI3()) {
    uint32_t SwappedInsn = ((Insn & 0xFF) << 16) | (Insn & 0xFF00) | ((Insn >> 16) & 0xFF);
    MCInst TmpMI;
    if (decodeInstruction(DecoderTableXtensaHIFI324, TmpMI, SwappedInsn, Address, this, STI) != MCDisassembler::Fail) {
      unsigned Opc = TmpMI.getOpcode();
      if (Opc == Xtensa::AE_SRAI64_HIFI3 || Opc == Xtensa::AE_SLAI32_HIFI3 ||
          Opc == Xtensa::AE_SLAI32S_HIFI3 ||
          Opc == Xtensa::AE_SRAI32_HIFI3 || Opc == Xtensa::AE_SEXT32X2D16_32) {
        MI = TmpMI;
        Size = 3;
        return MCDisassembler::Success;
      }
    }
  }

  // Try HiFi tables first
  if (hasHIFI5()) {
    LLVM_DEBUG(dbgs() << "Trying Xtensa HIFI5 24-bit instruction table :\n");
    Result = decodeInstruction(DecoderTableXtensaHIFI524, MI, Insn, Address, this, STI);
    if (Result != MCDisassembler::Fail) {
      if (isVLIWOnlyInstruction(MI.getOpcode())) {
        Result = MCDisassembler::Fail;
      } else {
        Size = 3;
        return Result;
      }
    }
  }
  if (hasHIFI4()) {
    LLVM_DEBUG(dbgs() << "Trying Xtensa HIFI4 24-bit instruction table :\n");
    Result = decodeInstruction(DecoderTableXtensaHIFI424, MI, Insn, Address, this, STI);
    if (Result != MCDisassembler::Fail) {
      if (isVLIWOnlyInstruction(MI.getOpcode())) {
        Result = MCDisassembler::Fail;
      } else {
        Size = 3;
        return Result;
      }
    }
  }
  if (hasHIFI3()) {
    LLVM_DEBUG(dbgs() << "Trying Xtensa HIFI3 24-bit instruction table :\n");
    Result = decodeInstruction(DecoderTableXtensaHIFI324, MI, Insn, Address, this, STI);
    if (Result != MCDisassembler::Fail) {
      if (isVLIWOnlyInstruction(MI.getOpcode())) {
        Result = MCDisassembler::Fail;
      } else {
        Size = 3;
        return Result;
      }
    }
  }
  if (hasHIFI3() || hasHIFI4()) {
    LLVM_DEBUG(dbgs() << "Trying Xtensa HIFIX 24-bit instruction table :\n");
    Result = decodeInstruction(DecoderTableXtensaHIFIX24, MI, Insn, Address, this, STI);
    if (Result != MCDisassembler::Fail) {
      if (isVLIWOnlyInstruction(MI.getOpcode())) {
        Result = MCDisassembler::Fail;
      } else {
        Size = 3;
        return Result;
      }
    }
  }

  LLVM_DEBUG(dbgs() << "Trying Xtensa 24-bit instruction table :\n");
  Result = decodeInstruction(DecoderTable24, MI, Insn, Address, this, STI);
  if (Result != MCDisassembler::Fail) {
    Size = 3;
    if (Result == MCDisassembler::Success && MI.getOpcode() == Xtensa::L32R) {
      if (MI.getNumOperands() > 1 && MI.getOperand(1).isImm()) {
        int64_t TargetVal = MI.getOperand(1).getImm();
        TargetVal &= ~0x3;
        uint64_t TargetAddr = ((Address + 3) & ~3) + TargetVal;
        uint64_t Diff = (Address > TargetAddr) ? (Address - TargetAddr) : (TargetAddr - Address);
        if (Diff <= 0x40000) {
          for (unsigned i = 0; i < 4; ++i) {
            LiteralAddresses.insert(TargetAddr + i);
          }
        }
      }
    }
    return Result;
  }

  return Result;
}

bool XtensaDisassembler::decodeSlotVal(MCInst &MI, uint32_t Val, uint64_t Address) const {
  // First check if it is a slot NOP
  if (Val == 0x260B74 || Val == 0x0F3016 || Val == 0x0B000040 || Val == 0x3900 ||
      Val == 0x27B205 || Val == 0 || Val == 0xf9000 ||
      Val == 0x1E1C15 || Val == 0x0F1670 || Val == 0x176011 || Val == 0x070B1D ||
      Val == 0x10341D35 || Val == 0x0E57D0 || Val == 0x011400A0 || Val == 0x0114000a) {
    MI.setOpcode(Xtensa::NOP);
    return true;
  }

  // Check manual remapped instructions
  if ((Val & 0xfffff0) == 0x17f700) {
    unsigned d = Val & 0xf;
    MI.setOpcode(Xtensa::AE_MOVFCRFSRV);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[d]));
    return true;
  }
  if ((Val & 0xfffff0) == 0x17f710) {
    unsigned d = Val & 0xf;
    MI.setOpcode(Xtensa::AE_MOVVFCRFSR);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[d]));
    return true;
  }

  // AE_SLAI64S_HIFI3
  if ((Val & 0xffff0030) == 0x10210020) {
    unsigned r = (Val >> 12) & 0xf;
    unsigned t = (Val >> 8) & 0xf;
    unsigned bundle_imm = Val & 0x3cf;
    unsigned imm = ((bundle_imm >> 2) & 0x30) | (bundle_imm & 0xf);
    MI.setOpcode(Xtensa::AE_SLAI64S_HIFI3);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    MI.addOperand(MCOperand::createImm(imm));
    return true;
  }

  // AE_SEXT32X2D16_10
  if ((Val & 0xffff00ff) == 0x1033003a) {
    unsigned r = (Val >> 12) & 0xf;
    unsigned s = (Val >> 8) & 0xf;
    MI.setOpcode(Xtensa::AE_SEXT32X2D16_10);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    return true;
  }

  // AE_MULAAFD32X16_H3_L2_HIFI3
  if ((Val & 0xfff000) == 0x056000) {
    unsigned t = Val & 0xf;
    unsigned r = (Val >> 4) & 0xf;
    unsigned s = (Val >> 8) & 0xf;
    MI.setOpcode(Xtensa::AE_MULAAFD32X16_H3_L2_HIFI3);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t])); // acc = t
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    return true;
  }

  // AE_MULAAFD32X16_H1_L0_HIFI3
  if ((Val & 0xfff000) == 0x052000) {
    unsigned t = Val & 0xf;
    unsigned r = (Val >> 4) & 0xf;
    unsigned s = (Val >> 8) & 0xf;
    MI.setOpcode(Xtensa::AE_MULAAFD32X16_H1_L0_HIFI3);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t])); // acc = t
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    return true;
  }

  // AE_MULAAAAFQ32X16
  if ((Val & 0xfff00000) == 0x01100000) {
    unsigned s = (Val >> 12) & 0xf;
    unsigned v = (Val >> 8) & 0xf;
    unsigned t = (Val >> 4) & 0xf;
    unsigned r = Val & 0xf;
    MI.setOpcode(Xtensa::AE_MULAAAAFQ32X16);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t])); // acc = t
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[v]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    return true;
  }

  // AE_SAT24S
  if ((Val & 0xfff0f0) == 0x1b7060) {
    unsigned dest = Val & 0xf;
    unsigned s = (Val >> 8) & 0xf;
    MI.setOpcode(Xtensa::AE_SAT24S);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    return true;
  }

  // AE_SAT16X4
  if ((Val & 0xfff000) == 0x19d000) {
    unsigned dest = Val & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned t = (Val >> 8) & 0xf;
    MI.setOpcode(Xtensa::AE_SAT16X4);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    return true;
  }

  // AE_MAXABS32S
  if ((Val & 0xfff000) == 0x18d000) {
    unsigned dest = Val & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned t = (Val >> 8) & 0xf;
    MI.setOpcode(Xtensa::AE_MAXABS32S);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    return true;
  }

  // AE_ROUND32X2F48SSYM_HIFI3
  if ((Val & 0xfff000) == 0x198000) {
    unsigned dest = Val & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned t = (Val >> 8) & 0xf;
    MI.setOpcode(Xtensa::AE_ROUND32X2F48SSYM_HIFI3);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    return true;
  }

  // AE_ROUND16X4F32SSYM
  if ((Val & 0xfff000) == 0x194000) {
    unsigned dest = Val & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned r = (Val >> 8) & 0xf;
    MI.setOpcode(Xtensa::AE_ROUND16X4F32SSYM);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    return true;
  }

  // AE_L32X2_XC (Format 0/1/2)
  if ((Val & 0xfff000) == 0x1f1000) {
    unsigned r = (Val >> 8) & 0xf;
    unsigned t = (Val >> 4) & 0xf;
    unsigned s = Val & 0xf;
    MI.setOpcode(Xtensa::AE_L32X2_XC);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s_out
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s
    MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
    return true;
  }

  // AE_S32X2_XC (Format 0/1/2)
  if ((Val & 0xfff000) == 0x217000) {
    unsigned r = (Val >> 8) & 0xf;
    unsigned t = (Val >> 4) & 0xf;
    unsigned s = Val & 0xf;
    MI.setOpcode(Xtensa::AE_S32X2_XC);
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s_out
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s
    MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
    return true;
  }

  // AE_L32X2_XC (Format 3)
  if ((Val & 0xff00f0) == 0x1800b0) {
    unsigned r = (Val >> 12) & 0xf;
    unsigned t = (Val >> 8) & 0xf;
    unsigned s = Val & 0xf;
    MI.setOpcode(Xtensa::AE_L32X2_XC);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s_out
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s
    MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
    return true;
  }

  // AE_S32X2_XC (Format 3)
  if ((Val & 0xff00f0) == 0x1800e0) {
    unsigned r = (Val >> 12) & 0xf;
    unsigned t = (Val >> 8) & 0xf;
    unsigned s = Val & 0xf;
    MI.setOpcode(Xtensa::AE_S32X2_XC);
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s_out
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s
    MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
    return true;
  }

  // AE_L32X2_XC (Format 4)
  if ((Val & 0xffff00f0) == 0x102f00b0) {
    unsigned r = (Val >> 12) & 0xf;
    unsigned t = (Val >> 8) & 0xf;
    unsigned s = Val & 0xf;
    MI.setOpcode(Xtensa::AE_L32X2_XC);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s_out
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s
    MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
    return true;
  }

  // AE_S32X2_XC (Format 4)
  if ((Val & 0xffff00f0) == 0x102f00e0) {
    unsigned r = (Val >> 12) & 0xf;
    unsigned t = (Val >> 8) & 0xf;
    unsigned s = Val & 0xf;
    MI.setOpcode(Xtensa::AE_S32X2_XC);
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s_out
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s
    MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
    return true;
  }

  // 1. AE_L16X4_X
  if ((Val & 0xfff000) == 0x1e1000) {
    unsigned r = (Val >> 8) & 0xf;
    unsigned t = (Val >> 4) & 0xf;
    unsigned s = Val & 0xf;
    MI.setOpcode(Xtensa::AE_L16X4_X);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
    return true;
  }
  // 2. AE_L16X4_XC
  if ((Val & 0xfff000) == 0x1e2000) {
    unsigned r = (Val >> 8) & 0xf;
    unsigned t = (Val >> 4) & 0xf;
    unsigned s = Val & 0xf;
    MI.setOpcode(Xtensa::AE_L16X4_XC);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s_out
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s
    MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
    return true;
  }
  // 3. AE_S16X4_X
  if ((Val & 0xfff000) == 0x1ff000) {
    unsigned r = (Val >> 8) & 0xf;
    unsigned t = (Val >> 4) & 0xf;
    unsigned s = Val & 0xf;
    MI.setOpcode(Xtensa::AE_S16X4_X);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
    return true;
  }
  // 4. AE_S16X4_XC
  if ((Val & 0xfff000) == 0x200000) {
    unsigned r = (Val >> 8) & 0xf;
    unsigned t = (Val >> 4) & 0xf;
    unsigned s = Val & 0xf;
    MI.setOpcode(Xtensa::AE_S16X4_XC);
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s_out
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s
    MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
    return true;
  }
  // 5. AE_S16X4_XP
  if ((Val & 0xfff000) == 0x201000) {
    unsigned r = (Val >> 8) & 0xf;
    unsigned t = (Val >> 4) & 0xf;
    unsigned s = Val & 0xf;
    MI.setOpcode(Xtensa::AE_S16X4_XP);
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s_out
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s])); // s
    MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
    return true;
  }
  // 6. AE_L16_X_HIFI3
  if ((Val & 0xfff000) == 0x1e4000) {
    unsigned r = (Val >> 8) & 0xf;
    unsigned t = (Val >> 4) & 0xf;
    unsigned s = Val & 0xf;
    MI.setOpcode(Xtensa::AE_L16_X_HIFI3);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
    return true;
  }
  // 7. AE_ABS32_HIFI3, AE_ABS64S_HIFI3, AE_ABSSQ56S_HIFI3
  if ((Val & 0xffff0000) == 0x10340000) {
    unsigned t_val_plus_9 = (Val >> 8) & 0xf;
    unsigned r = (Val >> 4) & 0xf;
    unsigned s = Val & 0xf;
    unsigned t_val = t_val_plus_9 - 9;
    if (t_val == 2) MI.setOpcode(Xtensa::AE_ABS32_HIFI3);
    else if (t_val == 5) MI.setOpcode(Xtensa::AE_ABS64S_HIFI3);
    else if (t_val == 6) MI.setOpcode(Xtensa::AE_ABSSQ56S_HIFI3);
    else return false;
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    return true;
  }
  // 8. AE_ADDSQ56S_HIFI3, AE_ADDSUB32_HIFI3, AE_ADDSUB32S_HIFI3
  if ((Val & 0xffff0f00) == 0x102d0400) { // op_19_12 = 0xEC
    unsigned s = (Val >> 12) & 0xf;
    unsigned r = (Val >> 4) & 0xf;
    unsigned t = Val & 0xf;
    MI.setOpcode(Xtensa::AE_ADDSQ56S_HIFI3);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    return true;
  }
  if ((Val & 0xffff0f00) == 0x102d0800) { // op_19_12 = 0xED
    unsigned s = (Val >> 12) & 0xf;
    unsigned r = (Val >> 4) & 0xf;
    unsigned t = Val & 0xf;
    MI.setOpcode(Xtensa::AE_ADDSUB32_HIFI3);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    return true;
  }
  if ((Val & 0xffff0f00) == 0x102d0c00) { // op_19_12 = 0xEE
    unsigned s = (Val >> 12) & 0xf;
    unsigned r = (Val >> 4) & 0xf;
    unsigned t = Val & 0xf;
    MI.setOpcode(Xtensa::AE_ADDSUB32S_HIFI3);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    return true;
  }

  // Fallback: try standard decoding tables, searching for the correct bits 23-20
  for (unsigned mask = 0; mask < 16; ++mask) {
    uint32_t CandidateVal = ((Val & ~(0xf << 20)) & 0xffffff) | (mask << 20);
    if (decodeInstruction(DecoderTableXtensaHIFI524, MI, CandidateVal, Address, this, STI) == MCDisassembler::Success) return true;
    if (decodeInstruction(DecoderTableXtensaHIFI424, MI, CandidateVal, Address, this, STI) == MCDisassembler::Success) return true;
    if (decodeInstruction(DecoderTableXtensaHIFI324, MI, CandidateVal, Address, this, STI) == MCDisassembler::Success) return true;
    if (decodeInstruction(DecoderTableXtensaHIFIX24, MI, CandidateVal, Address, this, STI) == MCDisassembler::Success) return true;
    if (decodeInstruction(DecoderTable24, MI, CandidateVal, Address, this, STI) == MCDisassembler::Success) return true;
  }

  return false;
}
