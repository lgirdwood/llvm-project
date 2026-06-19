//===- XtensaHIFIInstrInfo.td - Xtensa Target Description ---*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the Xtensa HiFi Audio Engine instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

let Predicates = [HasHIFI3] in {

  // Load Instructions
  let mayLoad = 1 in {
    let Predicates = [HasHIFI4], DecoderNamespace = "XtensaHIFI4" in {
      def AE_L32_I_HIFI4 : RRR_Inst<0x04, 0x0D, 0x0B, (outs AEDR:$r), (ins AR:$s, imm8:$t),
                                    "ae_l32.i\t$r, $s, $t", [(set AEDR:$r, (int_xtensa_ae_l32_i AR:$s, imm8:$t))]>;
      def AE_L16_I_HIFI4 : RRR_Inst<0x04, 0x00, 0x0B, (outs AEDR:$r), (ins AR:$s, imm8:$t),
                                    "ae_l16.i\t$r, $s, $t", [(set AEDR:$r, (int_xtensa_ae_l16_i AR:$s, imm8:$t))]>;

      def AE_L32_X_HIFI4 : RRR_Inst<0x04, 0x0F, 0x0B, (outs AEDR:$r), (ins AR:$s, AR:$t),
                                    "ae_l32.x\t$r, $s, $t", [(set AEDR:$r, (int_xtensa_ae_l32_x AR:$s, AR:$t))]>;
    }
    // HiFi3 variants for ae_l32.i and ae_l16.i (same opcode as HiFi4)
    let Predicates = [HasHIFI3, HasNoHIFI4], DecoderNamespace = "XtensaHIFI3" in {
      def AE_L32_I_HIFI3 : RRR_Inst<0x04, 0x0D, 0x0B, (outs AEDR:$r), (ins AR:$s, imm8:$t),
                                    "ae_l32.i\t$r, $s, $t", [(set AEDR:$r, (int_xtensa_ae_l32_i AR:$s, imm8:$t))]>;
      def AE_L16_I_HIFI3 : RRR_Inst<0x04, 0x00, 0x0B, (outs AEDR:$r), (ins AR:$s, imm8:$t),
                                    "ae_l16.i\t$r, $s, $t", [(set AEDR:$r, (int_xtensa_ae_l16_i AR:$s, imm8:$t))]>;
    }
    
    let Predicates = [HasHIFI5], DecoderNamespace = "XtensaHIFI5" in {
      def AE_L32_I_HIFI5 : RRR_Inst<0x04, 0x07, 0x0C, (outs AEDR:$r), (ins AR:$s, imm8:$t),
                                    "ae_l32.i\t$r, $s, $t", [(set AEDR:$r, (int_xtensa_ae_l32_i AR:$s, imm8:$t))]>;
      def AE_L16_I_HIFI5 : RRR_Inst<0x04, 0x07, 0x04, (outs AEDR:$r), (ins AR:$s, imm8:$t),
                                    "ae_l16.i\t$r, $s, $t", [(set AEDR:$r, (int_xtensa_ae_l16_i AR:$s, imm8:$t))]>;

      def AE_L32_X_HIFI5 : RRR_Inst<0x04, 0x00, 0x08, (outs AEDR:$r), (ins AR:$s, AR:$t),
                                    "ae_l32.x\t$r, $s, $t", [(set AEDR:$r, (int_xtensa_ae_l32_x AR:$s, AR:$t))]>;
    }
  }

  // Store Instructions
  let mayStore = 1 in {
    let Predicates = [HasHIFI4], DecoderNamespace = "XtensaHIFI4" in {
      def AE_S32_L_I_HIFI4 : RRR_Inst<0x04, 0x00, 0x0E, (outs), (ins AEDR:$r, AR:$s, imm8:$t),
                                      "ae_s32.l.i\t$r, $s, $t", [(int_xtensa_ae_s32_l_i AEDR:$r, AR:$s, imm8:$t)]>;
      def AE_S16_I_HIFI4 : RRR_Inst<0x04, 0x02, 0x0D, (outs), (ins AEDR:$r, AR:$s, imm8:$t),
                                    "ae_s16.0.i\t$r, $s, $t", [(int_xtensa_ae_s16_i AEDR:$r, AR:$s, imm8:$t)]>;

      def AE_S32_X_HIFI4 : RRR_Inst<0x04, 0x02, 0x0E, (outs), (ins AEDR:$r, AR:$s, AR:$t),
                                    "ae_s32.l.x\t$r, $s, $t", [(int_xtensa_ae_s32_l_x AEDR:$r, AR:$s, AR:$t)]>;
    }
    // HiFi3 variant: ae_s32.l.i same opcode on HiFi3.
    def AE_S32_L_I_HIFI3 : RRR_Inst<0x04, 0x00, 0x0E, (outs), (ins AEDR:$r, AR:$s, imm8:$t),
                                    "ae_s32.l.i\t$r, $s, $t", [(int_xtensa_ae_s32_l_i AEDR:$r, AR:$s, imm8:$t)]> {
      let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3, HasNoHIFI4];
    }

    // AE_S32_L_X
    def AE_S32_L_X_HIFI4 : RRR_Inst<0x00, 0x0D, 0x01, (outs), (ins AEDR:$t, AR:$s, AR:$r),
                                    "ae_s32.l.x\t$t, $s, $r",
                                    [(int_xtensa_ae_s32_l_x AEDR:$t, AR:$s, AR:$r)]> {
      let DecoderNamespace = "XtensaHIFIX";
      let Predicates = [HasHIFI4];
    }
  } // End mayStore

  // Post-increment Load Instructions
  let mayLoad = 1 in {
    // AE_L32_IP (Post-increment load)
    // Outputs: AEDR (value loaded), AR (updated pointer ptr + imm8)
    // Opcode is identical on HiFi3 and HiFi4; HiFi4 def fires when available,
    // HiFi3 def fires on HiFi3-only targets (e.g. intel_tgl_adsp / CAVS 2.5).
    def AE_L32_IP_HIFI4 : RRR_Inst<0x04, 0x0E, 0x0B, (outs AEDR:$t, AR:$s_out), (ins AR:$s, imm8:$r),
                                    "ae_l32.ip\t$t, $s, $r",
                                    [(set AEDR:$t, AR:$s_out, (int_xtensa_ae_l32_ip AR:$s, imm8:$r))]> {
      let Constraints = "$s = $s_out";
      let DecoderNamespace = "XtensaHIFIX";
      let Predicates = [HasHIFI4];
    }
    def AE_L32_IP_HIFI3 : RRR_Inst<0x04, 0x0E, 0x0B, (outs AEDR:$t, AR:$s_out), (ins AR:$s, imm8:$r),
                                    "ae_l32.ip\t$t, $s, $r",
                                    [(set AEDR:$t, AR:$s_out, (int_xtensa_ae_l32_ip AR:$s, imm8:$r))]> {
      let Constraints = "$s = $s_out";
      let DecoderNamespace = "XtensaHIFI3";
      let Predicates = [HasHIFI3, HasNoHIFI4];
      let mayLoad = 1;
    }
  }

  let mayStore = 1 in {
    let Predicates = [HasHIFI5], DecoderNamespace = "XtensaHIFI5" in {
      def AE_S32_L_I_HIFI5 : RRR_Inst<0x04, 0x0A, 0x04, (outs), (ins AEDR:$r, AR:$s, imm8:$t),
                                      "ae_s32.l.i\t$r, $s, $t", [(int_xtensa_ae_s32_l_i AEDR:$r, AR:$s, imm8:$t)]>;
      def AE_S16_I_HIFI5 : RRR_Inst<0x04, 0x00, 0x09, (outs), (ins AEDR:$r, AR:$s, imm8:$t),
                                    "ae_s16.0.i\t$r, $s, $t", [(int_xtensa_ae_s16_i AEDR:$r, AR:$s, imm8:$t)]>;

      def AE_S32_X_HIFI5 : RRR_Inst<0x04, 0x00, 0x0B, (outs), (ins AEDR:$r, AR:$s, AR:$t),
                                    "ae_s32.l.x\t$r, $s, $t", [(int_xtensa_ae_s32_l_x AEDR:$r, AR:$s, AR:$t)]>;
    }

    // AE_S32_H_I: Store high 32 bits of AE_DR to memory with immediate offset
    def AE_S32_H_I_PSEUDO : Pseudo<(outs), (ins AEDR:$r, AR:$s, imm8:$t),
                                   "ae_s32.h.i\t$r, $s, $t",
                                   [(int_xtensa_ae_s32_h_i AEDR:$r, AR:$s, imm8:$t)]> {
      let Predicates = [HasHIFI3];
    }
  }

  // --- Alignment and State Loads ---
  // AE_LA64_PP primes the u-register from the load pointer's alignment bits.
  // T-nibble encoding: Inst[7:6]=t_base=0b11, Inst[5:4]=u-reg#.
  // (Original fixed opcode for u0: t_val=0x0C=1100, i.e. t_base=11, u=00)
  let hasSideEffects = 1 in {
    def AE_LA64_PP_HIFI4 : RRR_TFC_U_Inst<0x04, 0x07, 0x07, 0x02, 0b11,
                                          (outs AR:$s_out, VALIGN:$align_out), (ins AR:$s),
                                          "ae_la64.pp\tu0, $s",
                                          [(set AR:$s_out, VALIGN:$align_out,
                                            (int_xtensa_ae_la64_pp AR:$s))]> {
      let Constraints = "$s = $s_out";
      let DecoderNamespace = "XtensaHIFIX";
      let Predicates = [HasHIFI4];
      let mayLoad = 1;
    }
    def AE_LA64_PP_HIFI3 : RRR_TFC_U_Inst<0x04, 0x07, 0x07, 0x02, 0b11,
                                          (outs AR:$s_out, VALIGN:$align_out), (ins AR:$s),
                                          "ae_la64.pp\tu0, $s",
                                          [(set AR:$s_out, VALIGN:$align_out,
                                            (int_xtensa_ae_la64_pp AR:$s))]> {
      let Constraints = "$s = $s_out";
      let DecoderNamespace = "XtensaHIFI3";
      let Predicates = [HasHIFI3, HasNoHIFI4];
      let mayLoad = 1;
    }

    // ae_la32x2.ip: reads VALIGN:$align_in, produces data + VALIGN:$align_out
    // T-nibble: Inst[7:6]=t_base=0b11, Inst[5:4]=u-reg#
    def AE_LA32X2_IP_HIFI4 : RRR_TC_U_Inst<0x04, 0x0D, 0x00, 0b11,
                                           (outs AEDR:$r, VALIGN:$align_out, AR:$s_out),
                                           (ins VALIGN:$align_in, AR:$s),
                                           "ae_la32x2.ip\t$r, u0, $s",
                                           [(set AEDR:$r, VALIGN:$align_out, AR:$s_out,
                                             (int_xtensa_ae_la32x2_ip VALIGN:$align_in, AR:$s))]> {
      let Constraints = "$s = $s_out, $align_in = $align_out";
      let DecoderNamespace = "XtensaHIFIX";
      let Predicates = [HasHIFI4];
      let mayLoad = 1;
    }
    def AE_LA32X2_IP_HIFI3 : RRR_TC_U_Inst<0x04, 0x0D, 0x00, 0b11,
                                           (outs AEDR:$r, VALIGN:$align_out, AR:$s_out),
                                           (ins VALIGN:$align_in, AR:$s),
                                           "ae_la32x2.ip\t$r, u0, $s",
                                           [(set AEDR:$r, VALIGN:$align_out, AR:$s_out,
                                             (int_xtensa_ae_la32x2_ip VALIGN:$align_in, AR:$s))]> {
      let Constraints = "$s = $s_out, $align_in = $align_out";
      let DecoderNamespace = "XtensaHIFI3";
      let Predicates = [HasHIFI3, HasNoHIFI4];
      let mayLoad = 1;
    }

    def AE_LA16X4_IP_HIFI4 : RRR_TC_U_Inst<0x04, 0x07, 0x03, 0b11,
                                           (outs AEDR:$r, VALIGN:$align_out, AR:$s_out),
                                           (ins VALIGN:$align_in, AR:$s),
                                           "ae_la16x4.ip\t$r, u0, $s",
                                           [(set AEDR:$r, VALIGN:$align_out, AR:$s_out,
                                             (int_xtensa_ae_la16x4_ip VALIGN:$align_in, AR:$s))]> {
      let Constraints = "$s = $s_out, $align_in = $align_out";
      let DecoderNamespace = "XtensaHIFIX";
      let Predicates = [HasHIFI4];
      let mayLoad = 1;
    }
    def AE_LA16X4_IP_HIFI3 : RRR_TC_U_Inst<0x04, 0x07, 0x03, 0b11,
                                           (outs AEDR:$r, VALIGN:$align_out, AR:$s_out),
                                           (ins VALIGN:$align_in, AR:$s),
                                           "ae_la16x4.ip\t$r, u0, $s",
                                           [(set AEDR:$r, VALIGN:$align_out, AR:$s_out,
                                             (int_xtensa_ae_la16x4_ip VALIGN:$align_in, AR:$s))]> {
      let Constraints = "$s = $s_out, $align_in = $align_out";
      let DecoderNamespace = "XtensaHIFI3";
      let Predicates = [HasHIFI3, HasNoHIFI4];
      let mayLoad = 1;
    }
  }

  // --- AUTO GENERATED PHASE 6 INTRINSICS ---
    // Note: AE memory instructions where offset/register is 't' use RRR_AE_Inst
    // where t is mapped to 23-20, and op_7_4 is fixed.
    // Wait, for AE_L16X4_IP, the immediate offset is at Bits 7-4.
    // So AE_L16X4_IP maps imm to 7-4, r to 15-12, s to 11-8, op1 to 19-16, op2 to 23-20.
    // We already have RRR_Inst which maps:
    // r => 15-12, s => 11-8, t => 7-4, op1 => 19-16, op2 => 23-20.
    // So RRR_Inst IS CORRECT FOR IP INSTRUCTIONS IF t IS THE IMMEDIATE!
    // BUT we must name the parameters carefully: $t for the immediate, $r for dest, $s for ptr!
    
    // AE_L16X4_IP_HIFI4: op0=4, op1=0, op2=9, t=imm(8..15), r=dest, s=ptr
    def AE_L16X4_IP_HIFI4 : RRR_Inst<0x04, 0x00, 0x09, (outs AEDR:$r, AR:$s_out), (ins AR:$s, imm8_x4_add8:$t),
                                    "ae_l16x4.ip\t$r, $s, $t",
                                    [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l16x4_ip AR:$s, imm8_x4_add8:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI4"; let Predicates = [HasHIFI4]; let mayLoad = 1;
    }
    // HiFi3 variant — same encoding, different predicate
    def AE_L16X4_IP_HIFI3 : RRR_Inst<0x04, 0x00, 0x09, (outs AEDR:$r, AR:$s_out), (ins AR:$s, imm8_x4_add8:$t),
                                    "ae_l16x4.ip\t$r, $s, $t",
                                    [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l16x4_ip AR:$s, imm8_x4_add8:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3, HasNoHIFI4]; let mayLoad = 1;
    }
    
    // AE_L16X4_XP_HIFI4: post-increment load 4x int16 with register offset
    def AE_L16X4_XP_HIFI4 : RRR_AE_Inst<0x0D, 0x04, 0x0C, (outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                    "ae_l16x4.xp\t$r, $s, $t",
                                    [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l16x4_xp AR:$s, AR:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI4"; let Predicates = [HasHIFI4]; let mayLoad = 1;
    }
    // HiFi3 variant
    def AE_L16X4_XP_HIFI3 : RRR_AE_Inst<0x0D, 0x04, 0x0C, (outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                    "ae_l16x4.xp\t$r, $s, $t",
                                    [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l16x4_xp AR:$s, AR:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3, HasNoHIFI4]; let mayLoad = 1;
    }
    
    // For AE_L32_XC: circular load (register offset). Same opcode on HiFi3 and HiFi4.
    def AE_L32_XC_HIFI4 : RRR_AE_Inst<0x0B, 0x0C, 0x04, (outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                    "ae_l32.xc\t$r, $s, $t",
                                    [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32_xc AR:$s, AR:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI4"; let Predicates = [HasHIFI4]; let mayLoad = 1;
    }
    // HiFi3 variant
    def AE_L32_XC_HIFI3 : RRR_AE_Inst<0x0B, 0x0C, 0x04, (outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                    "ae_l32.xc\t$r, $s, $t",
                                    [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32_xc AR:$s, AR:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3, HasNoHIFI4]; let mayLoad = 1;
    }

    // AE_S16X4_IP_HIFI4: op0=4, op1=0, op2=8
    def AE_S16X4_IP_HIFI4 : RRR_Inst<0x04, 0x00, 0x08, (outs AR:$s_out), (ins AEDR:$r, AR:$s, imm8_x4_add8:$t),
                                        "ae_s16x4.ip\t$r, $s, $t",
                                        [(set AR:$s_out, (int_xtensa_ae_s16x4_ip AEDR:$r, AR:$s, imm8_x4_add8:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI4"; let Predicates = [HasHIFI4]; let mayStore = 1;
    }
    // HiFi3 variant — same encoding, different predicate
    def AE_S16X4_IP_HIFI3 : RRR_Inst<0x04, 0x00, 0x08, (outs AR:$s_out), (ins AEDR:$r, AR:$s, imm8_x4_add8:$t),
                                        "ae_s16x4.ip\t$r, $s, $t",
                                        [(set AR:$s_out, (int_xtensa_ae_s16x4_ip AEDR:$r, AR:$s, imm8_x4_add8:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3, HasNoHIFI4]; let mayStore = 1;
    }
    // AE_S16_0_IP: store low 16b with post-increment. Same opcode on HiFi3 and HiFi4.
    def AE_S16_0_IP_HIFI4 : RRR_Inst<0x04, 0x03, 0x0D, (outs AR:$s_out), (ins AEDR:$r, AR:$s, imm8n_7_x2:$t),
                                        "ae_s16.0.ip\t$r, $s, $t",
                                        [(set AR:$s_out, (int_xtensa_ae_s16_0_ip AEDR:$r, AR:$s, imm8n_7_x2:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI4"; let Predicates = [HasHIFI4]; let mayStore = 1;
    }
    // HiFi3 variant
    def AE_S16_0_IP_HIFI3 : RRR_Inst<0x04, 0x03, 0x0D, (outs AR:$s_out), (ins AEDR:$r, AR:$s, imm8n_7_x2:$t),
                                        "ae_s16.0.ip\t$r, $s, $t",
                                        [(set AR:$s_out, (int_xtensa_ae_s16_0_ip AEDR:$r, AR:$s, imm8n_7_x2:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3, HasNoHIFI4]; let mayStore = 1;
    }

    // AE_S32_L_IP_HIFI4: op0=4, op1=1, op2=E
    def AE_S32_L_IP_HIFI4 : RRR_Inst<0x04, 0x01, 0x0E, (outs AR:$s_out), (ins AEDR:$r, AR:$s, imm8n_7_x4:$t),
                                        "ae_s32.l.ip\t$r, $s, $t",
                                        [(set AR:$s_out, (int_xtensa_ae_s32_l_ip AEDR:$r, AR:$s, imm8n_7_x4:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI4"; let Predicates = [HasHIFI4]; let mayStore = 1;
    }
    // HiFi3 variant
    def AE_S32_L_IP_HIFI3 : RRR_Inst<0x04, 0x01, 0x0E, (outs AR:$s_out), (ins AEDR:$r, AR:$s, imm8n_7_x4:$t),
                                        "ae_s32.l.ip\t$r, $s, $t",
                                        [(set AR:$s_out, (int_xtensa_ae_s32_l_ip AEDR:$r, AR:$s, imm8n_7_x4:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3, HasNoHIFI4]; let mayStore = 1;
    }
    // Fallback pseudo for AE_S32_L_IP when the immediate is outside [-32,28]
    // (step 4). The pattern uses unconstrained `imm` so it can catch any
    // constant not already matched by AE_S32_L_IP_HIFI4's imm8n_7_x4 predicate.
    // XtensaISelLowering::PerformDAGCombine rewrites INTRINSIC_W_CHAIN nodes
    // for this intrinsic with out-of-range immediates to ae_s32_l_xp before
    // ISel, so in practice only values in [-32,28]/4 reach ISel and are
    // caught by AE_S32_L_IP_HIFI4 first.  This pseudo exists as a safety net
    // to prevent a hard "Cannot select" crash if a constant slips through.
    def AE_S32_L_IP_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, i32imm:$t),
                                        "ae_s32.l.ip\t$r, $s, $t",
                                        [(set AR:$s_out, (int_xtensa_ae_s32_l_ip AEDR:$r, AR:$s, (i32 imm:$t)))]> {
      let Constraints = "$s = $s_out"; let Predicates = [HasHIFI3]; let mayStore = 1;
    }
    
    // AE_L16_IP: post-increment load int16. Same opcode on HiFi3 and HiFi4.
    def AE_L16_IP_HIFI4 : RRR_Inst<0x04, 0x04, 0x0B, (outs AEDR:$r, AR:$s_out), (ins AR:$s, imm8n_7_x2:$t),
                                    "ae_l16.ip\t$r, $s, $t",
                                    [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l16_ip AR:$s, imm8n_7_x2:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI4"; let Predicates = [HasHIFI4]; let mayLoad = 1;
    }
    // HiFi3 variant
    def AE_L16_IP_HIFI3 : RRR_Inst<0x04, 0x04, 0x0B, (outs AEDR:$r, AR:$s_out), (ins AR:$s, imm8n_7_x2:$t),
                                    "ae_l16.ip\t$r, $s, $t",
                                    [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l16_ip AR:$s, imm8n_7_x2:$t))]> {
      let Constraints = "$s = $s_out"; let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3, HasNoHIFI4]; let mayLoad = 1;
    }
    
    // Batch 3 Real 16x4 Loads/Stores
    let Predicates = [HasHIFI3], DecoderNamespace = "XtensaHIFI3" in {
      // Loads
      def AE_L16X4_I : RRR_Inst<0x04, 0x0C, 0x0C, (outs AEDR:$r), (ins AR:$s, imm8n_7_x8:$t),
                               "ae_l16x4.i\t$r, $s, $t",
                               [(set AEDR:$r, (int_xtensa_ae_l16x4_i AR:$s, imm8n_7_x8:$t))]> {
        let mayLoad = 1;
      }

      def AE_L16X4_X : RRR_Inst<0x04, 0x06, 0x0A, (outs AEDR:$r), (ins AR:$s, AR:$t),
                               "ae_l16x4.x\t$r, $s, $t",
                               [(set AEDR:$r, (int_xtensa_ae_l16x4_x AR:$s, AR:$t))]> {
        let mayLoad = 1;
      }

      def AE_L16X4_XC : RRR_AE_Inst<0x04, 0x06, 0x09, (outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                   "ae_l16x4.xc\t$r, $s, $t",
                                   [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l16x4_xc AR:$s, AR:$t))]> {
        let Constraints = "$s = $s_out";
        let mayLoad = 1;
      }

      // Stores
      def AE_S16X4_I : RRR_Inst<0x04, 0x0C, 0x0D, (outs), (ins AEDR:$r, AR:$s, imm8n_7_x8:$t),
                               "ae_s16x4.i\t$r, $s, $t",
                               [(int_xtensa_ae_s16x4_i AEDR:$r, AR:$s, imm8n_7_x8:$t)]> {
        let mayStore = 1;
      }

      def AE_S16X4_X : RRR_Inst<0x04, 0x0C, 0x0E, (outs), (ins AEDR:$r, AR:$s, AR:$t),
                               "ae_s16x4.x\t$r, $s, $t",
                               [(int_xtensa_ae_s16x4_x AEDR:$r, AR:$s, AR:$t)]> {
        let mayStore = 1;
      }

      def AE_S16X4_XC : RRR_AE_Inst<0x04, 0x0C, 0x09, (outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                   "ae_s16x4.xc\t$r, $s, $t",
                                   [(set AR:$s_out, (int_xtensa_ae_s16x4_xc AEDR:$r, AR:$s, AR:$t))]> {
        let Constraints = "$s = $s_out";
        let mayStore = 1;
      }

      def AE_S16X4_XP : RRR_AE_Inst<0x04, 0x0C, 0x0A, (outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                   "ae_s16x4.xp\t$r, $s, $t",
                                   [(set AR:$s_out, (int_xtensa_ae_s16x4_xp AEDR:$r, AR:$s, AR:$t))]> {
        let Constraints = "$s = $s_out";
        let mayStore = 1;
      }
    }

    // --- HiFi5 Equivalents (Cleaned up temporary placeholders) ---
    // To complete full integration without incorrect opcodes crashing LLVM, we define the missing intrinsics to use builtins but we map them correctly later if needed.
    // For now we remove broken HiFi5 definitions because they were equally broken.
class RRR_AE_SHIFT_Inst<bits<3> op0, bits<4> op1, bits<4> op_7_4, string asmstr,
                        dag outs, dag ins, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> r;
  bits<4> t;
  bits<5> imm;

  let Inst{23-20} = t;
  let Inst{19-16} = op1;
  let Inst{15-12} = r;
  let Inst{11-8}  = imm{3-0};
  let Inst{7-4}   = op_7_4;
  let Inst{3-1}   = op0; 
  let Inst{0}     = imm{4}; 
}

class RRR_AE_SHIFT64_Inst<bits<2> op0, bits<4> op1, bits<4> op_7_4, string asmstr,
                          dag outs, dag ins, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> r;
  bits<4> t;
  bits<6> imm;

  let Inst{23-20} = t;
  let Inst{19-16} = op1;
  let Inst{15-12} = r;
  let Inst{11-8}  = imm{3-0};
  let Inst{7-4}   = op_7_4;
  let Inst{3-2}   = op0; 
  let Inst{1}     = imm{5}; 
  let Inst{0}     = imm{4}; 
}

// MAC and Arithmetic Operations
//===----------------------------------------------------------------------===//
let mayLoad = 0, mayStore = 0 in {
  def AE_MUL32_LL_HIFI3 : RRR_AE_Inst<0x00, 0x00, 0x0A, (outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                      "ae_mul32.ll\t$t, $s, $r",
                                      [(set AEDR:$t, (int_xtensa_ae_mul32_ll AEDR:$s, AEDR:$r))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

class FLIX_MAC_Inst_Base<bits<8> byte0, bits<8> byte1, bits<8> byte2, dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst64<outs, ins, asmstr, pattern> {
  bits<4> t;
  bits<4> s;
  bits<4> r;

  let Inst{63-56} = 0x3e;
  let Inst{55-48} = 0xb1;
  let Inst{47-45} = s{2-0};
  let Inst{44}    = 0;
  let Inst{43-40} = r;
  let Inst{39-33} = 0x1a;
  let Inst{32}    = s{3};
  let Inst{31}    = 0;
  let Inst{30-27} = t;
  let Inst{26-24} = 3;
  let Inst{23-16} = byte2;
  let Inst{15-8}  = byte1;
  let Inst{7-0}   = byte0;
}

class FLIX_MAC_Inst<bits<8> byte0, bits<8> byte2, dag outs, dag ins, string asmstr, list<dag> pattern>
  : FLIX_MAC_Inst_Base<byte0, 0x83, byte2, outs, ins, asmstr, pattern>;

  def AE_MULAFD32X16X2_FIR_HH_HIFI3 : Pseudo<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c),
                                      "ae_mulafd32x16x2.fir.hh\t$q0, $q1, $d0, $d1, $c",
                                      [(set AEDR:$t1, AEDR:$t2, (int_xtensa_ae_mulafd32x16x2_fir_hh AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c))]> {
    let Constraints = "$q0 = $t1, $q1 = $t2";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULAFD32X16X2_FIR_HH : XtensaInst64<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c),
                                             "ae_mulafd32x16x2.fir.hh\t$q0, $q1, $d0, $d1, $c", []> {
    let isCodeGenOnly = 1;
    bits<4> t1;
    bits<4> t2;
    bits<4> q0;
    bits<4> q1;
    bits<4> d0;
    bits<4> d1;
    bits<4> c;

    let Inst{3-0}   = t1;
    let Inst{7-4}   = t2;
    let Inst{11-8}  = q0;
    let Inst{15-12} = q1;
    let Inst{19-16} = d0;
    let Inst{23-20} = d1;
    let Inst{27-24} = c;
    let Inst{63-28} = 0x123456789;
  }
  
  def AE_MULAFD32X16X2_FIR_HL_HIFI3 : Pseudo<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c),
                                      "ae_mulafd32x16x2.fir.hl\t$q0, $q1, $d0, $d1, $c",
                                      [(set AEDR:$t1, AEDR:$t2, (int_xtensa_ae_mulafd32x16x2_fir_hl AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c))]> {
    let Constraints = "$q0 = $t1, $q1 = $t2";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  
  def AE_MULAFD32X16X2_FIR_LH_HIFI3 : Pseudo<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c),
                                      "ae_mulafd32x16x2.fir.lh\t$q0, $q1, $d0, $d1, $c",
                                      [(set AEDR:$t1, AEDR:$t2, (int_xtensa_ae_mulafd32x16x2_fir_lh AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c))]> {
    let Constraints = "$q0 = $t1, $q1 = $t2";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  
  // HiFi 5 Pseudos for Inline Assembly Support
  def AE_MULF32X2R_HH_LL_HIFI5 : Pseudo<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$d0, AEDR:$d1),
                                      "ae_mulf32x2r.hh.ll\t$t1, $t2, $d0, $d1",
                                      []> {
    let Predicates = [HasHIFI5];
    let isCodeGenOnly = 0;
  }
  
  def AE_MULF2P32X16X4RS_HIFI5 : Pseudo<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$d0, AEDR:$d1, AEDR:$d2),
                                      "ae_mulf2p32x16x4rs\t$t1, $t2, $d0, $d1, $d2",
                                      []> {
    let Predicates = [HasHIFI5];
    let isCodeGenOnly = 0;
  }
  
  def AE_MULF2P32X4RS_HIFI5 : Pseudo<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$d0, AEDR:$d1, AEDR:$d2, AEDR:$d3),
                                      "ae_mulf2p32x4rs\t$t1, $t2, $d0, $d1, $d2, $d3",
                                      []> {
    let Predicates = [HasHIFI5];
    let isCodeGenOnly = 0;
  }
  
  def AE_L32X2X2_XC_HIFI5 : Pseudo<(outs AR:$ptr_out, AEDR:$t1, AEDR:$t2), (ins AR:$ptr_in, imm8:$offset),
                                      "ae_l32x2x2.xc\t$t1, $t2, $ptr_in, $offset",
                                      []> {
    let Predicates = [HasHIFI5];
    let Constraints = "$ptr_out = $ptr_in";
    let isCodeGenOnly = 0;
  }

  def AE_S32X2X2_XC1_HIFI5 : Pseudo<(outs AR:$ptr_out), (ins AEDR:$t1, AEDR:$t2, AR:$ptr_in, imm8:$offset),
                                      "ae_s32x2x2.xc1\t$t1, $t2, $ptr_in, $offset",
                                      []> {
    let Predicates = [HasHIFI5];
    let Constraints = "$ptr_out = $ptr_in";
    let isCodeGenOnly = 0;
  }

  // AE_L32X2X2_XC1: circular load (buffer 1) with register offset
  // Syntax: ae_l32x2x2.xc1 aed0, aed1, a_ptr, a_offset
  def AE_L32X2X2_XC1_HIFI5 : Pseudo<(outs AR:$ptr_out, AEDR:$t1, AEDR:$t2), (ins AR:$ptr_in, AR:$offset),
                                      "ae_l32x2x2.xc1\t$t1, $t2, $ptr_in, $offset",
                                      []> {
    let Predicates = [HasHIFI5];
    let Constraints = "$ptr_out = $ptr_in";
    let isCodeGenOnly = 0;
    let mayLoad = 1;
  }

  // AE_LA16X4X2_IP: aligned load 2x ae_int16x4 with post-increment
  // Syntax: ae_la16x4x2_ip aed0, aed1, u0, a_ptr
  def AE_LA16X4X2_IP_HIFI5 : Pseudo<(outs AEDR:$t1, AEDR:$t2, AR:$ptr_out), (ins AR:$ptr_in),
                                      "ae_la16x4x2_ip\t$t1, $t2, u0, $ptr_in",
                                      []> {
    let Predicates = [HasHIFI5];
    let Constraints = "$ptr_out = $ptr_in";
    let isCodeGenOnly = 0;
    let mayLoad = 1;
    // Uses VALIGN via explicit DAG operand
  }

  // AE_LA32X2X2_IP: aligned load 2x ae_int32x2 with post-increment
  // Syntax: ae_la32x2x2_ip aed0, aed1, u0, a_ptr
  def AE_LA32X2X2_IP_HIFI5 : Pseudo<(outs AEDR:$t1, AEDR:$t2, AR:$ptr_out), (ins AR:$ptr_in),
                                      "ae_la32x2x2_ip\t$t1, $t2, u0, $ptr_in",
                                      []> {
    let Predicates = [HasHIFI5];
    let Constraints = "$ptr_out = $ptr_in";
    let isCodeGenOnly = 0;
    let mayLoad = 1;
    // Uses VALIGN via explicit DAG operand
  }

  // AE_SA16X4X2_IP: aligned store 2x ae_int16x4 with post-increment
  // Syntax: ae_sa16x4x2_ip aed0, aed1, u0, a_ptr
  def AE_SA16X4X2_IP_HIFI5 : Pseudo<(outs AR:$ptr_out), (ins AEDR:$t1, AEDR:$t2, AR:$ptr_in),
                                      "ae_sa16x4x2_ip\t$t1, $t2, u1, $ptr_in",
                                      []> {
    let Predicates = [HasHIFI5];
    let Constraints = "$ptr_out = $ptr_in";
    let isCodeGenOnly = 0;
    let mayStore = 1;
    // Defs VALIGN via explicit DAG operand
  }

  // AE_SA32X2X2_IP: aligned store 2x ae_int32x2 with post-increment
  // Syntax: ae_sa32x2x2_ip aed0, aed1, u0, a_ptr
  def AE_SA32X2X2_IP_HIFI5 : Pseudo<(outs AR:$ptr_out), (ins AEDR:$t1, AEDR:$t2, AR:$ptr_in),
                                      "ae_sa32x2x2_ip\t$t1, $t2, u1, $ptr_in",
                                      []> {
    let Predicates = [HasHIFI5];
    let Constraints = "$ptr_out = $ptr_in";
    let isCodeGenOnly = 0;
    let mayStore = 1;
    // Defs VALIGN via explicit DAG operand
  }


  
  def AE_MULAFD32X16X2_FIR_LL_HIFI3 : Pseudo<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c),
                                      "ae_mulafd32x16x2.fir.ll\t$q0, $q1, $d0, $d1, $c",
                                      [(set AEDR:$t1, AEDR:$t2, (int_xtensa_ae_mulafd32x16x2_fir_ll AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c))]> {
    let Constraints = "$q0 = $t1, $q1 = $t2";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  
  def AE_MUL16X4_HIFI3 : Pseudo<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$q0, AEDR:$q1, AEDR:$r, AEDR:$s),
                                "ae_mul16x4\t$q0, $q1, $r, $s",
                                [(set AEDR:$t1, AEDR:$t2, (int_xtensa_ae_mul16x4 AEDR:$q0, AEDR:$q1, AEDR:$r, AEDR:$s))]> {
    let Constraints = "$q0 = $t1, $q1 = $t2";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

class AE_Unary_Inst<bits<4> op0, bits<4> op1, bits<4> op2, bits<4> s_val,
                     dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> r;
  bits<4> t;
  let Inst{23-20} = op2;
  let Inst{19-16} = op1;
  let Inst{15-12} = r;
  let Inst{11-8}  = s_val;
  let Inst{7-4}   = t;
  let Inst{3-0}   = op0;
}

class AE_Unary1_Inst<bits<4> op0, bits<4> op1, bits<4> op2, bits<4> t_val,
                     dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> r;
  bits<4> s;
  let Inst{23-20} = op2;
  let Inst{19-16} = op1;
  let Inst{15-12} = r;
  let Inst{11-8}  = s;
  let Inst{7-4}   = t_val;
  let Inst{3-0}   = op0;
}

class RRR_Slot1_Inst<bits<8> op_19_12, dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> r;
  bits<4> s;
  bits<4> t;

  let Inst{23-20} = 0;
  let Inst{19-12} = op_19_12;
  let Inst{11-8}  = t;
  let Inst{7-4}   = s;
  let Inst{3-0}   = r;
}

class RRR_Slot3_Inst<bits<4> op0, bits<4> op1, bits<4> op2, dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> r;
  bits<4> s;
  bits<4> t;

  let Inst{23-20} = op2;
  let Inst{19-16} = op1;
  let Inst{15-12} = op0;
  let Inst{11-8}  = t;
  let Inst{7-4}   = s;
  let Inst{3-0}   = r;
}

class AE_Sext_Inst<bits<4> op0, bits<4> op1, bits<4> op2, bits<4> t_val, dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> r;
  bits<4> s;

  let Inst{23-20} = op2;
  let Inst{19-16} = op1;
  let Inst{15-12} = s;
  let Inst{11-8}  = r;
  let Inst{7-4}   = t_val;
  let Inst{3-0}   = op0;
}

