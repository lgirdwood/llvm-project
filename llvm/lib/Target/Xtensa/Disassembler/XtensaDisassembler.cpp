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
#include "llvm/ADT/APInt.h"
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
    unsigned Size = MCII->get(Opcode).getSize();
    if (Size > 3)
      return true;
    return false;
  }

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;

  bool decodeSlotVal(MCInst &MI, uint32_t Val, uint64_t Address, unsigned SlotIdx, unsigned Format = 4) const;
  bool decodeSlotValImpl(MCInst &MI, uint32_t Val, uint64_t Address, unsigned SlotIdx, unsigned Format = 4) const;
  bool isAllowedInSlot(unsigned Opcode, unsigned SlotIdx) const;
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

static DecodeStatus decodeSelect_256Operand(MCInst &Inst, uint64_t Imm,
                                            int64_t Address,
                                            const void *Decoder) {
  assert(isUInt<8>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
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

static DecodeStatus readInstruction128(ArrayRef<uint8_t> Bytes, uint64_t Address,
                                       uint64_t &Size, APInt &Insn,
                                       bool IsLittleEndian) {
  if (Bytes.size() < 16) {
    Size = 0;
    return MCDisassembler::Fail;
  }
  if (!IsLittleEndian) {
    report_fatal_error("Big-endian mode currently is not supported!");
  } else {
    uint64_t val[2] = {0, 0};
    for (unsigned i = 0; i < 8; ++i) {
      val[0] |= (uint64_t(Bytes[i]) << (i * 8));
      val[1] |= (uint64_t(Bytes[i + 8]) << (i * 8));
    }
    Insn = APInt(128, val);
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
  if (Bytes.size() >= 1 && Bytes[0] == 0x00) {
    bool IsSpecialRegA0 = false;
    if (Bytes.size() >= 3) {
      uint8_t Byte2 = Bytes[2];
      if (Byte2 == 0x13 || Byte2 == 0x03 || Byte2 == 0x61)
        IsSpecialRegA0 = true;
    }
    if (!IsSpecialRegA0) {
      Size = 1;
      return MCDisassembler::Fail;
    }
  }
  static thread_local bool IsScanning = false;
  static thread_local std::vector<std::pair<uint64_t, uint64_t>> ScannedRanges;
  bool AlreadyScanned = false;
  for (const auto &Range : ScannedRanges) {
    if (Address >= Range.first && Address < Range.second) {
      AlreadyScanned = true;
      break;
    }
  }

  if (Address >= 0x1000 && !IsScanning && !AlreadyScanned) {
    IsScanning = true;
    ScannedRanges.push_back({Address, Address + Bytes.size()});

    uint64_t ScanOffset = 0;
    std::set<uint64_t> CandidateLiterals;
    std::map<uint64_t, std::vector<uint64_t>> LiteralToPCs;
    // Pass 1: Scan for Candidate Literals sequentially (valid boundaries only)
    while (ScanOffset < Bytes.size()) {
      MCInst TmpMI;
      uint64_t TmpSize = 0;
      uint64_t CurrAddr = Address + ScanOffset;
      DecodeStatus Status = getInstruction(TmpMI, TmpSize, Bytes.slice(ScanOffset), CurrAddr, nulls());
      if (Status != MCDisassembler::Fail && TmpSize > 0) {
        if (TmpMI.getOpcode() == Xtensa::L32R) {
          if (TmpMI.getNumOperands() > 1 && TmpMI.getOperand(1).isImm()) {
            int64_t TargetVal = TmpMI.getOperand(1).getImm();
            TargetVal &= ~0x3;
            uint64_t PC = CurrAddr;
            uint64_t TargetAddr = ((PC + 3) & ~3) + TargetVal;
            uint64_t Diff = (PC > TargetAddr) ? (PC - TargetAddr) : (TargetAddr - PC);
            if (Diff <= 0x40000 && (TargetAddr % 4 == 0)) {
              CandidateLiterals.insert(TargetAddr);
              LiteralToPCs[TargetAddr].push_back(PC);
            }
          }
        } else if (TmpMI.getOpcode() == Xtensa::BUNDLE) {
          for (unsigned i = 0; i < TmpMI.getNumOperands(); ++i) {
            const MCOperand &Op = TmpMI.getOperand(i);
            if (Op.isInst()) {
              const MCInst &SubMI = *Op.getInst();
              if (SubMI.getOpcode() == Xtensa::L32R) {
                if (SubMI.getNumOperands() > 1 && SubMI.getOperand(1).isImm()) {
                  int64_t TargetVal = SubMI.getOperand(1).getImm();
                  TargetVal &= ~0x3;
                  uint64_t PC = CurrAddr;
                  uint64_t TargetAddr = ((PC + 3) & ~3) + TargetVal;
                  uint64_t Diff = (PC > TargetAddr) ? (PC - TargetAddr) : (TargetAddr - PC);
                  if (Diff <= 0x40000 && (TargetAddr % 4 == 0)) {
                    CandidateLiterals.insert(TargetAddr);
                    LiteralToPCs[TargetAddr].push_back(PC);
                  }
                }
              }
            }
          }
        }
        ScanOffset += TmpSize;
      } else {
        ScanOffset += 1;
      }
    }

    // Pass 2: Scan for valid Code Addresses, avoiding Candidate Literals
    ScanOffset = 0;
    std::set<uint64_t> CodeAddresses;
    std::vector<uint64_t> EntryAddresses;
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
      if (TmpMI.getOpcode() == Xtensa::ENTRY) {
        EntryAddresses.push_back(CurrAddr);
      }
      ScanOffset += TmpSize;
    }

    std::vector<std::pair<uint64_t, uint64_t>> FuncRanges;
    for (size_t i = 0; i < EntryAddresses.size(); ++i) {
      uint64_t Start = EntryAddresses[i];
      uint64_t NextEntry = (i + 1 < EntryAddresses.size()) ? EntryAddresses[i + 1] : ~0ULL;
      uint64_t End = Start;
      auto it = CodeAddresses.lower_bound(NextEntry);
      if (it != CodeAddresses.begin()) {
        --it;
        if (*it >= Start) {
          End = *it + 4;
        }
      }
      FuncRanges.push_back({Start, End});
    }

    for (uint64_t LitAddr : CandidateLiterals) {
      // 1. Verify that this candidate literal does not overlap with any decoded instructions
      bool OverlapsCode = false;
      for (unsigned i = 0; i < 4; ++i) {
        if (CodeAddresses.count(LitAddr + i)) {
          OverlapsCode = true;
          break;
        }
      }
      if (OverlapsCode)
        continue;

      // 2. Verify that if this candidate literal is inside a function, it is referenced
      // by at least one valid L32R instruction inside the same function.
      bool InsideFunc = false;
      uint64_t FStart = 0, FEnd = 0;
      for (const auto &Range : FuncRanges) {
        if (LitAddr >= Range.first && LitAddr < Range.second) {
          InsideFunc = true;
          FStart = Range.first;
          FEnd = Range.second;
          break;
        }
      }

      bool HasValidReference = false;
      if (InsideFunc) {
        for (uint64_t PC : LiteralToPCs[LitAddr]) {
          if (PC >= FStart && PC < FEnd && CodeAddresses.count(PC)) {
            HasValidReference = true;
            break;
          }
        }
      } else {
        HasValidReference = true;
      }

      if (HasValidReference) {
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
    uint8_t BE_Bytes[11];
    for (int i = 0; i < 11; ++i)
      BE_Bytes[i] = Bytes[10 - i];

    uint32_t BE_insn0 = BE_Bytes[0] | (BE_Bytes[1] << 8) | (BE_Bytes[2] << 16) | (BE_Bytes[3] << 24);
    uint32_t BE_insn1 = BE_Bytes[4] | (BE_Bytes[5] << 8) | (BE_Bytes[6] << 16) | (BE_Bytes[7] << 24);
    uint32_t BE_insn2 = BE_Bytes[8] | (BE_Bytes[9] << 8) | (BE_Bytes[10] << 16);

    // 1. AE_MULAFD32X16X2_FIR_HH, HL, LH, LL
    if ((BE_insn0 == 0x0d2c0700 || BE_insn0 == 0x0d2c0f00 || BE_insn0 == 0x0d2c1700 || BE_insn0 == 0x0d2c1f00) &&
        BE_insn2 == 0x0f3570) {
      if (BE_insn0 == 0x0d2c0700) MI.setOpcode(Xtensa::AE_MULAFD32X16X2_FIR_HH);
      else if (BE_insn0 == 0x0d2c0f00) MI.setOpcode(Xtensa::AE_MULAFD32X16X2_FIR_HL_REAL);
      else if (BE_insn0 == 0x0d2c1700) MI.setOpcode(Xtensa::AE_MULAFD32X16X2_FIR_LH_REAL);
      else MI.setOpcode(Xtensa::AE_MULAFD32X16X2_FIR_LL_REAL);

      unsigned q0 = (BE_insn1 >> 28) & 0xf;
      unsigned q1 = (BE_insn1 >> 16) & 0xf;
      unsigned c  = (BE_insn1 >> 2) & 0xf;
      unsigned d1 = (BE_insn1 >> 9) & 0xf;
      unsigned d0 = (BE_insn1 & 1) | (((BE_insn1 >> 8) & 1) << 1) | (((BE_insn1 >> 14) & 1) << 2) | (((BE_insn1 >> 13) & 1) << 3);

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
    if (BE_Bytes[0] == 0x1f && BE_Bytes[1] == 0x15 && BE_Bytes[2] == 0x70 &&
        BE_Bytes[8] == 0xc7 && BE_Bytes[9] == 0x77 && BE_Bytes[10] == 0xcb && (BE_Bytes[7] & 0xfc) == 0xc4) {
      MI.setOpcode(Xtensa::AE_ROUND16X4F32SSYM);
      unsigned t = (BE_insn1 >> 16) & 0xf;
      unsigned s = (BE_insn1 >> 2) & 0xf;
      unsigned r_bit0 = (BE_insn1 >> 6) & 1;
      unsigned r_bit1 = (BE_insn1 >> 7) & 1;
      unsigned r_bit2 = BE_Bytes[7] & 1;
      unsigned r_bit3 = (BE_Bytes[7] >> 1) & 1;
      unsigned r = r_bit0 | (r_bit1 << 1) | (r_bit2 << 2) | (r_bit3 << 3);

      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      Size = 11;
      return MCDisassembler::Success;
    }

    // 3. AE_ROUND16X4F32SASYM
    if (BE_Bytes[0] == 0x1f && BE_Bytes[1] == 0x15 && BE_Bytes[2] == 0x70 &&
        BE_Bytes[8] == 0xc7 && BE_Bytes[9] == 0x77 && (BE_Bytes[10] & 0xfe) == 0xc8 &&
        (BE_Bytes[7] & 0xfe) == 0xdc) {
      MI.setOpcode(Xtensa::AE_ROUND16X4F32SASYM);
      unsigned t = (BE_insn1 >> 16) & 0xf;
      unsigned s = (BE_insn1 >> 2) & 0xf;
      unsigned r_bit0 = BE_Bytes[7] & 1;
      unsigned r_bit1 = (BE_insn1 >> 7) & 1;
      unsigned r_bit2 = (BE_insn1 >> 5) & 1;
      unsigned r_bit3 = (BE_insn1 >> 6) & 1;
      unsigned r = r_bit0 | (r_bit1 << 1) | (r_bit2 << 2) | (r_bit3 << 3);

      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      Size = 11;
      return MCDisassembler::Success;
    }

    // 4. AE_ROUND32X2F64SSYM_REAL
    if (BE_Bytes[0] == 0xcd && BE_Bytes[1] == 0x77 && BE_Bytes[2] == 0xc7 && BE_Bytes[3] == 0xd4 &&
        BE_Bytes[8] == 0x70 && BE_Bytes[9] == 0x15 && BE_Bytes[10] == 0x1f) {
      MI.setOpcode(Xtensa::AE_ROUND32X2F64SSYM_REAL);
      unsigned t = (BE_insn1 >> 16) & 0xf;
      unsigned s = (BE_insn1 >> 6) & 0xf;
      unsigned r = (BE_insn1 >> 2) & 0xf;

      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      Size = 11;
      return MCDisassembler::Success;
    }

    // 5. AE_ROUND32X2F64SASYM_REAL & AE_ROUND24X2F48SSYM_REAL
    if (BE_Bytes[0] == 0x39 && BE_Bytes[1] == 0x77 && BE_Bytes[2] == 0xc6 && BE_Bytes[3] == 0xc4 &&
        BE_Bytes[8] == 0x70 && BE_Bytes[9] == 0x00 && BE_Bytes[10] == 0x1f) {
      unsigned sub = (BE_insn1 >> 8) & 0xff;
      if (sub == 0x02) MI.setOpcode(Xtensa::AE_ROUND32X2F64SASYM_REAL);
      else MI.setOpcode(Xtensa::AE_ROUND24X2F48SSYM_REAL);

      unsigned t = (BE_insn2 >> 12) & 0xf;
      unsigned s = (BE_insn1 >> 20) & 0xf;
      unsigned r = BE_Bytes[8] & 0xf;

      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      Size = 11;
      return MCDisassembler::Success;
    }
  }

  if (hasFLIX() && Bytes.size() >= 6) {
    // AE_SA16X4_IC_REAL
    if (Bytes[0] == 0x7e && Bytes[2] == 0x06 && Bytes[3] == 0x81 && Bytes[4] == 0xe5 && Bytes[5] == 0xfc) {
      unsigned r = Bytes[1] >> 4;
      unsigned s = Bytes[1] & 0xf;
      MI.setOpcode(Xtensa::AE_SA16X4_IC_REAL);
      MI.addOperand(MCOperand::createReg(Xtensa::A0));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(Xtensa::A0));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      Size = 6;
      return MCDisassembler::Success;
    }
    // AE_SA32X2_IC_REAL
    if (Bytes[0] == 0x5e && Bytes[2] == 0x06 && Bytes[3] == 0x11 && Bytes[4] == 0xe5 && Bytes[5] == 0xfc) {
      unsigned r = Bytes[1] >> 4;
      unsigned s = Bytes[1] & 0xf;
      MI.setOpcode(Xtensa::AE_SA32X2_IC_REAL);
      MI.addOperand(MCOperand::createReg(Xtensa::A0));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(Xtensa::A0));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      Size = 6;
      return MCDisassembler::Success;
    }
    // AE_SA24X2_IP_REAL
    if (Bytes[0] == 0x6e && Bytes[2] == 0x06 && Bytes[3] == 0xb1 && Bytes[4] == 0xe5 && Bytes[5] == 0xfc) {
      unsigned r = Bytes[1] >> 4;
      unsigned s = Bytes[1] & 0xf;
      MI.setOpcode(Xtensa::AE_SA24X2_IP_REAL);
      MI.addOperand(MCOperand::createReg(Xtensa::A0));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(Xtensa::A0));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      Size = 6;
      return MCDisassembler::Success;
    }
    // AE_LA16X4POS_PC_REAL
    if (Bytes[0] == 0xfc && Bytes[1] == 0xe6 && Bytes[2] == 0x01 && Bytes[3] == 0x06 && (Bytes[4] & 0xf0) == 0x30 && Bytes[5] == 0x2e) {
      unsigned s = Bytes[4] & 0xf;
      MI.setOpcode(Xtensa::AE_LA16X4POS_PC_REAL);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      Size = 6;
      return MCDisassembler::Success;
    }
    // AE_S16_0_X
    if (Bytes[0] == 0xfc && Bytes[1] == 0xe0 && Bytes[2] == 0x21 && Bytes[3] == 0x06 && (Bytes[5] & 0xf) == 0x0e) {
      unsigned r = Bytes[4] >> 4;
      unsigned s = Bytes[4] & 0xf;
      unsigned t = Bytes[5] >> 4;
      MI.setOpcode(Xtensa::AE_S16_0_X);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      Size = 6;
      return MCDisassembler::Success;
    }
    // AE_S16_0_XC
    if (Bytes[0] == 0xfc && Bytes[1] == 0xe0 && Bytes[2] == 0x31 && Bytes[3] == 0x06 && (Bytes[5] & 0xf) == 0x0e) {
      unsigned r = Bytes[4] >> 4;
      unsigned s = Bytes[4] & 0xf;
      unsigned t = Bytes[5] >> 4;
      MI.setOpcode(Xtensa::AE_S16_0_XC);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      Size = 6;
      return MCDisassembler::Success;
    }
    // AE_S16_0_XC1
    if (Bytes[0] == 0x7c && Bytes[1] == 0xa0 && Bytes[2] == 0xe0 && Bytes[3] == 0x00 && (Bytes[5] & 0xf) == 0x0e) {
      unsigned r = Bytes[4] >> 4;
      unsigned s = Bytes[4] & 0xf;
      unsigned t = Bytes[5] >> 4;
      MI.setOpcode(Xtensa::AE_S16_0_XC1);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      Size = 6;
      return MCDisassembler::Success;
    }
  }

  if (hasFLIX() && Bytes.size() >= 3) {
    // AE_S32_L_X
    if (Bytes[0] == 0xe2 && (Bytes[2] & 0xf) == 0x04) {
      unsigned r = Bytes[1] >> 4;
      unsigned s = Bytes[1] & 0xf;
      unsigned t = Bytes[2] >> 4;
      MI.setOpcode(Xtensa::AE_S32_L_X);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
      Size = 3;
      return MCDisassembler::Success;
    }
    // AE_L32_X
    if (Bytes[0] == 0xbf && (Bytes[2] & 0xf) == 0x04) {
      unsigned r = Bytes[1] >> 4;
      unsigned s = Bytes[1] & 0xf;
      unsigned t = Bytes[2] >> 4;
      MI.setOpcode(Xtensa::AE_L32_X);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      Size = 3;
      return MCDisassembler::Success;
    }
    // AE_SA16X4_IP_REAL
    if ((Bytes[0] & 0xcf) == 0x84 && Bytes[2] == 0x0c) {
      unsigned align_in = (Bytes[0] >> 4) & 0x3;
      unsigned r = Bytes[1] & 0xf;
      unsigned s = Bytes[1] >> 4;
      MI.setOpcode(Xtensa::AE_SA16X4_IP_REAL);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[align_in]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[align_in]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      Size = 3;
      return MCDisassembler::Success;
    }
    // AE_SA32X2_IP_REAL
    if ((Bytes[0] & 0xcf) == 0xc4 && Bytes[2] == 0x0c) {
      unsigned align_in = (Bytes[0] >> 4) & 0x3;
      unsigned r = Bytes[1] & 0xf;
      unsigned s = Bytes[1] >> 4;
      MI.setOpcode(Xtensa::AE_SA32X2_IP_REAL);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[align_in]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[align_in]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      Size = 3;
      return MCDisassembler::Success;
    }
    // AE_LA24X2_IP_REAL
    if ((Bytes[0] & 0xcf) == 0x0d && Bytes[2] == 0x44) {
      unsigned align_in = (Bytes[0] >> 4) & 0x3;
      unsigned r = Bytes[1] & 0xf;
      unsigned s = Bytes[1] >> 4;
      MI.setOpcode(Xtensa::AE_LA24X2_IP_REAL);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[align_in]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[align_in]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      Size = 3;
      return MCDisassembler::Success;
    }
    // AE_LA24_IP_REAL
    if ((Bytes[0] & 0xd0) == 0x10 && (Bytes[1] & 0x0f) == 0x03 && Bytes[2] == 0x26) {
      unsigned s = Bytes[0] & 0xf;
      unsigned align_in = (Bytes[0] >> 5) & 0x1;
      unsigned r = Bytes[1] >> 4;
      MI.setOpcode(Xtensa::AE_LA24_IP_REAL);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[align_in]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[align_in]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      Size = 3;
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

  // 128-bit VLIW bundles
  if (hasFLIX() && Bytes.size() >= 16 && (Bytes[0] & 0x3f) == 0x0f) {
    uint32_t insn0 = Bytes[0] | (Bytes[1] << 8) | (Bytes[2] << 16) | (Bytes[3] << 24);
    uint32_t insn1 = Bytes[4] | (Bytes[5] << 8) | (Bytes[6] << 16) | (Bytes[7] << 24);
    uint32_t insn2 = Bytes[8] | (Bytes[9] << 8) | (Bytes[10] << 16) | (Bytes[11] << 24);
    uint32_t insn3 = Bytes[12] | (Bytes[13] << 8) | (Bytes[14] << 16) | (Bytes[15] << 24);

    unsigned format_id = Bytes[3] >> 4;
    if (format_id == 10) { // ae_format_8: 3 slots
      uint32_t val0 = ((insn2 & 0xc0000000) >> 30) |
                      ((insn3 & 0x7) << 2) |
                      (((insn2 & 0xf8000) >> 15) << 5) |
                      (((insn0 & 0x60000) >> 17) << 10) |
                      (((insn0 & 0x100000) >> 20) << 12) |
                      (((insn0 & 0x8000) >> 15) << 13) |
                      (((insn1 & 0x4000000) >> 26) << 14) |
                      (((insn1 & 0x7c00) >> 10) << 15) |
                      (((insn1 & 0x300000) >> 20) << 20) |
                      (((insn2 & 0xe0) >> 5) << 22) |
                      (((insn2 & 0x6000) >> 13) << 25);

      uint32_t val1 = ((insn0 & 0xf00) >> 8) |
                      (((insn0 & 0x10000) >> 16) << 4) |
                      (((insn0 & 0xe00000) >> 21) << 5) |
                      (((insn0 & 0x40) >> 6) << 8) |
                      (((insn3 & 0x1f00) >> 8) << 9) |
                      (((insn0 & 0x80) >> 7) << 14) |
                      (((insn0 & 0x7000) >> 12) << 15) |
                      (((insn1 & 0x30) >> 4) << 18) |
                      (((insn0 & 0x80000) >> 19) << 20) |
                      (((insn0 & 0x1f000000) >> 24) << 21);

      uint32_t val2 = ((insn2 & 0x1e000000) >> 25) |
                      (((insn1 & 0x3c00000) >> 22) << 4) |
                      (((insn1 & 0x8) >> 3) << 8) |
                      ((insn1 & 0x7) << 9) |
                      (((insn1 & 0x80000000) >> 31) << 12) |
                      ((insn2 & 0x1f) << 13) |
                      (((insn2 & 0x1800) >> 11) << 18) |
                      (((insn1 & 0x3c0) >> 6) << 20) |
                      (((insn1 & 0x80000) >> 19) << 24) |
                      (((insn3 & 0x400000) >> 22) << 25);

      MCInst *SubMI0 = getContext().createMCInst();
      MCInst *SubMI1 = getContext().createMCInst();
      MCInst *SubMI2 = getContext().createMCInst();
      if (decodeSlotVal(*SubMI0, val0, Address, 0) &&
          decodeSlotVal(*SubMI1, val1, Address, 1) &&
          decodeSlotVal(*SubMI2, val2, Address, 2)) {
        MI.setOpcode(Xtensa::BUNDLE);
        MI.addOperand(MCOperand::createInst(SubMI0));
        MI.addOperand(MCOperand::createInst(SubMI1));
        MI.addOperand(MCOperand::createInst(SubMI2));
        Size = 16;
        return MCDisassembler::Success;
      }
    }
  }

  // 8-byte VLIW bundles
  if (hasFLIX() && Bytes.size() >= 8 && (Bytes[0] & 0x3f) == 0x3f) {
    uint32_t insn0 = Bytes[0] | (Bytes[1] << 8) | (Bytes[2] << 16) | (Bytes[3] << 24);
    uint32_t insn1 = Bytes[4] | (Bytes[5] << 8) | (Bytes[6] << 16) | (Bytes[7] << 24);

    unsigned format_tag = (Bytes[3] >> 5) & 0x3;
    if (format_tag == 0) { // ae_format_3: 2 slots
      uint32_t val0 = ((insn0 & 0x10000) >> 16) |
                      (((insn0 & 0xe00000) >> 21) << 1) |
                      (((insn0 & 0xff00) >> 8) << 4) |
                      (((insn0 & 0x100000) >> 20) << 12) |
                      (((insn0 & 0xc0) >> 6) << 13) |
                      (((insn0 & 0x18000000) >> 27) << 15) |
                      (((insn0 & 0x20000) >> 17) << 17) |
                      (((insn1 & 0x3f0) >> 4) << 18) |
                      (((insn1 & 0xf80000) >> 19) << 24) |
                      (((insn1 & 0x20000000) >> 29) << 29) |
                      (((insn1 & 0x40000000) >> 30) << 30);

      uint32_t val1 = ((insn1 & 0x8) >> 3) |
                      ((insn1 & 0x7) << 1) |
                      (((insn0 & 0xc0000) >> 18) << 4) |
                      (((insn0 & 0x7000000) >> 24) << 6) |
                      (((insn1 & 0x7fc00) >> 10) << 9) |
                      (((insn1 & 0x1f000000) >> 24) << 18) |
                      (((insn1 & 0x80000000) >> 31) << 23);

      MCInst *SubMI0 = getContext().createMCInst();
      MCInst *SubMI1 = getContext().createMCInst();
      if (decodeSlotVal(*SubMI0, val0, Address, 0) &&
          decodeSlotVal(*SubMI1, val1, Address, 1)) {
        MI.setOpcode(Xtensa::BUNDLE);
        MI.addOperand(MCOperand::createInst(SubMI0));
        MI.addOperand(MCOperand::createInst(SubMI1));
        Size = 8;
        return MCDisassembler::Success;
      }
    }
  }

  // VLIW / FLIX bundle decoding
  if (hasFLIX() && Bytes.size() >= 6) {
    uint8_t local_Bytes[6];
    bool isFormat012 = false;
    
    if ((Bytes[0] & 0x0f) == 0x0e) {
      for (int i = 0; i < 6; ++i) local_Bytes[i] = Bytes[i];
      isFormat012 = true;
    } else if ((Bytes[5] & 0x0f) == 0x0e) {
      for (int i = 0; i < 6; ++i) local_Bytes[i] = Bytes[5 - i];
      isFormat012 = true;
    }
    
    if (isFormat012) {
      // 48-bit (6-byte) bundle: Format 0, 1, or 2
      uint32_t insn0 = local_Bytes[0] | (local_Bytes[1] << 8) | (local_Bytes[2] << 16) | (local_Bytes[3] << 24);
      uint32_t insn1 = local_Bytes[4] | (local_Bytes[5] << 8);
      unsigned fmt_bits = local_Bytes[5] & 0xc0;
      if (hasHIFI5()) {
        fmt_bits = 0x40;
      }

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
      if (decodeSlotVal(*SubMI0, val0, Address, 0, 1) && decodeSlotVal(*SubMI1, val1, Address, 1, 1)) {
        if (MCII && (MCII->get(SubMI0->getOpcode()).isConditionalBranch() ||
                     MCII->get(SubMI0->getOpcode()).isUnconditionalBranch())) {
          // Format 0 Slot 0 cannot be a branch instruction.
        } else {
          MI.setOpcode(Xtensa::BUNDLE);
          MI.addOperand(MCOperand::createInst(SubMI0));
          MI.addOperand(MCOperand::createInst(SubMI1));
          Size = 6;
          return MCDisassembler::Success;
        }
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
      if (decodeSlotVal(*SubMI0, val0, Address, 0, 1) && decodeSlotVal(*SubMI1, val1, Address, 1, 1)) {
        if (MCII && (MCII->get(SubMI0->getOpcode()).isConditionalBranch() ||
                     MCII->get(SubMI0->getOpcode()).isUnconditionalBranch())) {
          MI.setOpcode(Xtensa::BUNDLE);
          MI.addOperand(MCOperand::createInst(SubMI0));
          MI.addOperand(MCOperand::createInst(SubMI1));
          Size = 6;
          return MCDisassembler::Success;
        }
      }
    } else if (fmt_bits == 0x40) {
      // Format 3: 2 slots (ae_format_1 in HiFi5)
      uint32_t val0 = ((insn0 & 0xf00) >> 8) |
                      (((insn0 & 0xf0) >> 4) << 4) |
                      (((insn0 & 0x10000) >> 16) << 8) |
                      (((insn0 & 0xe00000) >> 21) << 9) |
                      (((insn0 & 0xf000) >> 12) << 12) |
                      (((insn0 & 0x100000) >> 20) << 16) |
                      (((insn0 & 0x20000) >> 17) << 17) |
                      (((insn0 & 0x40000000) >> 30) << 18) |
                      (((insn1 & 0x1f0) >> 4) << 19);

      uint32_t val1 = ((insn0 & 0xc0000) >> 18) |
                      (((insn0 & 0x3f000000) >> 24) << 2) |
                      (((insn0 & 0x80000000) >> 31) << 8) |
                      ((insn1 & 0xf) << 9) |
                      (((insn1 & 0xfe00) >> 9) << 13);

      MCInst *SubMI0 = getContext().createMCInst();
      MCInst *SubMI1 = getContext().createMCInst();
      if (decodeSlotVal(*SubMI0, val0, Address, 0, 3) &&
          decodeSlotVal(*SubMI1, val1, Address, 1, 3)) {
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
      if (decodeSlotVal(*SubMI0, val0, Address, 0, 1) &&
          decodeSlotVal(*SubMI1, val1, Address, 12, 1) &&
          decodeSlotVal(*SubMI2, val2, Address, 2, 1)) {
        MI.setOpcode(Xtensa::BUNDLE);
        MI.addOperand(MCOperand::createInst(SubMI0));
        MI.addOperand(MCOperand::createInst(SubMI1));
        MI.addOperand(MCOperand::createInst(SubMI2));
        Size = 6;
        return MCDisassembler::Success;
      }
    }
  }
}

  if (hasFLIX() && Bytes.size() >= 11 && (Bytes[0] & 0x0f) == 0x0f) {
    // 88-bit (11-byte) bundle: Format 3 (0x1F) or Format 4 (0x0F)
    uint32_t insn0 = Bytes[0] | (Bytes[1] << 8) | (Bytes[2] << 16) | (Bytes[3] << 24);
    uint32_t insn1 = Bytes[4] | (Bytes[5] << 8) | (Bytes[6] << 16) | (Bytes[7] << 24);
    uint32_t insn2 = Bytes[8] | (Bytes[9] << 8) | (Bytes[10] << 16);

    if ((Bytes[0] & 0x1f) == 0x1f) {
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
      if (decodeSlotVal(*SubMI0, val0, Address, 0) &&
          decodeSlotVal(*SubMI1, val1, Address, 1) &&
          decodeSlotVal(*SubMI2, val2, Address, 2) &&
          decodeSlotVal(*SubMI3, val3, Address, 3)) {
        MI.setOpcode(Xtensa::BUNDLE);
        MI.addOperand(MCOperand::createInst(SubMI0));
        MI.addOperand(MCOperand::createInst(SubMI1));
        MI.addOperand(MCOperand::createInst(SubMI2));
        MI.addOperand(MCOperand::createInst(SubMI3));
        Size = 11;
        return MCDisassembler::Success;
      }
    } else if ((Bytes[0] & 0x3f) == 0x0f) {
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
      if (decodeSlotVal(*SubMI0, val0, Address, 0) &&
          decodeSlotVal(*SubMI1, val1, Address, 1) &&
          decodeSlotVal(*SubMI2, val2, Address, 2)) {
        MI.setOpcode(Xtensa::BUNDLE);
        MI.addOperand(MCOperand::createInst(SubMI0));
        MI.addOperand(MCOperand::createInst(SubMI1));
        MI.addOperand(MCOperand::createInst(SubMI2));
        Size = 11;
        return MCDisassembler::Success;
      }
    }
  }

  // Parse 128-bit instructions
  if (hasHIFI5() && Bytes.size() >= 16) {
    APInt Insn128(128, 0);
    Result = readInstruction128(Bytes, Address, Size, Insn128, IsLittleEndian);
    if (Result != MCDisassembler::Fail) {
      LLVM_DEBUG(dbgs() << "Trying Xtensa HIFI5 128-bit instruction table :\n");
      Result = decodeInstruction(DecoderTable128, MI, Insn128, Address, this, STI);
      if (Result != MCDisassembler::Fail) {
        Size = 16;
        return Result;
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

  // Custom decoders for standalone AE_L64_I_REAL / AE_S64_I_REAL
  if (Bytes.size() >= 3 && (Bytes[0] & 0xf) == 0x04) {
    if (Bytes[2] == 0xcf) { // AE_L64_I_REAL
      unsigned off = Bytes[0] >> 4;
      unsigned s = Bytes[1] & 0xf;
      unsigned t = Bytes[1] >> 4;
      MI.setOpcode(Xtensa::AE_L64_I_REAL);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createImm(off));
      Size = 3;
      return MCDisassembler::Success;
    }
    if (Bytes[2] == 0x01) { // AE_S64_I_REAL
      unsigned off = Bytes[0] >> 4;
      unsigned s = Bytes[1] & 0xf;
      unsigned d = Bytes[1] >> 4;
      MI.setOpcode(Xtensa::AE_S64_I_REAL);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[d]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createImm(off));
      Size = 3;
      return MCDisassembler::Success;
    }
  }

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
    if (Result == MCDisassembler::Success && MI.getOpcode() == Xtensa::L32R && Address < 0x1000) {
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

bool XtensaDisassembler::decodeSlotValImpl(MCInst &MI, uint32_t Val, uint64_t Address, unsigned SlotIdx, unsigned Format) const {
  if (Format == 3) {
    if (SlotIdx == 0) {
      unsigned s = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned op = (Val >> 8) & 0x1f;
      unsigned major = (Val >> 21) & 0x7;

      if (major == 4) {
        if (op == 10) { // L32I
          int32_t imm = (Val >> 13) & 0xff;
          MI.setOpcode(Xtensa::L32I);
          MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
          MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
          MI.addOperand(MCOperand::createImm(imm * 4));
          return true;
        }
        if (op == 27) { // S32I
          int32_t imm = (Val >> 13) & 0xff;
          MI.setOpcode(Xtensa::S32I);
          MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
          MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
          MI.addOperand(MCOperand::createImm(imm * 4));
          return true;
        }
        if (op == 2) { // ADDI
          int32_t imm = (Val >> 13) & 0xff;
          if (imm & 0x80) imm |= ~0xff;
          MI.setOpcode(Xtensa::ADDI);
          MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
          MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
          MI.addOperand(MCOperand::createImm(imm));
          return true;
        }
        if (op == 25) { // L16UI
          int32_t imm = (Val >> 13) & 0xff;
          MI.setOpcode(Xtensa::L16UI);
          MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
          MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
          MI.addOperand(MCOperand::createImm(imm * 2));
          return true;
        }
        if (op == 9) { // L16SI
          int32_t imm = (Val >> 13) & 0xff;
          MI.setOpcode(Xtensa::L16SI);
          MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
          MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
          MI.addOperand(MCOperand::createImm(imm * 2));
          return true;
        }
        if (op == 26) { // L8UI
          int32_t imm = (Val >> 13) & 0xff;
          MI.setOpcode(Xtensa::L8UI);
          MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
          MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
          MI.addOperand(MCOperand::createImm(imm));
          return true;
        }
      }

      if (((Val >> 17) & 0x7f) == 56 && ((Val >> 11) & 1) == 0) { // MOVI
        int32_t imm = (((Val >> 12) & 0x1f) << 7) | (((Val >> 8) & 0x7) << 4) | (Val & 0xf);
        if (imm & 0x800) imm |= ~0xfff;
        MI.setOpcode(Xtensa::MOVI);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        MI.addOperand(MCOperand::createImm(imm));
        return true;
      }

      // 3-register instructions
      unsigned op12 = (Val >> 12) & 0xfff;
      if (op12 == 3276) { // ADD
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::ADD);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 3278) { // ADDX2
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::ADDX2);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 3277) { // ADDX4
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::ADDX4);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 3279) { // ADDX8
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::ADDX8);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 3464) { // SUB
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::SUB);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 3280) { // AND
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::AND);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 3289) { // OR
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::OR);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 3466) { // XOR
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::XOR);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 3283) { // MULL
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::MULL);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
    }

    if (SlotIdx == 1) {
      unsigned t = Val & 0xf;
      unsigned s = (Val >> 4) & 0xf;
      unsigned op = (Val >> 16) & 0xf;

      if (op == 6) { // L32I
        int32_t imm = (Val >> 8) & 0xff;
        MI.setOpcode(Xtensa::L32I);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createImm(imm * 4));
        return true;
      }
      if (op == 4) { // L16SI
        int32_t imm = (Val >> 8) & 0xff;
        MI.setOpcode(Xtensa::L16SI);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createImm(imm * 2));
        return true;
      }
      if (op == 5) { // L16UI
        int32_t imm = (Val >> 8) & 0xff;
        MI.setOpcode(Xtensa::L16UI);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createImm(imm * 2));
        return true;
      }
      if (op == 7) { // L8UI
        int32_t imm = (Val >> 8) & 0xff;
        MI.setOpcode(Xtensa::L8UI);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createImm(imm));
        return true;
      }
      if (op == 2) { // ADDI
        int32_t imm = (Val >> 8) & 0xff;
        if (imm & 0x80) imm |= ~0xff;
        MI.setOpcode(Xtensa::ADDI);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createImm(imm));
        return true;
      }
      if (op == 8) { // MOVI
        int32_t imm = (Val >> 4) & 0xfff;
        if (imm & 0x800) imm |= ~0xfff;
        MI.setOpcode(Xtensa::MOVI);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        MI.addOperand(MCOperand::createImm(imm));
        return true;
      }

      unsigned op12 = (Val >> 12) & 0xff;
      if (op12 == 182) { // ADD
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::ADD);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 183) { // ADDX2
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::ADDX2);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 192) { // ADDX4
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::ADDX4);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 193) { // ADDX8
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::ADDX8);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 205) { // SUB
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::SUB);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 194) { // AND
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::AND);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 201) { // OR
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::OR);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
      if (op12 == 206) { // XOR
        unsigned r = (Val >> 8) & 0xf;
        MI.setOpcode(Xtensa::XOR);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      }
    }
  }

  unsigned prefix = Val >> 16;
  auto DecodeImm4 = [](unsigned t) -> int {
    if (t == 0) return -1;
    if (t < 8) return t;
    if (t == 8) return 8;
    if (t == 9) return 10;
    if (t == 10) return 12;
    if (t == 11) return 16;
    if (t == 12) return 32;
    if (t == 13) return 64;
    if (t == 14) return 128;
    return 256;
  };
  // First check if it is a slot NOP
  if (Val == 0x260B74 || Val == 0x0F3016 || Val == 0x0B000040 || Val == 0x3900 ||
      Val == 0x27B205 || Val == 0 || Val == 0xf9000 ||
      Val == 0x1E1C15 || Val == 0x0F1670 || Val == 0x176011 || Val == 0x070B1D ||
      Val == 0x10341D35 || Val == 0x0E57D0 || Val == 0x011400A0 || Val == 0x0114000a ||
      Val == 0xcbd70) {
    MI.setOpcode(Xtensa::NOP);
    return true;
  }

  // Manual decoders for standard density and scalar instructions in Slot 0 and Slot 1
  if (SlotIdx == 0) {
    unsigned prefix = Val >> 16;
    unsigned imm8_hi = (Val >> 12) & 0xf;
    unsigned imm8_lo = (Val >> 8) & 0xf;
    unsigned t0 = (Val >> 4) & 0xf;
    unsigned s0 = Val & 0xf;

    // MOV_N
    if (prefix == 0x26 && imm8_hi == 0 && imm8_lo == 9) {
      MI.setOpcode(Xtensa::MOV_N);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t0]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s0]));
      return true;
    }
    // ADD / ADD_N
    if (prefix == 0x1c && imm8_hi == 4) {
      MI.setOpcode(Xtensa::ADD);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[imm8_lo])); // dest
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s0]));      // src1
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t0]));      // src2
      return true;
    }
    // SUB
    if (prefix == 0x1d && imm8_hi == 3) {
      MI.setOpcode(Xtensa::SUB);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[imm8_lo]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s0]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t0]));
      return true;
    }
    // AND
    if (prefix == 0x1c && imm8_hi == 9) {
      MI.setOpcode(Xtensa::AND);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[imm8_lo]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s0]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t0]));
      return true;
    }
    // OR
    if (prefix == 0x1d && imm8_hi == 2) {
      MI.setOpcode(Xtensa::OR);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[imm8_lo]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s0]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t0]));
      return true;
    }
    // XOR
    if (prefix == 0x1d && imm8_hi == 7) {
      MI.setOpcode(Xtensa::XOR);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[imm8_lo]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s0]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t0]));
      return true;
    }
    // NEG
    if (prefix == 0x26 && imm8_hi == 8 && t0 == s0) {
      MI.setOpcode(Xtensa::NEG);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[imm8_lo]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s0]));
      return true;
    }
    // ABS
    if (prefix == 0x26 && imm8_hi == 8 && s0 == 0) {
      MI.setOpcode(Xtensa::ABS);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[imm8_lo]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t0]));
      return true;
    }
    // L32I / L32I_N
    if (prefix == 0x16) {
      int32_t imm = imm8_lo | (imm8_hi << 4);
      MI.setOpcode(Xtensa::L32I);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t0])); // dest
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s0])); // base
      MI.addOperand(MCOperand::createImm(imm * 4));
      return true;
    }
    // S32I / S32I_N
    if (prefix == 0x19) {
      int32_t imm = imm8_lo | (imm8_hi << 4);
      MI.setOpcode(Xtensa::S32I);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t0])); // src
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s0])); // base
      MI.addOperand(MCOperand::createImm(imm * 4));
      return true;
    }
    // L32R
    if (prefix == 0x00) {
      int32_t offset = (Val & 0xf) | (imm8_lo << 4) | (imm8_hi << 8);
      if (offset != 0 && (imm8_lo >= 10 && imm8_lo <= 15)) {
        // Fall through to branch decoder
      } else {
        if (offset & 0x800) offset |= ~0xfff;
        MI.setOpcode(Xtensa::L32R);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t0]));
        if (!::tryAddingSymbolicOperand(offset * 4 + ((Address + 3) & ~3), true, Address, 0, 3, MI, this))
          MI.addOperand(MCOperand::createImm(offset));
        return true;
      }
    }
    // MOVI / MOVI_N
    if (prefix == 0x1b) {
      int32_t imm = s0 | (imm8_lo << 4) | (imm8_hi << 8);
      if (imm & 0x800) imm |= ~0xfff;
      MI.setOpcode(Xtensa::MOVI);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t0]));
      MI.addOperand(MCOperand::createImm(imm));
      return true;
    }
    // ADDI / ADDI_N
    if (prefix == 0x12) {
      int32_t imm = imm8_lo | (imm8_hi << 4);
      if (imm & 0x80) imm |= ~0xff;
      MI.setOpcode(Xtensa::ADDI);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t0]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s0]));
      MI.addOperand(MCOperand::createImm(imm));
      return true;
    }
  }

  if (SlotIdx == 1) {
    unsigned opc12 = Val >> 12;
    unsigned r1 = (Val >> 8) & 0xf;
    unsigned t1 = (Val >> 4) & 0xf;
    unsigned s1 = Val & 0xf;

    // ADD / ADD_N
    if (opc12 == 0x1a4) {
      MI.setOpcode(Xtensa::ADD);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[r1])); // dest
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s1])); // src1
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t1])); // src2
      return true;
    }
    // SUB
    if (opc12 == 0x1b6) {
      MI.setOpcode(Xtensa::SUB);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[r1]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s1]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t1]));
      return true;
    }
    // AND
    if (opc12 == 0x1ab) {
      MI.setOpcode(Xtensa::AND);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[r1]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s1]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t1]));
      return true;
    }
    // OR
    if (opc12 == 0x1b4) {
      MI.setOpcode(Xtensa::OR);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[r1]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s1]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t1]));
      return true;
    }
    // XOR
    if (opc12 == 0x1ba) {
      MI.setOpcode(Xtensa::XOR);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[r1]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s1]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t1]));
      return true;
    }
    // NEG
    if (opc12 == 0x1f3 && t1 == s1) {
      MI.setOpcode(Xtensa::NEG);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[r1]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s1]));
      return true;
    }
    // ABS
    if (opc12 == 0x1f3 && s1 == 0) {
      MI.setOpcode(Xtensa::ABS);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[r1]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t1]));
      return true;
    }
    // MOV_N
    if (opc12 == 0x1f4 && r1 == 0) {
      MI.setOpcode(Xtensa::MOV_N);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t1])); // dest
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s1])); // src
      return true;
    }
    // MOVI / MOVI_N
    if (opc12 == 0x180 && r1 == 0) {
      int32_t imm = s1;
      if (imm & 0x8) imm |= ~0xf;
      MI.setOpcode(Xtensa::MOVI);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t1]));
      MI.addOperand(MCOperand::createImm(imm));
      return true;
    }
    // ADDI / ADDI_N
    if ((opc12 & 0xff0) == 0x120) {
      int32_t imm = r1 | ((opc12 & 0xf) << 4);
      if (imm & 0x80) imm |= ~0xff;
      MI.setOpcode(Xtensa::ADDI);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t1]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s1]));
      MI.addOperand(MCOperand::createImm(imm));
      return true;
    }
  }

  // Custom manual decoders for AE_SEL16I in Slot 0 and Slot 2
  if (SlotIdx == 0 && (Val >> 16) == 9) {
    unsigned r = (Val >> 12) & 0xf;
    unsigned s = (Val >> 8) & 0xf;
    unsigned sel = (Val >> 4) & 0xf;
    unsigned t = Val & 0xf;
    MI.setOpcode(Xtensa::AE_SEL16I);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    MI.addOperand(MCOperand::createImm(sel));
    return true;
  }

  if (SlotIdx == 2 && (Val >> 14) == 0xd) {
    unsigned r = Val & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned t = (Val >> 8) & 0xf;
    unsigned sel = (Val >> 12) & 0x3;
    MI.setOpcode(Xtensa::AE_SEL16I);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    MI.addOperand(MCOperand::createImm(sel));
    return true;
  }

  if (SlotIdx == 1 && (Val >> 12) == 0xf2 && ((Val >> 4) & 0xf) == 1) {
    unsigned r = (Val >> 8) & 0xf;
    unsigned t = Val & 0xf;
    MI.setOpcode(Xtensa::AE_MOV);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    return true;
  }

  // Shift decoders in Slot 0 and Slot 1
  if (SlotIdx == 1 && ((Val >> 12) == 0x0e1 || (Val >> 12) == 0x0e0 || (Val >> 12) == 0xee || (Val >> 12) == 0xd0 || (Val >> 12) == 0xd1)) {
    unsigned dest = (Val >> 8) & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned shift = Val & 0xf;
    MI.setOpcode(Xtensa::SRLI);
    MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
    MI.addOperand(MCOperand::createImm(shift));
    return true;
  }
  if (SlotIdx == 1 && ((Val >> 12) == 0x0a1 || (Val >> 12) == 0x0a0 || (Val >> 12) == 0x90 || (Val >> 12) == 0x91)) {
    unsigned dest = (Val >> 8) & 0xf;
    unsigned s = Val & 0xf;
    unsigned shift_val = (Val >> 4) & 0xf;
    unsigned shift = (32 - shift_val) & 0x1f;
    MI.setOpcode(Xtensa::SLLI);
    MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
    MI.addOperand(MCOperand::createImm(shift));
    return true;
  }
  if (SlotIdx == 1 && ((Val >> 12) == 0x0a2)) {
    unsigned dest = (Val >> 8) & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned shift = Val & 0xf;
    MI.setOpcode(Xtensa::SRAI);
    MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
    MI.addOperand(MCOperand::createImm(shift));
    return true;
  }
  if (SlotIdx == 0 && (Val >> 12) == 0x241) {
    unsigned dest = (Val >> 8) & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned shift = Val & 0xf;
    MI.setOpcode(Xtensa::SRLI);
    MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
    MI.addOperand(MCOperand::createImm(shift));
    return true;
  }
  if (SlotIdx == 0 && (Val >> 12) == 0x1c1) {
    unsigned dest = (Val >> 8) & 0xf;
    unsigned s = Val & 0xf;
    unsigned shift_val = (Val >> 4) & 0xf;
    unsigned shift = (32 - shift_val) & 0x1f;
    MI.setOpcode(Xtensa::SLLI);
    MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
    MI.addOperand(MCOperand::createImm(shift));
    return true;
  }
  if (SlotIdx == 0 && (Val >> 12) == 0x1c2) {
    unsigned dest = (Val >> 8) & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned shift = Val & 0xf;
    MI.setOpcode(Xtensa::SRAI);
    MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
    MI.addOperand(MCOperand::createImm(shift));
    return true;
  }

  // Format 4 Slot 0 branches
  if (SlotIdx == 0) {
    unsigned s = Val & 0xf;
    unsigned t = (Val >> 4) & 0xf;
    unsigned cond_reg = (Val >> 8) & 0xf;
    unsigned cond_imm = (Val >> 4) & 0xf;
    unsigned v_28_26 = (Val >> 26) & 0x7;

    unsigned Opc = 0;
    bool is_imm = false;
    bool is_reg = false;
    int16_t offset = 0;

    // Check if it is a VLIW branch (TableGen uses bit 28-26 to encode offset/format, or major prefix is non-zero)
    // Wait! Let's check: is the major prefix non-zero?
    // Since major prefix is at bits 28-23, if (Val >> 23) == 0, then v_28_26 is also 0.
    if (v_28_26 == 0 || (Format == 1 && cond_reg >= 10 && cond_reg <= 15)) {
      // Register-Register compare branches (prefix == 0x00)
      if (cond_reg == 10) Opc = Xtensa::BEQ;
      else if (cond_reg == 11) Opc = Xtensa::BNE;
      else if (cond_reg == 12) Opc = Xtensa::BLT;
      else if (cond_reg == 13) Opc = Xtensa::BGE;
      else if (cond_reg == 14) Opc = Xtensa::BLTU;
      else if (cond_reg == 15) Opc = Xtensa::BGEU;
      is_reg = true;

      // offset is 8 bits signed: bits 23-16 of Val
      int8_t raw_offset = (Val >> 16) & 0xff;
      offset = raw_offset;
    } else {
      // One-register and Immediate compare branches
      if (Format == 1) {
        // Format 1/2 branch condition codes
        if (cond_imm == 0) { Opc = Xtensa::BEQZ; }
        else if (cond_imm == 1) { Opc = Xtensa::BNEI; is_imm = true; }
        else if (cond_imm == 2) { Opc = Xtensa::BGEI; is_imm = true; }
        else if (cond_imm == 3) { Opc = Xtensa::BGEUI; is_imm = true; }
        else if (cond_imm == 4) { Opc = Xtensa::BLTI; is_imm = true; }
        else if (cond_imm == 5) { Opc = Xtensa::BLTUI; is_imm = true; }
        else if (cond_imm == 6) { Opc = Xtensa::BEQI; is_imm = true; }
        else if (cond_imm == 8) { Opc = Xtensa::BEQZ; }
        else if (cond_imm == 9) { Opc = Xtensa::BNEZ; }
        else if (cond_imm == 10) { Opc = Xtensa::BGEZ; }
        else if (cond_imm == 11) { Opc = Xtensa::BLTZ; }
      } else {
        // Format 4 branch condition codes
        if (cond_imm == 6) { Opc = Xtensa::BEQI; is_imm = true; }
        else if (cond_imm == 7) { Opc = Xtensa::BNEI; is_imm = true; }
        else if (cond_imm == 8) { Opc = Xtensa::BGEI; is_imm = true; }
        else if (cond_imm == 9) { Opc = Xtensa::BGEUI; is_imm = true; }
        else if (cond_imm == 12) { Opc = Xtensa::BLTI; is_imm = true; }
        else if (cond_imm == 13) { Opc = Xtensa::BLTUI; is_imm = true; }
        else if (cond_imm == 10) { Opc = Xtensa::BEQZ; }
        else if (cond_imm == 11) { Opc = Xtensa::BNEZ; }
        else if (cond_imm == 4) { Opc = Xtensa::BGEZ; }
        else if (cond_imm == 5) { Opc = Xtensa::BLTZ; }
      }

      // offset is 15 bits: bits 28-26, 11-8, 22-12
      int16_t raw_offset = (((Val >> 16) & 0x7f) << 8) | (((Val >> 12) & 0xf) << 4) | ((Val >> 8) & 0xf);
      if (raw_offset & 0x4000)
        raw_offset |= 0x8000;
      offset = raw_offset;
    }

    if (Opc) {
      MI.setOpcode(Opc);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      if (is_reg) {
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      } else if (is_imm) {
        MI.addOperand(MCOperand::createImm(DecodeImm4((Val >> 8) & 0xf)));
      }
      if (!::tryAddingSymbolicOperand(offset + 4 + Address, true, Address, 0, 3, MI, this))
        MI.addOperand(MCOperand::createImm(offset));
      return true;
    }
  }

  // Manual decoding for VLIW branches
  if (SlotIdx == 0) {
    unsigned major = Val >> 23;
    unsigned s = Val & 0xf;
    unsigned t = (Val >> 4) & 0xf;
    int16_t offset = (((Val >> 16) & 0x7f) << 8) | (((Val >> 12) & 0xf) << 4) | ((Val >> 8) & 0xf);
    if (offset & 0x4000)
      offset |= 0x8000;



    if (major == 0x16) { // One-register branches
      unsigned Opc = 0;
      if (t == 0) Opc = Xtensa::BEQZ;
      else if (t == 1) Opc = Xtensa::BGEZ;
      else if (t == 2) Opc = Xtensa::BLTZ;
      else if (t == 3) Opc = Xtensa::BNEZ;
      if (Opc) {
        MI.setOpcode(Opc);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        if (!::tryAddingSymbolicOperand(offset + 4 + Address, true, Address, 0, 3, MI, this))
          MI.addOperand(MCOperand::createImm(offset));
        return true;
      }
    } else if (major == 8 || major == 14 || major == 11 || major == 15) { // Immediate compare branches
      unsigned Opc = 0;
      if (major == 8) Opc = Xtensa::BEQI;
      else if (major == 14) Opc = Xtensa::BLTI;
      else if (major == 11) Opc = Xtensa::BGEUI;
      else if (major == 15) Opc = Xtensa::BLTUI;
      if (Opc) {
        MI.setOpcode(Opc);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createImm(DecodeImm4(t)));
        if (!::tryAddingSymbolicOperand(offset + 4 + Address, true, Address, 0, 3, MI, this))
          MI.addOperand(MCOperand::createImm(offset));
        return true;
      }
    } else if (major == 12 || major == 13 || major == 17 || major == 20) { // Register compare branches
      unsigned Opc = 0;
      if (major == 12) Opc = Xtensa::BEQ;
      else if (major == 13) Opc = Xtensa::BGE;
      else if (major == 17) Opc = Xtensa::BLT;
      else if (major == 20) Opc = Xtensa::BNE;
      if (Opc) {
        MI.setOpcode(Opc);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        if (!::tryAddingSymbolicOperand(offset + 4 + Address, true, Address, 0, 3, MI, this))
          MI.addOperand(MCOperand::createImm(offset));
        return true;
      }
    }
  }

  // Manual decoding for Slot 1 density instructions
  if (SlotIdx == 1) {
    if (((Val >> 12) & 0x3) == 3) {
      unsigned s = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned opc = (Val >> 8) & 0xf;
      if (opc < 8) { // MOVI_N
        unsigned imm7 = (opc << 4) | t;
        int sign_imm = imm7;
        if (sign_imm >= 96)
          sign_imm -= 128;
        MI.setOpcode(Xtensa::MOVI_N);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createImm(sign_imm));
        return true;
      } else if (opc == 8) { // MOV_N
        MI.setOpcode(Xtensa::MOV_N);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        return true;
      }
    }
  }

  // Manual decoding for custom HiFi instructions
  if (SlotIdx == 0) {
    if (((Val >> 8) & 0xf) == 3 && (Val >> 12) < 16) {
      unsigned s = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned r = (Val >> 12) & 0xf;
      MI.setOpcode(Xtensa::AE_L32X2_XC);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      return true;
    }
    unsigned prefix = Val >> 16;
    unsigned r = (Val >> 8) & 0xf;
    unsigned t = (Val >> 4) & 0xf;
    unsigned s = Val & 0xf;
    if (prefix == 0x26 && r == 11 && t == 0) {
      MI.setOpcode(Xtensa::AE_LA64_PP_HIFI3);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[0]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      return true;
    }
    if (prefix == 0x26 && r == 11 && t == 1) {
      MI.setOpcode(Xtensa::AE_SA64POS_FP_REAL);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[0]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      return true;
    }
    if (prefix == 0x26 && t == 1) {
      MI.setOpcode(Xtensa::AE_LA16X4_IP_HIFI3);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[0]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[0]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      return true;
    }
    if (prefix == 0x25 && t == 7) {
      if (((Val >> 4) & 0xf) == 2) {
        MI.setOpcode(Xtensa::AE_LA32X2_IP_HIFI3);
        MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[2]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[2]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        return true;
      }
    }
    if (prefix == 0x1032) {
      MI.setOpcode(Xtensa::AE_LA16X4_IP_HIFI3);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[0]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[0]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      return true;
    }
  }

  if (SlotIdx == 1) {
    if ((Val >> 12) == 0x91) {
      unsigned s = Val & 0xf;
      unsigned r = (Val >> 4) & 0xf;
      unsigned t = (Val >> 8) & 0xf;
      MI.setOpcode(Xtensa::AE_L32X2_XC);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      return true;
    }
    unsigned op1 = (Val >> 16) & 0xf;
    unsigned op0 = (Val >> 12) & 0xf;
    if (op1 == 0xd && op0 == 1) {
      unsigned s = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned r = (Val >> 8) & 0xf;
      MI.setOpcode(Xtensa::AE_L32X2_XC);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      return true;
    }
    if (op1 == 0xd && op0 == 4) {
      unsigned s = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned r = (Val >> 8) & 0xf;
      MI.setOpcode(Xtensa::AE_L32_XC_HIFI3);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      return true;
    }
    if (((Val >> 12) & 0xff) == 0xcc) {
      unsigned s = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned r = (Val >> 8) & 0xf;
      MI.setOpcode(Xtensa::AE_L32_XC_HIFI3);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      return true;
    }
  }

  if (SlotIdx == 2) {
    if ((Val >> 16) == 0x110) {
      unsigned r = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned v = (Val >> 8) & 0xf;
      unsigned s = (Val >> 12) & 0xf;
      MI.setOpcode(Xtensa::AE_MULAAAAFQ32X16);
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[v]));
      MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
      return true;
    }
  }

  // Check manual remapped scalar instructions
  if (SlotIdx == 0 || SlotIdx == 1) {
    if (prefix == 0x17 || prefix == 0x07 || prefix == 0x15 || prefix == 0x05 ||
        prefix == 0x14 || prefix == 0x04 || prefix == 0x16 || prefix == 0x06) {
      unsigned s = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned imm8 = (Val >> 8) & 0xff;
      unsigned Opc = Xtensa::L8UI;
      unsigned scale = 1;
      if (prefix == 0x15 || prefix == 0x05) { Opc = Xtensa::L16UI; scale = 2; }
      else if (prefix == 0x14 || prefix == 0x04) { Opc = Xtensa::L16SI; scale = 2; }
      else if (prefix == 0x16 || prefix == 0x06) { Opc = Xtensa::L32I; scale = 4; }
      MI.setOpcode(Opc);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createImm(imm8 * scale));
      return true;
    }

    if (SlotIdx == 0 && (prefix == 0x1a || prefix == 0x18 || prefix == 0x19)) {
      unsigned s = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned imm8 = (Val >> 8) & 0xff;
      unsigned Opc = Xtensa::S8I;
      unsigned scale = 1;
      if (prefix == 0x18) { Opc = Xtensa::S16I; scale = 2; }
      else if (prefix == 0x19) { Opc = Xtensa::S32I; scale = 4; }
      MI.setOpcode(Opc);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createImm(imm8 * scale));
      return true;
    }

    if (prefix == 0x12 || prefix == 0x02) {
      unsigned s = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned imm8 = (Val >> 8) & 0xff;
      int sign_imm = (imm8 ^ 0x80) - 0x80;
      MI.setOpcode(Xtensa::ADDI);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createImm(sign_imm));
      return true;
    }

    if (prefix == 0x1b || prefix == 0x08) {
      unsigned imm_low = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned imm_high = (Val >> 8) & 0xff;
      unsigned imm12 = (imm_high << 4) | imm_low;
      int sign_imm = (imm12 ^ 0x800) - 0x800;
      MI.setOpcode(Xtensa::MOVI);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
      MI.addOperand(MCOperand::createImm(sign_imm));
      return true;
    }

    if (prefix == 0x1c || prefix == 0x0a || prefix == 0x1d || prefix == 0x0b || prefix == 0x0d) {
      unsigned s = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned dest = (Val >> 8) & 0xf;
      unsigned opcExt = (Val >> 12) & 0xf;
      unsigned Opc = 0;
      if (prefix == 0x1c || prefix == 0x0a) {
        if (opcExt == 4) Opc = Xtensa::ADD;
        else if (opcExt == 5) Opc = Xtensa::ADDX2;
        else if (opcExt == 6) Opc = Xtensa::ADDX4;
        else if (opcExt == 7) Opc = Xtensa::ADDX8;
        else if (opcExt == 9 || opcExt == 0xb) Opc = Xtensa::AND;
        else if (opcExt == 1) Opc = Xtensa::SLLI;
        else if (opcExt == 2 || opcExt == 3) Opc = Xtensa::SRAI;
      } else {
        if (prefix == 0x0d) {
          if (opcExt == 10) Opc = Xtensa::SEXT;
        } else {
          if (opcExt == 3 || opcExt == 6) Opc = Xtensa::SUB;
          else if (opcExt == 2 || opcExt == 4) Opc = Xtensa::OR;
          else if (opcExt == 7 || opcExt == 0xa) Opc = Xtensa::XOR;
          else if (opcExt == 9) Opc = Xtensa::SEXT;
        }
      }
      
      if (Opc) {
        MI.setOpcode(Opc);
        if (Opc == Xtensa::SLLI) {
          unsigned shift_val = (16 - t) & 0xf;
          MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
          MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
          MI.addOperand(MCOperand::createImm(shift_val));
        } else if (Opc == Xtensa::SRAI) {
          unsigned shift_val = ((opcExt & 1) << 4) | s;
          MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
          MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
          MI.addOperand(MCOperand::createImm(shift_val));
        } else if (Opc == Xtensa::SEXT) {
          MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
          MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
          MI.addOperand(MCOperand::createImm(7));
        } else {
          MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
          MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
          MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        }
        return true;
      }
    }

    if (prefix == 0x24 || prefix == 0x0e) {
      unsigned shift_val = Val & 0xf;
      unsigned s = (Val >> 4) & 0xf;
      unsigned dest = (Val >> 8) & 0xf;
      MI.setOpcode(Xtensa::SRLI);
      MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
      MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
      MI.addOperand(MCOperand::createImm(shift_val));
      return true;
    }

    if (prefix == 0x26 || prefix == 0x0f || prefix == 0x36 || prefix == 0x1f) {
      unsigned s = Val & 0xf;
      unsigned t = (Val >> 4) & 0xf;
      unsigned dest = (Val >> 8) & 0xf;
      unsigned opcExt = (Val >> 12) & 0xf;
      if (opcExt == 8 || opcExt == 3) {
        MI.setOpcode(Xtensa::NEG);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        return true;
      } else if (opcExt == 4) {
        MI.setOpcode(Xtensa::MOV_N);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[t]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        return true;
      } else if (opcExt == 1) {
        MI.setOpcode(Xtensa::AE_SUB64_HIFI3);
        MI.addOperand(MCOperand::createReg(AEDRDecoderTable[dest]));
        MI.addOperand(MCOperand::createReg(AEDRDecoderTable[dest]));
        MI.addOperand(MCOperand::createReg(AEDRDecoderTable[dest]));
        return true;
      }
    }

    if ((prefix >> 1) == 0x08 || (prefix >> 1) == 0x00) {
      unsigned shift = Val & 0xf;
      unsigned s = (Val >> 4) & 0xf;
      unsigned dest = (Val >> 8) & 0xf;
      unsigned opcExt = ((Val >> 12) & 0xf) | ((prefix & 1) << 4);
      if ((opcExt & 1) == 0) {
        unsigned len = (opcExt >> 1) + 1;
        MI.setOpcode(Xtensa::EXTUI);
        MI.addOperand(MCOperand::createReg(ARDecoderTable[dest]));
        MI.addOperand(MCOperand::createReg(ARDecoderTable[s]));
        MI.addOperand(MCOperand::createImm(shift));
        MI.addOperand(MCOperand::createImm(len));
        return true;
      }
    }
  }

  // Check manual remapped instructions
  if (SlotIdx == 0 && (Val & 0xfffff0) == 0x17f700) {
    unsigned d = Val & 0xf;
    MI.setOpcode(Xtensa::AE_MOVFCRFSRV);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[d]));
    return true;
  }
  if (SlotIdx == 0 && (Val & 0xfffff0) == 0x17f710) {
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

  // AE_MUL16X4_REAL
  if ((Val & 0xfff00000) == 0x01000000) {
    unsigned q0 = Val & 0xf;
    unsigned q1 = (Val >> 4) & 0xf;
    unsigned s = (Val >> 8) & 0xf;
    unsigned r = (Val >> 12) & 0xf;
    MI.setOpcode(Xtensa::AE_MUL16X4_REAL);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[q0]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[q1]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[q0]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[q1]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
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

  // AE_ROUND16X4F32SSYM & AE_ROUND16X4F32SASYM
  if ((Val & 0xffe000) == 0x194000) {
    unsigned dest = Val & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned r = (Val >> 8) & 0xf;
    unsigned Opc = ((Val & 0xfff000) == 0x194000) ? Xtensa::AE_ROUND16X4F32SSYM : Xtensa::AE_ROUND16X4F32SASYM;
    MI.setOpcode(Opc);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    return true;
  }

  // AE_MULFP16X4S_REAL etc.
  if ((Val & 0xfff000) == 0x172000 ||
      (Val & 0xfff000) == 0x171000 ||
      (Val & 0xfff000) == 0x170000 ||
      (Val & 0xfff000) == 0x05f000 ||
      (Val & 0xfff000) == 0x05e000 ||
      (Val & 0xfff000) == 0x05d000 ||
      (Val & 0xfff000) == 0x05c000 ||
      (Val & 0xfff000) == 0x05b000 ||
      (Val & 0xfff000) == 0x05a000 ||
      (Val & 0xfff000) == 0x06c000 ||
      (Val & 0xfff000) == 0x06d000 ||
      (Val & 0xfff000) == 0x06e000 ||
      (Val & 0xfff000) == 0x06f000 ||
      (Val & 0xfff000) == 0x073000 ||
      (Val & 0xfff000) == 0x06a000 ||
      (Val & 0xfff000) == 0x71d000 ||
      (Val & 0xfff000) == 0x41d000 ||
      (Val & 0xfff000) == 0x51d000 ||
      (Val & 0xfff000) == 0x04e000 ||
      (Val & 0xfff000) == 0x0e3000 ||
      (Val & 0xfff000) == 0x0e2000 ||
      (Val & 0xfff000) == 0x0e1000 ||
      (Val & 0xfff000) == 0x0e0000 ||
      (Val & 0xfff000) == 0x0df000 ||
      (Val & 0xfff000) == 0x0de000 ||
      (Val & 0xfff000) == 0xf7000 ||
      (Val & 0xfff000) == 0xf2000 ||
      (Val & 0xfff000) == 0xf3000) {
    unsigned dest = Val & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned t = (Val >> 8) & 0xf;
    unsigned Opc = 0;
    switch (Val & 0xfff000) {
      case 0x172000: Opc = Xtensa::AE_MULFP16X4S_REAL; break;
      case 0x171000: Opc = Xtensa::AE_MULFP16X4RAS_REAL; break;
      case 0x170000: Opc = Xtensa::AE_MULFP16X4RS_REAL; break;
      case 0x05f000: Opc = Xtensa::AE_MULF32S_LL_REAL; break;
      case 0x05e000: Opc = Xtensa::AE_MULF32S_LH_REAL; break;
      case 0x05d000: Opc = Xtensa::AE_MULF32S_HH_REAL; break;
      case 0x05c000: Opc = Xtensa::AE_MULF32R_LL_REAL; break;
      case 0x05b000: Opc = Xtensa::AE_MULF32R_LH_REAL; break;
      case 0x05a000: Opc = Xtensa::AE_MULF32R_HH_REAL; break;
      case 0x06c000: Opc = Xtensa::AE_MULFP32X16X2RAS_H_REAL; break;
      case 0x06d000: Opc = Xtensa::AE_MULFP32X16X2RAS_L_REAL; break;
      case 0x06e000: Opc = Xtensa::AE_MULFP32X16X2RS_H_REAL; break;
      case 0x06f000: Opc = Xtensa::AE_MULFP32X16X2RS_L_REAL; break;
      case 0x073000: Opc = Xtensa::AE_MULFP32X2RS_REAL; break;
      case 0x06a000: Opc = Xtensa::AE_MULFP24X2R_REAL; break;
      case 0x71d000: Opc = Xtensa::AE_ADD32_HL_LH_REAL; break;
      case 0x41d000: Opc = Xtensa::AE_ADD64S_REAL; break;
      case 0x51d000: Opc = Xtensa::AE_SUB24S_REAL; break;
      case 0x04e000: Opc = Xtensa::AE_MUL32_HH_REAL; break;
      case 0x0e3000: Opc = Xtensa::AE_MULF32S_LL_REAL; break;
      case 0x0e2000: Opc = Xtensa::AE_MULF32S_LH_REAL; break;
      case 0x0e1000: Opc = Xtensa::AE_MULF32S_HH_REAL; break;
      case 0x0e0000: Opc = Xtensa::AE_MULF32R_LL_REAL; break;
      case 0x0df000: Opc = Xtensa::AE_MULF32R_LH_REAL; break;
      case 0x0de000: Opc = Xtensa::AE_MULF32R_HH_REAL; break;
      case 0x0f7000: Opc = Xtensa::AE_MULFP32X2RS_REAL; break;
      case 0x0f2000: Opc = Xtensa::AE_MULFP32X16X2RS_H_REAL; break;
      case 0x0f3000: Opc = Xtensa::AE_MULFP32X16X2RS_L_REAL; break;
    }
    MI.setOpcode(Opc);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    return true;
  }

  // AE_MULFC24RA_REAL
  if ((Val & 0xfff000) == 0x0f5000) {
    unsigned dest = Val & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned t = (Val >> 8) & 0xf;
    MI.setOpcode(Xtensa::AE_MULFC24RA_REAL);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    return true;
  }

  // AE_MULF16SS_00_REAL
  if ((Val & 0xfff000) == 0x059000) {
    unsigned dest = (Val >> 8) & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned r = Val & 0xf;
    MI.setOpcode(Xtensa::AE_MULF16SS_00_REAL);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[dest]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    return true;
  }

  // AE_MULSF32S_LL_REAL etc. (Multiply-Accumulate Format)
  if (SlotIdx == 2 &&
      ((Val & 0xfff000) == 0x093000 ||
       (Val & 0xfff000) == 0x092000 ||
       (Val & 0xfff000) == 0x091000 ||
       (Val & 0xfff000) == 0x090000 ||
       (Val & 0xfff000) == 0x08f000 ||
       (Val & 0xfff000) == 0x08e000 ||
       (Val & 0xfff000) == 0x041000 ||
       (Val & 0xfff000) == 0x042000 ||
       (Val & 0xfff000) == 0x046000 ||
       (Val & 0xfff000) == 0x02c000 ||
       (Val & 0xfff000) == 0x0aa000 ||
       (Val & 0xfff000) == 0x0a9000 ||
       (Val & 0xfff000) == 0x0a8000 ||
       (Val & 0xfff000) == 0x124000 ||
       (Val & 0xfff000) == 0x123000 ||
       (Val & 0xfff000) == 0x122000 ||
       (Val & 0xfff000) == 0x0a7000 ||
       (Val & 0xfff000) == 0x0a6000 ||
       (Val & 0xfff000) == 0x0a5000)) {
    unsigned t = (Val >> 8) & 0xf;
    unsigned s = (Val >> 4) & 0xf;
    unsigned r = Val & 0xf;
    unsigned Opc = 0;
    switch (Val & 0xfff000) {
      case 0x093000: Opc = Xtensa::AE_MULSF32S_LL_REAL; break;
      case 0x092000: Opc = Xtensa::AE_MULSF32S_LH_REAL; break;
      case 0x091000: Opc = Xtensa::AE_MULSF32S_HH_REAL; break;
      case 0x090000: Opc = Xtensa::AE_MULSF32R_LL_REAL; break;
      case 0x08f000: Opc = Xtensa::AE_MULSF32R_LH_REAL; break;
      case 0x08e000: Opc = Xtensa::AE_MULSF32R_HH_REAL; break;
      case 0x041000: Opc = Xtensa::AE_MULAFP32X16X2RS_H_REAL; break;
      case 0x042000: Opc = Xtensa::AE_MULAFP32X16X2RS_L_REAL; break;
      case 0x046000: Opc = Xtensa::AE_MULAFP32X2RS_REAL; break;
      case 0x02c000: Opc = Xtensa::AE_MULAF16SS_00_REAL; break;
      case 0x0aa000: Opc = Xtensa::AE_MULAF32S_LL_HIFI3; break;
      case 0x0a9000: Opc = Xtensa::AE_MULAF32S_LH_HIFI3; break;
      case 0x0a8000: Opc = Xtensa::AE_MULAF32S_HH_HIFI3; break;
      case 0x124000: Opc = Xtensa::AE_MULSF32S_LL_REAL; break;
      case 0x123000: Opc = Xtensa::AE_MULSF32S_LH_REAL; break;
      case 0x122000: Opc = Xtensa::AE_MULSF32S_HH_REAL; break;
      case 0x0a7000: Opc = Xtensa::AE_MULAF32R_LL_HIFI3; break;
      case 0x0a6000: Opc = Xtensa::AE_MULAF32R_LH_HIFI3; break;
      case 0x0a5000: Opc = Xtensa::AE_MULAF32R_HH_HIFI3; break;
    }
    MI.setOpcode(Opc);
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[t])); // acc = t
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[s]));
    MI.addOperand(MCOperand::createReg(AEDRDecoderTable[r]));
    return true;
  }

  // AE_L32X2_XC (Format 0/1/2)
  if ((SlotIdx == 0 || SlotIdx == 1) && (Val & 0xfff000) == 0x1f1000) {
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
  if (SlotIdx == 0 && (Val & 0xfff000) == 0x217000) {
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
  if ((SlotIdx == 0 || SlotIdx == 1) && (Val & 0xff00f0) == 0x1800b0) {
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
  if (SlotIdx == 0 && (Val & 0xff00f0) == 0x1800e0) {
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

    auto TryDecode = [&](uint32_t CV) {
      MCInst TmpMI;
      if (decodeInstruction(DecoderTableXtensaHIFI524, TmpMI, CV, Address, this, STI) == MCDisassembler::Success && isAllowedInSlot(TmpMI.getOpcode(), SlotIdx) && MCII->getName(TmpMI.getOpcode()).starts_with("AE_")) { MI = TmpMI; return true; }
      if (decodeInstruction(DecoderTableXtensaHIFI424, TmpMI, CV, Address, this, STI) == MCDisassembler::Success && isAllowedInSlot(TmpMI.getOpcode(), SlotIdx) && MCII->getName(TmpMI.getOpcode()).starts_with("AE_")) { MI = TmpMI; return true; }
      if (decodeInstruction(DecoderTableXtensaHIFI324, TmpMI, CV, Address, this, STI) == MCDisassembler::Success && isAllowedInSlot(TmpMI.getOpcode(), SlotIdx) && MCII->getName(TmpMI.getOpcode()).starts_with("AE_")) { MI = TmpMI; return true; }
      if (decodeInstruction(DecoderTableXtensaHIFIX24, TmpMI, CV, Address, this, STI) == MCDisassembler::Success && isAllowedInSlot(TmpMI.getOpcode(), SlotIdx) && MCII->getName(TmpMI.getOpcode()).starts_with("AE_")) { MI = TmpMI; return true; }
      if (decodeInstruction(DecoderTable24, TmpMI, CV, Address, this, STI) == MCDisassembler::Success && isAllowedInSlot(TmpMI.getOpcode(), SlotIdx) && MCII->getName(TmpMI.getOpcode()).starts_with("AE_")) { MI = TmpMI; return true; }
      return false;
    };

    // 1. Try standard candidate value
    if (TryDecode(CandidateVal)) return true;

    // 2. Try byte-swapped candidate value
    uint32_t ByteSwapped = ((CandidateVal & 0xFF) << 16) | (CandidateVal & 0xFF00) | ((CandidateVal >> 16) & 0xFF);
    if (TryDecode(ByteSwapped)) return true;

    // 3. Try nibble-rearranged candidate value
    uint8_t newB0 = CandidateVal & 0xFF;
    uint8_t newB1 = (CandidateVal >> 8) & 0xFF;
    uint8_t newB2 = (CandidateVal >> 16) & 0xFF;
    uint8_t b0 = (newB0 & 0xF0) | ((newB2 & 0xF0) >> 4);
    uint8_t b2 = ((newB2 & 0x0F) << 4) | (newB0 & 0x0F);
    uint32_t NibbleRearranged = b0 | (newB1 << 8) | (b2 << 16);
    if (TryDecode(NibbleRearranged)) return true;
  }

  return false;
}

bool XtensaDisassembler::decodeSlotVal(MCInst &MI, uint32_t Val, uint64_t Address, unsigned SlotIdx, unsigned Format) const {
  if (!decodeSlotValImpl(MI, Val, Address, SlotIdx, Format)) {
    return false;
  }
  return isAllowedInSlot(MI.getOpcode(), SlotIdx);
}

bool XtensaDisassembler::isAllowedInSlot(unsigned Opcode, unsigned SlotIdx) const {
  if (!MCII) return false;

  if (Opcode == Xtensa::NOP || Opcode == Xtensa::NOP_N)
    return true;

  if (SlotIdx == 12) {
    return false; // Slot 1 of Format 2 is a 1-bit slot that can only be NOP
  }

  StringRef Name = MCII->getName(Opcode);
  const MCInstrDesc &Desc = MCII->get(Opcode);

  if (Name.starts_with("AE_")) {
    if (Name.starts_with("AE_ABS32_HIFI3") || Name.starts_with("AE_ABS64S_HIFI3") ||
        Name.starts_with("AE_ABSSQ56S_HIFI3") || Name.starts_with("AE_ADDSQ56S_HIFI3") ||
        Name.starts_with("AE_ADDSUB32_HIFI3") || Name.starts_with("AE_ADDSUB32S_HIFI3") ||
        Name == "AE_SLAI64S_HIFI3" || Name.starts_with("AE_SEXT32X2D16_10")) {
      return SlotIdx == 0;
    }
    if (Desc.mayLoad() && !Desc.mayStore()) {
      return SlotIdx == 0 || SlotIdx == 1;
    }
    if (Name.starts_with("AE_MUL")) {
      return SlotIdx == 2;
    }
    if (Name.starts_with("AE_ROUND") || Name.starts_with("AE_SAT") ||
        Name.starts_with("AE_CVT") || Name.starts_with("AE_SLAI") ||
        Name.starts_with("AE_SRAI") || Name.starts_with("AE_MAX") ||
        Name.starts_with("AE_MIN") || Name.starts_with("AE_ABS") ||
        Name.starts_with("AE_NEG") || Name.starts_with("AE_TRUNC") ||
        Name.starts_with("AE_PKSR") || Name.starts_with("AE_SEXT")) {
      return SlotIdx == 3;
    }
    if (Name.starts_with("AE_ADD32_HL_LH") || Name.starts_with("AE_ADD64S") ||
        Name.starts_with("AE_SUB24S")) {
      return SlotIdx == 3;
    }
    if (Name.starts_with("AE_ADD") || Name.starts_with("AE_SUB") ||
        Name.starts_with("AE_SEL") || Name.starts_with("AE_AND") ||
        Name.starts_with("AE_OR") || Name.starts_with("AE_XOR") ||
        (Name.starts_with("AE_MOV") &&
         !Name.starts_with("AE_MOVAE") &&
         !Name.starts_with("AE_MOVEA") &&
         !Name.starts_with("AE_MOVFCRFSRV") &&
         !Name.starts_with("AE_MOVVFCRFSR")) ||
        Name.starts_with("AE_ZERO")) {
      return SlotIdx == 0 || SlotIdx == 1 || SlotIdx == 2 || SlotIdx == 3;
    }
    return SlotIdx == 0;
  }

  // Scalar instructions
  switch (Opcode) {
  case Xtensa::L32I:
  case Xtensa::L8UI:
  case Xtensa::L16UI:
  case Xtensa::L16SI:
  case Xtensa::L32I_N:
  case Xtensa::L32AI:
    return SlotIdx == 0 || SlotIdx == 1;

  case Xtensa::S32I:
  case Xtensa::L32R:
  case Xtensa::S8I:
  case Xtensa::S16I:
  case Xtensa::S32I_N:
  case Xtensa::S32RI:
    return SlotIdx == 0;

  case Xtensa::ADD:
  case Xtensa::ADDI:
  case Xtensa::SUB:
  case Xtensa::AND:
  case Xtensa::OR:
  case Xtensa::XOR:
  case Xtensa::MOVI:
  case Xtensa::MOV_N:
  case Xtensa::MOVI_N:
  case Xtensa::ADD_N:
  case Xtensa::ADDI_N:
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
    return SlotIdx == 0 || SlotIdx == 1;

  default:
    if (Desc.isConditionalBranch() || Desc.isUnconditionalBranch()) {
      return SlotIdx == 0;
    }
    break;
  }

  return false;
}
