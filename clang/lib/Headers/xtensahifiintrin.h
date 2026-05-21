/*===---- xtensahifiintrin.h - Xtensa HiFi Audio Engine Intrinsics ---------===
 *
 * Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
 * See https://llvm.org/LICENSE.txt for license information.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __XTENSA_HIFI_INTRIN_H
#define __XTENSA_HIFI_INTRIN_H

#ifdef __clang__

#include <stdint.h>

/* All HiFi ae_ types are 64-bit AEDR register values.
 * In Cadence's compiler they are opaque TIE types with C-cast support.
 * We represent them as int64_t since C allows scalar casts between
 * int64_t and int32_t/int16_t, matching Cadence's cast semantics.
 * The builtins use v2i32/v4i16; macros bridge via __ae_v2i32/__ae_v4i16. */
typedef int64_t ae_int32x2;
typedef int64_t ae_int16x4;
typedef int64_t ae_f64;
typedef int32_t ae_int32;
typedef int16_t ae_int16;
typedef int64_t ae_f32;
typedef int64_t ae_f32x2;
typedef int64_t ae_f24x2;
typedef int64_t ae_f16;
typedef int64_t ae_f16x4;
typedef int64_t ae_int64;
typedef int64_t ae_valign;
typedef int64_t ae_q56s;

/* Legacy/alternate names used by SOF */
typedef int64_t ae_p24x2f;
typedef int64_t ae_p16x2s;
typedef int64_t ae_p24f;
typedef int64_t ae_p16s;
typedef int64_t ae_p24x2s;
typedef int64_t ae_int24x2;
typedef int64_t ae_f24;

/* HiFi5 128-bit types — pairs of 64-bit AE register values.
 * These are used by HiFi5 "x2" instructions (ae_l32x2x2, etc.) which
 * operate on two 64-bit values simultaneously. The inline assembly
 * macros decompose operations into pairs of 64-bit register operands. */
typedef struct { ae_int32x2 lo, hi; } ae_int32x4;
typedef struct { ae_int16x4 lo, hi; } ae_int16x8;
typedef struct { ae_valign  lo, hi; } ae_valignx2;

/* Internal vector types for builtin bridging - DO NOT use in user code */
typedef int __attribute__((__vector_size__(8))) __ae_v2i32;
typedef short __attribute__((__vector_size__(8))) __ae_v4i16;

/* Boolean types for comparison results */
typedef unsigned char xtbool;
typedef unsigned char xtbool2;
typedef unsigned char xtbool4;

/* Bridging macros: convert between int64_t (ae_ types) and vector types
 * expected by builtins. These compile to zero-cost bitcasts at IR level. */
#define __TO_V2I32(x)   __builtin_bit_cast(__ae_v2i32, (int64_t)(x))
#define __FROM_V2I32(x) __builtin_bit_cast(int64_t, (__ae_v2i32)(x))
#define __TO_V4I16(x)   __builtin_bit_cast(__ae_v4i16, (int64_t)(x))
#define __FROM_V4I16(x) __builtin_bit_cast(int64_t, (__ae_v4i16)(x))


/* AE_MOVINT macros for SOF compatibility */
#define AE_MOVINT64_FROMINT32X2(a) (a)
#define AE_MOVINT32X2_FROMINT64(a) (a)
/* HiFi Load/Store Intrinsics */
#define AE_L32_I(ptr, offset) (__builtin_xtensa_ae_l32_i((const void *)(ptr), (offset)))
#define AE_L16_I(ptr, offset) (__builtin_xtensa_ae_l16_i((const void *)(ptr), (offset)))
#define AE_L32_X(ptr, offset) (__builtin_xtensa_ae_l32_x((const void *)(ptr), (offset)))
#define AE_L16X4_X3(val, ptr, offset) \
    val = (__builtin_xtensa_ae_l16x4_x((void **)&(ptr), (offset)))
#define AE_L16X4_X(ptr, offset) \
    (__builtin_xtensa_ae_l16x4_x((void *)(ptr), (offset)))

//* Alignment & State Loads */
#define AE_LA64_PP(ptr) \
  ({ __builtin_xtensa_ae_la64_pp((void **)&(ptr)); AE_ZALIGN64(); })
#define AE_LA128_PP(ptr) \
  ({ __builtin_xtensa_ae_la128_pp((void **)&(ptr)); \
     (ae_valignx2){0, 0}; })
#define AE_LA32X2_IP(val, align, ptr) \
    (val) = (__builtin_xtensa_ae_la32x2_ip((align), (void **)&(ptr)))
#define AE_LA32X2_IC(val, align, ptr) \
    (val) = (__builtin_xtensa_ae_la32x2_ip((align), (void **)&(ptr)))
#define AE_LA16X4_IP(val, align, ptr) \
    (val) = (__builtin_xtensa_ae_la16x4_ip((align), (void **)&(ptr)))
#define AE_LA16X4_IC(val, align, ptr) \
    (val) = (__builtin_xtensa_ae_la16x4_ip((align), (void **)&(ptr)))
#define AE_LA24X2_IP(val, align, ptr) \
    (val) = (__builtin_xtensa_ae_la24x2_ip((align), (void **)&(ptr)))
#define AE_LA24_IP(val, align, ptr) \
    (val) = (__builtin_xtensa_ae_la24_ip((align), (void **)&(ptr)))
#define AE_SA24_IP(val, align, ptr) \
    __builtin_xtensa_ae_sa24x2_ip((val), (align), (void **)&(ptr))


#define AE_LA16X4POS_PC(align, ptr) \
  ((void)(align), __builtin_xtensa_ae_la16x4pos_pc((const void *)(ptr)))
#define AE_LA32X2POS_PC(align, ptr) \
  ((void)(align), __builtin_xtensa_ae_la32x2pos_pc((const void *)(ptr)))
#define AE_SA64POS_FP(align, ptr) \
  ((void)(align), __builtin_xtensa_ae_sa64pos_fp((const void *)(ptr)))
#define AE_SA128POS_FP(align, ptr) \
  ((void)(align), __builtin_xtensa_ae_sa128pos_fp((const void *)(ptr)))

/* Post-increment Loads */
#define AE_L32_IP(val, ptr, imm) \
    val = (__builtin_xtensa_ae_l32_ip((void **)&(ptr), (imm)))
#define AE_L16_IP(val, ptr, imm) \
    val = (__builtin_xtensa_ae_l16_ip((void **)&(ptr), (imm)))
#define AE_L16X4_IP(val, ptr, imm) \
    val = (__builtin_xtensa_ae_l16x4_ip((void **)&(ptr), (imm)))