class AE_Mul_Inst<bits<4> op1, bits<4> op2, bits<4> op_7_4, dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> r;
  bits<4> s;
  bits<4> t;

  let Inst{23-20} = op2;
  let Inst{19-16} = op1;
  let Inst{15-12} = s;
  let Inst{11-8}  = t;
  let Inst{7-4}   = op_7_4;
  let Inst{3-0}   = r;
}

class AE_Store_Inst<bits<4> op0, bits<4> op1, bits<4> op2, dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> r;
  bits<4> s;
  bits<4> t;

  let Inst{23-20} = op2;
  let Inst{19-16} = op1;
  let Inst{15-12} = op0;
  let Inst{11-8}  = r;
  let Inst{7-4}   = t;
  let Inst{3-0}   = s;
}

class AE_Sel_Inst<bits<4> op0, bits<4> op1, bits<4> op_7_4, dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> r;
  bits<4> s;
  bits<4> t;
  bits<4> sel;

  let Inst{23-20} = t;
  let Inst{19-16} = op1;
  let Inst{15-12} = r;
  let Inst{11-8}  = s;
  let Inst{7-4}   = op_7_4;
  let Inst{3-0}   = sel;
}

  def AE_ABS24S_HIFI3 : AE_Unary_Inst<0x04, 0x0B, 0x02, 0x01, (outs AEDR:$r), (ins AEDR:$t),
                                "ae_abs24s\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_abs24s AEDR:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_ABS32_HIFI3 : AE_Unary1_Inst<0x00, 0x0F, 0x00, 0x02, (outs AEDR:$r), (ins AEDR:$s),
                                "ae_abs32\t$r, $s",
                                [(set AEDR:$r, (int_xtensa_ae_abs32 AEDR:$s))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_ABS64_HIFI3 : AE_Unary_Inst<0x04, 0x07, 0x02, 0x08, (outs AEDR:$r), (ins AEDR:$t),
                                "ae_abs64\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_abs64 AEDR:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_ABS64S_HIFI3 : AE_Unary1_Inst<0x00, 0x0F, 0x00, 0x05, (outs AEDR:$r), (ins AEDR:$s),
                                "ae_abs64s\t$r, $s",
                                [(set AEDR:$r, (int_xtensa_ae_abs64s AEDR:$s))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_ABSSQ56S_HIFI3 : AE_Unary1_Inst<0x00, 0x0F, 0x00, 0x06, (outs AEDR:$r), (ins AEDR:$s),
                                "ae_abssq56s\t$r, $s",
                                [(set AEDR:$r, (int_xtensa_ae_abssq56s AEDR:$s))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_ADDSQ56S_HIFI3 : RRR_Slot1_Inst<0xEC, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                   "ae_addsq56s\t$r, $s, $t",
                                   [(set AEDR:$r, (int_xtensa_ae_addsq56s AEDR:$s, AEDR:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_ADDSUB32_HIFI3 : RRR_Slot1_Inst<0xED, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                   "ae_addsub32\t$r, $s, $t",
                                   [(set AEDR:$r, (int_xtensa_ae_addsub32 AEDR:$s, AEDR:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_ADDSUB32S_HIFI3 : RRR_Slot1_Inst<0xEE, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                   "ae_addsub32s\t$r, $s, $t",
                                   [(set AEDR:$r, (int_xtensa_ae_addsub32s AEDR:$s, AEDR:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_L16_X_HIFI3 : RRR_Inst<0x04, 0x06, 0x0B, (outs AEDR:$r), (ins AR:$s, AR:$t),
                                "ae_l16.x\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_l16_x AR:$s, AR:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
    let mayLoad = 1;
  }

  def AE_L16M_I_HIFI3 : RRR_Inst<0x04, 0x01, 0x0B, (outs AEDR:$r), (ins AR:$s, imm8n_7_x2:$t),
                               "ae_l16m.i\t$r, $s, $t",
                               [(set AEDR:$r, (int_xtensa_ae_l16m_i AR:$s, imm8n_7_x2:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
    let mayLoad = 1;
  }

  def AE_MAX32_HIFI3 : RRR_Inst<0x04, 0x02, 0x05, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_max32\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_max32 AEDR:$s, AEDR:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  
  def AE_MIN32_HIFI3 : RRR_Inst<0x04, 0x03, 0x05, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_min32\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_min32 AEDR:$s, AEDR:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_ABS16S_HIFI3 : AE_Unary_Inst<0x04, 0x0B, 0x02, 0x02, (outs AEDR:$r), (ins AEDR:$t),
                                "ae_abs16s\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_abs16s AEDR:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  
  def AE_ABS32S_HIFI3 : AE_Unary_Inst<0x04, 0x0B, 0x02, 0x03, (outs AEDR:$r), (ins AEDR:$t),
                                "ae_abs32s\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_abs32s AEDR:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MAXABS32S_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_maxabs32s\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_maxabs32s AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_MAXABS32S : RRR_Slot3_Inst<0x0d, 0x08, 0x01, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_maxabs32s\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  
  def AE_NEG16S_HIFI3 : AE_Unary_Inst<0x04, 0x0B, 0x02, 0x06, (outs AEDR:$r), (ins AEDR:$t),
                                "ae_neg16s\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_neg16s AEDR:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_NEG32S_HIFI3 : AE_Unary_Inst<0x04, 0x0B, 0x02, 0x07, (outs AEDR:$r), (ins AEDR:$t),
                                "ae_neg32s\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_neg32s AEDR:$t))]> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULAAFD32X16_H3_L2_HIFI3 : FLIX_MAC_Inst<0x1c, 0x5f, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                                "ae_mulaafd32x16.h3.l2\t$t, $s, $r",
                                                [(set AEDR:$t, (int_xtensa_ae_mulaafd32x16_h3_l2 AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  
  def AE_MULAAFD32X16_H1_L0_HIFI3 : FLIX_MAC_Inst<0x1c, 0x5d, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                                "ae_mulaafd32x16.h1.l0\t$t, $s, $r",
                                                [(set AEDR:$t, (int_xtensa_ae_mulaafd32x16_h1_l0 AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULAF32R_HH_HIFI3 : FLIX_MAC_Inst<0x1e, 0x46, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                         "ae_mulaf32r.hh\t$t, $s, $r",
                                         [(set AEDR:$t, (int_xtensa_ae_mulaf32r_hh AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }


  
  def AE_SRAI32_HIFI3 : RRR_AE_SHIFT_Inst<0x7, 0x4, 0x9,
                                        "ae_srai32\t$r, $t, $imm",
                                        (outs AEDR:$r), (ins AEDR:$t, uimm5:$imm),
                                        [(set AEDR:$r, (int_xtensa_ae_srai32 AEDR:$t, uimm5:$imm))]> {
    let DecoderNamespace = "XtensaHIFI3";  let Predicates = [HasHIFI3];
  }
  def AE_SLAI32_HIFI3 : RRR_AE_SHIFT_Inst<0x3, 0x4, 0x9,
                                        "ae_slai32\t$r, $t, $imm",
                                        (outs AEDR:$r), (ins AEDR:$t, uimm5:$imm),
                                        [(set AEDR:$r, (int_xtensa_ae_slai32 AEDR:$t, uimm5:$imm))]> {
    let DecoderNamespace = "XtensaHIFI3";  let Predicates = [HasHIFI3];
  }
  def AE_SLAI32S_HIFI3 : RRR_AE_SHIFT_Inst<0x5, 0x4, 0x9,
                                        "ae_slai32s\t$r, $t, $imm",
                                        (outs AEDR:$r), (ins AEDR:$t, uimm5:$imm),
                                        [(set AEDR:$r, (int_xtensa_ae_slai32s AEDR:$t, uimm5:$imm))]> {
    let DecoderNamespace = "XtensaHIFI3";  let Predicates = [HasHIFI3];
  }

  def AE_SRAI64_HIFI3 : RRR_AE_SHIFT64_Inst<0x3, 0x4, 0x8,
                                        "ae_srai64\t$r, $t, $imm",
                                        (outs AEDR:$r), (ins AEDR:$t, uimm6:$imm),
                                        [(set AEDR:$r, (int_xtensa_ae_srai64 AEDR:$t, uimm6:$imm))]> {
    let DecoderNamespace = "XtensaHIFI3";  let Predicates = [HasHIFI3];
  }

  def AE_SLAI64S_HIFI3 : RRR_AE_SHIFT64_Inst<0x4, 0x4, 0x8,
                                        "ae_slai64s\t$r, $t, $imm",
                                        (outs AEDR:$r), (ins AEDR:$t, uimm6:$imm),
                                        [(set AEDR:$r, (int_xtensa_ae_slai64s AEDR:$t, uimm6:$imm))]> {
    let DecoderNamespace = "XtensaHIFI3";  let Predicates = [HasHIFI3];
  }

  def AE_SLAA64S_HIFI3 : RRR_Inst<0x5, 0x5, 0x5, (outs AEDR:$r), (ins AEDR:$t, AR:$s),
                                "ae_slaa64s\t$r, $t, $s",
                                [(set AEDR:$r, (int_xtensa_ae_slaa64s AEDR:$t, AR:$s))]> {
    let DecoderNamespace = "XtensaHIFI3";  let Predicates = [HasHIFI3];
  }

  def AE_SRAI32R_HIFI3 : RRR_AE_SHIFT_Inst<0x5, 0x5, 0x6,
                                        "ae_srai32r\t$r, $t, $imm",
                                        (outs AEDR:$r), (ins AEDR:$t, uimm5:$imm),
                                        [(set AEDR:$r, (int_xtensa_ae_srai32r AEDR:$t, uimm5:$imm))]> {
    let DecoderNamespace = "XtensaHIFI3";  let Predicates = [HasHIFI3];
  }

  def AE_ADD16_HIFI3 : RRR_Inst<0x5, 0x5, 0x7, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_add16\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_add16 AEDR:$s, AEDR:$t))]> { let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3]; }
  def AE_ADD32_HIFI3 : RRR_Inst<0x5, 0x5, 0x8, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_add32\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_add32 AEDR:$s, AEDR:$t))]> { let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3]; }
  def AE_ADD32S_HIFI3 : RRR_Inst<0x5, 0x5, 0x9, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_add32s\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_add32s AEDR:$s, AEDR:$t))]> { let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3]; }
  def AE_ADD64_HIFI3 : RRR_Inst<0x5, 0x5, 0xA, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_add64\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_add64 AEDR:$s, AEDR:$t))]> { let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3]; }

  def AE_SUB16_HIFI3 : RRR_Inst<0x5, 0x5, 0xB, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_sub16\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sub16 AEDR:$s, AEDR:$t))]> { let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3]; }
  def AE_SUB32_HIFI3 : RRR_Inst<0x5, 0x5, 0xC, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_sub32\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sub32 AEDR:$s, AEDR:$t))]> { let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3]; }
  def AE_SUB32S_HIFI3 : RRR_Inst<0x5, 0x5, 0xD, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_sub32s\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sub32s AEDR:$s, AEDR:$t))]> { let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3]; }
  def AE_SUB64_HIFI3 : RRR_Inst<0x5, 0x5, 0xE, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_sub64\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sub64 AEDR:$s, AEDR:$t))]> { let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3]; }

  def AE_ROUND32X2F48SASYM_HIFI3 : RRR_Inst<0x5, 0x6, 0x5, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_round32x2f48sasym\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_round32x2f48sasym AEDR:$s, AEDR:$t))]> { let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3]; }

  def AE_ROUND32X2F48SSYM_HIFI3 : RRR_Inst<0x5, 0x7, 0x5, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_round32x2f48ssym\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_round32x2f48ssym AEDR:$s, AEDR:$t))]> { let DecoderNamespace = "XtensaHIFI3"; let Predicates = [HasHIFI3]; }

  def AE_ZERO16_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins),
                               "ae_sub16s\t$r, $r, $r",
                               [(set AEDR:$r, (int_xtensa_ae_zero16))]> { let Predicates = [HasHIFI3]; }
                               
  def AE_ZERO24_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins),
                               "ae_sub24s\t$r, $r, $r",
                               [(set AEDR:$r, (int_xtensa_ae_zero24))]> { let Predicates = [HasHIFI3]; }
                               
  def AE_ZERO32_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins),
                               "ae_sub32s\t$r, $r, $r",
                               [(set AEDR:$r, (int_xtensa_ae_zero32))]> { let Predicates = [HasHIFI3]; }
                               
  def AE_ZEROP48_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins),
                               "ae_sub64\t$r, $r, $r",
                               [(set AEDR:$r, (int_xtensa_ae_zerop48))]> { let Predicates = [HasHIFI3]; }
                               
  def AE_ZERO64_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins),
                               "ae_sub64\t$r, $r, $r",
                               [(set AEDR:$r, (int_xtensa_ae_zero64))]> { let Predicates = [HasHIFI3]; }
                               
  def AE_ZEROQ56_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins),
                               "ae_sub64\t$r, $r, $r",
                               [(set AEDR:$r, (int_xtensa_ae_zeroq56))]> { let Predicates = [HasHIFI3]; }
                               
  def AE_CBEGIN0_Pseudo : Pseudo<(outs AR:$r), (ins),
                                 "rur.ae_cbegin0\t$r",
                                 [(set AR:$r, (int_xtensa_ae_cbegin0))]> { let Predicates = [HasHIFI3]; }
  def AE_CBEGIN1_Pseudo : Pseudo<(outs AR:$r), (ins),
                                 "rur.ae_cbegin1\t$r",
                                 [(set AR:$r, (int_xtensa_ae_cbegin1))]> { let Predicates = [HasHIFI3]; }
  def AE_CEND0_Pseudo : Pseudo<(outs AR:$r), (ins),
                               "rur.ae_cend0\t$r",
                               [(set AR:$r, (int_xtensa_ae_cend0))]> { let Predicates = [HasHIFI3]; }
  def AE_CEND1_Pseudo : Pseudo<(outs AR:$r), (ins),
                               "rur.ae_cend1\t$r",
                               [(set AR:$r, (int_xtensa_ae_cend1))]> { let Predicates = [HasHIFI3]; }
                               
  def AE_SETCBEGIN0_Pseudo : Pseudo<(outs), (ins AR:$t),
                                    "wur.ae_cbegin0\t$t",
                                    [(int_xtensa_ae_setcbegin0 AR:$t)]> { let Predicates = [HasHIFI3]; }
  def AE_SETCBEGIN1_Pseudo : Pseudo<(outs), (ins AR:$t),
                                    "wur.ae_cbegin1\t$t",
                                    [(int_xtensa_ae_setcbegin1 AR:$t)]> { let Predicates = [HasHIFI3]; }
  def AE_SETCEND0_Pseudo : Pseudo<(outs), (ins AR:$t),
                                  "wur.ae_cend0\t$t",
                                  [(int_xtensa_ae_setcend0 AR:$t)]> { let Predicates = [HasHIFI3]; }
  def AE_SETCEND1_Pseudo : Pseudo<(outs), (ins AR:$t),
                                  "wur.ae_cend1\t$t",
                                  [(int_xtensa_ae_setcend1 AR:$t)]> { let Predicates = [HasHIFI3]; }

  // Boolean comparison pseudo-instructions
  // Each does: ae_XX b0, $s, $t; rsr $r, br
  let Defs = [B0, B1, B2, B3], Predicates = [HasHIFI3] in {
    def AE_EQ16_Pseudo : Pseudo<(outs AR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_eq16\tb0, $s, $t\n\trsr\t$r, br",
                                [(set AR:$r, (int_xtensa_ae_eq16 AEDR:$s, AEDR:$t))]>;
    def AE_LT16_Pseudo : Pseudo<(outs AR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_lt16\tb0, $s, $t\n\trsr\t$r, br",
                                [(set AR:$r, (int_xtensa_ae_lt16 AEDR:$s, AEDR:$t))]>;
    def AE_LE16_Pseudo : Pseudo<(outs AR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_le16\tb0, $s, $t\n\trsr\t$r, br",
                                [(set AR:$r, (int_xtensa_ae_le16 AEDR:$s, AEDR:$t))]>;
  }
  let Defs = [B0, B1], Predicates = [HasHIFI3] in {
    def AE_EQ32_Pseudo : Pseudo<(outs AR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_eq32\tb0, $s, $t\n\trsr\t$r, br",
                                [(set AR:$r, (int_xtensa_ae_eq32 AEDR:$s, AEDR:$t))]>;
    def AE_LT32_Pseudo : Pseudo<(outs AR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_lt32\tb0, $s, $t\n\trsr\t$r, br",
                                [(set AR:$r, (int_xtensa_ae_lt32 AEDR:$s, AEDR:$t))]>;
    def AE_LE32_Pseudo : Pseudo<(outs AR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_le32\tb0, $s, $t\n\trsr\t$r, br",
                                [(set AR:$r, (int_xtensa_ae_le32 AEDR:$s, AEDR:$t))]>;
  }
  let Defs = [B0], Predicates = [HasHIFI3] in {
    def AE_EQ64_Pseudo : Pseudo<(outs AR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_eq64\tb0, $s, $t\n\trsr\t$r, br",
                                [(set AR:$r, (int_xtensa_ae_eq64 AEDR:$s, AEDR:$t))]>;
    def AE_LT64_Pseudo : Pseudo<(outs AR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_lt64\tb0, $s, $t\n\trsr\t$r, br",
                                [(set AR:$r, (int_xtensa_ae_lt64 AEDR:$s, AEDR:$t))]>;
    def AE_LE64_Pseudo : Pseudo<(outs AR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_le64\tb0, $s, $t\n\trsr\t$r, br",
                                [(set AR:$r, (int_xtensa_ae_le64 AEDR:$s, AEDR:$t))]>;
  }
  // Miscellaneous operations
  def AE_NSAZ32_L_Pseudo : Pseudo<(outs AR:$r), (ins AEDR:$s),
                                  "ae_nsaz32.l\t$r, $s",
                                  [(set AR:$r, (int_xtensa_ae_nsaz32_l AEDR:$s))]> {
    let Predicates = [HasHIFI3];
  }

  def AE_NSAZ32_L : RRR_Inst<0x05, 0x04, 0x0f, (outs AR:$r), (ins AEDR:$s),
                             "ae_nsaz32.l\t$r, $s", []> {
    let t = 2;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
                               
  def AE_PKSR32_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$r_in, AEDR:$s, i32imm:$imm),
                               "ae_pksr32\t$r, $s, $imm",
                               [(set AEDR:$r, (int_xtensa_ae_pksr32 AEDR:$r_in, AEDR:$s, imm:$imm))]> {
                                  let Constraints = "$r = $r_in";
                                  let Predicates = [HasHIFI3];
                               }

  def AE_ROUND16X4F32SASYM_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_round16x4f32sasym\t$r, $s, $t",
                               [(set AEDR:$r, (int_xtensa_ae_round16x4f32sasym AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }
  
  def AE_ROUND16X4F32SSYM_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_round16x4f32ssym\t$r, $s, $t",
                               [(set AEDR:$r, (int_xtensa_ae_round16x4f32ssym AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_ROUND24X2F48SSYM_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_round24x2f48ssym\t$r, $s, $t",
                               [(set AEDR:$r, (int_xtensa_ae_round24x2f48ssym AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_ROUND32X2F64SASYM_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_round32x2f64sasym\t$r, $s, $t",
                               [(set AEDR:$r, (int_xtensa_ae_round32x2f64sasym AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_ROUND32X2F64SSYM_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_round32x2f64ssym\t$r, $s, $t",
                               [(set AEDR:$r, (int_xtensa_ae_round32x2f64ssym AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  // Single-operand wrappers: AE_ROUND32F48SSYM(a) == ae_round32x2f48ssym(a, a)
  // AE_ROUND32F48SASYM(a) == ae_round32x2f48sasym(a, a)
  // AE_ROUND32F64SSYM(a)  == ae_round32x2f64ssym(a, a)
  // These are trivially handled by the existing two-operand patterns with identical
  // inputs; no new ISel patterns are needed — the header macros expand them directly
  // to the two-operand builtins.

  def AE_ROUNDSP16Q48X2SYM_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_roundsp16q48x2sym\t$r, $s, $t",
                               [(set AEDR:$r, (int_xtensa_ae_roundsp16q48x2sym AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_ROUNDSQ32F48SYM_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s),
                               "ae_roundsq32f48sym\t$r, $s",
                               [(set AEDR:$r, (int_xtensa_ae_roundsq32f48sym AEDR:$s))]> { let Predicates = [HasHIFI3]; }

  def AE_ROUND24X2F48SASYM_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_round24x2f48sasym\t$r, $s, $t",
                               [(set AEDR:$r, (int_xtensa_ae_round24x2f48sasym AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_ROUNDSP16Q48X2ASYM_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_roundsp16q48x2asym\t$r, $s, $t",
                               [(set AEDR:$r, (int_xtensa_ae_roundsp16q48x2asym AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_ROUNDSP16F24ASYM_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s),
                               "ae_roundsp16f24asym\t$r, $s",
                               [(set AEDR:$r, (int_xtensa_ae_roundsp16f24asym AEDR:$s))]> { let Predicates = [HasHIFI3]; }

  def AE_ROUNDSP16F24SYM_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s),
                               "ae_roundsp16f24sym\t$r, $s",
                               [(set AEDR:$r, (int_xtensa_ae_roundsp16f24sym AEDR:$s))]> { let Predicates = [HasHIFI3]; }

  def AE_ROUNDSQ32F48ASYM_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s),
                               "ae_roundsq32f48asym\t$r, $s",
                               [(set AEDR:$r, (int_xtensa_ae_roundsq32f48asym AEDR:$s))]> { let Predicates = [HasHIFI3]; }

  def AE_SAT16X4_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_sat16x4\t$r, $s, $t",
                               [(set AEDR:$r, (int_xtensa_ae_sat16x4 AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_SAT24S_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s),
                               "ae_sat24s\t$r, $s",
                               [(set AEDR:$r, (int_xtensa_ae_sat24s AEDR:$s))]> { let Predicates = [HasHIFI3]; }

  def AE_SAT16X4 : RRR_Slot3_Inst<0x0d, 0x09, 0x01, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_sat16x4\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_SAT24S : RRR_Slot3_Inst<0x07, 0x0b, 0x01, (outs AEDR:$r), (ins AEDR:$s),
                               "ae_sat24s\t$r, $s", []> {
    let t = 6;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_SEL32_HH_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_sel16i\t$r, $s, $t, 3",
                               [(set AEDR:$r, (int_xtensa_ae_sel32_hh AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_SEL32_HL_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_sel16i\t$r, $s, $t, 2",
                               [(set AEDR:$r, (int_xtensa_ae_sel32_hl AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_SEL32_LH_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_sel16i\t$r, $s, $t, 1",
                               [(set AEDR:$r, (int_xtensa_ae_sel32_lh AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_SEL32_LL_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_sel16i\t$r, $s, $t, 0",
                               [(set AEDR:$r, (int_xtensa_ae_sel32_ll AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_SEL16I : AE_Sel_Inst<0x00, 0x04, 10, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t, i32imm:$sel),
                               "ae_sel16i\t$r, $s, $t, $sel", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_SELP24_HH_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_sel16i\t$r, $s, $t, 3",
                               [(set AEDR:$r, (int_xtensa_ae_selp24_hh AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }

  def AE_SELP24_LH_Pseudo_HIFI3 : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_sel16i\t$r, $s, $t, 1",
                               [(set AEDR:$r, (int_xtensa_ae_selp24_lh AEDR:$s, AEDR:$t))]> { let Predicates = [HasHIFI3]; }


}

let Predicates = [HasHIFI3] in {
let mayLoad = 1 in {
  def AE_L64_I_HIFI3 : XtensaInst24<(outs AEDR:$t), (ins mem32:$addr),
                                  "ae_l64.i\t$t, $addr",
                                  [(set AEDR:$t, (v2i32 (load addr_ish8:$addr)))]> {
    let Inst{23-20} = 0;
    let Inst{19-16} = 0;
    let Inst{15-12} = 0;
    let Inst{11-8}  = 0;
    let Inst{7-4}   = 0;
    let Inst{3-0}   = 0;
    let isCodeGenOnly = 1;
  }

  // HiFi3 Loads
  def AE_L16M_X : RRR_Inst<0x0E, 0x03, 0x00, (outs AEDR:$r), (ins AR:$s, AR:$t),
                           "ae_l16m.x\t$r, $s, $t",
                           [(set AEDR:$r, (int_xtensa_ae_l16m_x AR:$s, AR:$t))]>;
  def AE_L16M_XU : RRR_Inst<0x04, 0x03, 0x0B, (outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                            "ae_l16m.xu\t$r, $s, $t",
                            []> { let Constraints = "$s = $s_out"; }
  def AE_L32F24_XC : RRR_Inst<0x04, 0x05, 0x0B, (outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                              "ae_l32f24.xc\t$r, $s, $t",
                              []> { let Constraints = "$s = $s_out"; }
  def AE_L32M_X : RRR_Inst<0x04, 0x0A, 0x0C, (outs AEDR:$r), (ins AR:$s, AR:$t),
                           "ae_l32m.x\t$r, $s, $t",
                           [(set AEDR:$r, (int_xtensa_ae_l32m_x AR:$s, AR:$t))]>;
  def AE_L32X2F24_IP : RRR_Inst<0x04, 0x00, 0x02, (outs AEDR:$r, AR:$s_out), (ins AR:$s, uimm4_x8:$t),
                                "ae_l32x2f24.ip\t$r, $s, $t",
                                []> {
    let Inst{19-16} = 0xC;
    let Constraints = "$s = $s_out";
  }
  def AE_L32X2F24_XC : RRR_Inst<0x04, 0x04, 0x0C, (outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                "ae_l32x2f24.xc\t$r, $s, $t",
                                []> { let Constraints = "$s = $s_out"; }
  def AE_L32X2_I : RRR_Inst<0x04, 0x00, 0x0C, (outs AEDR:$r), (ins AR:$s, uimm4_x8:$t),
                            "ae_l32x2.i\t$r, $s, $t",
                            [(set AEDR:$r, (int_xtensa_ae_l32x2_i AR:$s, uimm4_x8:$t))]> {
    let Inst{19-16} = 0x1;
  }
  def AE_L32X2_IP : RRR_Inst<0x04, 0x00, 0x03, (outs AEDR:$r, AR:$s_out), (ins AR:$s, uimm4_x8:$t),
                             "ae_l32x2.ip\t$r, $s, $t",
                             [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32x2_ip AR:$s, uimm4_x8:$t))]> {
    let Inst{19-16} = 0xE;
    let Constraints = "$s = $s_out";
  }
  def AE_L32X2_XC : RRR_Inst<0x04, 0x00, 0x0C, (outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                             "ae_l32x2.xc\t$r, $s, $t",
                             [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32x2_xc AR:$s, AR:$t))]> { let Constraints = "$s = $s_out"; }

  def AE_L32X2_XC1_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                             "ae_l32x2.xc1\t$r, $s, $t",
                             [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32x2_xc1 AR:$s, AR:$t))]> { let Constraints = "$s = $s_out"; }

  def AE_L32X2_XC1 : XtensaInst64<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                   "ae_l32x2.xc1\t$r, $s, $t", []> {
    bits<4> r;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = r;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{63-12} = 0xbbbbbbbbbbbbb;
    let Constraints = "$s = $s_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

}

let mayStore = 1 in {
  def AE_S64_I_HIFI3 : XtensaInst24<(outs), (ins AEDR:$t, mem32:$addr),
                                  "ae_s64.i\t$t, $addr",
                                  [(store (v2i32 AEDR:$t), addr_ish8:$addr)]> {
    bits<4> t;
    bits<12> addr;
    let Inst{3-0} = t; // dummy encoding
    let DecoderNamespace = "XtensaHIFI3";
    let isCodeGenOnly = 1;
  }
}

def : Pat<(v4i16 (load addr_ish8:$addr)), (AE_L64_I_HIFI3 addr_ish8:$addr)>;
def : Pat<(v8i8  (load addr_ish8:$addr)), (AE_L64_I_HIFI3 addr_ish8:$addr)>;

def : Pat<(store (v4i16 AEDR:$t), addr_ish8:$addr), (AE_S64_I_HIFI3 AEDR:$t, addr_ish8:$addr)>;
def : Pat<(store (v8i8  AEDR:$t), addr_ish8:$addr), (AE_S64_I_HIFI3 AEDR:$t, addr_ish8:$addr)>;



  def AE_L32_X_PSEUDO : Pseudo<(outs AEDR:$t), (ins AR:$s, AR:$r),
                                         "ae_l32.x\t$t, $s, $r",
                                         [(set AEDR:$t, (int_xtensa_ae_l32_x AR:$s, AR:$r))]>;

  def AE_S32_L_X_PSEUDO : Pseudo<(outs), (ins AEDR:$t, AR:$s, AR:$r),
                                         "ae_s32.l.x\t$t, $s, $r",
                                         [(int_xtensa_ae_s32_l_x AEDR:$t, AR:$s, AR:$r)]>;

  def AE_S16_0_X_PSEUDO : Pseudo<(outs), (ins AEDR:$t, AR:$s, AR:$r),
                                         "ae_s16.0.x\t$t, $s, $r",
                                         [(int_xtensa_ae_s16_0_x AEDR:$t, AR:$s, AR:$r)]>;

  def AE_S16_0_X : XtensaInst64<(outs), (ins AEDR:$t, AR:$s, AR:$r),
                                 "ae_s16.0.x\t$t, $s, $r", []> {
    bits<4> t;
    bits<4> s;
    bits<4> r;
    let Inst{3-0} = t;
    let Inst{7-4} = s;
    let Inst{11-8} = r;
    let Inst{63-12} = 0x1111111111111;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_S16_0_XC : XtensaInst64<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                  "ae_s16.0.xc\t$r, $s, $t", []> {
    bits<4> r;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = r;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{63-12} = 0x2222222222222;
    let Constraints = "$s = $s_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_S16_0_XC1 : XtensaInst64<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                   "ae_s16.0.xc1\t$r, $s, $t", []> {
    bits<4> r;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = r;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{63-12} = 0x3333333333333;
    let Constraints = "$s = $s_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_S16_0_XP : XtensaInst24<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                  "ae_s16.0.xp\t$r, $s, $t", []> {
    bits<4> r;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = r;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{23-12} = 0x444;
    let Constraints = "$s = $s_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_L16X4_X_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$s, AR:$t),
                                "ae_l16x4.x\t$r, $s, $t", []>;


  def AE_L16_XP_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                "ae_l16.xp\t$r, $s, $t",
                                [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l16_xp AR:$s, AR:$t))]>;

  def AE_L16_XP : AE_Store_Inst<0x05, 0x0E, 0x02, (outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                               "ae_l16.xp\t$r, $s, $t", []> {
    let Constraints = "$s = $s_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
                                
  def AE_L32_XP_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                "ae_l32.xp\t$r, $s, $t",
                                [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32_xp AR:$s, AR:$t))]>;

  def AE_L32_XP : AE_Store_Inst<0x05, 0x0F, 0x01, (outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                               "ae_l32.xp\t$r, $s, $t", []> {
    let Constraints = "$s = $s_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_S32_L_XP : XtensaInst24<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                  "ae_s32.l.xp\t$r, $s, $t", []> {
    bits<4> r;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = r;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{23-12} = 0x555;
    let Constraints = "$s = $s_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_S32_L_X : XtensaInst24<(outs), (ins AEDR:$t, AR:$s, AR:$r),
                                 "ae_s32.l.x\t$t, $s, $r", []> {
    bits<4> t;
    bits<4> s;
    bits<4> r;
    let Inst{3-0} = t;
    let Inst{7-4} = s;
    let Inst{11-8} = r;
    let Inst{23-12} = 0x666;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_L32_X : XtensaInst24<(outs AEDR:$t), (ins AR:$s, AR:$r),
                               "ae_l32.x\t$t, $s, $r", []> {
    bits<4> t;
    bits<4> s;
    bits<4> r;
    let Inst{3-0} = t;
    let Inst{7-4} = s;
    let Inst{11-8} = r;
    let Inst{23-12} = 0x777;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_L32_XC1 : XtensaInst64<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                 "ae_l32.xc1\t$r, $s, $t", []> {
    bits<4> r;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = r;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{63-12} = 0x8888888888888;
    let Constraints = "$s = $s_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_S32_L_XC1 : XtensaInst64<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                   "ae_s32.l.xc1\t$r, $s, $t", []> {
    bits<4> r;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = r;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{63-12} = 0x9999999999999;
    let Constraints = "$s = $s_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

                                
  def AE_S16_0_XP_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                "ae_s16.0.xp\t$r, $s, $t",
                                [(set AR:$s_out, (int_xtensa_ae_s16_0_xp AEDR:$r, AR:$s, AR:$t))]> {
    let Constraints = "$s = $s_out";
  }
                                
  def AE_S32_L_XP_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                "ae_s32.l.xp\t$r, $s, $t",
                                [(set AR:$s_out, (int_xtensa_ae_s32_l_xp AEDR:$r, AR:$s, AR:$t))]> {
    let Constraints = "$s = $s_out";
  }

  def AE_L16_XC_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                "ae_l16.xc\t$r, $s, $t",
                                [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l16_xc AR:$s, AR:$t))]>;

  def AE_L16_XC : AE_Store_Inst<0x05, 0x0E, 0x01, (outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                               "ae_l16.xc\t$r, $s, $t", []> {
    let Constraints = "$s = $s_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_L32_XC1_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                "ae_l32.xc1\t$r, $s, $t",
                                [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32_xc1 AR:$s, AR:$t))]>;

  def AE_S16_0_XC_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                "ae_s16.0.xc\t$r, $s, $t",
                                [(set AR:$s_out, (int_xtensa_ae_s16_0_xc AEDR:$r, AR:$s, AR:$t))]> {
    let Constraints = "$s = $s_out";
  }
  def AE_S16_0_XC1_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                "ae_s16.0.xc1\t$r, $s, $t",
                                [(set AR:$s_out, (int_xtensa_ae_s16_0_xc1 AEDR:$r, AR:$s, AR:$t))]> {
    let Constraints = "$s = $s_out";
  }
  def AE_S32_L_XC_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                "ae_s32.l.xc\t$r, $s, $t",
                                [(set AR:$s_out, (int_xtensa_ae_s32_l_xc AEDR:$r, AR:$s, AR:$t))]> {
    let Constraints = "$s = $s_out";
  }

  def AE_S32_L_XC : AE_Store_Inst<0x0a, 0x01, 0x02, (outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                               "ae_s32.l.xc\t$r, $s, $t", []> {
    let Constraints = "$s = $s_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_S32_L_XC1_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                "ae_s32.l.xc1\t$r, $s, $t",
                                [(set AR:$s_out, (int_xtensa_ae_s32_l_xc1 AEDR:$r, AR:$s, AR:$t))]> {
    let Constraints = "$s = $s_out";
  }

  def AE_MULA2Q32X16_FIR_H_PSEUDO : Pseudo<(outs AEDR:$aOut), (ins AEDR:$a, AEDR:$b, AEDR:$c),
                                "mula2q.32x16.fir.h\t$a, $b, $c",
                                [(set AEDR:$aOut, (int_xtensa_ae_mula2q32x16_fir_h AEDR:$a, AEDR:$b, AEDR:$c))]>;

  def AE_MUL16_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_mul16\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_mul16 AEDR:$s, AEDR:$t))]>;

  def AE_L16M_X_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$s, AR:$t),
                                "ae_l16m.x\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_l16m_x AR:$s, AR:$t))]>;
  def AE_L16M_XU_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                "ae_l16m.xu\t$r, $s, $t",
                                [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l16m_xu AR:$s, AR:$t))]>;
  def AE_L32F24_XC_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                "ae_l32f24.xc\t$r, $s, $t",
                                [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32f24_xc AR:$s, AR:$t))]>;
  def AE_L32M_X_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$s, AR:$t),
                                "ae_l32m.x\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_l32m_x AR:$s, AR:$t))]>;
  def AE_L32X2F24_IP_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, uimm4_x8:$t),
                                "ae_l32x2f24.ip\t$r, $s, $t",
                                [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32x2f24_ip AR:$s, uimm4_x8:$t))]>;
  def AE_L32X2F24_XC_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                "ae_l32x2f24.xc\t$r, $s, $t",
                                [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32x2f24_xc AR:$s, AR:$t))]>;
  def AE_L32X2_I_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$s, uimm4_x8:$t),
                                "ae_l32x2.i\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_l32x2_i AR:$s, uimm4_x8:$t))]>;
  def AE_L32X2_IP_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, uimm4_x8:$t),
                                "ae_l32x2.ip\t$r, $s, $t",
                                [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32x2_ip AR:$s, uimm4_x8:$t))]>;
  def AE_L32X2_XC_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                "ae_l32x2.xc\t$r, $s, $t",
                                [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32x2_xc AR:$s, AR:$t))]>;

  def AE_S32X2_I_PSEUDO : Pseudo<(outs), (ins AEDR:$r, AR:$s, imm8:$t),
                                 "ae_s32x2.i\t$r, $s, $t", []>;
  def AE_S16X2M_I_PSEUDO : Pseudo<(outs), (ins AEDR:$r, AR:$s, imm8:$t),
                                 "ae_s16x2m.i\t$r, $s, $t", []>;
  def AE_S32X2_IP_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, imm8:$t),
                                 "ae_s32x2.ip\t$r, $s, $t", []> {
    let Constraints = "$s = $s_out";
  }
  def AE_S32X2_X_PSEUDO : Pseudo<(outs), (ins AEDR:$t, AR:$s, AR:$r),
                                 "ae_s32x2.x\t$t, $s, $r", []>;
  def AE_S32X2_XC_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                 "ae_s32x2.xc\t$r, $s, $t", []> {
    let Constraints = "$s = $s_out";
  }
  def AE_S32X2_XC1_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                 "ae_s32x2.xc1\t$r, $s, $t", []> {
    let Constraints = "$s = $s_out";
  }

  let Predicates = [HasHIFI3], mayStore = 1 in {
    def AE_S32X2_I : RRR_AE_Inst<0x09, 0x04, 0x0E, (outs), (ins AEDR:$r, AR:$s, imm8n_7_x8:$t),
                                 "ae_s32x2.i\t$r, $s, $t",
                                 [(int_xtensa_ae_s32x2_i AEDR:$r, AR:$s, imm8n_7_x8:$t)]>;

    def AE_S16X2M_I : RRR_AE_Inst<0x08, 0x04, 0x0D, (outs), (ins AEDR:$r, AR:$s, imm8n_7_x4:$t),
                                  "ae_s16x2m.i\t$r, $s, $t",
                                  [(int_xtensa_ae_s16x2m_i AEDR:$r, AR:$s, imm8n_7_x4:$t)]>;

    def AE_S32X2_IP : RRR_AE_Inst<0x02, 0x04, 0x06, (outs AR:$s_out), (ins AEDR:$r, AR:$s, uimm4_x8:$t),
                                  "ae_s32x2.ip\t$r, $s, $t",
                                  [(set AR:$s_out, (int_xtensa_ae_s32x2_ip AEDR:$r, AR:$s, uimm4_x8:$t))]> {
      let Constraints = "$s = $s_out";
    }

    def AE_S32X2_X : RRR_AE_Inst<0x0A, 0x04, 0x0E, (outs), (ins AEDR:$r, AR:$s, AR:$t),
                                 "ae_s32x2.x\t$r, $s, $t",
                                 [(int_xtensa_ae_s32x2_x AEDR:$r, AR:$s, AR:$t)]>;

    def AE_S32X2_XC : RRR_AE_Inst<0x0E, 0x04, 0x08, (outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                  "ae_s32x2.xc\t$r, $s, $t",
                                  [(set AR:$s_out, (int_xtensa_ae_s32x2_xc AEDR:$r, AR:$s, AR:$t))]> {
      let Constraints = "$s = $s_out";
    }

    def AE_S32X2_XC1 : RRR_AE_Inst<0x0E, 0x04, 0x0C, (outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                   "ae_s32x2.xc1\t$r, $s, $t",
                                   [(set AR:$s_out, (int_xtensa_ae_s32x2_xc1 AEDR:$r, AR:$s, AR:$t))]> {
      let Constraints = "$s = $s_out";
    }
  }

  def AE_SA16X4_IP_PSEUDO : Pseudo<(outs VALIGN:$align_out, AR:$s_out),
                                   (ins AEDR:$r, VALIGN:$align_in, AR:$s),
                                   "ae_sa16x4.ip\t$r, u1, $s",
                                   [(set VALIGN:$align_out, AR:$s_out,
                                     (int_xtensa_ae_sa16x4_ip AEDR:$r, VALIGN:$align_in, AR:$s))]> {
    let Constraints = "$s = $s_out, $align_in = $align_out";
  }
  def AE_SA16X4_IC_PSEUDO : Pseudo<(outs VALIGN:$align_out, AR:$s_out),
                                   (ins AEDR:$r, VALIGN:$align_in, AR:$s),
                                   "ae_sa16x4.ic\t$r, u1, $s",
                                   [(set VALIGN:$align_out, AR:$s_out,
                                     (int_xtensa_ae_sa16x4_ic AEDR:$r, VALIGN:$align_in, AR:$s))]> {
    let Constraints = "$s = $s_out, $align_in = $align_out";
  }
  def AE_SA32X2_IP_PSEUDO : Pseudo<(outs VALIGN:$align_out, AR:$s_out),
                                   (ins AEDR:$r, VALIGN:$align_in, AR:$s),
                                   "ae_sa32x2.ip\t$r, u1, $s",
                                   [(set VALIGN:$align_out, AR:$s_out,
                                     (int_xtensa_ae_sa32x2_ip AEDR:$r, VALIGN:$align_in, AR:$s))]> {
    let Constraints = "$s = $s_out, $align_in = $align_out";
  }

  def AE_SA24X2_IP_PSEUDO : Pseudo<(outs VALIGN:$align_out, AR:$s_out),
                                   (ins AEDR:$r, VALIGN:$align_in, AR:$s),
                                   "ae_sa24x2.ip\t$r, u1, $s",
                                   [(set VALIGN:$align_out, AR:$s_out,
                                     (int_xtensa_ae_sa24x2_ip AEDR:$r, VALIGN:$align_in, AR:$s))]> {
    let Constraints = "$s = $s_out, $align_in = $align_out";
  }

  def AE_LA24X2_IP_PSEUDO : Pseudo<(outs AEDR:$r, VALIGN:$align_out, AR:$s_out),
                                   (ins VALIGN:$align_in, AR:$s),
                                   "ae_la24x2.ip\t$r, u0, $s",
                                   [(set AEDR:$r, VALIGN:$align_out, AR:$s_out,
                                     (int_xtensa_ae_la24x2_ip VALIGN:$align_in, AR:$s))]> {
    let Constraints = "$align_in = $align_out";
  }

  def AE_LA128_PP_PSEUDO : Pseudo<(outs AR:$s_out, VALIGN:$align_out), (ins AR:$s),
                                 "ae_la128.pp\tu0, $s",
                                 [(set AR:$s_out, VALIGN:$align_out,
                                       (int_xtensa_ae_la128_pp AR:$s))]> {
    let Constraints = "$s = $s_out";
  }

  def AE_LA24_IP_PSEUDO : Pseudo<(outs AEDR:$r, VALIGN:$align_out, AR:$s_out),
                                 (ins VALIGN:$align_in, AR:$s),
                                 "ae_la24.ip\t$r, u0, $s",
                                 [(set AEDR:$r, VALIGN:$align_out, AR:$s_out,
                                   (int_xtensa_ae_la24_ip VALIGN:$align_in, AR:$s))]> {
    let Constraints = "$align_in = $align_out";
  }

  def AE_LA16X4POS_PC_PSEUDO : Pseudo<(outs), (ins AR:$s),
                                       "ae_la64.pp\tu0, $s",
                                       [(int_xtensa_ae_la16x4pos_pc AR:$s)]>;

  def AE_LA32X2POS_PC_PSEUDO : Pseudo<(outs), (ins AR:$s),
                                       "ae_la64.pp\tu0, $s",
                                       [(int_xtensa_ae_la32x2pos_pc AR:$s)]>;

  def AE_SA64POS_FP_PSEUDO : Pseudo<(outs), (ins VALIGN:$align, AR:$s),
                                    "ae_sa64pos.fp\tu1, $s",
                                    [(int_xtensa_ae_sa64pos_fp VALIGN:$align, AR:$s)]>;

  def AE_SA128POS_FP_PSEUDO : Pseudo<(outs), (ins VALIGN:$align, AR:$s),
                                     "ae_sa128pos.fp\tu1, $s",
                                     [(int_xtensa_ae_sa128pos_fp VALIGN:$align, AR:$s)]>;
  def AE_SA32X2_IC_PSEUDO : Pseudo<(outs VALIGN:$align_out, AR:$s_out),
                                   (ins AEDR:$r, VALIGN:$align_in, AR:$s),
                                   "ae_sa32x2.ic\t$r, u1, $s",
                                   [(set VALIGN:$align_out, AR:$s_out,
                                     (int_xtensa_ae_sa32x2_ic AEDR:$r, VALIGN:$align_in, AR:$s))]> {
    let Constraints = "$s = $s_out, $align_in = $align_out";
  }

  // Conversions and Moves Pseudos
  // ae_cvt16x4 is not in the GCC assembler. Use ae_round16x4f32sasym
  // which is the available equivalent (asymmetric rounding truncation).
  def AE_CVT16X4_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_round16x4f32sasym\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_cvt16x4 AEDR:$s, AEDR:$t))]>;
  def AE_TRUNC16X4F32_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                      "ae_round16x4f32sasym\t$r, $s, $t",
                                      [(set AEDR:$r, (int_xtensa_ae_trunc16x4f32 AEDR:$s, AEDR:$t))]>;
  // ae_cvt32x2f16.10/32 are not in the GCC assembler.
  // Use ae_sext32x2d16.10/32 which are available and equivalent.
  def AE_CVT32X2F16_10_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$t),
                                "ae_sext32x2d16.10\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_cvt32x2f16_10 AEDR:$t))]>;
  def AE_CVT32X2F16_32_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$t),
                                "ae_sext32x2d16.32\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_cvt32x2f16_32 AEDR:$t))]>;
  def AE_CVT48A32_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$t),
                                "ae_cvt48a32\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_cvt48a32 AR:$t))]>;

  def AE_CVT48A32 : RRR_AE_Inst<0x0c, 0x04, 0x03, (outs AEDR:$r), (ins AR:$s),
                                "ae_cvt48a32\t$r, $s", []> {
    let t = 1;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_CVTP24A16X2_LL_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$s, AR:$t),
                                "ae_cvtp24a16x2.ll\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_cvtp24a16x2_ll AR:$s, AR:$t))]>;
  // ae_cvtq48a32s is not in the GCC assembler. Use ae_cvt48a32 which is.
  def AE_CVTQ48A32S_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$t),
                                "ae_cvt48a32\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_cvtq48a32s AR:$t))]>;

  def AE_MOVAD16_0_PSEUDO : Pseudo<(outs AR:$r), (ins AEDR:$t),
                                "ae_movad16.0\t$r, $t",
                                [(set AR:$r, (int_xtensa_ae_movad16_0 AEDR:$t))]>;
  def AE_MOVAD16_1_PSEUDO : Pseudo<(outs AR:$r), (ins AEDR:$t),
                                "ae_movad16.1\t$r, $t",
                                [(set AR:$r, (int_xtensa_ae_movad16_1 AEDR:$t))]>;
  def AE_MOVAD16_2_PSEUDO : Pseudo<(outs AR:$r), (ins AEDR:$t),
                                "ae_movad16.2\t$r, $t",
                                [(set AR:$r, (int_xtensa_ae_movad16_2 AEDR:$t))]>;
  def AE_MOVAD16_3_PSEUDO : Pseudo<(outs AR:$r), (ins AEDR:$t),
                                "ae_movad16.3\t$r, $t",
                                [(set AR:$r, (int_xtensa_ae_movad16_3 AEDR:$t))]>;
  def AE_MOVAD32_H_PSEUDO : Pseudo<(outs AR:$r), (ins AEDR:$t),
                                "ae_movad32.h\t$r, $t",
                                [(set AR:$r, (int_xtensa_ae_movad32_h AEDR:$t))]>;
  def AE_MOVAD32_L_PSEUDO : Pseudo<(outs AR:$r), (ins AEDR:$t),
                                "ae_movad32.l\t$r, $t",
                                [(set AR:$r, (int_xtensa_ae_movad32_l AEDR:$t))]>;
                                
  def AE_MOVDA32_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$t),
                                "ae_movda32\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_movda32 AR:$t))]>;

  def AE_MOVAD32_L : RRR_Inst<0x05, 0x04, 0x0c, (outs AR:$r), (ins AEDR:$s),
                              "ae_movad32.l\t$r, $s", []> {
    let t = 2;
  }
  def AE_MOVAD32_H : RRR_Inst<0x05, 0x04, 0x0b, (outs AR:$r), (ins AEDR:$s),
                              "ae_movad32.h\t$r, $s", []> {
    let t = 2;
  }
  def AE_MOVDA32 : RRR_Inst<0x0c, 0x04, 0x03, (outs AEDR:$r), (ins AR:$s),
                             "ae_movda32\t$r, $s", []> {
    let t = 3;
  }
  def AE_MOVAD16_0 : RRR_Inst<0x05, 0x04, 0x08, (outs AR:$r), (ins AEDR:$s),
                              "ae_movad16.0\t$r, $s", []> {
    let t = 2;
  }
  def AE_MOVAD16_2 : RRR_Inst<0x05, 0x04, 0x09, (outs AR:$r), (ins AEDR:$s),
                              "ae_movad16.2\t$r, $s", []> {
    let t = 2;
  }
  def AE_MOVAD16_3 : RRR_Inst<0x05, 0x04, 0x0a, (outs AR:$r), (ins AEDR:$s),
                              "ae_movad16.3\t$r, $s", []> {
    let t = 2;
  }
  def AE_MOVDA32X2 : RRR_Inst<0x04, 0x05, 0x01, (outs AEDR:$r), (ins AR:$s, AR:$t),
                              "ae_movda32x2\t$r, $s, $t",
                              [(set AEDR:$r, (int_xtensa_ae_movda32x2 AR:$s, AR:$t))]>;

  def AE_MOVDA32X2_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$s, AR:$t),
                                "ae_movda32x2\t$r, $s, $t", []>;

  def : Pat<(v2i32 (build_vector AR:$s, AR:$t)),
            (AE_MOVDA32X2 AR:$s, AR:$t)>;

  // scalar_to_vector: put a scalar into a v2i32 (other lane is undef,
  // so we just splat it into both lanes for simplicity)
  def : Pat<(v2i32 (scalar_to_vector AR:$s)),
            (AE_MOVDA32X2 AR:$s, AR:$s)>;

  // Type reinterpretation casts (AEDR -> AEDR): these are no-ops in hardware.
  // The GCC assembler doesn't have these pseudo-opcodes. Use ae_mov instead.
  def AE_MOVF16X4_FROMINT64_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$t),
                                "ae_mov\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_movf16x4_fromint64 AEDR:$t))]>;
  def AE_MOVF24X2_FROMINT32X2_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$t),
                                "ae_mov\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_movf24x2_fromint32x2 AEDR:$t))]>;
  def AE_MOVINT16X4_FROMINT32X2_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$t),
                                "ae_mov\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_movint16x4_fromint32x2 AEDR:$t))]>;
  def AE_MOVINT24X2_FROMF24X2_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$t),
                                "ae_mov\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_movint24x2_fromf24x2 AEDR:$t))]>;
  def AE_MOVINT24X2_FROMF32X2_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$t),
                                "ae_mov\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_movint24x2_fromf32x2 AEDR:$t))]>;
  // Identity bitcasts: ae_int64 and ae_int32x2 are the same AEDR register.
  // AE_MOVINT32X2_FROMINT64(a) and AE_MOVINT64_FROMINT32X2(a) are both (a) in the header,
  // but if the compiler emits the intrinsic form, lower it to a no-op ae_mov.
  def AE_MOVINT32X2_FROMINT64_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$t),
                                "ae_mov\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_movint32x2_fromint64 AEDR:$t))]>;
  def AE_MOVINT64_FROMINT32X2_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$t),
                                "ae_mov\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_movint64_fromint32x2 AEDR:$t))]>;
  // AR -> AR type casts: no-op, use or (self-move)
  def AE_MOVINT32_FROMINT16_PSEUDO : Pseudo<(outs AR:$r), (ins AR:$t),
                                "or\t$r, $t, $t",
                                [(set AR:$r, (int_xtensa_ae_movint32_fromint16 AR:$t))]>;
  // AEDR -> AR extractions: use ae_movad32.l (extract low 32 bits)
  def AE_MOVINT32_FROMINT24X2_PSEUDO : Pseudo<(outs AR:$r), (ins AEDR:$t),
                                "ae_movad32.l\t$r, $t",
                                [(set AR:$r, (int_xtensa_ae_movint32_fromint24x2 AEDR:$t))]>;
  def AE_MOVINT32_FROMINT64_PSEUDO : Pseudo<(outs AR:$r), (ins AEDR:$t),
                                "ae_movad32.l\t$r, $t",
                                [(set AR:$r, (int_xtensa_ae_movint32_fromint64 AEDR:$t))]>;
                                
  def AE_SEXT32X2D16_10_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$t),
                                "ae_sext32x2d16.10\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sext32x2d16_10 AEDR:$t))]>;
  def AE_SEXT32X2D16_32_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$t),
                                "ae_sext32x2d16.32\t$r, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sext32x2d16_32 AEDR:$t))]>;

  def AE_SEXT32X2D16_10 : AE_Sext_Inst<0x03, 0x03, 0x03, 10, (outs AEDR:$r), (ins AEDR:$s),
                                 "ae_sext32x2d16.10\t$r, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_SEXT32X2D16_32 : RRR_Inst<0x0c, 0x04, 0x01, (outs AEDR:$r), (ins AEDR:$s),
                                 "ae_sext32x2d16.32\t$r, $s", []> {
    let t = 1;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_ROUND16X4F32SSYM : XtensaInst64<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                         "ae_round16x4f32ssym\t$t, $s, $r", []> {
    bits<4> t;
    bits<4> s;
    bits<4> r;

    let Inst{3-0}   = t;
    let Inst{7-4}   = s;
    let Inst{11-8}  = r;
    let Inst{63-12} = 0xfedcba9876543;
  }

  // Shifts Pseudos
  def AE_SLAA16S_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AR:$t),
                                "ae_slaa16s\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_slaa16s AEDR:$s, AR:$t))]>;
  def AE_SLAA32_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AR:$t),
                                "ae_slaa32\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_slaa32 AEDR:$s, AR:$t))]>;
  def AE_SLAA32S_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AR:$t),
                                "ae_slaa32s\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_slaa32s AEDR:$s, AR:$t))]>;
  def AE_SLAA64_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AR:$t),
                                "ae_slaa64\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_slaa64 AEDR:$s, AR:$t))]>;
  def AE_SLAI16S_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, uimm5:$imm),
                                "ae_slai16s\t$r, $s, $imm",
                                [(set AEDR:$r, (int_xtensa_ae_slai16s AEDR:$s, uimm5:$imm))]>;
  def AE_SLAI24S_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, uimm5:$imm),
                                "ae_slai24s\t$r, $s, $imm",
                                [(set AEDR:$r, (int_xtensa_ae_slai24s AEDR:$s, uimm5:$imm))]>;
  def AE_SLAI64_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, uimm6:$imm),
                                "ae_slai64\t$r, $s, $imm",
                                [(set AEDR:$r, (int_xtensa_ae_slai64 AEDR:$s, uimm6:$imm))]>;
  def AE_SRAA16RS_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AR:$t),
                                "ae_sraa16rs\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sraa16rs AEDR:$s, AR:$t))]>;
  def AE_SRAA32_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AR:$t),
                                "ae_sraa32\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sraa32 AEDR:$s, AR:$t))]>;

  def AE_SRAA32 : RRR_AE_Inst<0x07, 0x04, 0x00, (outs AEDR:$r), (ins AEDR:$t, AR:$s),
                              "ae_sraa32\t$r, $t, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_SRAA32RS_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AR:$t),
                                "ae_sraa32rs\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sraa32rs AEDR:$s, AR:$t))]>;
  def AE_SRAA32S_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AR:$t),
                                "ae_sraa32s\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sraa32s AEDR:$s, AR:$t))]>;
  def AE_SRAA64_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AR:$t),
                                "ae_sraa64\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sraa64 AEDR:$s, AR:$t))]>;
  def AE_SRAAQ56_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AR:$t),
                                "ae_sraaq56\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sraaq56 AEDR:$s, AR:$t))]>;

}

