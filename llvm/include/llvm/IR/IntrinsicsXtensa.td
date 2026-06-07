//===- IntrinsicsXtensa.td - Defines Xtensa intrinsics ---------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines all of the Xtensa-specific intrinsics.
//
//===----------------------------------------------------------------------===//

let TargetPrefix = "xtensa" in {

  // Audio Engine Loads
  def int_xtensa_ae_l32_i : Intrinsic<[llvm_v2i32_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l16_i : Intrinsic<[llvm_v2i32_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l32_x : Intrinsic<[llvm_v2i32_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;

  // Audio Engine Stores
  def int_xtensa_ae_s32_l_i : Intrinsic<[], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s32x2_i : Intrinsic<[], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly, Align<ArgIndex<1>, 8>]>;
  def int_xtensa_ae_s16x2m_i : Intrinsic<[], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s16_i   : Intrinsic<[], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s32_l_x : Intrinsic<[], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s32x2_x : Intrinsic<[], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly, Align<ArgIndex<1>, 8>]>;
  def int_xtensa_ae_s32_h_i : Intrinsic<[], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s16_0_x : Intrinsic<[], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;

  // Post-increment Loads
  def int_xtensa_ae_l32_ip   : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l16_ip   : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l16x4_ip : Intrinsic<[llvm_v4i16_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly, Align<ArgIndex<0>, 8>]>;
  def int_xtensa_ae_l16x4_x  : Intrinsic<[llvm_v4i16_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly, Align<ArgIndex<0>, 8>]>;
  def int_xtensa_ae_l16x4_xp : Intrinsic<[llvm_v4i16_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly, Align<ArgIndex<0>, 8>]>;
  def int_xtensa_ae_l16_xp   : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l32_xp   : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  
  // Post-increment Stores
  def int_xtensa_ae_s32_l_ip : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s32x2_ip : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly, Align<ArgIndex<1>, 8>]>;
  def int_xtensa_ae_s16_0_ip : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s16x4_ip : Intrinsic<[llvm_ptr_ty], [llvm_v4i16_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly, Align<ArgIndex<1>, 8>]>;
  def int_xtensa_ae_s16_0_xp : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s32_l_xp : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;

  // Alignment & State Loads
  def int_xtensa_ae_la64_pp : Intrinsic<[llvm_ptr_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_la128_pp : Intrinsic<[llvm_ptr_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_la32x2_ip : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_la16x4_ip : Intrinsic<[llvm_v4i16_ty, llvm_ptr_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_la24x2_ip : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_la24_ip : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
  
  def int_xtensa_ae_la16x4pos_pc : Intrinsic<[], [llvm_ptr_ty], []>;
  def int_xtensa_ae_la32x2pos_pc : Intrinsic<[], [llvm_ptr_ty], []>;
  def int_xtensa_ae_sa64pos_fp   : Intrinsic<[], [llvm_ptr_ty], []>;
  def int_xtensa_ae_sa128pos_fp  : Intrinsic<[], [llvm_ptr_ty], []>;

  // Circular Base Loads
  def int_xtensa_ae_l16_xc   : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l32_xc   : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l32_xc1  : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;

  def int_xtensa_ae_l16m_x       : Intrinsic<[llvm_v4i16_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l16m_xu      : Intrinsic<[llvm_v4i16_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l32f24_xc    : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l32m_x       : Intrinsic<[llvm_v2i32_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l32x2f24_ip  : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly, Align<ArgIndex<0>, 8>]>;
  def int_xtensa_ae_l32x2f24_xc  : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly, Align<ArgIndex<0>, 8>]>;
  def int_xtensa_ae_l32x2_i      : Intrinsic<[llvm_v2i32_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly, Align<ArgIndex<0>, 8>]>;
  def int_xtensa_ae_l32x2_ip     : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly, Align<ArgIndex<0>, 8>]>;
  def int_xtensa_ae_l32x2_xc     : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly, Align<ArgIndex<0>, 8>]>;
  def int_xtensa_ae_l32x2_xc1    : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly, Align<ArgIndex<0>, 8>]>;

  //
  // HiFi stores
  //
  def int_xtensa_ae_s16_0_xc  : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s16_0_xc1 : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s32_l_xc  : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s32_l_xc1 : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s32x2_xc  : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly, Align<ArgIndex<1>, 8>]>;
  def int_xtensa_ae_s32x2_xc1 : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly, Align<ArgIndex<1>, 8>]>;
  
  def int_xtensa_ae_sa16x4_ip : Intrinsic<[llvm_ptr_ty], [llvm_v4i16_ty, llvm_ptr_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_sa16x4_ic : Intrinsic<[llvm_ptr_ty], [llvm_v4i16_ty, llvm_ptr_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_sa32x2_ip : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_sa32x2_ic : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_sa24x2_ip : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty], [IntrWriteMem, IntrArgMemOnly]>;

  // MAC and Arithmetic Operations
  def int_xtensa_ae_mul16 : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mula2q32x16_fir_h : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  
  def int_xtensa_ae_mul32_ll : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty],
                                         [IntrNoMem]>;

  def int_xtensa_ae_mulaf32r_hh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;

  def int_xtensa_ae_srai32 : ClangBuiltin<"__builtin_xtensa_ae_srai32">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_slai32 : ClangBuiltin<"__builtin_xtensa_ae_slai32">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_slai32s : ClangBuiltin<"__builtin_xtensa_ae_slai32s">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_srai64 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_slai64s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_slaa64s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_srai32r : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_add16  : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_add16s : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_add24s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_add32  : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_add32s : ClangBuiltin<"__builtin_xtensa_ae_add32s">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_add32_hl_lh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_add64  : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_add64s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sub16  : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_sub16s : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_sub24s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sub32  : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sub32s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sub64 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;

  def int_xtensa_ae_max16 : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;

  def int_xtensa_ae_and16 : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_or16 : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_xor16 : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_and64 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_or64 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_xor64 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_min16 : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_max32 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_min32 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;

  def int_xtensa_ae_abs16s : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_abs32s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_neg16s : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_neg32s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_round32x2f48sasym : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_round32x2f48ssym : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  
  def int_xtensa_ae_pksr32 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_round16x4f32sasym : Intrinsic<[llvm_v4i16_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_round16x4f32ssym : Intrinsic<[llvm_v4i16_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_round24x2f48sasym : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_round24x2f48ssym : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_round32x2f64sasym : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_round32x2f64ssym : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_roundsp16q48x2asym : Intrinsic<[llvm_v4i16_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_roundsp16q48x2sym : Intrinsic<[llvm_v4i16_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_roundsp16f24asym : Intrinsic<[llvm_v4i16_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_roundsp16f24sym : Intrinsic<[llvm_v4i16_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_roundsq32f48asym : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_roundsq32f48sym : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_maxabs32s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sat16x4 : Intrinsic<[llvm_v4i16_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sat24s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_zero16 : Intrinsic<[llvm_v4i16_ty], [], [IntrNoMem]>;
  def int_xtensa_ae_zero24 : Intrinsic<[llvm_v2i32_ty], [], [IntrNoMem]>;
  def int_xtensa_ae_zero32 : Intrinsic<[llvm_v2i32_ty], [], [IntrNoMem]>;
  def int_xtensa_ae_zerop48 : Intrinsic<[llvm_v2i32_ty], [], [IntrNoMem]>;
  def int_xtensa_ae_zero64 : Intrinsic<[llvm_v2i32_ty], [], [IntrNoMem]>;
  def int_xtensa_ae_zeroq56 : Intrinsic<[llvm_v2i32_ty], [], [IntrNoMem]>;

  def int_xtensa_ae_cbegin0 : ClangBuiltin<"__builtin_xtensa_ae_cbegin0">, Intrinsic<[llvm_i32_ty], [], [IntrNoMem]>;
  def int_xtensa_ae_cbegin1 : ClangBuiltin<"__builtin_xtensa_ae_cbegin1">, Intrinsic<[llvm_i32_ty], [], [IntrNoMem]>;
  def int_xtensa_ae_cend0   : ClangBuiltin<"__builtin_xtensa_ae_cend0">, Intrinsic<[llvm_i32_ty], [], [IntrNoMem]>;
  def int_xtensa_ae_cend1   : ClangBuiltin<"__builtin_xtensa_ae_cend1">, Intrinsic<[llvm_i32_ty], [], [IntrNoMem]>;
  
  def int_xtensa_ae_setcbegin0 : ClangBuiltin<"__builtin_xtensa_ae_setcbegin0">, Intrinsic<[], [llvm_i32_ty], [IntrNoMem, IntrHasSideEffects]>;
  def int_xtensa_ae_setcbegin1 : ClangBuiltin<"__builtin_xtensa_ae_setcbegin1">, Intrinsic<[], [llvm_i32_ty], [IntrNoMem, IntrHasSideEffects]>;
  def int_xtensa_ae_setcend0   : ClangBuiltin<"__builtin_xtensa_ae_setcend0">, Intrinsic<[], [llvm_i32_ty], [IntrNoMem, IntrHasSideEffects]>;
  def int_xtensa_ae_setcend1   : ClangBuiltin<"__builtin_xtensa_ae_setcend1">, Intrinsic<[], [llvm_i32_ty], [IntrNoMem, IntrHasSideEffects]>;

  // Boolean comparison intrinsics - return packed boolean result as i32
  // 16-bit: 4 boolean results in bits 0-3
  def int_xtensa_ae_eq16 : ClangBuiltin<"__builtin_xtensa_ae_eq16">, Intrinsic<[llvm_i32_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_lt16 : ClangBuiltin<"__builtin_xtensa_ae_lt16">, Intrinsic<[llvm_i32_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_le16 : ClangBuiltin<"__builtin_xtensa_ae_le16">, Intrinsic<[llvm_i32_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  // 32-bit: 2 boolean results in bits 0-1
  def int_xtensa_ae_eq32 : ClangBuiltin<"__builtin_xtensa_ae_eq32">, Intrinsic<[llvm_i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_lt32 : ClangBuiltin<"__builtin_xtensa_ae_lt32">, Intrinsic<[llvm_i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_le32 : ClangBuiltin<"__builtin_xtensa_ae_le32">, Intrinsic<[llvm_i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  // 64-bit: 1 boolean result in bit 0
  def int_xtensa_ae_eq64 : ClangBuiltin<"__builtin_xtensa_ae_eq64">, Intrinsic<[llvm_i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_lt64 : ClangBuiltin<"__builtin_xtensa_ae_lt64">, Intrinsic<[llvm_i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_le64 : ClangBuiltin<"__builtin_xtensa_ae_le64">, Intrinsic<[llvm_i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sel32_hh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sel32_hl : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sel32_lh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sel32_ll : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;

  def int_xtensa_ae_selp24_hh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_selp24_lh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mul32_hh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaaaafq32x16 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaaf2d32ra_hh_ll : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaafd24_hh_ll : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaafd32ra_hh_ll : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaafp24s_hh_ll : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
      
  // New MAC Intrinsics Batch 2
  def int_xtensa_ae_mulaafd32x16_h3_l2 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaafd32x16_h1_l0 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaf32s_ll : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaf32s_lh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  // AE_MULAF32S_HL: acc += hi(b)*lo(c). Maps to ae_mulaf32s.lh with swapped b/c.
  def int_xtensa_ae_mulaf32s_hl : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf32s_ll : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf32s_lh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;

  // New MAC Intrinsics Batch 3
  def int_xtensa_ae_mulaf32r_ll : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaf32r_lh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaf32s_hh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaf32ra_ll : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaf32ra_lh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaf32ra_hh : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulsf32s_ll : ClangBuiltin<"__builtin_xtensa_ae_mulsf32s_ll">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulsf32s_lh : ClangBuiltin<"__builtin_xtensa_ae_mulsf32s_lh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulsf32s_hh : ClangBuiltin<"__builtin_xtensa_ae_mulsf32s_hh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf32r_ll : ClangBuiltin<"__builtin_xtensa_ae_mulf32r_ll">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf32s_hh : ClangBuiltin<"__builtin_xtensa_ae_mulf32s_hh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf32r_lh : ClangBuiltin<"__builtin_xtensa_ae_mulf32r_lh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf32r_hh : ClangBuiltin<"__builtin_xtensa_ae_mulf32r_hh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf32ra_ll : ClangBuiltin<"__builtin_xtensa_ae_mulf32ra_ll">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf32ra_lh : ClangBuiltin<"__builtin_xtensa_ae_mulf32ra_lh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf32ra_hh : ClangBuiltin<"__builtin_xtensa_ae_mulf32ra_hh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;

  // Complete multiply family gaps (all available in ACE30 overlay)
  def int_xtensa_ae_mul32_lh     : ClangBuiltin<"__builtin_xtensa_ae_mul32_lh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mula32_hh    : ClangBuiltin<"__builtin_xtensa_ae_mula32_hh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mula32_lh    : ClangBuiltin<"__builtin_xtensa_ae_mula32_lh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mula32_ll    : ClangBuiltin<"__builtin_xtensa_ae_mula32_ll">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_muls32_hh    : ClangBuiltin<"__builtin_xtensa_ae_muls32_hh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_muls32_lh    : ClangBuiltin<"__builtin_xtensa_ae_muls32_lh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_muls32_ll    : ClangBuiltin<"__builtin_xtensa_ae_muls32_ll">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulsf32r_hh  : ClangBuiltin<"__builtin_xtensa_ae_mulsf32r_hh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulsf32r_lh  : ClangBuiltin<"__builtin_xtensa_ae_mulsf32r_lh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulsf32r_ll  : ClangBuiltin<"__builtin_xtensa_ae_mulsf32r_ll">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulsf32ra_hh : ClangBuiltin<"__builtin_xtensa_ae_mulsf32ra_hh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulsf32ra_lh : ClangBuiltin<"__builtin_xtensa_ae_mulsf32ra_lh">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulsf32ra_ll : ClangBuiltin<"__builtin_xtensa_ae_mulsf32ra_ll">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;

  // New MAC Intrinsics Batch 4
  def int_xtensa_ae_mulaf16ss_00 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaf16ss_11 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaf16ss_22 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulaf16ss_33 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf16ss_00 : Intrinsic<[llvm_v2i32_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf16ss_11 : Intrinsic<[llvm_v2i32_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf16ss_22 : Intrinsic<[llvm_v2i32_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf16ss_33 : Intrinsic<[llvm_v2i32_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;

  def int_xtensa_ae_mulfp16x4ras : ClangBuiltin<"__builtin_xtensa_ae_mulfp16x4ras">, Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulfp16x4rs  : ClangBuiltin<"__builtin_xtensa_ae_mulfp16x4rs">, Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulfp16x4s   : ClangBuiltin<"__builtin_xtensa_ae_mulfp16x4s">, Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;

  def int_xtensa_ae_mulfp32x16x2ras_h : ClangBuiltin<"__builtin_xtensa_ae_mulfp32x16x2ras_h">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulfp32x16x2ras_l : ClangBuiltin<"__builtin_xtensa_ae_mulfp32x16x2ras_l">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulfp32x16x2rs_h  : ClangBuiltin<"__builtin_xtensa_ae_mulfp32x16x2rs_h">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulfp32x16x2rs_l  : ClangBuiltin<"__builtin_xtensa_ae_mulfp32x16x2rs_l">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulfp32x2rs       : ClangBuiltin<"__builtin_xtensa_ae_mulfp32x2rs">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulfp24x2r        : ClangBuiltin<"__builtin_xtensa_ae_mulfp24x2r">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;

  // Additional MAC intrinsics
  def int_xtensa_ae_mulafp32x2rs       : ClangBuiltin<"__builtin_xtensa_ae_mulafp32x2rs">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulafp32x16x2rs_h  : ClangBuiltin<"__builtin_xtensa_ae_mulafp32x16x2rs_h">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulafp32x16x2rs_l  : ClangBuiltin<"__builtin_xtensa_ae_mulafp32x16x2rs_l">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulfc24ra           : ClangBuiltin<"__builtin_xtensa_ae_mulfc24ra">, Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;

  // FIR Pseudo Multi-return MACs
  def int_xtensa_ae_mulafd32x16x2_fir_hh : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulafd32x16x2_fir_hl : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulafd32x16x2_fir_lh : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulafd32x16x2_fir_ll : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mul16x4 : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty, llvm_v4i16_ty], [IntrNoMem]>;
  // Conversions and Moves
  def int_xtensa_ae_cvt16x4 : Intrinsic<[llvm_v4i16_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_cvt32x2f16_10 : Intrinsic<[llvm_v2i32_ty], [llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_cvt32x2f16_32 : Intrinsic<[llvm_v2i32_ty], [llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_cvt48a32 : Intrinsic<[llvm_v2i32_ty], [llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_cvtp24a16x2_ll : Intrinsic<[llvm_v2i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_cvtq48a32s : Intrinsic<[llvm_v2i32_ty], [llvm_i32_ty], [IntrNoMem]>;
  
  def int_xtensa_ae_movad16_0 : Intrinsic<[llvm_i32_ty], [llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_movad16_1 : Intrinsic<[llvm_i32_ty], [llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_movad16_2 : Intrinsic<[llvm_i32_ty], [llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_movad16_3 : Intrinsic<[llvm_i32_ty], [llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_movad32_h : Intrinsic<[llvm_i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_movad32_l : Intrinsic<[llvm_i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_movda32 : Intrinsic<[llvm_v2i32_ty], [llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_movda32x2 : Intrinsic<[llvm_v2i32_ty], [llvm_i32_ty, llvm_i32_ty], [IntrNoMem]>;
  
  def int_xtensa_ae_movf16x4_fromint64 : Intrinsic<[llvm_v4i16_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_movf24x2_fromint32x2 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_movint16x4_fromint32x2 : Intrinsic<[llvm_v4i16_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_movint24x2_fromf24x2 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_movint24x2_fromf32x2 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  // Identity bitcast conversions between ae_int64 and ae_int32x2 views.
  // Both types map to the same 64-bit AEDR register; these are no-ops at ISel.
  def int_xtensa_ae_movint32x2_fromint64 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_movint64_fromint32x2 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  
  def int_xtensa_ae_movint32_fromint16 : Intrinsic<[llvm_i32_ty], [llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_movint32_fromint24x2 : Intrinsic<[llvm_i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_movint32_fromint64 : Intrinsic<[llvm_i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;
  
  def int_xtensa_ae_sext32x2d16_10 : Intrinsic<[llvm_v2i32_ty], [llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_sext32x2d16_32 : Intrinsic<[llvm_v2i32_ty], [llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_trunc16x4f32 : Intrinsic<[llvm_v4i16_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  
  // Shifts
  def int_xtensa_ae_slaa16s : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_slaa32 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_slaa32s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_slaa64 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  
  def int_xtensa_ae_slai16s : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_slai24s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_slai64 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  
  def int_xtensa_ae_sraa16rs : Intrinsic<[llvm_v4i16_ty], [llvm_v4i16_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sraa32 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sraa32rs : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sraa32s : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sraa64 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sraaq56 : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;

  // Miscellaneous operations
  def int_xtensa_ae_nsaz32_l : ClangBuiltin<"__builtin_xtensa_ae_nsaz32_l">, Intrinsic<[llvm_i32_ty], [llvm_v2i32_ty], [IntrNoMem]>;

  // Missing HiFi3 intrinsics (from xt-clang reference analysis)

  // ae_sel16i: select 16-bit elements with 3-bit immediate (0-7)
  def int_xtensa_ae_sel16i : Intrinsic<[llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_i32_ty], [IntrNoMem]>;

  // ae_l64_i: 64-bit load with immediate offset
  def int_xtensa_ae_l64_i : Intrinsic<[llvm_v2i32_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem]>;
  // ae_s64_i: 64-bit store with immediate offset
  def int_xtensa_ae_s64_i : Intrinsic<[], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem]>;

  // ae_zalign64: initialize store alignment register (returns ae_valign as i64/v2i32)
  def int_xtensa_ae_zalign64 : Intrinsic<[llvm_v2i32_ty], [], [IntrNoMem]>;

  // ae_addandsub32s: simultaneous saturating add and subtract (2 results)
  def int_xtensa_ae_addandsub32s : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;

  // ae_l16x2m_xu: load 16x2 with register post-increment (returns value + updated ptr)
  def int_xtensa_ae_l16x2m_xu : Intrinsic<[llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem]>;

  // ae_movda16: move AR to AE_DR (16-bit replicate into v2i32)
  def int_xtensa_ae_movda16 : Intrinsic<[llvm_v2i32_ty], [llvm_i32_ty], [IntrNoMem]>;

  // ae_s32x2f24_i: store 32x2 in f24 format with immediate offset
  def int_xtensa_ae_s32x2f24_i : Intrinsic<[], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem]>;
  // ae_s32x2f24_ip: store 32x2 in f24 format with post-increment
  def int_xtensa_ae_s32x2f24_ip : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem]>;

  // Scalar AR operations
  def int_xtensa_ae_clamps16 : Intrinsic<[llvm_i32_ty], [llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_sext16 : Intrinsic<[llvm_i32_ty], [llvm_i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_zext16 : Intrinsic<[llvm_i32_ty], [llvm_i32_ty], [IntrNoMem]>;

  // ae_movab2: move AE bool2 to AR
  def int_xtensa_ae_movab2 : Intrinsic<[llvm_i32_ty], [llvm_i32_ty], [IntrNoMem]>;

  // =========================================================================
  // HiFi5 128-bit Loads (dual-output: two AE_DR values + updated pointer)
  // =========================================================================
  def int_xtensa_ae_la32x2x2_ip : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_la16x4x2_ip : Intrinsic<[llvm_v4i16_ty, llvm_v4i16_ty, llvm_ptr_ty], [llvm_ptr_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l32x2x2_xc  : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;
  def int_xtensa_ae_l32x2x2_xc1 : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty, llvm_ptr_ty], [llvm_ptr_ty, llvm_i32_ty], [IntrReadMem, IntrArgMemOnly]>;

  // HiFi5 128-bit Stores (dual-input + updated pointer)
  def int_xtensa_ae_sa32x2x2_ip : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_ptr_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_sa16x4x2_ip : Intrinsic<[llvm_ptr_ty], [llvm_v4i16_ty, llvm_v4i16_ty, llvm_ptr_ty], [IntrWriteMem, IntrArgMemOnly]>;
  def int_xtensa_ae_s32x2x2_xc1 : Intrinsic<[llvm_ptr_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_ptr_ty, llvm_i32_ty], [IntrWriteMem, IntrArgMemOnly]>;

  // HiFi5 MACs (dual-output)
  def int_xtensa_ae_mula2q32x16_fir_h5 : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf32x2r_hh_ll     : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf2p32x16x4rs     : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v4i16_ty], [IntrNoMem]>;
  def int_xtensa_ae_mulf2p32x4rs        : Intrinsic<[llvm_v2i32_ty, llvm_v2i32_ty], [llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty, llvm_v2i32_ty], [IntrNoMem]>;
}