#define AE_L16X4_XP(val, ptr, offset) \
    val = (__builtin_xtensa_ae_l16x4_xp((void **)&(ptr), (offset)))
#define AE_L16_XP(val, ptr, offset) \
    val = (__builtin_xtensa_ae_l16_xp((void **)&(ptr), (offset)))
#define AE_L32_XP(val, ptr, offset) \
    val = (__builtin_xtensa_ae_l32_xp((void **)&(ptr), (offset)))

// Circular Base Loads
#define AE_L16_XC(val, ptr, offset) \
    val = (__builtin_xtensa_ae_l16_xc((void **)&(ptr), (offset)))
#define AE_L32_XC(val, ptr, offset) \
    val = (__builtin_xtensa_ae_l32_xc((void **)&(ptr), (offset)))
#define AE_L32_XC1(val, ptr, offset) \
    val = (__builtin_xtensa_ae_l32_xc1((void **)&(ptr), (offset)))
#define AE_L32X2_XC1(val, ptr, offset) \
    val = (__builtin_xtensa_ae_l32x2_xc1((void **)&(ptr), (offset)))

// Audio Engine Stores
#define AE_S32_L_I(val, ptr, imm) \
    __builtin_xtensa_ae_s32_l_i(((val)), (void *)(ptr), (imm))
#define AE_S32X2_I(val, ptr, imm) \
    __builtin_xtensa_ae_s32x2_i(((val)), (void *)(ptr), (imm))
#define AE_S16X2M_I(val, ptr, imm) \
    __builtin_xtensa_ae_s16x2m_i(((val)), (void *)(ptr), (imm))
#define AE_S16_I(val, ptr, imm) \
    __builtin_xtensa_ae_s16_i(((val)), (void *)(ptr), (imm))
#define AE_S32_L_X(val, ptr, offset) \
    __builtin_xtensa_ae_s32_l_x(((val)), (void *)(ptr), (offset))
#define AE_S32X2_X(val, ptr, offset) \
    __builtin_xtensa_ae_s32x2_x(((val)), (void *)(ptr), (offset))
#define AE_S32_H_I(val, ptr, imm) \
    __builtin_xtensa_ae_s32_h_i(((val)), (void *)(ptr), (imm))
#define AE_S16_0_X(val, ptr, offset) \
    __builtin_xtensa_ae_s16_0_x(((val)), (void *)(ptr), (offset))

// Post-increment Stores
#define AE_S32_L_IP(val, ptr, imm) \
    __builtin_xtensa_ae_s32_l_ip((val), (void **)&(ptr), (imm))
#define AE_S32X2_IP(val, ptr, imm) \
    __builtin_xtensa_ae_s32x2_ip((val), (void **)&(ptr), (imm))
#define AE_S16_0_IP(val, ptr, imm) \
    __builtin_xtensa_ae_s16_0_ip((val), (void **)&(ptr), (imm))
#define AE_S16X4_IP(val, ptr, imm) \
    __builtin_xtensa_ae_s16x4_ip((val), (void **)&(ptr), (imm))
#define AE_S16_0_XP(val, ptr, offset) \
    __builtin_xtensa_ae_s16_0_xp((val), (void **)&(ptr), (offset))
#define AE_S32_L_XP(val, ptr, offset) \
    __builtin_xtensa_ae_s32_l_xp((val), (void **)&(ptr), (offset))

// Circular Base Stores
#define AE_S16_0_XC(val, ptr, offset) \
    __builtin_xtensa_ae_s16_0_xc((val), (void **)&(ptr), (offset))
#define AE_S16_0_XC1(val, ptr, offset) \
    __builtin_xtensa_ae_s16_0_xc1((val), (void **)&(ptr), (offset))
#define AE_S32_L_XC(val, ptr, offset) \
    __builtin_xtensa_ae_s32_l_xc((val), (void **)&(ptr), (offset))
#define AE_S32_L_XC1(val, ptr, offset) \
    __builtin_xtensa_ae_s32_l_xc1((val), (void **)&(ptr), (offset))
#define AE_S32X2_XC(val, ptr, offset) \
    __builtin_xtensa_ae_s32x2_xc((val), (void **)&(ptr), (offset))
#define AE_S32X2_XC1(val, ptr, offset) \
    __builtin_xtensa_ae_s32x2_xc1((val), (void **)&(ptr), (offset))
#define AE_SA16X4_IP(val, align, ptr) \
    __builtin_xtensa_ae_sa16x4_ip((val), (align), (void **)&(ptr))
#define AE_SA16X4_IC(val, align, ptr) \
    __builtin_xtensa_ae_sa16x4_ic((val), (align), (void **)&(ptr))
#define AE_SA32X2_IP(val, align, ptr) \
    __builtin_xtensa_ae_sa32x2_ip((val), (align), (void **)&(ptr))
#define AE_SA32X2_IC(val, align, ptr) \
    __builtin_xtensa_ae_sa32x2_ic((val), (align), (void **)&(ptr))
#define AE_SA24X2_IP(val, align, ptr) \
    __builtin_xtensa_ae_sa24x2_ip((val), (align), (void **)&(ptr))

// Audio Engine MAC Instructions
#define AE_MULAAFD32X16_H3_L2(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaafd32x16_h3_l2((a), ((b)), ((c)))))
#define AE_MULAAFD32X16_H1_L0(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaafd32x16_h1_l0((a), ((b)), ((c)))))
#define AE_MULAF32R_HH(a, b, c) \
    ((a) = ((__builtin_xtensa_ae_mulaf32r_hh(((a)), ((b)), ((c))))))
#define AE_MULFP32X16X2RAS_H(a, b) \
    (__builtin_xtensa_ae_mulfp32x16x2ras_h(((a)), ((b))))
#define AE_MULFP32X16X2RAS_L(a, b) \
    (__builtin_xtensa_ae_mulfp32x16x2ras_l(((a)), ((b))))
#define AE_MULFP16X4RS(a, b) \
    (__builtin_xtensa_ae_mulfp16x4rs(((a)), ((b))))

#define AE_MULAF32S_LL(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf32s_ll((a), ((b)), ((c)))))
#define AE_MULAF32S_LH(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf32s_lh((a), ((b)), ((c)))))
#define AE_MULF32S_LL(a, b) \
    (__builtin_xtensa_ae_mulf32s_ll(((a)), ((b))))
#define AE_MULF32S_LH(a, b) \
    (__builtin_xtensa_ae_mulf32s_lh(((a)), ((b))))
#define AE_MULF32S_HL(a, b) \
    (__builtin_xtensa_ae_mulf32s_lh((b), (a)))
#define AE_MULAF32S_HL(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf32s_lh((a), (c), (b))))

