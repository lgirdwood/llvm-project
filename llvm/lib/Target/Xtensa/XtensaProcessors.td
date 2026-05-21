//===- XtensaProcessors.td - Xtensa Processors -------------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Xtensa supported processors.
//===----------------------------------------------------------------------===//
class Proc<string Name, list<SubtargetFeature> Features>
    : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;

//===----------------------------------------------------------------------===//
// Cadence/Tensilica reference cores
//===----------------------------------------------------------------------===//

// dc233c - Diamond Standard 233L core (Xtensa LX reference)
def : Proc<"dc233c", [FeatureDensity, FeatureWindowed, FeatureLoop, FeatureSEXT, FeatureNSA,
                      FeatureMul16, FeatureMul32, FeatureDiv32, FeatureS32C1I, FeatureTHREADPTR,
                      FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                      FeatureHighPriInterruptsLevel6, FeatureCoprocessor, FeatureInterrupt,
                      FeatureDataCache, FeatureRelocatableVector, FeatureTimers3, FeaturePRID,
                      FeatureMINMAX, FeatureCLAMPS]>;

// sample_controller - Xtensa sample controller core (no loops, no FP)
def : Proc<"sample_controller", [FeatureDensity, FeatureWindowed, FeatureSEXT, FeatureNSA,
                                  FeatureMul16, FeatureMul32, FeatureDiv32, FeatureS32C1I,
                                  FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                  FeatureHighPriInterruptsLevel6, FeatureInterrupt,
                                  FeatureRelocatableVector, FeatureTimers3, FeaturePRID,
                                  FeatureMINMAX]>;

// sample_controller32 - Xtensa sample controller 32-bit core (no loops, no FP, no S32C1I)
def : Proc<"sample_controller32", [FeatureDensity, FeatureWindowed, FeatureSEXT, FeatureNSA,
                                    FeatureMul16, FeatureMul32, FeatureDiv32,
                                    FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                    FeatureHighPriInterruptsLevel6, FeatureInterrupt,
                                    FeatureRelocatableVector, FeatureTimers3, FeaturePRID,
                                    FeatureMINMAX]>;

//===----------------------------------------------------------------------===//
// Espressif ESP32 family
//===----------------------------------------------------------------------===//

def : Proc<"esp32", [FeatureDensity, FeatureSingleFloat, FeatureLoop, FeatureMAC16, FeatureWindowed, FeatureBoolean, FeatureSEXT,
                     FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDFPAccel, FeatureS32C1I, FeatureTHREADPTR, FeatureDiv32,
                     FeatureDebug, FeatureException, FeatureHighPriInterrupts, FeatureHighPriInterruptsLevel7, FeatureCoprocessor,
                     FeatureInterrupt, FeatureDataCache, FeatureRelocatableVector, FeatureTimers3, FeaturePRID, FeatureRegionProtection, FeatureMiscSR,
                     FeatureMINMAX, FeatureCLAMPS]>;

// esp32s2 - No loops, no booleans, no S32C1I, no FP, no MAC16
def : Proc<"espressif_esp32s2", [FeatureDensity, FeatureWindowed, FeatureSEXT, FeatureNSA,
                                  FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32,
                                  FeatureTHREADPTR, FeatureDebug, FeatureException,
                                  FeatureHighPriInterrupts, FeatureHighPriInterruptsLevel6,
                                  FeatureCoprocessor, FeatureInterrupt,
                                  FeatureRelocatableVector, FeatureTimers3, FeaturePRID,
                                  FeatureRegionProtection, FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR]>;

// esp32s3 - Full featured with FP, booleans, loops, MAC16
def : Proc<"espressif_esp32s3", [FeatureDensity, FeatureSingleFloat, FeatureMAC16, FeatureWindowed, FeatureBoolean,
                                  FeatureLoop, FeatureSEXT, FeatureNSA,
                                  FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32,
                                  FeatureS32C1I, FeatureTHREADPTR, FeatureDebug, FeatureException,
                                  FeatureHighPriInterrupts, FeatureHighPriInterruptsLevel6,
                                  FeatureCoprocessor, FeatureInterrupt,
                                  FeatureRelocatableVector, FeatureTimers3, FeaturePRID,
                                  FeatureRegionProtection, FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR]>;

def : Proc<"esp8266", [FeatureDensity, FeatureNSA, FeatureMul16, FeatureMul32, FeatureExtendedL32R, FeatureDebug, FeatureException,
                       FeatureHighPriInterrupts, FeatureHighPriInterruptsLevel3, FeatureInterrupt, FeatureRelocatableVector, FeatureTimers1,
					   FeatureRegionProtection, FeaturePRID]>;

//===----------------------------------------------------------------------===//
// Intel Audio DSP processors
//===----------------------------------------------------------------------===//

