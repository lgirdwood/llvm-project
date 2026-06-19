//===-- XtensaMCTargetDesc.cpp - Xtensa target descriptions ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#include "XtensaMCTargetDesc.h"
#include "TargetInfo/XtensaTargetInfo.h"
#include "XtensaInstPrinter.h"
#include "XtensaMCAsmInfo.h"
#include "XtensaTargetStreamer.h"
#include "llvm/MC/MCInst.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_INSTRINFO_MC_DESC
#include "XtensaGenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "XtensaGenRegisterInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "XtensaGenSubtargetInfo.inc"

using namespace llvm;

bool Xtensa::isValidAddrOffset(int Scale, int64_t OffsetVal) {
  bool Valid = false;

  switch (Scale) {
  case 1:
    Valid = (OffsetVal >= 0 && OffsetVal <= 255);
    break;
  case 2:
    Valid = (OffsetVal >= 0 && OffsetVal <= 510) && ((OffsetVal & 0x1) == 0);
    break;
  case 4:
    Valid = (OffsetVal >= 0 && OffsetVal <= 1020) && ((OffsetVal & 0x3) == 0);
    break;
  case 8:
    Valid = (OffsetVal >= 0 && OffsetVal <= 2040) && ((OffsetVal & 0x7) == 0);
    // Note: Some instructions like ae_s64.i might only support up to 56,
    // but the DAG selection will further constrain it if we use a specific Operand.
    // Wait, if selectMemRegAddr just checks isValidAddrOffset, we need to make sure
    // it doesn't allow 88! 
    // Actually, we can limit Scale=8 to 56 for now since only ae_s64.i uses it.
    Valid = (OffsetVal >= 0 && OffsetVal <= 56) && ((OffsetVal & 0x7) == 0);
    break;
  default:
    break;
  }
  return Valid;
}

bool Xtensa::isValidAddrOffsetForOpcode(unsigned Opcode, int64_t Offset) {
  int Scale = 0;

  switch (Opcode) {
  case Xtensa::L8UI:
  case Xtensa::S8I:
    Scale = 1;
    break;
  case Xtensa::L16SI:
  case Xtensa::L16UI:
  case Xtensa::S16I:
    Scale = 2;
    break;
  case Xtensa::AE_S64_I_HIFI3:
  case Xtensa::AE_L64_I_HIFI3:
    Scale = 8;
    break;
  case Xtensa::LEA_ADD:
    return (Offset >= -128 && Offset <= 127);
  default:
    // assume that MI is 32-bit load/store operation
    Scale = 4;
    break;
  }
  return isValidAddrOffset(Scale, Offset);
}