#define AE_MULAF32R_LL(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf32r_ll((a), ((b)), ((c)))))
#define AE_MULAF32R_LH(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf32r_lh((a), ((b)), ((c)))))
#define AE_MULAF32S_HH(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf32s_hh((a), ((b)), ((c)))))
#define AE_MULAF32RA_LL(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf32ra_ll((a), ((b)), ((c)))))
#define AE_MULAF32RA_LH(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf32ra_lh((a), ((b)), ((c)))))
#define AE_MULAF32RA_HH(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf32ra_hh((a), ((b)), ((c)))))
#define AE_MULSF32S_LL(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulsf32s_ll((a), ((b)), ((c)))))
#define AE_MULSF32S_LH(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulsf32s_lh((a), ((b)), ((c)))))
#define AE_MULSF32S_HH(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulsf32s_hh((a), ((b)), ((c)))))
#define AE_MULF32R_LL(a, b) \
    (__builtin_xtensa_ae_mulf32r_ll(((a)), ((b))))
#define AE_MULF32S_HH(a, b) \
    (__builtin_xtensa_ae_mulf32s_hh(((a)), ((b))))
#define AE_MULF32R_LH(a, b) \
    (__builtin_xtensa_ae_mulf32r_lh(((a)), ((b))))
#define AE_MULF32R_HH(a, b) \
    (__builtin_xtensa_ae_mulf32r_hh(((a)), ((b))))
#define AE_MULF32RA_LL(a, b) \
    (__builtin_xtensa_ae_mulf32ra_ll(((a)), ((b))))
#define AE_MULF32RA_LH(a, b) \
    (__builtin_xtensa_ae_mulf32ra_lh(((a)), ((b))))
#define AE_MULF32RA_HH(a, b) \
    (__builtin_xtensa_ae_mulf32ra_hh(((a)), ((b))))

#define AE_MULAF16SS_00(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf16ss_00((a), ((b)), ((c)))))
#define AE_MULAF16SS_11(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf16ss_11((a), ((b)), ((c)))))
#define AE_MULAF16SS_22(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf16ss_22((a), ((b)), ((c)))))
#define AE_MULAF16SS_33(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaf16ss_33((a), ((b)), ((c)))))
#define AE_MULF16SS_00(a, b) \
    (__builtin_xtensa_ae_mulf16ss_00(((a)), ((b))))
#define AE_MULF16SS_11(a, b) \
    (__builtin_xtensa_ae_mulf16ss_11(((a)), ((b))))
#define AE_MULF16SS_22(a, b) \
    (__builtin_xtensa_ae_mulf16ss_22(((a)), ((b))))
#define AE_MULF16SS_33(a, b) \
    (__builtin_xtensa_ae_mulf16ss_33(((a)), ((b))))

#define AE_MUL32_HH(a, b) \
    (__builtin_xtensa_ae_mul32_hh(((a)), ((b))))
#define AE_MULAAAAFQ32X16(a, b, c, d) \
    ((a) = (__builtin_xtensa_ae_mulaaaafq32x16((a), ((b)), ((c)), ((d)))))
#define AE_MULAAF2D32RA_HH_LL(a, b, c, d, e, f) \
    __builtin_xtensa_ae_mulaaf2d32ra_hh_ll(&(a), &(b), (c), (d), (e), (f))
#define AE_MULAAFD24_HH_LL(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaafd24_hh_ll((a), ((b)), ((c)))))
#define AE_MULAAFD32RA_HH_LL(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaafd32ra_hh_ll((a), ((b)), ((c)))))
#define AE_MULAAFP24S_HH_LL(a, b, c) \
    ((a) = (__builtin_xtensa_ae_mulaafp24s_hh_ll((a), ((b)), ((c)))))

/* MULFP MACs */
#define AE_MULFP16X4S(a, b) (__builtin_xtensa_ae_mulfp16x4s(((a)), ((b))))
#define AE_MULFP16X4RAS(a, b) (__builtin_xtensa_ae_mulfp16x4ras(((a)), ((b))))
#define AE_MULFP32X16X2RS_H(a, b) (__builtin_xtensa_ae_mulfp32x16x2rs_h(((a)), ((b))))
#define AE_MULFP32X16X2RS_L(a, b) (__builtin_xtensa_ae_mulfp32x16x2rs_l(((a)), ((b))))
#define AE_MULFP32X16X2RAS_H(a, b) (__builtin_xtensa_ae_mulfp32x16x2ras_h((a), (b)))
#define AE_MULFP32X16X2RAS_L(a, b) (__builtin_xtensa_ae_mulfp32x16x2ras_l((a), (b)))
#define AE_MULFP32X2RS(a, b) (__builtin_xtensa_ae_mulfp32x2rs(((a)), ((b))))
#define AE_MULFP24X2R(a, b) (__builtin_xtensa_ae_mulfp24x2r(((a)), ((b))))

/* FIR Pseudo Multi-return MACs */
#define AE_MULAFD32X16X2_FIR_HH(b, a, d0, d1, coefs) \
  __builtin_xtensa_ae_mulafd32x16x2_fir_hh((ae_f32x2 *)&(b), (ae_f32x2 *)&(a), (d0), (d1), (coefs))

#define AE_MULAFD32X16X2_FIR_HL(b, a, d0, d1, coefs) \
  __builtin_xtensa_ae_mulafd32x16x2_fir_hl((ae_f32x2 *)&(b), (ae_f32x2 *)&(a), (d0), (d1), (coefs))

#define AE_MULAFD32X16X2_FIR_LH(b, a, d0, d1, coefs) \
  __builtin_xtensa_ae_mulafd32x16x2_fir_lh((ae_f32x2 *)&(b), (ae_f32x2 *)&(a), (d0), (d1), (coefs))

#define AE_MULAFD32X16X2_FIR_LL(b, a, d0, d1, coefs) \
  __builtin_xtensa_ae_mulafd32x16x2_fir_ll((ae_f32x2 *)&(b), (ae_f32x2 *)&(a), (d0), (d1), (coefs))

#define AE_MUL16X4(d32_1, d32_2, d16_1, gains) \
  __builtin_xtensa_ae_mul16x4(&(d32_1), &(d32_2), (d16_1), (gains))

/* AE_MULA2Q32X16_FIR_H: supports both HiFi3 (3-arg) and HiFi5 (6-arg) variants */
#define __AE_MULA2Q_3(a, b, c) \
  __builtin_xtensa_ae_mula2q32x16_fir_h(&(a), (b), (c))

#define __AE_MULA2Q_6(out0, out1, d0, d1, d2, coefs) \
  __builtin_xtensa_ae_mula2q32x16_fir_h5(&(out0), &(out1), (d0), (d1), (d2), (coefs))