// intel_tgl_adsp - Tiger Lake Audio DSP (HiFi3, no MiscSR)
def : Proc<"intel_tgl_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                               FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32,
                               FeatureS32C1I, FeatureSingleFloat,
                               FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                               FeatureHighPriInterruptsLevel6, FeatureCoprocessor, FeatureInterrupt,
                               FeatureDataCache, FeatureRelocatableVector, FeatureTimers3, FeaturePRID,
                               FeatureMINMAX, FeatureCLAMPS, FeatureHIFI3]>;

// intel_ace15_adsp - ACE 1.5 Audio DSP (HiFi4)
// Note: FeatureSingleFloat intentionally omitted - the Zephyr SDK GNU
// assembler for this core does not recognize FP opcodes (ssi/lsi) or
// FP registers (f0..f15), so emitting them breaks the assembly step.
def : Proc<"intel_ace15_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                                FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32, FeatureS32C1I,
                                FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                FeatureHighPriInterruptsLevel4, FeatureCoprocessor, FeatureInterrupt, FeatureDataCache,
                                FeatureRelocatableVector, FeatureTimers1, FeaturePRID,
                                FeatureMINMAX, FeatureCLAMPS, FeatureHIFI4]>;

// intel_ace30_adsp - ACE 3.0 Audio DSP (HiFi4)
def : Proc<"intel_ace30_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                               FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32, FeatureS32C1I,
                               FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                               FeatureHighPriInterruptsLevel5, FeatureCoprocessor, FeatureInterrupt, FeatureDataCache,
                               FeatureRelocatableVector, FeatureTimers2, FeaturePRID, FeatureMiscSR, FeatureMINMAX,
                               FeatureCLAMPS, FeatureHIFI4]>;

// intel_ace40_adsp - ACE 4.0 Audio DSP (HiFi5)
def : Proc<"intel_ace40_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                           FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32, FeatureS32C1I,
                           FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                           FeatureHighPriInterruptsLevel5, FeatureCoprocessor, FeatureInterrupt, FeatureDataCache,
                           FeatureRelocatableVector, FeatureTimers2, FeaturePRID, FeatureMiscSR, FeatureMINMAX,
                           FeatureCLAMPS, FeatureHIFI5]>;

//===----------------------------------------------------------------------===//
// AMD Audio Co-Processor family
//===----------------------------------------------------------------------===//

// amd_acp_6_0_adsp - AMD ACP 6.0 Audio DSP (HiFi5)
def : Proc<"amd_acp_6_0_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                                 FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32,
                                 FeatureS32C1I, FeatureSingleFloat,
                                 FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                 FeatureHighPriInterruptsLevel6, FeatureCoprocessor, FeatureInterrupt,
                                 FeatureDataCache, FeaturePRID, FeatureTimers2,
                                 FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI5]>;

// amd_acp_7_0_adsp - AMD ACP 7.0 Audio DSP (HiFi5, no S32C1I)
def : Proc<"amd_acp_7_0_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                                 FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32,
                                 FeatureSingleFloat,
                                 FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                 FeatureHighPriInterruptsLevel6, FeatureCoprocessor, FeatureInterrupt,
                                 FeatureDataCache, FeaturePRID, FeatureTimers2,
                                 FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI5]>;

// amd_acp_7_3_adsp - AMD ACP 7.3 Audio DSP (HiFi5, no S32C1I)
def : Proc<"amd_acp_7_3_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                                 FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32,
                                 FeatureSingleFloat,
                                 FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                 FeatureHighPriInterruptsLevel6, FeatureCoprocessor, FeatureInterrupt,
                                 FeatureDataCache, FeaturePRID, FeatureTimers2,
                                 FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI5]>;

//===----------------------------------------------------------------------===//
// MediaTek Audio DSP processors
//===----------------------------------------------------------------------===//

// mtk_mt818x_adsp - MediaTek MT818x Audio DSP (HiFi3, no S32C1I, no Mul32High)
def : Proc<"mtk_mt818x_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                                FeatureNSA, FeatureMul16, FeatureMul32, FeatureDiv32,
                                FeatureSingleFloat,
                                FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                FeatureHighPriInterruptsLevel5, FeatureCoprocessor, FeatureInterrupt,
                                FeatureDataCache, FeatureRelocatableVector, FeatureTimers3, FeaturePRID,
                                FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI3]>;

// mtk_mt8195_adsp - MediaTek MT8195 Audio DSP (HiFi4, no S32C1I, no Mul32High)
def : Proc<"mtk_mt8195_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                                FeatureNSA, FeatureMul16, FeatureMul32, FeatureDiv32,
                                FeatureSingleFloat,
                                FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                FeatureHighPriInterruptsLevel4, FeatureCoprocessor, FeatureInterrupt,
                                FeatureDataCache, FeatureRelocatableVector, FeatureTimers3, FeaturePRID,
                                FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI4]>;