// Verify Special Register
bool Xtensa::checkRegister(MCRegister RegNo, const FeatureBitset &FeatureBits,
                           RegisterAccessType RAType) {
  switch (RegNo) {
  case Xtensa::BREG:
    return FeatureBits[Xtensa::FeatureBoolean];
  case Xtensa::CCOUNT:
  case Xtensa::CCOMPARE0:
    if (FeatureBits[Xtensa::FeatureTimers1])
      return true;
    [[fallthrough]];
  case Xtensa::CCOMPARE1:
    if (FeatureBits[Xtensa::FeatureTimers2])
      return true;
    [[fallthrough]];
  case Xtensa::CCOMPARE2:
    if (FeatureBits[Xtensa::FeatureTimers3])
      return true;
    return false;
  case Xtensa::CONFIGID0:
    return RAType != Xtensa::REGISTER_EXCHANGE;
  case Xtensa::CONFIGID1:
    return RAType == Xtensa::REGISTER_READ;
  case Xtensa::CPENABLE:
    return FeatureBits[Xtensa::FeatureCoprocessor];
  case Xtensa::DEBUGCAUSE:
    return RAType == Xtensa::REGISTER_READ && FeatureBits[Xtensa::FeatureDebug];
  case Xtensa::DEPC:
  case Xtensa::EPC1:
  case Xtensa::EXCCAUSE:
  case Xtensa::EXCSAVE1:
  case Xtensa::EXCVADDR:
    return FeatureBits[Xtensa::FeatureException];
    [[fallthrough]];
  case Xtensa::EPC2:
  case Xtensa::EPS2:
  case Xtensa::EXCSAVE2:
    if (FeatureBits[Xtensa::FeatureHighPriInterrupts])
      return true;
    [[fallthrough]];
  case Xtensa::EPC3:
  case Xtensa::EPS3:
  case Xtensa::EXCSAVE3:
    if (FeatureBits[Xtensa::FeatureHighPriInterruptsLevel3])
      return true;
    [[fallthrough]];
  case Xtensa::EPC4:
  case Xtensa::EPS4:
  case Xtensa::EXCSAVE4:
    if (FeatureBits[Xtensa::FeatureHighPriInterruptsLevel4])
      return true;
    [[fallthrough]];
  case Xtensa::EPC5:
  case Xtensa::EPS5:
  case Xtensa::EXCSAVE5:
    if (FeatureBits[Xtensa::FeatureHighPriInterruptsLevel5])
      return true;
    [[fallthrough]];
  case Xtensa::EPC6:
  case Xtensa::EPS6:
  case Xtensa::EXCSAVE6:
    if (FeatureBits[Xtensa::FeatureHighPriInterruptsLevel6])
      return true;
    [[fallthrough]];
  case Xtensa::EPC7:
  case Xtensa::EPS7:
  case Xtensa::EXCSAVE7:
    if (FeatureBits[Xtensa::FeatureHighPriInterruptsLevel7])
      return true;
    return false;
  case Xtensa::INTENABLE:
    return FeatureBits[Xtensa::FeatureInterrupt];
  case Xtensa::INTERRUPT:
    return RAType == Xtensa::REGISTER_READ &&
           FeatureBits[Xtensa::FeatureInterrupt];
  case Xtensa::INTSET:
  case Xtensa::INTCLEAR:
    return RAType == Xtensa::REGISTER_WRITE &&
           FeatureBits[Xtensa::FeatureInterrupt];
  case Xtensa::ICOUNT:
  case Xtensa::ICOUNTLEVEL:
  case Xtensa::IBREAKENABLE:
  case Xtensa::DDR:
  case Xtensa::IBREAKA0:
  case Xtensa::IBREAKA1:
  case Xtensa::DBREAKA0:
  case Xtensa::DBREAKA1:
  case Xtensa::DBREAKC0:
  case Xtensa::DBREAKC1:
    return FeatureBits[Xtensa::FeatureDebug];
  case Xtensa::LBEG:
  case Xtensa::LEND:
  case Xtensa::LCOUNT:
    return FeatureBits[Xtensa::FeatureLoop];
  case Xtensa::LITBASE:
    return FeatureBits[Xtensa::FeatureExtendedL32R];
  case Xtensa::MEMCTL:
    return FeatureBits[Xtensa::FeatureDataCache];
  case Xtensa::ACCLO:
  case Xtensa::ACCHI:
  case Xtensa::M0:
  case Xtensa::M1:
  case Xtensa::M2:
  case Xtensa::M3:
    return FeatureBits[Xtensa::FeatureMAC16];
  case Xtensa::MISC0:
  case Xtensa::MISC1:
  case Xtensa::MISC2:
  case Xtensa::MISC3:
    return FeatureBits[Xtensa::FeatureMiscSR];
  case Xtensa::PRID:
    return RAType == Xtensa::REGISTER_READ && FeatureBits[Xtensa::FeaturePRID];
  case Xtensa::THREADPTR:
    return FeatureBits[FeatureTHREADPTR];
  case Xtensa::VECBASE:
    return FeatureBits[Xtensa::FeatureRelocatableVector];
  case Xtensa::FCR:
  case Xtensa::FSR:
    return FeatureBits[FeatureSingleFloat];
  case Xtensa::F64R_LO:
  case Xtensa::F64R_HI:
  case Xtensa::F64S:
    return FeatureBits[FeatureDFPAccel];
  case Xtensa::WINDOWBASE:
  case Xtensa::WINDOWSTART:
    return FeatureBits[Xtensa::FeatureWindowed];
  case Xtensa::ATOMCTL:
  case Xtensa::SCOMPARE1:
    return FeatureBits[Xtensa::FeatureS32C1I];
  case Xtensa::NoRegister:
    return false;
  }

  return true;
}