#define __AE_MULA2Q_SEL(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
#define AE_MULA2Q32X16_FIR_H(...) \
  __AE_MULA2Q_SEL(__VA_ARGS__, __AE_MULA2Q_6, _5, _4, __AE_MULA2Q_3)(__VA_ARGS__)
#define AE_MUL32_LL(a, b) (__builtin_xtensa_ae_mul32_ll(((a)), ((b))))
#define AE_MUL32_LH(a, b) (__builtin_xtensa_ae_mul32_lh(((a)), ((b))))
#define AE_MUL32_HH(a, b) __builtin_xtensa_ae_mul32_hh((a), (b))
#define AE_MULA32_LL(a, b, c) ((a) = (__builtin_xtensa_ae_mula32_ll(((a)), ((b)), ((c)))))
#define AE_MULA32_LH(a, b, c) ((a) = (__builtin_xtensa_ae_mula32_lh(((a)), ((b)), ((c)))))
#define AE_MULA32_HH(a, b, c) ((a) = (__builtin_xtensa_ae_mula32_hh(((a)), ((b)), ((c)))))
#define AE_MULS32_LL(a, b, c) ((a) = (__builtin_xtensa_ae_muls32_ll(((a)), ((b)), ((c)))))
#define AE_MULS32_LH(a, b, c) ((a) = (__builtin_xtensa_ae_muls32_lh(((a)), ((b)), ((c)))))
#define AE_MULS32_HH(a, b, c) ((a) = (__builtin_xtensa_ae_muls32_hh(((a)), ((b)), ((c)))))
#define AE_MULSF32R_LL(a, b, c) ((a) = (__builtin_xtensa_ae_mulsf32r_ll(((a)), ((b)), ((c)))))
#define AE_MULSF32R_LH(a, b, c) ((a) = (__builtin_xtensa_ae_mulsf32r_lh(((a)), ((b)), ((c)))))
#define AE_MULSF32R_HH(a, b, c) ((a) = (__builtin_xtensa_ae_mulsf32r_hh(((a)), ((b)), ((c)))))
#define AE_MULSF32RA_LL(a, b, c) ((a) = (__builtin_xtensa_ae_mulsf32ra_ll(((a)), ((b)), ((c)))))
#define AE_MULSF32RA_LH(a, b, c) ((a) = (__builtin_xtensa_ae_mulsf32ra_lh(((a)), ((b)), ((c)))))
#define AE_MULSF32RA_HH(a, b, c) ((a) = (__builtin_xtensa_ae_mulsf32ra_hh(((a)), ((b)), ((c)))))
#define AE_MULAFP32X2RS(a, b, c) ((a) = (__builtin_xtensa_ae_mulafp32x2rs(((a)), ((b)), ((c)))))
#define AE_MULAFP32X16X2RS_H(a, b, c) ((a) = (__builtin_xtensa_ae_mulafp32x16x2rs_h(((a)), ((b)), ((c)))))
#define AE_MULAFP32X16X2RS_L(a, b, c) ((a) = (__builtin_xtensa_ae_mulafp32x16x2rs_l(((a)), ((b)), ((c)))))
#define AE_MULFC24RA(a, b) (__builtin_xtensa_ae_mulfc24ra(((a)), ((b))))
#define AE_MUL16(a, b) (__builtin_xtensa_ae_mul16(((a)), ((b))))
#define AE_SRAI32R(a, b) (__builtin_xtensa_ae_srai32r(((a)), (b)))
#define AE_SLAI32S(a, b) (__builtin_xtensa_ae_slai32s(((a)), (b)))
#define AE_SLAI32(a, b) (__builtin_xtensa_ae_slai32(((a)), (b)))
#define AE_ROUND32F48SSYM(a) (__builtin_xtensa_ae_round32x2f48ssym(((a)), ((a))))
#define AE_ADD16(a, b) (__builtin_xtensa_ae_add16(((a)), ((b))))
#define AE_ADD16S(a, b) (__builtin_xtensa_ae_add16s(((a)), ((b))))
#define AE_ADD24S(a, b) (__builtin_xtensa_ae_add24s(((a)), ((b))))
#define AE_ADD32(a, b) (__builtin_xtensa_ae_add32(((a)), ((b))))
#define AE_ADD32S(a, b) (__builtin_xtensa_ae_add32s(((a)), ((b))))
#define AE_ADD32_HL_LH(a, b) (__builtin_xtensa_ae_add32_hl_lh(((a)), ((b))))
#define AE_ADD64(a, b) (__builtin_xtensa_ae_add64((a), (b)))
#define AE_ADD64S(a, b) (__builtin_xtensa_ae_add64s((a), (b)))
#define AE_SUB16(a, b) (__builtin_xtensa_ae_sub16(((a)), ((b))))
#define AE_SUB16S(a, b) (__builtin_xtensa_ae_sub16s(((a)), ((b))))
#define AE_SUB24S(a, b) (__builtin_xtensa_ae_sub24s(((a)), ((b))))
#define AE_SUB32(a, b) (__builtin_xtensa_ae_sub32(((a)), ((b))))
#define AE_SUB32S(a, b) (__builtin_xtensa_ae_sub32s(((a)), ((b))))
#define AE_SUB64(a, b) (__builtin_xtensa_ae_sub64((a), (b)))

#define AE_MAX32(a, b) (__builtin_xtensa_ae_max32(((a)), ((b))))
#define AE_MIN32(a, b) (__builtin_xtensa_ae_min32(((a)), ((b))))
#define AE_MIN_32_signed(a, b) (__builtin_xtensa_ae_min32((a), (b)))
#define AE_ABS16S(a) (__builtin_xtensa_ae_abs16s(((a))))
#define AE_ABS32S(a) (__builtin_xtensa_ae_abs32s(((a))))
#define AE_MAXABS32S(a, b) (__builtin_xtensa_ae_maxabs32s(((a)), ((b))))
#define AE_NEG16S(a) (__builtin_xtensa_ae_neg16s(((a))))
#define AE_NEG32S(a) (__builtin_xtensa_ae_neg32s(((a))))