// mtk_mt8196_adsp - MediaTek MT8196 Audio DSP (HiFi3, no S32C1I, no Mul32High)
def : Proc<"mtk_mt8196_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                                FeatureNSA, FeatureMul16, FeatureMul32, FeatureDiv32,
                                FeatureSingleFloat,
                                FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                FeatureHighPriInterruptsLevel5, FeatureCoprocessor, FeatureInterrupt,
                                FeatureDataCache, FeatureRelocatableVector, FeatureTimers3, FeaturePRID,
                                FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI3]>;

// mtk_mt8365_adsp - MediaTek MT8365 Audio DSP (HiFi4, no S32C1I, no Mul32High)
def : Proc<"mtk_mt8365_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                                FeatureNSA, FeatureMul16, FeatureMul32, FeatureDiv32,
                                FeatureSingleFloat,
                                FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                FeatureHighPriInterruptsLevel4, FeatureCoprocessor, FeatureInterrupt,
                                FeatureDataCache, FeatureRelocatableVector, FeatureTimers3, FeaturePRID,
                                FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI4]>;

//===----------------------------------------------------------------------===//
// NXP Audio DSP processors
//===----------------------------------------------------------------------===//

// nxp_imx_adsp - NXP i.MX8 Audio DSP (HiFi4)
def : Proc<"nxp_imx_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                             FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32,
                             FeatureS32C1I, FeatureSingleFloat,
                             FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                             FeatureHighPriInterruptsLevel4, FeatureCoprocessor, FeatureInterrupt,
                             FeatureDataCache, FeatureRelocatableVector, FeatureTimers2, FeaturePRID,
                             FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI4]>;

// nxp_imx8m_adsp - NXP i.MX8M Audio DSP (HiFi4)
def : Proc<"nxp_imx8m_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                               FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32,
                               FeatureS32C1I, FeatureSingleFloat,
                               FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                               FeatureHighPriInterruptsLevel4, FeatureCoprocessor, FeatureInterrupt,
                               FeatureDataCache, FeatureRelocatableVector, FeatureTimers2, FeaturePRID,
                               FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI4]>;

// nxp_imx8ulp_adsp - NXP i.MX8ULP Audio DSP (HiFi4, DFPAccel)
def : Proc<"nxp_imx8ulp_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                                 FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32,
                                 FeatureS32C1I, FeatureSingleFloat, FeatureDFPAccel,
                                 FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                 FeatureHighPriInterruptsLevel4, FeatureCoprocessor, FeatureInterrupt,
                                 FeatureDataCache, FeatureRelocatableVector, FeatureTimers2, FeaturePRID,
                                 FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI4]>;

// nxp_rt500_adsp - NXP RT500 Audio DSP (HiFi3, no Mul16)
def : Proc<"nxp_rt500_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                               FeatureNSA, FeatureMul32, FeatureMul32High, FeatureDiv32,
                               FeatureS32C1I, FeatureSingleFloat,
                               FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                               FeatureHighPriInterruptsLevel4, FeatureCoprocessor, FeatureInterrupt,
                               FeatureRelocatableVector, FeatureTimers2, FeaturePRID,
                               FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI3]>;

// nxp_rt600_adsp - NXP RT600 Audio DSP (HiFi4, DFPAccel)
def : Proc<"nxp_rt600_adsp", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                               FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32,
                               FeatureS32C1I, FeatureSingleFloat, FeatureDFPAccel,
                               FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                               FeatureHighPriInterruptsLevel4, FeatureCoprocessor, FeatureInterrupt,
                               FeatureDataCache, FeatureRelocatableVector, FeatureTimers2, FeaturePRID,
                               FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI4]>;

// nxp_rt700_hifi1 - NXP RT700 HiFi1 core (HiFi3, no booleans, no Mul16/Mul32, no DCache)
def : Proc<"nxp_rt700_hifi1", [FeatureDensity, FeatureWindowed, FeatureLoop, FeatureSEXT,
                                FeatureNSA, FeatureDiv32,
                                FeatureS32C1I, FeatureSingleFloat,
                                FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                FeatureHighPriInterruptsLevel4, FeatureCoprocessor, FeatureInterrupt,
                                FeatureRelocatableVector, FeatureTimers2, FeaturePRID,
                                FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI3]>;

// nxp_rt700_hifi4 - NXP RT700 HiFi4 core (HiFi4, DFPAccel)
def : Proc<"nxp_rt700_hifi4", [FeatureDensity, FeatureWindowed, FeatureBoolean, FeatureLoop, FeatureSEXT,
                                FeatureNSA, FeatureMul16, FeatureMul32, FeatureMul32High, FeatureDiv32,
                                FeatureS32C1I, FeatureSingleFloat, FeatureDFPAccel,
                                FeatureTHREADPTR, FeatureDebug, FeatureException, FeatureHighPriInterrupts,
                                FeatureHighPriInterruptsLevel4, FeatureCoprocessor, FeatureInterrupt,
                                FeatureDataCache, FeatureRelocatableVector, FeatureTimers2, FeaturePRID,
                                FeatureMINMAX, FeatureCLAMPS, FeatureMiscSR, FeatureHIFI4]>;