let Predicates = [HasHIFI3] in {
  def AE_AND_HIFI3 : RRR_Inst<0x04, 0x01, 0x04, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                       "ae_and\t$r, $s, $t", []> { let DecoderNamespace = "XtensaHIFI3"; }
  def AE_OR_HIFI3 : RRR_Inst<0x04, 0x06, 0x01, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                      "ae_or\t$r, $s, $t", []> { let DecoderNamespace = "XtensaHIFI3"; }
  def AE_XOR_HIFI3 : RRR_Inst<0x04, 0x0E, 0x01, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                       "ae_xor\t$r, $s, $t", []> { let DecoderNamespace = "XtensaHIFI3"; }

  def : Pat<(int_xtensa_ae_and16 AEDR:$s, AEDR:$t), (AE_AND_HIFI3 AEDR:$s, AEDR:$t)>;
  def : Pat<(int_xtensa_ae_or16  AEDR:$s, AEDR:$t), (AE_OR_HIFI3 AEDR:$s, AEDR:$t)>;
  def : Pat<(int_xtensa_ae_xor16 AEDR:$s, AEDR:$t), (AE_XOR_HIFI3 AEDR:$s, AEDR:$t)>;

  def : Pat<(int_xtensa_ae_and64 AEDR:$s, AEDR:$t), (AE_AND_HIFI3 AEDR:$s, AEDR:$t)>;
  def : Pat<(int_xtensa_ae_or64  AEDR:$s, AEDR:$t), (AE_OR_HIFI3 AEDR:$s, AEDR:$t)>;
  def : Pat<(int_xtensa_ae_xor64 AEDR:$s, AEDR:$t), (AE_XOR_HIFI3 AEDR:$s, AEDR:$t)>;

  def AE_ADD16S_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_add16s\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_add16s AEDR:$s, AEDR:$t))]>;

  def AE_ADD16S : RRR_AE_Inst<0x03, 0x04, 0x03, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                              "ae_add16s\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