#define AE_AND16(a, b) (__builtin_xtensa_ae_and16(((a)), ((b))))
#define AE_OR16(a, b) (__builtin_xtensa_ae_or16(((a)), ((b))))
#define AE_AND64(a, b) (__builtin_xtensa_ae_and64((a), (b)))
#define AE_OR64(a, b) (__builtin_xtensa_ae_or64((a), (b)))
#define AE_XOR64(a, b) (__builtin_xtensa_ae_xor64((a), (b)))
#define AE_XOR16(a, b) (__builtin_xtensa_ae_xor16(((a)), ((b))))
#define AE_ZERO16() (__builtin_xtensa_ae_zero16())
#define AE_ZERO24() (__builtin_xtensa_ae_zero24())
#define AE_ZERO32() (__builtin_xtensa_ae_zero32())
#define AE_ZEROP48() (__builtin_xtensa_ae_zerop48())
#define AE_ZERO64() (__builtin_xtensa_ae_zero64())
#define AE_ZEROQ56() (__builtin_xtensa_ae_zeroq56())

#define AE_CBEGIN0() (void*)__builtin_xtensa_ae_cbegin0()
#define AE_CBEGIN1() (void*)__builtin_xtensa_ae_cbegin1()
#define AE_CEND0() (void*)__builtin_xtensa_ae_cend0()
#define AE_CEND1() (void*)__builtin_xtensa_ae_cend1()
#define AE_SETCBEGIN0(a) __builtin_xtensa_ae_setcbegin0((int)(a))
#define AE_SETCBEGIN1(a) __builtin_xtensa_ae_setcbegin1((int)(a))
#define AE_SETCEND0(a) __builtin_xtensa_ae_setcend0((int)(a))
#define AE_SETCEND1(a) __builtin_xtensa_ae_setcend1((int)(a))

#define AE_L32X2_IP(val, ptr, offset) \
    val = (__builtin_xtensa_ae_l32x2_ip((void **)&(ptr), (offset)))
#define AE_L32X2_I(ptr, offset) (__builtin_xtensa_ae_l32x2_i((void *)(ptr), (offset)))
#define AE_L16M_X(ptr, offset_ptr) (__builtin_xtensa_ae_l16m_x((const void *)(ptr), (offset_ptr)))
#define AE_L16M_XU(val, ptr, offset_ptr) \
    val = (__builtin_xtensa_ae_l16m_xu((void **)&(ptr), (offset_ptr)))
#define AE_L32M_X(ptr, offset_ptr) (__builtin_xtensa_ae_l32m_x((const void *)(ptr), (offset_ptr)))
#define AE_L32F24_XC(val, ptr, offset_ptr) \
    val = (__builtin_xtensa_ae_l32f24_xc((void **)&(ptr), (offset_ptr)))
#define AE_L32X2F24_XC(val, ptr, offset_ptr) \
    val = (__builtin_xtensa_ae_l32x2f24_xc((void **)&(ptr), (offset_ptr)))
#define AE_L32X2F24_IP(val, ptr, offset) \
    val = (__builtin_xtensa_ae_l32x2f24_ip((void **)&(ptr), (offset)))
#define AE_L32X2_XC(val, ptr, offset_ptr) \
    val = (__builtin_xtensa_ae_l32x2_xc((void **)&(ptr), (offset_ptr)))
// Conversions & Moves
#define AE_MOVDA32X2(a, b) (__builtin_xtensa_ae_movda32x2((a), (b)))
#define AE_MOVDA32(a) (__builtin_xtensa_ae_movda32((a)))
#define AE_MOVD32X2(a, b) __builtin_xtensa_ae_movd32x2((a), (b))
#define AE_MOVDR32X2(a, b) __builtin_xtensa_ae_movdr32x2((a), (b))
#define AE_MOVDA16X4(a, b) __builtin_xtensa_ae_movda16x4((a), (b))
#define AE_MOVDA16(a) __builtin_xtensa_ae_movda16((a))
#define AE_MOVD16X4(a, b) __builtin_xtensa_ae_movd16x4((a), (b))
#define AE_MOVDR16X4(a, b) __builtin_xtensa_ae_movdr16x4((a), (b))
#define AE_MOVINT(a) __builtin_xtensa_ae_movint((a))
#define AE_MOVINT16X4(a) __builtin_xtensa_ae_movint16x4((a))
#define AE_MOVINT32X2(a) __builtin_xtensa_ae_movint32x2((a))
#define AE_MOVINT16X4_FROMINT(a) __builtin_xtensa_ae_movint16x4_fromint((a))
#define AE_MOVINT32X2_FROMINT(a) __builtin_xtensa_ae_movint32x2_fromint((a))
#define AE_MOVAD32_H(a) __builtin_xtensa_ae_movad32_h(((a)))

#define AE_CVT16X4F32(a, b) __builtin_xtensa_ae_cvt16x4f32((a), (b))
#define AE_CVT32X2F16_10(a) (__builtin_xtensa_ae_cvt32x2f16_10(((a))))
#define AE_CVT32X2F16X4(a) __builtin_xtensa_ae_cvt32x2f16x4((a))
#define AE_CVT32X2F24(a) __builtin_xtensa_ae_cvt32x2f24((a))
#define AE_CVT32X2F32S(a) __builtin_xtensa_ae_cvt32x2f32s((a))
#define AE_CVTF32S32X2(a) __builtin_xtensa_ae_cvtf32s32x2((a))
#define AE_CVTF2432X2(a) __builtin_xtensa_ae_cvtf2432x2((a))

#define AE_ROUND16X4F32S(a, b) __builtin_xtensa_ae_round16x4f32s((a), (b))
#define AE_ROUND16X4F32R(a, b) __builtin_xtensa_ae_round16x4f32r((a), (b))
#define AE_ROUND32X2F64R(a, b) __builtin_xtensa_ae_round32x2f64r((a), (b))
#define AE_ROUND32X2F64S(a, b) __builtin_xtensa_ae_round32x2f64s((a), (b))

/* Rounding & Selects from TODO */
#define AE_PKSR32(d, s, imm) ((d) = (__builtin_xtensa_ae_pksr32((d), (s), (imm))))
#define AE_ROUND16X4F32SASYM(a, b) (__builtin_xtensa_ae_round16x4f32sasym(((a)), ((b))))
#define AE_ROUND16X4F32SSYM(a, b) (__builtin_xtensa_ae_round16x4f32ssym(((a)), ((b))))
#define AE_ROUND24X2F48SASYM(a, b) (__builtin_xtensa_ae_round24x2f48sasym((a), (b)))
#define AE_ROUND24X2F48SSYM(a, b) (__builtin_xtensa_ae_round24x2f48ssym((a), (b)))
#define AE_ROUND32X2F64SASYM(a, b) (__builtin_xtensa_ae_round32x2f64sasym((a), (b)))
#define AE_ROUND32X2F64SSYM(a, b) (__builtin_xtensa_ae_round32x2f64ssym((a), (b)))
#define AE_ROUNDSP16Q48X2ASYM(a, b) (__builtin_xtensa_ae_roundsp16q48x2asym((a), (b)))
#define AE_ROUNDSP16Q48X2SYM(a, b) (__builtin_xtensa_ae_roundsp16q48x2sym((a), (b)))
#define AE_ROUNDSP16F24ASYM(a) (__builtin_xtensa_ae_roundsp16f24asym((a)))
#define AE_ROUNDSP16F24SYM(a) (__builtin_xtensa_ae_roundsp16f24sym((a)))
#define AE_ROUNDSQ32F48ASYM(a) (__builtin_xtensa_ae_roundsq32f48asym((a)))
#define AE_ROUNDSQ32F48SYM(a) (__builtin_xtensa_ae_roundsq32f48sym((a)))
#define AE_SAT16X4(a, b) (__builtin_xtensa_ae_sat16x4(((a)), ((b))))
#define AE_SAT24S(a) (__builtin_xtensa_ae_sat24s(((a))))

