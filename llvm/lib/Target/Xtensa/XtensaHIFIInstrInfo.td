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
  let hasSideEffects = 1 in {
    def AE_LA64_PP_HIFI4 : RRR_TFC_Inst<0x04, 0x07, 0x07, 0x02, 0x0C, (outs AR:$s_out), (ins AR:$s),
                                    "ae_la64.pp\tu0, $s",
                                    [(set AR:$s_out, (int_xtensa_ae_la64_pp AR:$s))]> {
      let Constraints = "$s = $s_out";
      let DecoderNamespace = "XtensaHIFIX";
      let Predicates = [HasHIFI4];
      let mayLoad = 1;
      let Defs = [AE_VALIGN];
    }
    // HiFi3 variant — same opcode, fires on HiFi3-only targets (e.g. TGL/CAVS2.5)
    def AE_LA64_PP_HIFI3 : RRR_TFC_Inst<0x04, 0x07, 0x07, 0x02, 0x0C, (outs AR:$s_out), (ins AR:$s),
                                    "ae_la64.pp\tu0, $s",
                                    [(set AR:$s_out, (int_xtensa_ae_la64_pp AR:$s))]> {
      let Constraints = "$s = $s_out";
      let DecoderNamespace = "XtensaHIFI3";
      let Predicates = [HasHIFI3, HasNoHIFI4];
      let mayLoad = 1;
      let Defs = [AE_VALIGN];
    }

    def AE_LA32X2_IP_HIFI4 : RRR_TC_Inst<0x04, 0x0D, 0x00, 0x0C, (outs AEDR:$r, AR:$s_out), (ins AR:$s),
                                    "ae_la32x2.ip\t$r, u0, $s",
                                    [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_la32x2_ip AR:$s))]> {
      let Constraints = "$s = $s_out";
      let DecoderNamespace = "XtensaHIFIX";
      let Predicates = [HasHIFI4];
      let mayLoad = 1;
      let Uses = [AE_VALIGN];
    }
    // HiFi3 variant
    def AE_LA32X2_IP_HIFI3 : RRR_TC_Inst<0x04, 0x0D, 0x00, 0x0C, (outs AEDR:$r, AR:$s_out), (ins AR:$s),
                                    "ae_la32x2.ip\t$r, u0, $s",
                                    [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_la32x2_ip AR:$s))]> {
      let Constraints = "$s = $s_out";
      let DecoderNamespace = "XtensaHIFI3";
      let Predicates = [HasHIFI3, HasNoHIFI4];
      let mayLoad = 1;
      let Uses = [AE_VALIGN];
    }

    // AE_LA16X4_IP: Aligned load 4x int16 with post-increment through alignment register
    // Encoding from Cadence assembler: op0=0x04, op1=0x07, op2=0x03, t_val=0x0C
    def AE_LA16X4_IP_HIFI4 : RRR_TC_Inst<0x04, 0x07, 0x03, 0x0C, (outs AEDR:$r, AR:$s_out), (ins AR:$s),
                                    "ae_la16x4.ip\t$r, u0, $s",
                                    [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_la16x4_ip AR:$s))]> {
      let Constraints = "$s = $s_out";
      let DecoderNamespace = "XtensaHIFIX";
      let Predicates = [HasHIFI4];
      let mayLoad = 1;
      let Uses = [AE_VALIGN];
    }
    // HiFi3 variant
    def AE_LA16X4_IP_HIFI3 : RRR_TC_Inst<0x04, 0x07, 0x03, 0x0C, (outs AEDR:$r, AR:$s_out), (ins AR:$s),
                                    "ae_la16x4.ip\t$r, u0, $s",
                                    [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_la16x4_ip AR:$s))]> {
      let Constraints = "$s = $s_out";
      let DecoderNamespace = "XtensaHIFI3";
      let Predicates = [HasHIFI3, HasNoHIFI4];
      let mayLoad = 1;
      let Uses = [AE_VALIGN];
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

class FLIX_MAC_Inst<bits<8> byte0, bits<8> byte2, dag outs, dag ins, string asmstr, list<dag> pattern>
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
  let Inst{15-8}  = 0x83;
  let Inst{7-0}   = byte0;
}

  def AE_MULAFD32X16X2_FIR_HH_HIFI3 : Pseudo<(outs AEDR:$t1, AEDR:$t2), (ins AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c),
                                      "ae_mulafd32x16x2.fir.hh\t$q0, $q1, $d0, $d1, $c",
                                      [(set AEDR:$t1, AEDR:$t2, (int_xtensa_ae_mulafd32x16x2_fir_hh AEDR:$q0, AEDR:$q1, AEDR:$d0, AEDR:$d1, AEDR:$c))]> {
    let Constraints = "$q0 = $t1, $q1 = $t2";
    let DecoderNamespace = "XtensaHIFI3";
    let Predicates = [HasHIFI3];
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
    let Uses = [AE_VALIGN];
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
    let Uses = [AE_VALIGN];
  }

  // AE_SA16X4X2_IP: aligned store 2x ae_int16x4 with post-increment
  // Syntax: ae_sa16x4x2_ip aed0, aed1, u0, a_ptr
  def AE_SA16X4X2_IP_HIFI5 : Pseudo<(outs AR:$ptr_out), (ins AEDR:$t1, AEDR:$t2, AR:$ptr_in),
                                      "ae_sa16x4x2_ip\t$t1, $t2, u0, $ptr_in",
                                      []> {
    let Predicates = [HasHIFI5];
    let Constraints = "$ptr_out = $ptr_in";
    let isCodeGenOnly = 0;
    let mayStore = 1;
    let Defs = [AE_VALIGN];
  }

  // AE_SA32X2X2_IP: aligned store 2x ae_int32x2 with post-increment
  // Syntax: ae_sa32x2x2_ip aed0, aed1, u0, a_ptr
  def AE_SA32X2X2_IP_HIFI5 : Pseudo<(outs AR:$ptr_out), (ins AEDR:$t1, AEDR:$t2, AR:$ptr_in),
                                      "ae_sa32x2x2_ip\t$t1, $t2, u0, $ptr_in",
                                      []> {
    let Predicates = [HasHIFI5];
    let Constraints = "$ptr_out = $ptr_in";
    let isCodeGenOnly = 0;
    let mayStore = 1;
    let Defs = [AE_VALIGN];
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

  // Circular buffer 1 load
  def AE_L32X2_XC1 : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                             "ae_l32x2.xc1\t$r, $s, $t",
                             [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32x2_xc1 AR:$s, AR:$t))]> { let Constraints = "$s = $s_out"; }

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

  def AE_L16X4_X_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$s, AR:$t),
                                "ae_l16x4.x\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_l16x4_x AR:$s, AR:$t))]>;


  def AE_L16_XP_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                "ae_l16.xp\t$r, $s, $t",
                                [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l16_xp AR:$s, AR:$t))]>;
                                
  def AE_L32_XP_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s, AR:$t),
                                "ae_l32.xp\t$r, $s, $t",
                                [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_l32_xp AR:$s, AR:$t))]>;
                                
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
                                 "ae_s32x2.i\t$r, $s, $t",
                                 [(int_xtensa_ae_s32x2_i AEDR:$r, AR:$s, imm8:$t)]>;
  def AE_S16X2M_I_PSEUDO : Pseudo<(outs), (ins AEDR:$r, AR:$s, imm8:$t),
                                 "ae_s16x2m.i\t$r, $s, $t",
                                 [(int_xtensa_ae_s16x2m_i AEDR:$r, AR:$s, imm8:$t)]>;
  def AE_S32X2_IP_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, imm8:$t),
                                 "ae_s32x2.ip\t$r, $s, $t",
                                 [(set AR:$s_out, (int_xtensa_ae_s32x2_ip AEDR:$r, AR:$s, imm8:$t))]> {
    let Constraints = "$s = $s_out";
  }
  def AE_S32X2_X_PSEUDO : Pseudo<(outs), (ins AEDR:$t, AR:$s, AR:$r),
                                 "ae_s32x2.x\t$t, $s, $r",
                                 [(int_xtensa_ae_s32x2_x AEDR:$t, AR:$s, AR:$r)]>;
  def AE_S32X2_XC_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                 "ae_s32x2.xc\t$r, $s, $t",
                                 [(set AR:$s_out, (int_xtensa_ae_s32x2_xc AEDR:$r, AR:$s, AR:$t))]> {
    let Constraints = "$s = $s_out";
  }
  def AE_S32X2_XC1_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s, AR:$t),
                                 "ae_s32x2.xc1\t$r, $s, $t",
                                 [(set AR:$s_out, (int_xtensa_ae_s32x2_xc1 AEDR:$r, AR:$s, AR:$t))]> {
    let Constraints = "$s = $s_out";
  }

  def AE_SA16X4_IP_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s),
                                 "ae_sa16x4.ip\t$r, u0, $s",
                                 [(set AR:$s_out, (int_xtensa_ae_sa16x4_ip AEDR:$r, AR:$s))]> {
    let Constraints = "$s = $s_out";
  }
  def AE_SA16X4_IC_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s),
                                 "ae_sa16x4.ic\t$r, u0, $s",
                                 [(set AR:$s_out, (int_xtensa_ae_sa16x4_ic AEDR:$r, AR:$s))]> {
    let Constraints = "$s = $s_out";
  }
  def AE_SA32X2_IP_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s),
                                 "ae_sa32x2.ip\t$r, u0, $s",
                                 [(set AR:$s_out, (int_xtensa_ae_sa32x2_ip AEDR:$r, AR:$s))]> {
    let Constraints = "$s = $s_out";
  }

  def AE_SA24X2_IP_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s),
                                 "ae_sa24x2.ip\t$r, u0, $s",
                                 [(set AR:$s_out, (int_xtensa_ae_sa24x2_ip AEDR:$r, AR:$s))]> {
    let Constraints = "$s = $s_out";
  }

  def AE_LA24X2_IP_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s),
                                 "ae_la24x2.ip\t$r, u0, $s",
                                 [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_la24x2_ip AR:$s))]>;

  def AE_LA128_PP_PSEUDO : Pseudo<(outs AR:$s_out), (ins AR:$s),
                                 "ae_la128.pp\tu0, $s",
                                 [(set AR:$s_out, (int_xtensa_ae_la128_pp AR:$s))]> {
    let Constraints = "$s = $s_out";
  }

  def AE_LA24_IP_PSEUDO : Pseudo<(outs AEDR:$r, AR:$s_out), (ins AR:$s),
                                 "ae_la24.ip\t$r, u0, $s",
                                 [(set AEDR:$r, AR:$s_out, (int_xtensa_ae_la24_ip AR:$s))]>;

  def AE_LA16X4POS_PC_PSEUDO : Pseudo<(outs), (ins AR:$s),
                                 "ae_la16x4pos.pc\tu0, $s",
                                 [(int_xtensa_ae_la16x4pos_pc AR:$s)]>;

  def AE_LA32X2POS_PC_PSEUDO : Pseudo<(outs), (ins AR:$s),
                                 "ae_la32x2pos.pc\tu0, $s",
                                 [(int_xtensa_ae_la32x2pos_pc AR:$s)]>;

  def AE_SA64POS_FP_PSEUDO : Pseudo<(outs), (ins AR:$s),
                                 "ae_sa64pos.fp\tu0, $s",
                                 [(int_xtensa_ae_sa64pos_fp AR:$s)]>;

  def AE_SA128POS_FP_PSEUDO : Pseudo<(outs), (ins AR:$s),
                                 "ae_sa128pos.fp\tu0, $s",
                                 [(int_xtensa_ae_sa128pos_fp AR:$s)]>;
  def AE_SA32X2_IC_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$r, AR:$s),
                                 "ae_sa32x2.ic\t$r, u0, $s",
                                 [(set AR:$s_out, (int_xtensa_ae_sa32x2_ic AEDR:$r, AR:$s))]> {
    let Constraints = "$s = $s_out";
  }

  // Conversions and Moves Pseudos
  // ae_cvt16x4 is not in the GCC assembler. Use ae_round16x4f32sasym
  // which is the available equivalent (asymmetric rounding truncation).
  def AE_CVT16X4_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_round16x4f32sasym\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_cvt16x4 AEDR:$s, AEDR:$t))]>;
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
  def AE_MOVDA32X2_PSEUDO : Pseudo<(outs AEDR:$r), (ins AR:$s, AR:$t),
                                "ae_movda32x2\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_movda32x2 AR:$s, AR:$t))]>;

  def : Pat<(v2i32 (build_vector AR:$s, AR:$t)),
            (AE_MOVDA32X2_PSEUDO AR:$s, AR:$t)>;

  // scalar_to_vector: put a scalar into a v2i32 (other lane is undef,
  // so we just splat it into both lanes for simplicity)
  def : Pat<(v2i32 (scalar_to_vector AR:$s)),
            (AE_MOVDA32X2_PSEUDO AR:$s, AR:$s)>;

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
  // ae_trunc16x4f32 is not in the GCC assembler. Use ae_round16x4f32ssym
  // which is the closest available equivalent (symmetric rounding truncation).
  def AE_TRUNC16X4F32_PSEUDO : Pseudo<(outs AEDR:$r), (ins AEDR:$s, AEDR:$t),
                                "ae_round16x4f32ssym\t$r, $s, $t",
                                [(set AEDR:$r, (int_xtensa_ae_trunc16x4f32 AEDR:$s, AEDR:$t))]>;

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

  def AE_MULAAFP24S_HH_LL_PSEUDO : Pseudo<(outs AEDR:$t), (ins AEDR:$acc, AEDR:$s, AEDR:$r),
                                          "ae_mulaafp24s.hh.ll\t$t, $s, $r",
                                          [(set AEDR:$t, (int_xtensa_ae_mulaafp24s_hh_ll AEDR:$acc, AEDR:$s, AEDR:$r))]> {
    let Constraints = "$acc = $t";
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

// --- ae_zalign64: initialize store alignment register ---
def AE_ZALIGN64_PSEUDO : Pseudo<(outs AEDR:$r), (ins),
                                "ae_zalign64\tu0",
                                [(set AEDR:$r, (int_xtensa_ae_zalign64))]> {
  let Predicates = [HasHIFI3];
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
                                    "ae_s32x2f24.i\t$d, $a, $off",
                                    [(int_xtensa_ae_s32x2f24_i AEDR:$d, AR:$a, imm:$off)]> {
    let Predicates = [HasHIFI3];
  }
  def AE_S32X2F24_IP_PSEUDO : Pseudo<(outs AR:$a_out), (ins AEDR:$d, AR:$a, i32imm:$off),
                                     "ae_s32x2f24.ip\t$d, $a, $off",
                                     [(set AR:$a_out, (int_xtensa_ae_s32x2f24_ip AEDR:$d, AR:$a, imm:$off))]> {
    let Constraints = "$a = $a_out";
    let Predicates = [HasHIFI3];
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
  let Uses = [AE_VALIGN];
}

def AE_LA16X4X2_IP_PSEUDO : Pseudo<(outs AEDR:$r0, AEDR:$r1, AR:$s_out), (ins AR:$s),
                                   "ae_la16x4x2_ip\t$r0, $r1, u0, $s",
                                   [(set AEDR:$r0, AEDR:$r1, AR:$s_out,
                                         (int_xtensa_ae_la16x4x2_ip AR:$s))]> {
  let Constraints = "$s = $s_out";
  let Predicates = [HasHIFI5];
  let mayLoad = 1;
  let Uses = [AE_VALIGN];
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
                                   "ae_sa32x2x2_ip\t$d0, $d1, u0, $s",
                                   [(set AR:$s_out,
                                         (int_xtensa_ae_sa32x2x2_ip AEDR:$d0, AEDR:$d1, AR:$s))]> {
  let Constraints = "$s = $s_out";
  let Predicates = [HasHIFI5];
  let mayStore = 1;
  let Uses = [AE_VALIGN];
}

def AE_SA16X4X2_IP_PSEUDO : Pseudo<(outs AR:$s_out), (ins AEDR:$d0, AEDR:$d1, AR:$s),
                                   "ae_sa16x4x2_ip\t$d0, $d1, u0, $s",
                                   [(set AR:$s_out,
                                         (int_xtensa_ae_sa16x4x2_ip AEDR:$d0, AEDR:$d1, AR:$s))]> {
  let Constraints = "$s = $s_out";
  let Predicates = [HasHIFI5];
  let mayStore = 1;
  let Uses = [AE_VALIGN];
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

} // End let Predicates = [HasHIFI3]