// Get Xtensa User Register by encoding value.
MCRegister Xtensa::getUserRegister(unsigned Code, const MCRegisterInfo &MRI) {
  MCRegister UserReg = Xtensa::NoRegister;

  if (MRI.getEncodingValue(Xtensa::FCR) == Code) {
    UserReg = Xtensa::FCR;
  } else if (MRI.getEncodingValue(Xtensa::FSR) == Code) {
    UserReg = Xtensa::FSR;
  } else if (MRI.getEncodingValue(Xtensa::F64R_LO) == Code) {
    UserReg = Xtensa::F64R_LO;
  } else if (MRI.getEncodingValue(Xtensa::F64R_HI) == Code) {
    UserReg = Xtensa::F64R_HI;
  } else if (MRI.getEncodingValue(Xtensa::F64S) == Code) {
    UserReg = Xtensa::F64S;
  } else if (MRI.getEncodingValue(Xtensa::AE_CBEGIN0) == Code) {
    UserReg = Xtensa::AE_CBEGIN0;
  } else if (MRI.getEncodingValue(Xtensa::AE_CBEGIN1) == Code) {
    UserReg = Xtensa::AE_CBEGIN1;
  } else if (MRI.getEncodingValue(Xtensa::AE_CEND0) == Code) {
    UserReg = Xtensa::AE_CEND0;
  } else if (MRI.getEncodingValue(Xtensa::AE_CEND1) == Code) {
    UserReg = Xtensa::AE_CEND1;
  } else if (MRI.getEncodingValue(Xtensa::THREADPTR) == Code) {
    UserReg = Xtensa::THREADPTR;
  }

  return UserReg;
}

bool Xtensa::compress(MCInst &OutInst, const MCInst &MI, const MCSubtargetInfo &STI) {
  if (!STI.hasFeature(Xtensa::FeatureDensity))
    return false;

  unsigned Opcode = MI.getOpcode();
  switch (Opcode) {
  case Xtensa::OR: {
    if (MI.getNumOperands() == 3 && MI.getOperand(0).isReg() &&
        MI.getOperand(1).isReg() && MI.getOperand(2).isReg() &&
        MI.getOperand(1).getReg() == MI.getOperand(2).getReg()) {
      OutInst.setOpcode(Xtensa::MOV_N);
      OutInst.addOperand(MI.getOperand(0));
      OutInst.addOperand(MI.getOperand(1));
      return true;
    }
    break;
  }
  case Xtensa::ADD: {
    if (MI.getNumOperands() == 3 && MI.getOperand(0).isReg() &&
        MI.getOperand(1).isReg() && MI.getOperand(2).isReg()) {
      OutInst.setOpcode(Xtensa::ADD_N);
      OutInst.addOperand(MI.getOperand(0));
      OutInst.addOperand(MI.getOperand(1));
      OutInst.addOperand(MI.getOperand(2));
      return true;
    }
    break;
  }
  case Xtensa::ADDI: {
    if (MI.getNumOperands() == 3 && MI.getOperand(0).isReg() &&
        MI.getOperand(1).isReg() && MI.getOperand(2).isImm()) {
      int64_t Imm = MI.getOperand(2).getImm();
      if (Imm == -1 || (Imm >= 1 && Imm <= 15)) {
        OutInst.setOpcode(Xtensa::ADDI_N);
        OutInst.addOperand(MI.getOperand(0));
        OutInst.addOperand(MI.getOperand(1));
        OutInst.addOperand(MI.getOperand(2));
        return true;
      }
    }
    break;
  }
  case Xtensa::L32I: {
    if (MI.getNumOperands() == 3 && MI.getOperand(0).isReg() &&
        MI.getOperand(1).isReg() && MI.getOperand(2).isImm()) {
      int64_t Offset = MI.getOperand(2).getImm();
      if (Offset >= 0 && Offset <= 60 && (Offset % 4 == 0)) {
        OutInst.setOpcode(Xtensa::L32I_N);
        OutInst.addOperand(MI.getOperand(0));
        OutInst.addOperand(MI.getOperand(1));
        OutInst.addOperand(MI.getOperand(2));
        return true;
      }
    }
    break;
  }
  case Xtensa::S32I: {
    if (MI.getNumOperands() == 3 && MI.getOperand(0).isReg() &&
        MI.getOperand(1).isReg() && MI.getOperand(2).isImm()) {
      int64_t Offset = MI.getOperand(2).getImm();
      if (Offset >= 0 && Offset <= 60 && (Offset % 4 == 0)) {
        OutInst.setOpcode(Xtensa::S32I_N);
        OutInst.addOperand(MI.getOperand(0));
        OutInst.addOperand(MI.getOperand(1));
        OutInst.addOperand(MI.getOperand(2));
        return true;
      }
    }
    break;
  }
  case Xtensa::BEQZ: {
    if (MI.getNumOperands() == 2 && MI.getOperand(0).isReg() &&
        (MI.getOperand(1).isExpr() || MI.getOperand(1).isImm())) {
      OutInst.setOpcode(Xtensa::BEQZ_N);
      OutInst.addOperand(MI.getOperand(0));
      OutInst.addOperand(MI.getOperand(1));
      return true;
    }
    break;
  }
  case Xtensa::BNEZ: {
    if (MI.getNumOperands() == 2 && MI.getOperand(0).isReg() &&
        (MI.getOperand(1).isExpr() || MI.getOperand(1).isImm())) {
      OutInst.setOpcode(Xtensa::BNEZ_N);
      OutInst.addOperand(MI.getOperand(0));
      OutInst.addOperand(MI.getOperand(1));
      return true;
    }
    break;
  }
  case Xtensa::NOP: {
    OutInst.setOpcode(Xtensa::NOP_N);
    return true;
  }
  case Xtensa::RET: {
    OutInst.setOpcode(Xtensa::RET_N);
    return true;
  }
  case Xtensa::RETW: {
    OutInst.setOpcode(Xtensa::RETW_N);
    return true;
  }
  case Xtensa::MOVI: {
    if (MI.getNumOperands() == 2 && MI.getOperand(0).isReg() && MI.getOperand(1).isImm()) {
      int64_t Imm = MI.getOperand(1).getImm();
      if (Imm >= -32 && Imm <= 95) {
        OutInst.setOpcode(Xtensa::MOVI_N);
        OutInst.addOperand(MI.getOperand(0));
        OutInst.addOperand(MI.getOperand(1));
        return true;
      }
    }
    break;
  }
  }

  return false;
}