#define AE_SEL32_HH(a, b) (__builtin_xtensa_ae_sel32_hh(((a)), ((b))))
#define AE_SEL32_HL(a, b) (__builtin_xtensa_ae_sel32_hl(((a)), ((b))))
#define AE_SEL32_LH(a, b) (__builtin_xtensa_ae_sel32_lh(((a)), ((b))))
#define AE_SEL32_LL(a, b) (__builtin_xtensa_ae_sel32_ll(((a)), ((b))))

#define AE_SELP24_HH(a, b) (__builtin_xtensa_ae_selp24_hh(((a)), ((b))))
#define AE_SELP24_LH(a, b) (__builtin_xtensa_ae_selp24_lh(((a)), ((b))))
#define AE_SELP24_LL(a, b) (__builtin_xtensa_ae_sel32_ll((a), (b)))
#define AE_TRUNC16X4F32(a, b) (__builtin_xtensa_ae_trunc16x4f32(((a)), ((b))))
#define AE_TRUNC32X2F64(a, b) __builtin_xtensa_ae_trunc32x2f64((a), (b))

// Conversions & Moves (gap fills)
#define AE_CVT16X4(a, b)         (__builtin_xtensa_ae_cvt16x4(((a)), ((b))))
#define AE_CVT32X2F16_32(a)      (__builtin_xtensa_ae_cvt32x2f16_32(((a))))
#define AE_CVT48A32(a)           (__builtin_xtensa_ae_cvt48a32((a)))
#define AE_CVTP24A16X2_LL(a, b)  (__builtin_xtensa_ae_cvtp24a16x2_ll((a), (b)))
#define AE_CVTQ48A32S(a)         (__builtin_xtensa_ae_cvtq48a32s((a)))
#define AE_MOVAD16_0(a)          __builtin_xtensa_ae_movad16_0(((a)))
#define AE_MOVAD16_1(a)          __builtin_xtensa_ae_movad16_1(((a)))
#define AE_MOVAD16_2(a)          __builtin_xtensa_ae_movad16_2(((a)))
#define AE_MOVAD16_3(a)          __builtin_xtensa_ae_movad16_3(((a)))
#define AE_MOVAD32_L(a)          __builtin_xtensa_ae_movad32_l(((a)))
#define AE_MOVF16X4_FROMINT64(a) (a)
#define AE_MOVINT64_FROMF16X4(a) ((a)
#define AE_MOVF24X2_FROMINT32X2(a) (__builtin_xtensa_ae_movf24x2_fromint32x2(((a))))
#define AE_MOVINT16X4_FROMINT32X2(a) (__builtin_xtensa_ae_movint16x4_fromint32x2(((a))))
#define AE_MOVINT24X2_FROMF24X2(a) (__builtin_xtensa_ae_movint24x2_fromf24x2(((a))))
#define AE_MOVINT24X2_FROMF32X2(a) (__builtin_xtensa_ae_movint24x2_fromf32x2(((a))))
#define AE_MOVINT32_FROMINT16(a) __builtin_xtensa_ae_movint32_fromint16((a))
#define AE_MOVINT32_FROMINT24X2(a) __builtin_xtensa_ae_movint32_fromint24x2(((a)))
#define AE_MOVINT32_FROMINT64(a) ((a))
#define AE_SEXT32X2D16_10(a)     (__builtin_xtensa_ae_sext32x2d16_10(((a))))
#define AE_SEXT32X2D16_32(a)     (__builtin_xtensa_ae_sext32x2d16_32(((a))))

// Rounding (gap fills)
#define AE_ROUND32F48SASYM(a)      (__builtin_xtensa_ae_round32x2f48sasym(((a)), ((a))))
#define AE_ROUND32F64SSYM(a)       (__builtin_xtensa_ae_round32x2f64ssym(((a)), ((a))))
#define AE_ROUND32X2F48SASYM(a, b) (__builtin_xtensa_ae_round32x2f48sasym(((a)), ((b))))
#define AE_ROUND32X2F48SSYM(a, b)  (__builtin_xtensa_ae_round32x2f48ssym(((a)), ((b))))
#define AE_ROUNDSP16SYM(a)         (__builtin_xtensa_ae_roundsp16f24sym(((a))))
#define AE_ROUNDSQ32SYM(a)         (__builtin_xtensa_ae_roundsq32f48sym(((a))))

// Shifts (gap fills)
#define AE_SLAA64S(a, b) (__builtin_xtensa_ae_slaa64s(((a)), (b)))
#define AE_SLAI64S(a, b) (__builtin_xtensa_ae_slai64s(((a)), (b)))
#define AE_SRAI64(a, b)  (__builtin_xtensa_ae_srai64(((a)), (b)))


// Shifts
#define AE_SLAA16S(a, b) (__builtin_xtensa_ae_slaa16s(((a)), (b)))
#define AE_SLAA32(a, b) (__builtin_xtensa_ae_slaa32(((a)), (b)))
#define AE_SLAA32S(a, b) (__builtin_xtensa_ae_slaa32s(((a)), (b)))
#define AE_SLAA64(a, b) (__builtin_xtensa_ae_slaa64((a), (b)))

#define AE_SLAI16S(a, b) (__builtin_xtensa_ae_slai16s(((a)), (b)))
#define AE_SLAI24S(a, b) (__builtin_xtensa_ae_slai24s(((a)), (b)))
#define AE_SLAI64(a, b) (__builtin_xtensa_ae_slai64((a), (b)))