// MAC Batch 1 Pseudo Instructions
  def AE_MUL32_HH_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                  "ae_mul32.hh\t$t, $s, $r",
                                  [(set AEDR:$t, (int_xtensa_ae_mul32_hh AEDR:$s, AEDR:$r))]>;

  def AE_MULAAAAFQ32X16_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$v, AEDR:$r),
                                        "ae_mulaaaafq32x16\t$t, $s, $v, $r",
                                        [(set AEDR:$t, (int_xtensa_ae_mulaaaafq32x16 AEDR:$acc, AEDR:$s, AEDR:$v, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }

  def AE_MULAAF2D32RA_HH_LL_PSEUDO : Pseudo<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$acc1, AEDR:$acc2, AEDR:$d0, AEDR:$d1, AEDR:$d2, AEDR:$d3),
                                            "ae_mulaaf2d32ra.hh.ll\t$t1, $t2, $d0, $d1, $d2, $d3",
                                            [(set AEDR:$t1, AEDR:$t2, (int_xtensa_ae_mulaaf2d32ra_hh_ll AEDR:$acc1, AEDR:$acc2, AEDR:$d0, AEDR:$d1, AEDR:$d2, AEDR:$d3))]> {
    let Constraints = "$acc1 = $t1, $acc2 = $t2";
  }

  def AE_MULAAFD24_HH_LL_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                         "ae_mulaafd24.hh.ll\t$t, $s, $r",
                                         [(set AEDR:$t, (int_xtensa_ae_mulaafd24_hh_ll AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }

  def AE_MULAAFD32RA_HH_LL_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                           "ae_mulaafd32ra.hh.ll\t$t, $s, $r",
                                           [(set AEDR:$t, (int_xtensa_ae_mulaafd32ra_hh_ll AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }

  def AE_MULAAFD32RA_HH_LL_HIFI3 : FLIX_MAC_Inst<0x1a, 0x46, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                                "ae_mulaafd32ra.hh.ll\t$t, $s, $r",
                                                [(set AEDR:$t, (int_xtensa_ae_mulaafd32ra_hh_ll AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  // AE_MULAAFP24S_HH_LL: HiFi3 24-bit fractional MAC (saturating).
  // The mnemonic "ae_mulaafp24s.hh.ll" is NOT recognized by the TGL GCC assembler
  // (Zephyr SDK xtensa-intel_tgl_adsp_zephyr-elf-as).  On HiFi3 / CAVS 2.5 targets
  // the assembler-visible instruction is "ae_mulaafd24.hh.ll".  We therefore emit
  // that mnemonic so -fno-integrated-as builds succeed with the SDK assembler.
  //
  // Encoding (verified from xtensa-intel_tgl_adsp_zephyr-elf-as disassembly):
  //   Bytes (LE): 0x9e 0x95 (0x80|t) (0x30|s) (0xc0|r) 0x1a 0x80 0x0c
  //   i.e. Inst{63:56}=0x0c, Inst{55:48}=0x80, Inst{47:40}=0x1a,
  //        Inst{39:36}=0xc, Inst{35:32}=r, Inst{31:28}=0x3, Inst{27:24}=s,
  //        Inst{23:20}=0x8, Inst{19:16}=t, Inst{15:8}=0x95, Inst{7:0}=0x9e
  def AE_MULAAFP24S_HH_LL_HIFI3 : XtensaInst64<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                                "ae_mulaafd24.hh.ll\t$t, $s, $r",
                                                [(set AEDR:$t, (int_xtensa_ae_mulaafp24s_hh_ll AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
    bits<4> t;
    bits<4> s;
    bits<4> r;
    let Inst{63-56} = 0x0c;
    let Inst{55-48} = 0x80;
    let Inst{47-40} = 0x1a;
    let Inst{39-36} = 0xc;
    let Inst{35-32} = r;
    let Inst{31-28} = 0x3;
    let Inst{27-24} = s;
    let Inst{23-20} = 0x8;
    let Inst{19-16} = t;
    let Inst{15-8}  = 0x95;
    let Inst{7-0}   = 0x9e;
  }

// MAC Batch 2 Pseudo Instructions


  def AE_MULAF32S_LL_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                     "ae_mulaf32s.ll\t$t, $s, $r",
                                     [(set AEDR:$t, (int_xtensa_ae_mulaf32s_ll AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }

  def AE_MULAF32S_LH_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                     "ae_mulaf32s.lh\t$t, $s, $r",
                                     [(set AEDR:$t, (int_xtensa_ae_mulaf32s_lh AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }

  // AE_MULAF32S_HL: acc += hi(b)*lo(c) == ae_mulaf32s.lh(acc, c, b) (b and c swapped).
  def AE_MULAF32S_HL_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                     "ae_mulaf32s.lh\t$t, $r, $s",
                                     [(set AEDR:$t, (int_xtensa_ae_mulaf32s_hl AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }

  def AE_MULF32S_LL_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                    "ae_mulf32s.ll\t$t, $s, $r",
                                    [(set AEDR:$t, (int_xtensa_ae_mulf32s_ll AEDR:$s, AEDR:$r))]>;

  def AE_MULF32S_LH_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                    "ae_mulf32s.lh\t$t, $s, $r",
                                    [(set AEDR:$t, (int_xtensa_ae_mulf32s_lh AEDR:$s, AEDR:$r))]>;

// MAC Batch 3 Pseudo Instructions
  def AE_MULAF32R_LL_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                     "ae_mulaf32r.ll\t$t, $s, $r",
                                     [(set AEDR:$t, (int_xtensa_ae_mulaf32r_ll AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }

  def AE_MULAF32R_LL_HIFI3 : FLIX_MAC_Inst<0x1f, 0x46, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                         "ae_mulaf32r.ll\t$t, $s, $r",
                                         [(set AEDR:$t, (int_xtensa_ae_mulaf32r_ll AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULAF32R_LH_HIFI3 : FLIX_MAC_Inst_Base<0x1f, 0x03, 0x46, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                                "ae_mulaf32r.lh\t$t, $s, $r",
                                                [(set AEDR:$t, (int_xtensa_ae_mulaf32r_lh AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULAF32S_LH_HIFI3 : FLIX_MAC_Inst_Base<0x20, 0x83, 0x46, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                                "ae_mulaf32s.lh\t$t, $s, $r",
                                                [(set AEDR:$t, (int_xtensa_ae_mulaf32s_lh AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULAF32S_LL_HIFI3 : FLIX_MAC_Inst_Base<0x21, 0x03, 0x46, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                                "ae_mulaf32s.ll\t$t, $s, $r",
                                                [(set AEDR:$t, (int_xtensa_ae_mulaf32s_ll AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULAF32S_HH_HIFI3 : FLIX_MAC_Inst_Base<0x20, 0x03, 0x46, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                                "ae_mulaf32s.hh\t$t, $s, $r",
                                                [(set AEDR:$t, (int_xtensa_ae_mulaf32s_hh AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }


  def AE_MULAF32R_LL : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                            "# ae_mulaf32r.ll",
                            [(set AEDR:$acc, (int_xtensa_ae_mulaf32r_ll AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULAF32R_LH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                            "# ae_mulaf32r.lh",
                            [(set AEDR:$acc, (int_xtensa_ae_mulaf32r_lh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULAF32S_HH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                            "# ae_mulaf32s.hh",
                            [(set AEDR:$acc, (int_xtensa_ae_mulaf32s_hh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULAF32RA_LL : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                            "# ae_mulaf32ra.ll",
                            [(set AEDR:$acc, (int_xtensa_ae_mulaf32ra_ll AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULAF32RA_LH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                            "# ae_mulaf32ra.lh",
                            [(set AEDR:$acc, (int_xtensa_ae_mulaf32ra_lh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULAF32RA_HH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                            "# ae_mulaf32ra.hh",
                            [(set AEDR:$acc, (int_xtensa_ae_mulaf32ra_hh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULSF32S_LL : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                            "# ae_mulsf32s.ll",
                            [(set AEDR:$acc, (int_xtensa_ae_mulsf32s_ll AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULSF32S_LH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                            "# ae_mulsf32s.lh",
                            [(set AEDR:$acc, (int_xtensa_ae_mulsf32s_lh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULSF32S_HH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                            "# ae_mulsf32s.hh",
                            [(set AEDR:$acc, (int_xtensa_ae_mulsf32s_hh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}

  def AE_MULAF32R_LH_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                     "ae_mulaf32r.lh\t$t, $s, $r",
                                     [(set AEDR:$t, (int_xtensa_ae_mulaf32r_lh AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }

  def AE_MULF32S_LL : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                           "# ae_mulf32s.ll",
                           [(set AEDR:$acc, (int_xtensa_ae_mulf32s_ll AEDR:$s, AEDR:$r))]>;
def AE_MULF32S_LH : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                           "# ae_mulf32s.lh",
                           [(set AEDR:$acc, (int_xtensa_ae_mulf32s_lh AEDR:$s, AEDR:$r))]>;
def AE_MULF32R_LL : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                           "# ae_mulf32r.ll",
                           [(set AEDR:$acc, (int_xtensa_ae_mulf32r_ll AEDR:$s, AEDR:$r))]>;
def AE_MULF32S_HH : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                           "# ae_mulf32s.hh",
                           [(set AEDR:$acc, (int_xtensa_ae_mulf32s_hh AEDR:$s, AEDR:$r))]>;
def AE_MULF32R_LH : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                           "# ae_mulf32r.lh",
                           [(set AEDR:$acc, (int_xtensa_ae_mulf32r_lh AEDR:$s, AEDR:$r))]>;
def AE_MULF32R_HH : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                           "# ae_mulf32r.hh",
                           [(set AEDR:$acc, (int_xtensa_ae_mulf32r_hh AEDR:$s, AEDR:$r))]>;
def AE_MULF32RA_LL : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                           "# ae_mulf32ra.ll",
                           [(set AEDR:$acc, (int_xtensa_ae_mulf32ra_ll AEDR:$s, AEDR:$r))]>;
def AE_MULF32RA_LH : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                           "# ae_mulf32ra.lh",
                           [(set AEDR:$acc, (int_xtensa_ae_mulf32ra_lh AEDR:$s, AEDR:$r))]>;
def AE_MULF32RA_HH : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                           "# ae_mulf32ra.hh",
                           [(set AEDR:$acc, (int_xtensa_ae_mulf32ra_hh AEDR:$s, AEDR:$r))]>;

  def AE_MULF32S_LL_REAL : RRR_Slot3_Inst<0x0f, 0x05, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_mulf32s.ll\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULF32S_LH_REAL : RRR_Slot3_Inst<0x0e, 0x05, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_mulf32s.lh\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULF32S_HH_REAL : RRR_Slot3_Inst<0x0d, 0x05, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_mulf32s.hh\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULF32R_LL_REAL : RRR_Slot3_Inst<0x0c, 0x05, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_mulf32r.ll\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULF32R_LH_REAL : RRR_Slot3_Inst<0x0b, 0x05, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_mulf32r.lh\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULF32R_HH_REAL : RRR_Slot3_Inst<0x0a, 0x05, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_mulf32r.hh\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULF32RA_LL_REAL : AE_Mul_Inst<0x0f, 0x01, 0x07, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_mulf32ra.ll\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULF32RA_LH_REAL : AE_Mul_Inst<0x0f, 0x01, 0x06, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_mulf32ra.lh\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULF32RA_HH_REAL : AE_Mul_Inst<0x0f, 0x01, 0x05, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                               "ae_mulf32ra.hh\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULSF32S_LL_REAL : RRR_Slot3_Inst<0x03, 0x09, 0x00, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                          "ae_mulsf32s.ll\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULSF32S_LH_REAL : RRR_Slot3_Inst<0x02, 0x09, 0x00, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                          "ae_mulsf32s.lh\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULSF32S_HH_REAL : RRR_Slot3_Inst<0x01, 0x09, 0x00, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                          "ae_mulsf32s.hh\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULSF32R_LL_REAL : RRR_Slot3_Inst<0x00, 0x09, 0x00, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                          "ae_mulsf32r.ll\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULSF32R_LH_REAL : RRR_Slot3_Inst<0x0f, 0x08, 0x00, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                          "ae_mulsf32r.lh\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULSF32R_HH_REAL : RRR_Slot3_Inst<0x0e, 0x08, 0x00, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                          "ae_mulsf32r.hh\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULSF32RA_LL_REAL : AE_Mul_Inst<0x07, 0x01, 0x06, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                         "ae_mulsf32ra.ll\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULSF32RA_LH_REAL : AE_Mul_Inst<0x07, 0x01, 0x05, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                         "ae_mulsf32ra.lh\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULSF32RA_HH_REAL : AE_Mul_Inst<0x07, 0x01, 0x04, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                         "ae_mulsf32ra.hh\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULFP16X4RAS_REAL : RRR_Slot3_Inst<0x01, 0x07, 0x01, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                           "ae_mulfp16x4ras\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULFP16X4S_REAL : RRR_Slot3_Inst<0x02, 0x07, 0x01, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                         "ae_mulfp16x4s\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULFP16X4RS_REAL : RRR_Slot3_Inst<0x00, 0x07, 0x01, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                          "ae_mulfp16x4rs\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULFC24RA_REAL : RRR_Slot3_Inst<0x05, 0x0f, 0x00, (outs AEDR:$r), (ins AEDR:$t, AEDR:$s),
                                        "ae_mulfc24ra\t$r, $t, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULFP32X16X2RAS_H_REAL : RRR_Slot3_Inst<0x0c, 0x06, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                                "ae_mulfp32x16x2ras.h\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULFP32X16X2RAS_L_REAL : RRR_Slot3_Inst<0x0d, 0x06, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                                "ae_mulfp32x16x2ras.l\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULFP32X16X2RS_H_REAL : RRR_Slot3_Inst<0x0e, 0x06, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                               "ae_mulfp32x16x2rs.h\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULFP32X16X2RS_L_REAL : RRR_Slot3_Inst<0x0f, 0x06, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                               "ae_mulfp32x16x2rs.l\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULFP32X2RS_REAL : RRR_Slot3_Inst<0x03, 0x07, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                           "ae_mulfp32x2rs\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
  def AE_MULFP24X2R_REAL : RRR_Slot3_Inst<0x0a, 0x06, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                          "ae_mulfp24x2r\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }


// --- Multiply family completions (ACE30 overlay) ---
// Pure multiply: ae_mul32_lh
def AE_MUL32_LH : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                          "# ae_mul32.lh",
                          [(set AEDR:$acc, (int_xtensa_ae_mul32_lh AEDR:$s, AEDR:$r))]>;

// Multiply-accumulate: ae_mula32_{hh,lh,ll}
def AE_MULA32_HH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                           "# ae_mula32.hh",
                           [(set AEDR:$acc, (int_xtensa_ae_mula32_hh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULA32_LH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                           "# ae_mula32.lh",
                           [(set AEDR:$acc, (int_xtensa_ae_mula32_lh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULA32_LL : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                           "# ae_mula32.ll",
                           [(set AEDR:$acc, (int_xtensa_ae_mula32_ll AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}

// Multiply-subtract: ae_muls32_{hh,lh,ll}
def AE_MULS32_HH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                           "# ae_muls32.hh",
                           [(set AEDR:$acc, (int_xtensa_ae_muls32_hh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULS32_LH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                           "# ae_muls32.lh",
                           [(set AEDR:$acc, (int_xtensa_ae_muls32_lh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULS32_LL : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                           "# ae_muls32.ll",
                           [(set AEDR:$acc, (int_xtensa_ae_muls32_ll AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}

// Multiply-subtract fractional rounding: ae_mulsf32r_{hh,lh,ll}
def AE_MULSF32R_HH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                             "# ae_mulsf32r.hh",
                             [(set AEDR:$acc, (int_xtensa_ae_mulsf32r_hh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULSF32R_LH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                             "# ae_mulsf32r.lh",
                             [(set AEDR:$acc, (int_xtensa_ae_mulsf32r_lh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULSF32R_LL : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                             "# ae_mulsf32r.ll",
                             [(set AEDR:$acc, (int_xtensa_ae_mulsf32r_ll AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}

// Multiply-subtract fractional rounding asymmetric: ae_mulsf32ra_{hh,lh,ll}
def AE_MULSF32RA_HH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                              "# ae_mulsf32ra.hh",
                              [(set AEDR:$acc, (int_xtensa_ae_mulsf32ra_hh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULSF32RA_LH : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                              "# ae_mulsf32ra.lh",
                              [(set AEDR:$acc, (int_xtensa_ae_mulsf32ra_lh AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}
def AE_MULSF32RA_LL : Pseudo<(outs AEDR:$acc), (ins AEDR:$t, AEDR:$s, AEDR:$r),
                              "# ae_mulsf32ra.ll",
                              [(set AEDR:$acc, (int_xtensa_ae_mulsf32ra_ll AEDR:$t, AEDR:$s, AEDR:$r))]> {
  let Constraints = "$acc = $t";
}

def AE_MULFP16X4RAS : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                             "ae_mulfp16x4ras\t$acc, $s, $r",
                             [(set AEDR:$acc, (int_xtensa_ae_mulfp16x4ras AEDR:$s, AEDR:$r))]> {
}
def AE_MULFP16X4RS : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                             "ae_mulfp16x4rs\t$acc, $s, $r",
                             [(set AEDR:$acc, (int_xtensa_ae_mulfp16x4rs AEDR:$s, AEDR:$r))]> {
}
def AE_MULFP16X4S : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                           "ae_mulfp16x4s\t$acc, $s, $r",
                           [(set AEDR:$acc, (int_xtensa_ae_mulfp16x4s AEDR:$s, AEDR:$r))]> {
}

def AE_MULFP32X16X2RAS_H : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                                  "ae_mulfp32x16x2ras.h\t$acc, $s, $r",
                                  [(set AEDR:$acc, (int_xtensa_ae_mulfp32x16x2ras_h AEDR:$s, AEDR:$r))]> {
}
def AE_MULFP32X16X2RAS_L : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                                  "ae_mulfp32x16x2ras.l\t$acc, $s, $r",
                                  [(set AEDR:$acc, (int_xtensa_ae_mulfp32x16x2ras_l AEDR:$s, AEDR:$r))]> {
}
def AE_MULFP32X16X2RS_H : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                                 "ae_mulfp32x16x2rs.h\t$acc, $s, $r",
                                 [(set AEDR:$acc, (int_xtensa_ae_mulfp32x16x2rs_h AEDR:$s, AEDR:$r))]> {
}
def AE_MULFP32X16X2RS_L : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                                 "ae_mulfp32x16x2rs.l\t$acc, $s, $r",
                                 [(set AEDR:$acc, (int_xtensa_ae_mulfp32x16x2rs_l AEDR:$s, AEDR:$r))]> {
}
def AE_MULFP32X2RS : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                            "ae_mulfp32x2rs\t$acc, $s, $r",
                            [(set AEDR:$acc, (int_xtensa_ae_mulfp32x2rs AEDR:$s, AEDR:$r))]> {
}
def AE_MULFP24X2R : Pseudo<(outs AEDR:$acc), (ins AEDR:$s, AEDR:$r),
                           "ae_mulfp24x2r\t$acc, $s, $r",
                           [(set AEDR:$acc, (int_xtensa_ae_mulfp24x2r AEDR:$s, AEDR:$r))]> {
}

  // 16x16 symmetric multiplies
  def AE_MULAF16SS_00_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                      "ae_mulaf16ss.00\t$t, $s, $r",
                                      [(set AEDR:$t, (int_xtensa_ae_mulaf16ss_00 AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }
  def AE_MULAF16SS_11_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                      "ae_mulaf16ss.11\t$t, $s, $r",
                                      [(set AEDR:$t, (int_xtensa_ae_mulaf16ss_11 AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }
  def AE_MULAF16SS_22_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                      "ae_mulaf16ss.22\t$t, $s, $r",
                                      [(set AEDR:$t, (int_xtensa_ae_mulaf16ss_22 AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }
  def AE_MULAF16SS_33_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                      "ae_mulaf16ss.33\t$t, $s, $r",
                                      [(set AEDR:$t, (int_xtensa_ae_mulaf16ss_33 AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }
  
  def AE_MULF16SS_00_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                     "ae_mulf16ss.00\t$t, $s, $r",
                                     [(set AEDR:$t, (int_xtensa_ae_mulf16ss_00 AEDR:$s, AEDR:$r))]>;
  def AE_MULF16SS_11_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                     "ae_mulf16ss.11\t$t, $s, $r",
                                     [(set AEDR:$t, (int_xtensa_ae_mulf16ss_11 AEDR:$s, AEDR:$r))]>;
  def AE_MULF16SS_22_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                     "ae_mulf16ss.22\t$t, $s, $r",
                                     [(set AEDR:$t, (int_xtensa_ae_mulf16ss_22 AEDR:$s, AEDR:$r))]>;
  def AE_MULF16SS_33_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                     "ae_mulf16ss.33\t$t, $s, $r",
                                     [(set AEDR:$t, (int_xtensa_ae_mulf16ss_33 AEDR:$s, AEDR:$r))]>;
  def AE_ADD24S_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_add24s\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_add24s AEDR:$s, AEDR:$t))]>;
  def AE_ADD32_HL_LH_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_add32_hl_lh\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_add32_hl_lh AEDR:$s, AEDR:$t))]>;
  def AE_ADD64S_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_add64s\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_add64s AEDR:$s, AEDR:$t))]>;
  def AE_SUB16S_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_sub16s\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sub16s AEDR:$s, AEDR:$t))]>;
  def AE_SUB24S_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_sub24s\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_sub24s AEDR:$s, AEDR:$t))]>;

  // Additional MAC intrinsics
  def AE_MULAFP32X2RS_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                      "ae_mulafp32x2rs\t$t, $s, $r",
                                      [(set AEDR:$t, (int_xtensa_ae_mulafp32x2rs AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }
  def AE_MULAFP32X16X2RS_H_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                            "ae_mulafp32x16x2rs.h\t$t, $s, $r",
                                            [(set AEDR:$t, (int_xtensa_ae_mulafp32x16x2rs_h AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }
  def AE_MULAFP32X16X2RS_L_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                            "ae_mulafp32x16x2rs.l\t$t, $s, $r",
                                            [(set AEDR:$t, (int_xtensa_ae_mulafp32x16x2rs_l AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
  }
  def AE_MULFC24RA_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                   "ae_mulfc24ra\t$r, $s, $t",
                                   [(set AEDR:$r, (int_xtensa_ae_mulfc24ra AEDR:$s, AEDR:$t))]>;
}

// Vector bitcasts (no-op within AEDR)
def : Pat<(v2i32 (bitconvert (v4i16 AEDR:$src))), (v2i32 AEDR:$src)>;
def : Pat<(v4i16 (bitconvert (v2i32 AEDR:$src))), (v4i16 AEDR:$src)>;
def : Pat<(v2i32 (bitconvert (v8i8 AEDR:$src))), (v2i32 AEDR:$src)>;
def : Pat<(v8i8 (bitconvert (v2i32 AEDR:$src))), (v8i8 AEDR:$src)>;
def : Pat<(v4i16 (bitconvert (v8i8 AEDR:$src))), (v4i16 AEDR:$src)>;
def : Pat<(v8i8 (bitconvert (v4i16 AEDR:$src))), (v8i8 AEDR:$src)>;

// extract_vector_elt from v2i32 → use AE_MOVAD32_L/H
def : Pat<(i32 (extractelt (v2i32 AEDR:$src), (i32 0))),
          (AE_MOVAD32_L_PSEUDO AEDR:$src)>;
def : Pat<(i32 (extractelt (v2i32 AEDR:$src), (i32 1))),
          (AE_MOVAD32_H_PSEUDO AEDR:$src)>;

// v4i16 extractelt would require i16 legalization, handled via
// AE_MOVAD16_* intrinsics directly in the Clang builtins instead.

// Truncating stores: store low 32 bits of v2i32
def : Pat<(truncstorei32 (v2i32 AEDR:$src), addr_ish8:$addr),
          (AE_S64_I_HIFI3 AEDR:$src, addr_ish8:$addr)>;

// ============================================================
// Missing HiFi3 intrinsics (from xt-clang reference analysis)
// ============================================================

// --- ae_sel16i: select 16-bit elements with 3-bit immediate ---
def AE_SEL16I_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t, i32imm:$sel),
                              "ae_sel16i\t$r, $s, $t, $sel",
                              [(set AEDR:$r, (int_xtensa_ae_sel16i AEDR:$s, AEDR:$t, imm:$sel))]> {
  let Predicates = [HasHIFI3];
}

// --- ae_l64_i / ae_s64_i: 64-bit load/store via intrinsic (separate ptr + offset) ---
let mayLoad = 1 in {
  def AE_L64_I_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$s, i32imm:$off),
                               "ae_l64.i\t$r, $s, $off",
                               [(set AEDR:$r, (int_xtensa_ae_l64_i AR:$s, imm:$off))]> {
    let Predicates = [HasHIFI3];
  }
}
let mayStore = 1 in {
  def AE_S64_I_PSEUDO : Pseudo<(outs), (ins AEDR:$d, AR:$s, i32imm:$off),
                               "ae_s64.i\t$d, $s, $off",
                               [(int_xtensa_ae_s64_i AEDR:$d, AR:$s, imm:$off)]> {
    let Predicates = [HasHIFI3];
  }
}

let mayLoad = 1 in {
  def AE_L64_I_REAL : XtensaInst24<(outs AEDR:$t), (ins AR:$s, uimm4_x8:$off),
                                   "ae_l64.i\t$t, $s, $off", []> {
    bits<4> t;
    bits<4> s;
    bits<4> off;

    let Inst{23-20} = off;
    let Inst{19-16} = 4;
    let Inst{15-12} = t;
    let Inst{11-8}  = s;
    let Inst{7-0}   = 0xcf;

    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
}

let mayStore = 1 in {
  def AE_S64_I_REAL : XtensaInst24<(outs), (ins AEDR:$d, AR:$s, uimm4_x8:$off),
                                   "ae_s64.i\t$d, $s, $off", []> {
    bits<4> d;
    bits<4> s;
    bits<4> off;

    let Inst{23-20} = off;
    let Inst{19-16} = 4;
    let Inst{15-12} = d;
    let Inst{11-8}  = s;
    let Inst{7-0}   = 0x01;

    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
}


// --- ae_zalign64: initialize store alignment register u0 to zero ---
// IMPORTANT: must declare Defs = [AE_VALIGN] so LLVM knows this writes the
// hardware u0 register.  Without it, LLVM sees AEDR:$r as dead (immediately
// overwritten by ae_la64.pp) and eliminates the instruction.  The result is
// then consumed by ae_sa32x2.ip which reads AE_VALIGN, creating a true
// def-use chain that prevents dead-code elimination.
let hasSideEffects = 1 in {
  def AE_ZALIGN64_PSEUDO_HIFI4 : Pseudo<(outs VALIGN:$align), (ins),
                                  "ae_zalign64\tu1",
                                  [(set VALIGN:$align, (int_xtensa_ae_zalign64))]> {
    let Predicates = [HasHIFI4];
  }
  def AE_ZALIGN64_PSEUDO_HIFI3 : Pseudo<(outs VALIGN:$align), (ins),
                                  "ae_zalign64\tu1",
                                  [(set VALIGN:$align, (int_xtensa_ae_zalign64))]> {
    let Predicates = [HasHIFI3, HasNoHIFI4];
  }
}

// --- ae_addandsub32s: simultaneous saturating add and subtract ---
def AE_ADDANDSUB32S_PSEUDO : Pseudo<(outs AEDR:$sum, AEDR:$diff), (ins AEDR:$a, AEDR:$b),
                                    "ae_addandsub32s\t$sum, $diff, $a, $b",
                                    [(set AEDR:$sum, AEDR:$diff, (int_xtensa_ae_addandsub32s AEDR:$a, AEDR:$b))]> {
  let Predicates = [HasHIFI3];
}

// --- ae_l16x2m_xu: load 16x2 with register post-increment ---
let mayLoad = 1 in {
  def AE_L16X2M_XU_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                   "ae_l16x2m.xu\t$r, $s, $t",
                                   [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l16x2m_xu AR:$s, AR:$t))]> {
    let Constraints = "$s = $s_out";
    let Predicates = [HasHIFI3];
  }
}

// --- ae_movda16: move AR to AE_DR (16-bit replicate) ---
def AE_MOVDA16_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$t),
                               "ae_movda16\t$r, $t",
                               [(set AEDR:$r, (int_xtensa_ae_movda16 AR:$t))]> {
  let Predicates = [HasHIFI3];
}

// --- ae_s32x2f24_i: store 32x2 in f24 format with immediate offset ---
let mayStore = 1 in {
  def AE_S32X2F24_I_PSEUDO : Pseudo<(outs), (ins AEDR:$d, AR:$a, i32imm:$off),
                                    "ae_s32x2f24.i\t$d, $a, $off", []> {
    let Predicates = [HasHIFI3];
  }
  def AE_S32X2F24_IP_PSEUDO : Pseudo<(outs AR:$a_out), (ins AEDR:$d, AR:$a, i32imm:$off),
                                     "ae_s32x2f24.ip\t$d, $a, $off", []> {
    let Constraints = "$a = $a_out";
    let Predicates = [HasHIFI3];
  }

  let Predicates = [HasHIFI3] in {
    def AE_S32X2F24_I : RRR_AE_Inst<0x0D, 0x04, 0x0E, (outs), (ins AEDR:$r, AR:$s, uimm4_x8:$t),
                                    "ae_s32x2f24.i\t$r, $s, $t",
                                    [(int_xtensa_ae_s32x2f24_i AEDR:$r, AR:$s, uimm4_x8:$t)]>;

    def AE_S32X2F24_IP : RRR_AE_Inst<0x02, 0x04, 0x06, (outs AR:$s_out), (ins AEDR:$r, AR:$s, imm8_x4_add8:$t),
                                     "ae_s32x2f24.ip\t$r, $s, $t",
                                     [(set AR:$s_out, (int_xtensa_ae_s32x2f24_ip AEDR:$r, AR:$s, imm8_x4_add8:$t))]> {
      let Constraints = "$s = $s_out";
      let isCodeGenOnly = 1;
    }
  }
}

// --- ae_clamps16: clamp to signed 16-bit (AR -> AR) ---
def AE_CLAMPS16_PSEUDO : Pseudo<(outs AR:$r), (ins AR:$s),
                                "ae_clamps16\t$r, $s",
                                [(set AR:$r, (int_xtensa_ae_clamps16 AR:$s))]> {
  let Predicates = [HasHIFI3];
}

// --- ae_sext16: sign-extend 16-bit (AR -> AR) ---
def AE_SEXT16_PSEUDO : Pseudo<(outs AR:$r), (ins AR:$s),
                              "ae_sext16\t$r, $s",
                              [(set AR:$r, (int_xtensa_ae_sext16 AR:$s))]> {
  let Predicates = [HasHIFI3];
}

// --- ae_zext16: zero-extend 16-bit (AR -> AR) ---
def AE_ZEXT16_PSEUDO : Pseudo<(outs AR:$r), (ins AR:$s),
                              "ae_zext16\t$r, $s",
                              [(set AR:$r, (int_xtensa_ae_zext16 AR:$s))]> {
  let Predicates = [HasHIFI3];
}

// --- ae_movab2: move AE bool2 to AR ---
def AE_MOVAB2_PSEUDO : Pseudo<(outs AR:$r), (ins AR:$b),
                              "ae_movab2\t$r, $b",
                              [(set AR:$r, (int_xtensa_ae_movab2 AR:$b))]> {
  let Predicates = [HasHIFI3];
}

// =========================================================================
// HiFi5 128-bit Loads
// =========================================================================

def AE_LA32X2X2_IP_PSEUDO : Pseudo<(outs AEDR:$r0, AEDR:$r1, AR:$s_out), (ins AR:$s),
                                   "ae_la32x2x2_ip\t$r0, $r1, u0, $s",
                                   [(set AEDR:$r0, AEDR:$r1, AR:$s_out,
                                         (int_xtensa_ae_la32x2x2_ip AR:$s))]> {
  let Constraints = "$s = $s_out";
  let Predicates = [HasHIFI5];
  let mayLoad = 1;
  // Uses VALIGN via explicit DAG operand
}

def AE_LA16X4X2_IP_PSEUDO : Pseudo<(outs AEDR:$r0, AEDR:$r1, AR:$s_out), (ins AR:$s),
                                   "ae_la16x4x2_ip\t$r0, $r1, u0, $s",
                                   [(set AEDR:$r0, AEDR:$r1, AR:$s_out,
                                         (int_xtensa_ae_la16x4x2_ip AR:$s))]> {
  let Constraints = "$s = $s_out";
  let Predicates = [HasHIFI5];
  let mayLoad = 1;
  // Uses VALIGN via explicit DAG operand
}

def AE_L32X2X2_XC_PSEUDO : Pseudo<(outs AEDR:$r0, AEDR:$r1, AR:$s_out), (ins AR:$s, AR:$off),
                                  "ae_l32x2x2.xc\t$r0, $r1, $s, $off",
                                  [(set AEDR:$r0, AEDR:$r1, AR:$s_out,
                                        (int_xtensa_ae_l32x2x2_xc AR:$s, AR:$off))]> {
  let Constraints = "$s = $s_out";
  let Predicates = [HasHIFI5];
  let mayLoad = 1;
}

def AE_L32X2X2_XC1_PSEUDO : Pseudo<(outs AEDR:$r0, AEDR:$r1, AR:$s_out), (ins AR:$s, AR:$off),
                                   "ae_l32x2x2.xc1\t$r0, $r1, $s, $off",
                                   [(set AEDR:$r0, AEDR:$r1, AR:$s_out,
                                         (int_xtensa_ae_l32x2x2_xc1 AR:$s, AR:$off))]> {
  let Constraints = "$s = $s_out";
  let Predicates = [HasHIFI5];
  let mayLoad = 1;
}

// =========================================================================
// HiFi5 128-bit Stores
// =========================================================================

def AE_SA32X2X2_IP_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$d0, AEDR:$d1, AR:$s),
                                   "ae_sa32x2x2_ip\t$d0, $d1, u1, $s",
                                   [(set AR:$s_out,
                                         (int_xtensa_ae_sa32x2x2_ip AEDR:$d0, AEDR:$d1, AR:$s))]> {
  let Constraints = "$s = $s_out";
  let Predicates = [HasHIFI5];
  let mayStore = 1;
  // Uses VALIGN via explicit DAG operand
}

def AE_SA16X4X2_IP_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$d0, AEDR:$d1, AR:$s),
                                   "ae_sa16x4x2_ip\t$d0, $d1, u1, $s",
                                   [(set AR:$s_out,
                                         (int_xtensa_ae_sa16x4x2_ip AEDR:$d0, AEDR:$d1, AR:$s))]> {
  let Constraints = "$s = $s_out";
  let Predicates = [HasHIFI5];
  let mayStore = 1;
  // Uses VALIGN via explicit DAG operand
}

def AE_S32X2X2_XC1_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$d0, AEDR:$d1, AR:$s, AR:$off),
                                   "ae_s32x2x2.xc1\t$d0, $d1, $s, $off",
                                   [(set AR:$s_out,
                                         (int_xtensa_ae_s32x2x2_xc1 AEDR:$d0, AEDR:$d1, AR:$s, AR:$off))]> {
  let Constraints = "$s = $s_out";
  let Predicates = [HasHIFI5];
  let mayStore = 1;
}

// =========================================================================
// HiFi5 MACs (dual-output)
// =========================================================================

def AE_MULA2Q32X16_FIR_H5_PSEUDO : Pseudo<(outs AEDR:$t1, AEDR:$t2),
                                    (ins AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$d2, AEDR:$c),
                                    "ae_mula2q32x16.fir.h\t$q0, $q1, $d0, $d1, $d2, $c",
                                    [(set AEDR:$t1, AEDR:$t2,
                                          (int_xtensa_ae_mula2q32x16_fir_h5
                                           AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$d2, AEDR:$c))]> {
  let Constraints = "$q0 = $t1, $q1 = $t2";
  let Predicates = [HasHIFI5];
}

def AE_MULF32X2R_HH_LL_PSEUDO : Pseudo<(outs AEDR:$r0, AEDR:$r1), (ins AEDR:$d0, AEDR:$d1),
                                        "ae_mulf32x2r.hh.ll\t$r0, $r1, $d0, $d1",
                                        [(set AEDR:$r0, AEDR:$r1,
                                              (int_xtensa_ae_mulf32x2r_hh_ll AEDR:$d0, AEDR:$d1))]> {
  let Predicates = [HasHIFI5];
}

def AE_MULF2P32X16X4RS_PSEUDO : Pseudo<(outs AEDR:$r0, AEDR:$r1),
                                        (ins AEDR:$d0, AEDR:$d1, AEDR:$c),
                                        "ae_mulf2p32x16x4rs\t$r0, $r1, $d0, $d1, $c",
                                        [(set AEDR:$r0, AEDR:$r1,
                                              (int_xtensa_ae_mulf2p32x16x4rs AEDR:$d0, AEDR:$d1, AEDR:$c))]> {
  let Predicates = [HasHIFI5];
}

def AE_MULF2P32X4RS_PSEUDO : Pseudo<(outs AEDR:$r0, AEDR:$r1),
                                     (ins AEDR:$d0, AEDR:$d1, AEDR:$d2, AEDR:$d3),
                                     "ae_mulf2p32x4rs\t$r0, $r1, $d0, $d1, $d2, $d3",
                                     [(set AEDR:$r0, AEDR:$r1,
                                           (int_xtensa_ae_mulf2p32x4rs AEDR:$d0, AEDR:$d1, AEDR:$d2, AEDR:$d3))]> {
  let Predicates = [HasHIFI5];
}

//===----------------------------------------------------------------------===//
// Audio Engine Assembly-Only / missing instructions
//===----------------------------------------------------------------------===//

let DecoderNamespace = "XtensaHIFI3" in {

// --- ae_movae: move from AEP register to AR register ---
class AE_MOVAE_Inst<bits<4> op1, bits<4> op2, dag outs, dag ins,
                    string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> t;
  bits<2> b;

  let Inst{23-20} = op2;
  let Inst{19-16} = op1;
  let Inst{15-12} = 0;
  let Inst{11-8} = 15;
  let Inst{7-4} = t;
  let Inst{3-2} = 0;
  let Inst{1-0} = b;
}

def AE_MOVAE : AE_MOVAE_Inst<6, 2, (outs AR:$t), (ins BR_AEP:$b),
                             "ae_movae\t$t, $b", []>;

// --- ae_movea: move from AR register to AEP register ---
class AE_MOVEA_Inst<bits<2> op0_high, bits<4> op1, bits<4> op2, dag outs, dag ins,
                    string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> t;
  bits<2> b;

  let Inst{23-20} = op2;
  let Inst{19-16} = op1;
  let Inst{15-12} = 0;
  let Inst{11-8} = 15;
  let Inst{7-4} = t;
  let Inst{3-2} = op0_high;
  let Inst{1-0} = b;
}

def AE_MOVEA : AE_MOVEA_Inst<1, 6, 2, (outs BR_AEP:$b), (ins AR:$t),
                             "ae_movea\t$b, $t", []>;

// --- ae_movfcrfsrv / ae_movvfcrfsr: move floating-point state ---
class AE_MOVState_Inst<bits<4> op2, bits<4> op1, bits<4> r_val, bits<4> s_val, bits<4> t_val,
                       dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<4> d;

  let Inst{23-20} = op2;
  let Inst{19-16} = op1;
  let Inst{15-12} = r_val;
  let Inst{11-8} = s_val;
  let Inst{7-4} = t_val;
  let Inst{3-0} = d;
}

def AE_MOVFCRFSRV : AE_MOVState_Inst<1, 7, 15, 7, 0, (outs), (ins AEDR:$d),
                                     "ae_movfcrfsrv\t$d", []>;
def AE_MOVVFCRFSR : AE_MOVState_Inst<1, 7, 15, 7, 1, (outs AEDR:$d), (ins),
                                     "ae_movvfcrfsr\t$d", []>;

// --- ae_zalign64: initialize alignment register to zero ---
class AE_ZALIGN64_Inst<bits<4> op0, bits<4> op1, bits<4> op2, bits<4> r_val, bits<4> s_val,
                       dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<2> align;

  let Inst{23-20} = op2;
  let Inst{19-16} = op1;
  let Inst{15-12} = r_val;
  let Inst{11-8} = s_val;
  let Inst{7-6} = align;
  let Inst{5-4} = 0;
  let Inst{3-0} = op0;
}

def AE_ZALIGN64 : AE_ZALIGN64_Inst<4, 8, 2, 1, 3, (outs), (ins URAlign:$align),
                                   "ae_zalign64\t$align", []>;

// --- ae_lalign64.i / ae_salign64.i: load/store alignment state ---
class AE_LALIGN64_I_Inst<bits<4> op0, bits<4> op1, bits<4> op2, bits<2> r_high,
                         dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst24<outs, ins, asmstr, pattern> {
  bits<2> align;
  bits<4> s;
  bits<4> imm;

  let Inst{23-20} = op2;
  let Inst{19-16} = op1;
  let Inst{15-14} = r_high;
  let Inst{13-12} = imm{3-2};
  let Inst{11-8} = s;
  let Inst{7-6} = align;
  let Inst{5-4} = imm{1-0};
  let Inst{3-0} = op0;
}

def AE_LALIGN64_I : AE_LALIGN64_I_Inst<14, 6, 0, 0, (outs), (ins URAlign:$align, AR:$s, uimm4_x8:$imm),
                                       "ae_lalign64.i\t$align, $s, $imm", []>;

def AE_SALIGN64_I : AE_LALIGN64_I_Inst<14, 6, 0, 1, (outs), (ins URAlign:$align, AR:$s, uimm4_x8:$imm),
                                       "ae_salign64.i\t$align, $s, $imm", []>;

// --- ae_sb: store boolean state ---
def AE_SB : RRR_Inst<0x04, 0x07, 0x07, (outs), (ins AR:$s, AR:$t),
                     "ae_sb\t$s, $t", []> {
  let r = 10;
}

// --- ae_sbi: store boolean state immediate ---
def AE_SBI : RRR_Inst<0x04, 0x03, 0x06, (outs), (ins AR:$s, AR:$t, uimm4:$r),
                      "ae_sbi\t$s, $t, $r", []>;

} // End let DecoderNamespace = "XtensaHIFI3"

} // End let Predicates = [HasHIFI3]

//===----------------------------------------------------------------------===//
// HiFi5 (ACE4.0) instructions
//===----------------------------------------------------------------------===//

class AE_LALIGN128_I_Inst<dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst48<outs, ins, asmstr, pattern> {
  bits<2> align;
  bits<4> s;
  bits<4> imm;

  let Inst{47-44} = 0xd;
  let Inst{43}    = 1;
  let Inst{42}    = 0;
  let Inst{41}    = imm{1};
  let Inst{40}    = 1;
  let Inst{39-32} = 0xb0;
  let Inst{31-30} = 0;
  let Inst{29-26} = s;
  let Inst{25}    = imm{0};
  let Inst{24}    = 0;
  let Inst{23-20} = 0;
  let Inst{19-18} = align;
  let Inst{17-16} = 0;
  let Inst{15-0}  = 0x155e;
}

class AE_SALIGN128_I_Inst<dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst48<outs, ins, asmstr, pattern> {
  bits<2> align;
  bits<4> s;
  bits<4> imm;

  let Inst{47-24} = 0xd1b801;
  let Inst{23-20} = 1;
  let Inst{19-17} = 0b100;
  let Inst{16}    = imm{1};
  let Inst{15-12} = 8;
  let Inst{11}    = imm{0};
  let Inst{10}    = 0;
  let Inst{9-8}   = align;
  let Inst{7-4}   = s;
  let Inst{3-0}   = 0xe;
}

class AE_MOV_DR_ZB_Inst<dag outs, dag ins, string asmstr, list<dag> pattern>
  : XtensaInst64<outs, ins, asmstr, pattern> {
  bits<4> r;

  let Inst{63-16} = 0;
  let Inst{15-12} = r;
  let Inst{11-0} = 0;
  let DecoderNamespace = "XtensaHIFI128";
}

//===----------------------------------------------------------------------===//
// Group 2: Moves, Conversions, and Sign/Zero Extensions Real Instructions
//===----------------------------------------------------------------------===//
let Predicates = [HasHIFI3] in {

  // Base class for AE SEXT/ZEXT 16-bit instructions
  class AE_SEXT_ZEXT_Inst<bits<4> op4, dag outs, dag ins, string asmstr, list<dag> pattern>
    : XtensaInst16<outs, ins, asmstr, pattern> {
    bits<4> r;
    bits<4> s;

    let Inst{15-12} = op4;
    let Inst{11-8} = s;
    let Inst{7-4} = r;
    let Inst{3-0} = 0xd;
  }

  def AE_SEXT16 : AE_SEXT_ZEXT_Inst<0x8, (outs AR:$r), (ins AR:$s),
                                   "ae_sext16\t$r, $s", []>;

  def AE_ZEXT16 : AE_SEXT_ZEXT_Inst<0x9, (outs AR:$r), (ins AR:$s),
                                   "ae_zext16\t$r, $s", []>;

  def AE_MOVDA16 : RRR_Inst<0x0c, 0x03, 0x02, (outs AEDR:$r), (ins AR:$s),
                             "ae_movda16\t$r, $s", []> {
    let t = 4;
    // Swap r and s fields compared to standard RRR_Inst to match hardware encoding
    let Inst{15-12} = r;
    let Inst{11-8} = s;
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_MOVAD16_1 : XtensaInst48<(outs AR:$r), (ins AEDR:$s),
                                  "ae_movad16.1\t$r, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_MOVAB2 : XtensaInst24<(outs AR:$r), (ins),
                               "ae_movab2\t$r, b0", []> {
    bits<4> r;
    let Inst{3-0} = 0xf;
    let Inst{7-4} = 0x2;
    let Inst{11-8} = 0x8;
    let Inst{15-12} = r;
    let Inst{19-16} = 0x4;
    let Inst{23-20} = 0; // Hardcoded to b0
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_CVTP24A16X2_LL : RRR_AE_Inst<0x03, 0x04, 0x01, (outs AEDR:$r), (ins AR:$s, AR:$t),
                                      "ae_cvtp24a16x2.ll\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_ROUND16X4F32SASYM : XtensaInst64<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                          "ae_round16x4f32sasym\t$t, $s, $r", []> {
    bits<4> t;
    bits<4> s;
    bits<4> r;
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_MOV : RRR_AE_Inst<0x0e, 0x04, 0x03, (outs AEDR:$r), (ins AEDR:$s),
                           "ae_mov\t$r, $s", []> {
    let t = 1;
    let DecoderNamespace = "XtensaHIFI3";
  }

  //===----------------------------------------------------------------------===//
  // Group 4: Shift Real Instructions
  //===----------------------------------------------------------------------===//
  def AE_SLAA16S : RRR_AE_Inst<0x03, 0x04, 0x02, (outs AEDR:$r), (ins AEDR:$t, AR:$s),
                               "ae_slaa16s\t$r, $t, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SLAA32 : RRR_AE_Inst<0x00, 0x04, 0x02, (outs AEDR:$r), (ins AEDR:$t, AR:$s),
                              "ae_slaa32\t$r, $t, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SLAA32S : RRR_AE_Inst<0x05, 0x04, 0x00, (outs AEDR:$r), (ins AEDR:$t, AR:$s),
                               "ae_slaa32s\t$r, $t, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SLAA64 : RRR_AE_Inst<0x02, 0x04, 0x02, (outs AEDR:$r), (ins AEDR:$t, AR:$s),
                              "ae_slaa64\t$r, $t, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SRAA32S : RRR_AE_Inst<0x02, 0x04, 0x08, (outs AEDR:$r), (ins AEDR:$t, AR:$s),
                               "ae_sraa32s\t$r, $t, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SRAA64 : RRR_AE_Inst<0x01, 0x04, 0x08, (outs AEDR:$r), (ins AEDR:$t, AR:$s),
                              "ae_sraa64\t$r, $t, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SRAAQ56 : RRR_AE_Inst<0x01, 0x04, 0x09, (outs AEDR:$r), (ins AEDR:$t, AR:$s),
                               "ae_sraaq56\t$r, $t, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SLAI24S : RRR_AE_SHIFT_Inst<0x4, 0x1, 0x9,
                                     "ae_slai24s\t$r, $t, $imm",
                                     (outs AEDR:$r), (ins AEDR:$t, uimm5:$imm), []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SLAI64 : RRR_AE_SHIFT64_Inst<0x2, 0x1, 0x8,
                                     "ae_slai64\t$r, $t, $imm",
                                     (outs AEDR:$r), (ins AEDR:$t, uimm6:$imm), []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SRAA16RS : XtensaInst64<(outs AEDR:$r), (ins AEDR:$t, AR:$s),
                                 "ae_sraa16rs\t$r, $t, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SRAA32RS : XtensaInst64<(outs AEDR:$r), (ins AEDR:$t, AR:$s),
                                 "ae_sraa32rs\t$r, $t, $s", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SLAI16S : XtensaInst64<(outs AEDR:$r), (ins AEDR:$t, uimm5:$imm),
                                "ae_slai16s\t$r, $t, $imm", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  //===----------------------------------------------------------------------===//
  // Group 3: Real Arithmetic Instructions
  //===----------------------------------------------------------------------===//
  def AE_ADD24S : RRR_AE_Inst<0x02, 0x04, 0x03, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                             "ae_add24s\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SUB16S : RRR_AE_Inst<0x0e, 0x04, 0x04, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                             "ae_sub16s\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_ADD32_HL_LH_REAL : RRR_Slot3_Inst<0x0d, 0x01, 0x07, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                          "ae_add32_hl_lh\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_ADD64S_REAL : RRR_Slot3_Inst<0x0d, 0x01, 0x04, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                      "ae_add64s\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SUB24S_REAL : RRR_Slot3_Inst<0x0d, 0x01, 0x05, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                      "ae_sub24s\t$r, $s, $t", []> {
    let DecoderNamespace = "XtensaHIFI3";
  }

  class AE_PKSR32_Inst<dag outs, dag ins, string asmstr, list<dag> pattern>
    : XtensaInst24<outs, ins, asmstr, pattern> {
    bits<4> r;
    bits<4> s;
    bits<2> imm;

    let Inst{23-20} = 0x2;
    let Inst{19-16} = 0x7;
    let Inst{15-12} = r;
    let Inst{11-10} = 0x2;
    let Inst{9-8}   = imm;
    let Inst{7-4}   = s;
    let Inst{3-0}   = 0x4;
  }

  def AE_PKSR32_REAL : AE_PKSR32_Inst<(outs AEDR:$r), (ins AEDR:$r_in, AEDR:$s, uimm2:$imm),
                                      "ae_pksr32\t$r, $s, $imm", []> {
    let Constraints = "$r = $r_in";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MUL32_HH_REAL : RRR_Slot3_Inst<0x0e, 0x04, 0x00, (outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                       "ae_mul32.hh\t$r, $s, $t", []> {
    let isCodeGenOnly = 1;
    let Predicates = [HasHIFI3];
  }

  def AE_MULAFD32X16X2_FIR_HL_REAL : XtensaInst64<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c),
                                             "ae_mulafd32x16x2.fir.hl\t$q0, $q1, $d0, $d1, $c", []> {
    let isCodeGenOnly = 1;
    bits<4> t1;
    bits<4> t2;
    bits<4> q0;
    bits<4> q1;
    bits<4> d0;
    bits<4> d1;
    bits<4> c;

    let Inst{3-0}   = t1;
    let Inst{7-4}   = t2;
    let Inst{11-8}  = q0;
    let Inst{15-12} = q1;
    let Inst{19-16} = d0;
    let Inst{23-20} = d1;
    let Inst{27-24} = c;
    let Inst{63-28} = 0x123456789;
  }

  def AE_MULAFD32X16X2_FIR_LH_REAL : XtensaInst64<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c),
                                             "ae_mulafd32x16x2.fir.lh\t$q0, $q1, $d0, $d1, $c", []> {
    let isCodeGenOnly = 1;
    bits<4> t1;
    bits<4> t2;
    bits<4> q0;
    bits<4> q1;
    bits<4> d0;
    bits<4> d1;
    bits<4> c;

    let Inst{3-0}   = t1;
    let Inst{7-4}   = t2;
    let Inst{11-8}  = q0;
    let Inst{15-12} = q1;
    let Inst{19-16} = d0;
    let Inst{23-20} = d1;
    let Inst{27-24} = c;
    let Inst{63-28} = 0x123456789;
  }

  def AE_MULAFD32X16X2_FIR_LL_REAL : XtensaInst64<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c),
                                             "ae_mulafd32x16x2.fir.ll\t$q0, $q1, $d0, $d1, $c", []> {
    let isCodeGenOnly = 1;
    bits<4> t1;
    bits<4> t2;
    bits<4> q0;
    bits<4> q1;
    bits<4> d0;
    bits<4> d1;
    bits<4> c;

    let Inst{3-0}   = t1;
    let Inst{7-4}   = t2;
    let Inst{11-8}  = q0;
    let Inst{15-12} = q1;
    let Inst{19-16} = d0;
    let Inst{23-20} = d1;
    let Inst{27-24} = c;
    let Inst{63-28} = 0x123456789;
  }

  def AE_MUL16X4_REAL : XtensaInst64<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$q0, AEDR:$q1, AEDR:$r, AEDR:$s),
                                     "ae_mul16x4\t$q0, $q1, $r, $s", []> {
    let isCodeGenOnly = 1;
    let Constraints = "$q0 = $t1, $q1 = $t2";
    bits<4> t1;
    bits<4> t2;
    bits<4> q0;
    bits<4> q1;
    bits<4> r;
    bits<4> s;

    let Inst{3-0}   = t1;
    let Inst{7-4}   = t2;
    let Inst{11-8}  = q0;
    let Inst{15-12} = q1;
    let Inst{19-16} = r;
    let Inst{23-20} = s;
    let Inst{63-24} = 0x123456789a;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_EQ16_REAL : XtensaInst64<(outs), (ins BR:$br, AEDR:$s, AEDR:$t),
                                  "ae_eq16\t$br, $s, $t", []> {
    let isCodeGenOnly = 1;
    bits<4> br;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = br;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{63-12} = 0;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_LT16_REAL : XtensaInst64<(outs), (ins BR:$br, AEDR:$s, AEDR:$t),
                                  "ae_lt16\t$br, $s, $t", []> {
    let isCodeGenOnly = 1;
    bits<4> br;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = br;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{63-12} = 0;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_LE16_REAL : XtensaInst64<(outs), (ins BR:$br, AEDR:$s, AEDR:$t),
                                  "ae_le16\t$br, $s, $t", []> {
    let isCodeGenOnly = 1;
    bits<4> br;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = br;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{63-12} = 0;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_EQ32_REAL : XtensaInst24<(outs), (ins BR:$br, AEDR:$s, AEDR:$t),
                                  "ae_eq32\t$br, $s, $t", []> {
    let isCodeGenOnly = 1;
    bits<4> br;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = br;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{23-12} = 0;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_LT32_REAL : XtensaInst24<(outs), (ins BR:$br, AEDR:$s, AEDR:$t),
                                  "ae_lt32\t$br, $s, $t", []> {
    let isCodeGenOnly = 1;
    bits<4> br;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = br;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{23-12} = 0;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_LE32_REAL : XtensaInst64<(outs), (ins BR:$br, AEDR:$s, AEDR:$t),
                                  "ae_le32\t$br, $s, $t", []> {
    let isCodeGenOnly = 1;
    bits<4> br;
    bits<4> s;
    bits<4> t;
    let Inst{3-0} = br;
    let Inst{7-4} = s;
    let Inst{11-8} = t;
    let Inst{63-12} = 0;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULAFP32X16X2RS_H_REAL : RRR_Slot3_Inst<0x01, 0x04, 0x00, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                                "ae_mulafp32x16x2rs.h\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let isCodeGenOnly = 1;
    let Predicates = [HasHIFI3];
  }

  def AE_MULAFP32X16X2RS_L_REAL : RRR_Slot3_Inst<0x02, 0x04, 0x00, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                                "ae_mulafp32x16x2rs.l\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let isCodeGenOnly = 1;
    let Predicates = [HasHIFI3];
  }

  def AE_MULAFP32X2RS_REAL : RRR_Slot3_Inst<0x06, 0x04, 0x00, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                            "ae_mulafp32x2rs\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let isCodeGenOnly = 1;
    let Predicates = [HasHIFI3];
  }

  def AE_ROUND32X2F64SSYM_REAL : XtensaInst64<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                              "ae_round32x2f64ssym\t$t, $s, $r", []> {
    bits<4> t;
    bits<4> s;
    bits<4> r;
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_ROUND32X2F64SASYM_REAL : XtensaInst64<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                               "ae_round32x2f64sasym\t$t, $s, $r", []> {
    bits<4> t;
    bits<4> s;
    bits<4> r;
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_MULAF16SS_00_REAL : RRR_Slot3_Inst<0x0c, 0x02, 0x00, (outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                           "ae_mulaf16ss.00\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULF16SS_00_REAL : RRR_Slot3_Inst<0x09, 0x05, 0x00, (outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                          "ae_mulf16ss.00\t$t, $s, $r", []> {
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_MULAF16SS_11_REAL : XtensaInst64<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                          "ae_mulaf16ss.11\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let isCodeGenOnly = 1;
    bits<4> t; bits<4> s; bits<4> r;
    let Inst{3-0} = t; let Inst{7-4} = s; let Inst{11-8} = r; let Inst{63-12} = 0;
    let DecoderNamespace = "XtensaHIFI3";
  }
  def AE_MULAF16SS_22_REAL : XtensaInst64<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                          "ae_mulaf16ss.22\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let isCodeGenOnly = 1;
    bits<4> t; bits<4> s; bits<4> r;
    let Inst{3-0} = t; let Inst{7-4} = s; let Inst{11-8} = r; let Inst{63-12} = 0;
    let DecoderNamespace = "XtensaHIFI3";
  }
  def AE_MULAF16SS_33_REAL : XtensaInst64<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                          "ae_mulaf16ss.33\t$t, $s, $r", []> {
    let Constraints = "$acc = $t";
    let isCodeGenOnly = 1;
    bits<4> t; bits<4> s; bits<4> r;
    let Inst{3-0} = t; let Inst{7-4} = s; let Inst{11-8} = r; let Inst{63-12} = 0;
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_MULF16SS_11_REAL : XtensaInst64<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                         "ae_mulf16ss.11\t$t, $s, $r", []> {
    let isCodeGenOnly = 1;
    bits<4> t; bits<4> s; bits<4> r;
    let Inst{3-0} = t; let Inst{7-4} = s; let Inst{11-8} = r; let Inst{63-12} = 0;
    let DecoderNamespace = "XtensaHIFI3";
  }
  def AE_MULF16SS_22_REAL : XtensaInst64<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                         "ae_mulf16ss.22\t$t, $s, $r", []> {
    let isCodeGenOnly = 1;
    bits<4> t; bits<4> s; bits<4> r;
    let Inst{3-0} = t; let Inst{7-4} = s; let Inst{11-8} = r; let Inst{63-12} = 0;
    let DecoderNamespace = "XtensaHIFI3";
  }
  def AE_MULF16SS_33_REAL : XtensaInst64<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                         "ae_mulf16ss.33\t$t, $s, $r", []> {
    let isCodeGenOnly = 1;
    bits<4> t; bits<4> s; bits<4> r;
    let Inst{3-0} = t; let Inst{7-4} = s; let Inst{11-8} = r; let Inst{63-12} = 0;
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_LA24_IP_REAL : XtensaInst24<(outs AEDR:$r, VALIGN:$align_out, AR:$s_out),
                                     (ins VALIGN:$align_in, AR:$s),
                                     "ae_la24.ip\t$r, u0, $s", []> {
    bits<4> r;
    bits<4> s;
    bits<1> align_in;

    let Inst{23-20} = 2;
    let Inst{19-16} = 6;
    let Inst{15-12} = 3;
    let Inst{11-8}  = r;
    let Inst{7-6}   = 0;
    let Inst{5}     = align_in;
    let Inst{4}     = 1;
    let Inst{3-0}   = s;

    let Constraints = "$align_in = $align_out, $s = $s_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_LA24X2_IP_REAL : XtensaInst24<(outs AEDR:$r, VALIGN:$align_out, AR:$s_out),
                                       (ins VALIGN:$align_in, AR:$s),
                                       "ae_la24x2.ip\t$r, u0, $s", []> {
    bits<4> r;
    bits<4> s;
    bits<1> align_in;
    
    let Inst{23-20} = 4;
    let Inst{19-16} = 4;
    let Inst{15-12} = r;
    let Inst{11-8}  = s;
    let Inst{7-5}   = 0;
    let Inst{4}     = align_in;
    let Inst{3-0}   = 0xd;
    
    let Constraints = "$s = $s_out, $align_in = $align_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_SA16X4_IP_REAL : XtensaInst24<(outs VALIGN:$align_out, AR:$s_out),
                                       (ins AEDR:$r, VALIGN:$align_in, AR:$s),
                                       "ae_sa16x4.ip\t$r, u1, $s", []> {
    bits<4> r;
    bits<4> s;
    bits<1> align_in;
    bits<1> align_out;

    let Inst{23-20} = 0;
    let Inst{19-16} = 0xc;
    let Inst{15-12} = r;
    let Inst{11-8}  = s;
    let Inst{7-5}   = 0x4;
    let Inst{4}     = align_in;
    let Inst{3-0}   = 0x4;

    let Constraints = "$s = $s_out, $align_in = $align_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_SA16X4_IC_REAL : XtensaInst48<(outs VALIGN:$align_out, AR:$s_out),
                                       (ins AEDR:$r, VALIGN:$align_in, AR:$s),
                                       "ae_sa16x4.ic\t$r, u1, $s", []> {
    bits<4> r;
    bits<4> s;
    bits<1> align_in;
    bits<1> align_out;

    let Constraints = "$s = $s_out, $align_in = $align_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_ROUND24X2F48SSYM_REAL : XtensaInst64<(outs AEDR:$t), (ins AEDR:$s, AEDR:$r),
                                              "ae_round24x2f48ssym\t$t, $s, $r", []> {
    let isCodeGenOnly = 1;
    bits<4> t;
    bits<4> s;
    bits<4> r;
    let DecoderNamespace = "XtensaHIFI3";
  }

  def AE_SA32X2_IP_REAL : XtensaInst24<(outs VALIGN:$align_out, AR:$s_out),
                                       (ins AEDR:$r, VALIGN:$align_in, AR:$s),
                                       "ae_sa32x2.ip\t$r, u1, $s", []> {
    bits<4> r;
    bits<4> s;
    bits<1> align_in;
    bits<1> align_out;

    let Inst{23-20} = 0;
    let Inst{19-16} = 0xc;
    let Inst{15-12} = r;
    let Inst{11-8}  = s;
    let Inst{7-5}   = 0x6;
    let Inst{4}     = align_in;
    let Inst{3-0}   = 0x4;

    let Constraints = "$s = $s_out, $align_in = $align_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_SA32X2_IC_REAL : XtensaInst48<(outs VALIGN:$align_out, AR:$s_out),
                                       (ins AEDR:$r, VALIGN:$align_in, AR:$s),
                                       "ae_sa32x2.ic\t$r, u1, $s", []> {
    bits<4> r;
    bits<4> s;
    bits<1> align_in;
    bits<1> align_out;

    let Constraints = "$s = $s_out, $align_in = $align_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_SA24X2_IP_REAL : XtensaInst48<(outs VALIGN:$align_out, AR:$s_out),
                                       (ins AEDR:$r, VALIGN:$align_in, AR:$s),
                                       "ae_sa24x2.ip\t$r, u1, $s", []> {
    bits<4> r;
    bits<4> s;
    bits<1> align_in;
    bits<1> align_out;

    let Constraints = "$s = $s_out, $align_in = $align_out";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_SA64POS_FP_REAL : XtensaInst24<(outs), (ins VALIGN:$align, AR:$s),
                                        "ae_sa64pos.fp\tu1, $s", []> {
    let isCodeGenOnly = 1;
    bits<4> s;

    let Inst{23-22} = 3;
    let Inst{21-20} = 1; // u1 is hardcoded
    let Inst{19-16} = 4;
    let Inst{15-12} = 3;
    let Inst{11-8}  = s;
    let Inst{7-0}   = 0x77;

    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_LA16X4POS_PC_REAL : XtensaInst48<(outs), (ins AR:$s),
                                          "ae_la16x4pos.pc\tu0, $s", []> {
    let isCodeGenOnly = 1;
    bits<4> s;
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }

  def AE_LA32X2POS_PC_REAL : XtensaInst24<(outs), (ins AR:$s),
                                          "ae_la32x2pos.pc\tu0, $s", []> {
    let isCodeGenOnly = 1;
    bits<4> s;

    let Inst{23-22} = 3;
    let Inst{21-20} = 0; // u0 is hardcoded
    let Inst{19-16} = 4;
    let Inst{15-12} = 1;
    let Inst{11-8}  = s;
    let Inst{7-0}   = 0x77;

    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
  }
}

let Predicates = [HasHIFI5], DecoderNamespace = "XtensaHIFI5" in {

def AE_LALIGN128_I : AE_LALIGN128_I_Inst<(outs), (ins URAlign:$align, AR:$s, uimm4_x16:$imm),
                                         "ae_lalign128.i\t$align, $s, $imm", []>;

def AE_SALIGN128_I : AE_SALIGN128_I_Inst<(outs), (ins URAlign:$align, AR:$s, uimm4_x16:$imm),
                                         "ae_salign128.i\t$align, $s, $imm", []>;

def AE_MOVDRZBVC : AE_MOV_DR_ZB_Inst<(outs), (ins AEDR:$r),
                                     "ae_movdrzbvc\t$r", []> {
  let DecoderNamespace = "XtensaHIFI128_1";
}

def AE_MOVZBVCDR : AE_MOV_DR_ZB_Inst<(outs), (ins AEDR:$r),
                                     "ae_movzbvcdr\t$r", []> {
  let DecoderNamespace = "XtensaHIFI128_2";
}

} // End let Predicates = [HasHIFI5]