static MCAsmInfo *createXtensaMCAsmInfo(const MCRegisterInfo &MRI,
                                        const Triple &TT,
                                        const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new XtensaMCAsmInfo(TT, Options);
  return MAI;
}

static MCInstrInfo *createXtensaMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitXtensaMCInstrInfo(X);
  return X;
}

static MCInstPrinter *createXtensaMCInstPrinter(const Triple &TT,
                                                unsigned SyntaxVariant,
                                                const MCAsmInfo &MAI,
                                                const MCInstrInfo &MII,
                                                const MCRegisterInfo &MRI) {
  return new XtensaInstPrinter(MAI, MII, MRI);
}

static MCRegisterInfo *createXtensaMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitXtensaMCRegisterInfo(X, Xtensa::SP);
  return X;
}

static MCSubtargetInfo *
createXtensaMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createXtensaMCSubtargetInfoImpl(TT, CPU, CPU, FS);
}

static MCTargetStreamer *
createXtensaAsmTargetStreamer(MCStreamer &S, formatted_raw_ostream &OS,
                              MCInstPrinter *InstPrint) {
  return new XtensaTargetAsmStreamer(S, OS);
}

static MCTargetStreamer *
createXtensaObjectTargetStreamer(MCStreamer &S, const MCSubtargetInfo &STI) {
  return new XtensaTargetELFStreamer(S, STI);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeXtensaTargetMC() {
  // Register the MCAsmInfo.
  TargetRegistry::RegisterMCAsmInfo(getTheXtensaTarget(),
                                    createXtensaMCAsmInfo);

  // Register the MCCodeEmitter.
  TargetRegistry::RegisterMCCodeEmitter(getTheXtensaTarget(),
                                        createXtensaMCCodeEmitter);

  // Register the MCInstrInfo.
  TargetRegistry::RegisterMCInstrInfo(getTheXtensaTarget(),
                                      createXtensaMCInstrInfo);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(getTheXtensaTarget(),
                                        createXtensaMCInstPrinter);

  // Register the MCRegisterInfo.
  TargetRegistry::RegisterMCRegInfo(getTheXtensaTarget(),
                                    createXtensaMCRegisterInfo);

  // Register the MCSubtargetInfo.
  TargetRegistry::RegisterMCSubtargetInfo(getTheXtensaTarget(),
                                          createXtensaMCSubtargetInfo);

  // Register the MCAsmBackend.
  TargetRegistry::RegisterMCAsmBackend(getTheXtensaTarget(),
                                       createXtensaAsmBackend);

  // Register the asm target streamer.
  TargetRegistry::RegisterAsmTargetStreamer(getTheXtensaTarget(),
                                            createXtensaAsmTargetStreamer);

  // Register the ELF target streamer.
  TargetRegistry::RegisterObjectTargetStreamer(
      getTheXtensaTarget(), createXtensaObjectTargetStreamer);
}