#define AE_SRAA16RS(a, b) (__builtin_xtensa_ae_sraa16rs(((a)), (b)))
#define AE_SRAA32(a, b) (__builtin_xtensa_ae_sraa32(((a)), (b)))
#define AE_SRAA32RS(a, b) (__builtin_xtensa_ae_sraa32rs(((a)), (b)))
#define AE_SRAA32S(a, b) (__builtin_xtensa_ae_sraa32s(((a)), (b)))
#define AE_SRAA64(a, b) (__builtin_xtensa_ae_sraa64((a), (b)))
#define AE_SRAAQ56(a, b) (__builtin_xtensa_ae_sraaq56((a), (b)))

// Boolean Comparisons
#define AE_EQ16(a, b) ((xtbool4)__builtin_xtensa_ae_eq16((a), (b)))
#define AE_LT16(a, b) ((xtbool4)__builtin_xtensa_ae_lt16((a), (b)))
#define AE_LE16(a, b) ((xtbool4)__builtin_xtensa_ae_le16((a), (b)))
#define AE_EQ32(a, b) ((xtbool2)__builtin_xtensa_ae_eq32((a), (b)))
#define AE_LT32(a, b) ((xtbool2)__builtin_xtensa_ae_lt32((a), (b)))
#define AE_LE32(a, b) ((xtbool2)__builtin_xtensa_ae_le32((a), (b)))
#define AE_EQ64(a, b) ((xtbool)__builtin_xtensa_ae_eq64((a), (b)))
#define AE_LT64(a, b) ((xtbool)__builtin_xtensa_ae_lt64((a), (b)))
#define AE_LE64(a, b) ((xtbool)__builtin_xtensa_ae_le64((a), (b)))

// Boolean Reductions (operate on packed boolean results)
#define XT_ALL4(b)  (((b) & 0xF) == 0xF)
#define XT_ANY4(b)  (((b) & 0xF) != 0)
#define XT_ALL8(b)  (((b) & 0xFF) == 0xFF)
#define XT_ANY8(b)  (((b) & 0xFF) != 0)

// Alignment Zero
#define AE_ZALIGN64() ((ae_valign)0)
#define AE_ZALIGN128() ((ae_valignx2){0, 0})

// Miscellaneous Arithmetic
#define AE_F32_ADDS_F32(a, b) (__builtin_xtensa_ae_add32s((a), (b)))
#define AE_ADD32S(a, b) (__builtin_xtensa_ae_add32s((a), (b)))
#define AE_NSAZ32_L(a) __builtin_xtensa_ae_nsaz32_l((a))

// Immediate Shifts (32-bit)
#define AE_F32X2_SRAI(a, imm) (__builtin_xtensa_ae_srai32((a), (imm)))
#define AE_SRAI32(a, imm) (__builtin_xtensa_ae_srai32((a), (imm)))
#define AE_F32X2_SLAIS(a, imm) (__builtin_xtensa_ae_slai32s((a), (imm)))
#define AE_SLAI32S(a, imm) (__builtin_xtensa_ae_slai32s((a), (imm)))
#define AE_SLAI32(a, imm) (__builtin_xtensa_ae_slai32((a), (imm)))

// Fractional Multiplies (32-bit lane select)
#define AE_MULF32R_HH(a, b) __builtin_xtensa_ae_mulf32r_hh((a), (b))
#define AE_MULF32R_LH(a, b) __builtin_xtensa_ae_mulf32r_lh((a), (b))
#define AE_MULF32R_LL(a, b) __builtin_xtensa_ae_mulf32r_ll((a), (b))
#define AE_MULF32RA_HH(a, b) __builtin_xtensa_ae_mulf32ra_hh((a), (b))
#define AE_MULF32RA_LH(a, b) __builtin_xtensa_ae_mulf32ra_lh((a), (b))
#define AE_MULF32RA_LL(a, b) __builtin_xtensa_ae_mulf32ra_ll((a), (b))
#define AE_MULF32S_HH(a, b) __builtin_xtensa_ae_mulf32s_hh((a), (b))

// Multiply-subtract (accumulate variants)
#define AE_MULSF32S_LL(acc, a, b) ({ (acc) = __builtin_xtensa_ae_mulsf32s_ll((acc), (a), (b)); })
#define AE_MULSF32S_LH(acc, a, b) ({ (acc) = __builtin_xtensa_ae_mulsf32s_lh((acc), (a), (b)); })
#define AE_MULSF32S_HH(acc, a, b) ({ (acc) = __builtin_xtensa_ae_mulsf32s_hh((acc), (a), (b)); })

// Fractional multiplies (packed)
#define AE_MULFP32X2RS(a, b) (__builtin_xtensa_ae_mulfp32x2rs((a), (b)))
#define AE_MULFP24X2R(a, b) (__builtin_xtensa_ae_mulfp24x2r((a), (b)))
#define AE_MULFP16X4RAS(a, b) (__builtin_xtensa_ae_mulfp16x4ras((a), (b)))
#define AE_MULFP16X4S(a, b) (__builtin_xtensa_ae_mulfp16x4s((a), (b)))
#define AE_MULFP32X16X2RS_H(a, b) (__builtin_xtensa_ae_mulfp32x16x2rs_h((a), (b)))
#define AE_MULFP32X16X2RS_L(a, b) (__builtin_xtensa_ae_mulfp32x16x2rs_l((a), (b)))
#define AE_MULFC24RA(a, b) (__builtin_xtensa_ae_mulfc24ra((a), (b)))

// Multiply-accumulate (fractional packed)
#define AE_MULAFP32X2RS(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_mulafp32x2rs((acc), (a), (b))); })
#define AE_MULAFP32X16X2RS_H(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_mulafp32x16x2rs_h((acc), (a), (b))); })
#define AE_MULAFP32X16X2RS_L(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_mulafp32x16x2rs_l((acc), (a), (b))); })

// Pure multiply: ae_mul32_lh (complete the {hh,lh,ll} family)
#define AE_MUL32_LH(a, b) (__builtin_xtensa_ae_mul32_lh((a), (b)))

// Multiply-accumulate: ae_mula32_{hh,lh,ll}
#define AE_MULA32_HH(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_mula32_hh((acc), (a), (b))); })
#define AE_MULA32_LH(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_mula32_lh((acc), (a), (b))); })
#define AE_MULA32_LL(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_mula32_ll((acc), (a), (b))); })

// Multiply-subtract: ae_muls32_{hh,lh,ll}
#define AE_MULS32_HH(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_muls32_hh((acc), (a), (b))); })
#define AE_MULS32_LH(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_muls32_lh((acc), (a), (b))); })
#define AE_MULS32_LL(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_muls32_ll((acc), (a), (b))); })

// Multiply-subtract fractional rounding: ae_mulsf32r_{hh,lh,ll}
#define AE_MULSF32R_HH(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_mulsf32r_hh((acc), (a), (b))); })
#define AE_MULSF32R_LH(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_mulsf32r_lh((acc), (a), (b))); })
#define AE_MULSF32R_LL(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_mulsf32r_ll((acc), (a), (b))); })

// Multiply-subtract fractional rounding asymmetric: ae_mulsf32ra_{hh,lh,ll}
#define AE_MULSF32RA_HH(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_mulsf32ra_hh((acc), (a), (b))); })
#define AE_MULSF32RA_LH(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_mulsf32ra_lh((acc), (a), (b))); })
#define AE_MULSF32RA_LL(acc, a, b) ({ (acc) = (__builtin_xtensa_ae_mulsf32ra_ll((acc), (a), (b))); })

// Type conversion/reinterpret macros

// -----------------------------------------------------------------------------
// HiFi5 Multi-Output Instructions (via compiler builtins)
// -----------------------------------------------------------------------------
#define AE_MULF32X2R_HH_LL(out0, out1, in0, in1) \
  __builtin_xtensa_ae_mulf32x2r_hh_ll(&(out0), &(out1), (in0), (in1))

#define AE_MULF2P32X16X4RS(out0, out1, in0, in1, in2) \
  __builtin_xtensa_ae_mulf2p32x16x4rs(&(out0), &(out1), (in0), (in1), (in2))

#define AE_MULF2P32X4RS(out0, out1, in0, in1, in2, in3) \
  __builtin_xtensa_ae_mulf2p32x4rs(&(out0), &(out1), (in0), (in1), (in2), (in3))

#define AE_L32X2X2_XC(out0, out1, ptr, offset) \
  __builtin_xtensa_ae_l32x2x2_xc(&(out0), &(out1), (void **)&(ptr), (offset))

#define AE_S32X2X2_XC1(in0, in1, ptr, offset) \
  __builtin_xtensa_ae_s32x2x2_xc1((in0), (in1), (void **)&(ptr), (offset))

#define AE_L32X2X2_XC1(out0, out1, ptr, offset) \
  __builtin_xtensa_ae_l32x2x2_xc1(&(out0), &(out1), (void **)&(ptr), (offset))

#define AE_LA16X4X2_IP(out0, out1, align, ptr) \
  __builtin_xtensa_ae_la16x4x2_ip(&(out0), &(out1), (ae_valign)(align).lo, (void **)&(ptr))

#define AE_LA32X2X2_IP(out0, out1, align, ptr) \
  __builtin_xtensa_ae_la32x2x2_ip(&(out0), &(out1), (ae_valign)(align).lo, (void **)&(ptr))

#define AE_SA16X4X2_IP(in0, in1, align, ptr) \
  __builtin_xtensa_ae_sa16x4x2_ip((in0), (in1), (ae_valign)(align).lo, (void **)&(ptr))

#define AE_SA32X2X2_IP(in0, in1, align, ptr) \
  __builtin_xtensa_ae_sa32x2x2_ip((in0), (in1), (ae_valign)(align).lo, (void **)&(ptr))

// ============================================================
// Missing HiFi3 intrinsics (from xt-clang reference analysis)
// ============================================================

// Select 16-bit elements with immediate index (0-7)
#define AE_SEL16I(a, b, sel) ((ae_int16x4)__builtin_xtensa_ae_sel16i((a), (b), (sel)))

// 64-bit load/store with immediate offset
#define AE_L64_I(ptr, ofs) ((ae_int64)__builtin_xtensa_ae_l64_i((const void *)(ptr), (ofs)))
#define AE_S64_I(val, ptr, ofs) __builtin_xtensa_ae_s64_i((val), (void *)(ptr), (ofs))
// Post-increment variants use .i form (compiler optimizes these)
#define AE_L64_IP(val, ptr, ofs) \
  do { \
    (val) = AE_L64_I((ptr), 0); \
    (ptr) = (__typeof__(ptr))((char *)(ptr) + (ofs)); \
  } while(0)
#define AE_L64_XP(val, ptr, ofs) \
  do { \
    (val) = AE_L64_I((ptr), 0); \
    (ptr) = (__typeof__(ptr))((char *)(ptr) + (ofs)); \
  } while(0)
#define AE_S64_IP(val, ptr, ofs) \
  do { \
    AE_S64_I((val), (ptr), 0); \
    (ptr) = (__typeof__(ptr))((char *)(ptr) + (ofs)); \
  } while(0)

// Initialize store alignment register
#define AE_ZALIGN64() ((ae_valign)__builtin_xtensa_ae_zalign64())

// Simultaneous saturating add and subtract
#define AE_ADDANDSUB32S(sum, diff, a, b) \
  __builtin_xtensa_ae_addandsub32s(&(sum), &(diff), (a), (b))

// Load 16x2 with register post-increment
#define AE_L16X2M_XU(val, ptr, step) \
  do { \
    void *__p = (void *)(ptr); \
    int64_t __v = __builtin_xtensa_ae_l16x2m_xu(&__p, (int)(step)); \
    (val) = (ae_int32x2)__v; \
    (ptr) = (__typeof__(ptr))__p; \
  } while(0)

// Move AR to AE_DR (16-bit replicate)
#define AE_MOVDA16(a) ((ae_int16x4)__builtin_xtensa_ae_movda16((a)))

// Store 32x2 in f24 format
#define AE_S32X2F24_I(val, ptr, ofs) \
  __builtin_xtensa_ae_s32x2f24_i((ae_int32x2)(val), (void *)(ptr), (ofs))
#define AE_S32X2F24_IP(val, ptr, ofs) \
  do { \
    __builtin_xtensa_ae_s32x2f24_i((ae_int32x2)(val), (void *)(ptr), 0); \
    (ptr) = (__typeof__(ptr))((char *)(ptr) + (ofs)); \
  } while(0)

// Scalar AR operations
#define AE_CLAMPS16(a) __builtin_xtensa_ae_clamps16((a))
#define AE_SEXT16(a)   __builtin_xtensa_ae_sext16((a))
#define AE_ZEXT16(a)   __builtin_xtensa_ae_zext16((a))

// Move AE bool2 to AR
#define AE_MOVAB2(a) __builtin_xtensa_ae_movab2((a))

// 24-bit packed stores with index offset.
// ae_p24x2f is stored as 8 bytes (2 x 32-bit words, upper 8 bits unused).
// These are simple indexed 64-bit stores.
#define AE_SP24X2F_X(d, base, ofs) \
  do { *(ae_p24x2f *)((char *)(base) + (ofs)) = (d); } while(0)
#define AE_SP24X2S_X(d, base, ofs) \
  do { *(ae_p24x2s *)((char *)(base) + (ofs)) = (d); } while(0)

#endif /* __clang__ */
#endif
