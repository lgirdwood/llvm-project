//===---------- Xtensa.cpp - Emit LLVM Code for builtins ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This contains code to emit Builtin calls as LLVM code.
//
//===----------------------------------------------------------------------===//

#include "CGBuiltin.h"
#include "clang/Basic/TargetBuiltins.h"
#include "llvm/IR/IntrinsicsXtensa.h"

using namespace clang;
using namespace CodeGen;
using namespace llvm;

Value *CodeGenFunction::EmitXtensaBuiltinExpr(unsigned BuiltinID,
                                              const CallExpr *E,
                                              ReturnValueSlot ReturnValue) {
  SmallVector<Value *, 4> Ops;

  for (unsigned i = 0, e = E->getNumArgs(); i != e; i++)
    Ops.push_back(EmitScalarExpr(E->getArg(i)));

  Intrinsic::ID ID = Intrinsic::not_intrinsic;
  bool IsPostUpdateLoad = false;
  bool IsPostUpdateStore = false;

  switch (BuiltinID) {
  // MAC and Arithmetic Operations
  case Xtensa::BI__builtin_xtensa_ae_mul32_ll:
    ID = Intrinsic::xtensa_ae_mul32_ll; break;

  case Xtensa::BI__builtin_xtensa_ae_mul32_hh:
    ID = Intrinsic::xtensa_ae_mul32_hh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf32s_ll:
    ID = Intrinsic::xtensa_ae_mulf32s_ll; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf32s_lh:
    ID = Intrinsic::xtensa_ae_mulf32s_lh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf32r_ll:
    ID = Intrinsic::xtensa_ae_mulf32r_ll; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf32s_hh:
    ID = Intrinsic::xtensa_ae_mulf32s_hh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf32r_lh:
    ID = Intrinsic::xtensa_ae_mulf32r_lh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf32r_hh:
    ID = Intrinsic::xtensa_ae_mulf32r_hh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf32ra_ll:
    ID = Intrinsic::xtensa_ae_mulf32ra_ll; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf32ra_lh:
    ID = Intrinsic::xtensa_ae_mulf32ra_lh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf32ra_hh:
    ID = Intrinsic::xtensa_ae_mulf32ra_hh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf16ss_00:
    ID = Intrinsic::xtensa_ae_mulf16ss_00; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf16ss_11:
    ID = Intrinsic::xtensa_ae_mulf16ss_11; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf16ss_22:
    ID = Intrinsic::xtensa_ae_mulf16ss_22; break;
  case Xtensa::BI__builtin_xtensa_ae_mulf16ss_33:
    ID = Intrinsic::xtensa_ae_mulf16ss_33; break;
  case Xtensa::BI__builtin_xtensa_ae_mulfp16x4ras:
    ID = Intrinsic::xtensa_ae_mulfp16x4ras; break;
  case Xtensa::BI__builtin_xtensa_ae_mulfp16x4rs:
    ID = Intrinsic::xtensa_ae_mulfp16x4rs; break;
  case Xtensa::BI__builtin_xtensa_ae_mulfp16x4s:
    ID = Intrinsic::xtensa_ae_mulfp16x4s; break;
  case Xtensa::BI__builtin_xtensa_ae_mulfp32x16x2ras_h:
    ID = Intrinsic::xtensa_ae_mulfp32x16x2ras_h; break;
  case Xtensa::BI__builtin_xtensa_ae_mulfp32x16x2ras_l:
    ID = Intrinsic::xtensa_ae_mulfp32x16x2ras_l; break;
  case Xtensa::BI__builtin_xtensa_ae_mulfp32x16x2rs_h:
    ID = Intrinsic::xtensa_ae_mulfp32x16x2rs_h; break;
  case Xtensa::BI__builtin_xtensa_ae_mulfp32x16x2rs_l:
    ID = Intrinsic::xtensa_ae_mulfp32x16x2rs_l; break;
  case Xtensa::BI__builtin_xtensa_ae_mulfp32x2rs:
    ID = Intrinsic::xtensa_ae_mulfp32x2rs; break;
  case Xtensa::BI__builtin_xtensa_ae_mulfp24x2r:
    ID = Intrinsic::xtensa_ae_mulfp24x2r; break;

  case Xtensa::BI__builtin_xtensa_ae_mulaaaafq32x16:
    ID = Intrinsic::xtensa_ae_mulaaaafq32x16; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaafd24_hh_ll:
    ID = Intrinsic::xtensa_ae_mulaafd24_hh_ll; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaafd32ra_hh_ll:
    ID = Intrinsic::xtensa_ae_mulaafd32ra_hh_ll; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaafp24s_hh_ll:
    ID = Intrinsic::xtensa_ae_mulaafp24s_hh_ll; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaafd32x16_h3_l2:
    ID = Intrinsic::xtensa_ae_mulaafd32x16_h3_l2; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaafd32x16_h1_l0:
    ID = Intrinsic::xtensa_ae_mulaafd32x16_h1_l0; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf32s_ll:
    ID = Intrinsic::xtensa_ae_mulaf32s_ll; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf32s_lh:
    ID = Intrinsic::xtensa_ae_mulaf32s_lh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf32r_ll:
    ID = Intrinsic::xtensa_ae_mulaf32r_ll; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf32s_hh:
    ID = Intrinsic::xtensa_ae_mulaf32s_hh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf32r_lh:
    ID = Intrinsic::xtensa_ae_mulaf32r_lh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf32ra_ll:
    ID = Intrinsic::xtensa_ae_mulaf32ra_ll; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf32ra_lh:
    ID = Intrinsic::xtensa_ae_mulaf32ra_lh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf32ra_hh:
    ID = Intrinsic::xtensa_ae_mulaf32ra_hh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulsf32s_ll:
    ID = Intrinsic::xtensa_ae_mulsf32s_ll; break;
  case Xtensa::BI__builtin_xtensa_ae_mulsf32s_lh:
    ID = Intrinsic::xtensa_ae_mulsf32s_lh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulsf32s_hh:
    ID = Intrinsic::xtensa_ae_mulsf32s_hh; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf16ss_00:
    ID = Intrinsic::xtensa_ae_mulaf16ss_00; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf16ss_11:
    ID = Intrinsic::xtensa_ae_mulaf16ss_11; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf16ss_22:
    ID = Intrinsic::xtensa_ae_mulaf16ss_22; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf16ss_33:
    ID = Intrinsic::xtensa_ae_mulaf16ss_33; break;
  case Xtensa::BI__builtin_xtensa_ae_mulaf32r_hh:
    ID = Intrinsic::xtensa_ae_mulaf32r_hh; break;
  case Xtensa::BI__builtin_xtensa_ae_cbegin0:
    return Builder.CreateCall(CGM.getIntrinsic(Intrinsic::xtensa_ae_cbegin0));
  case Xtensa::BI__builtin_xtensa_ae_cbegin1:
    return Builder.CreateCall(CGM.getIntrinsic(Intrinsic::xtensa_ae_cbegin1));
  case Xtensa::BI__builtin_xtensa_ae_cend0:
    return Builder.CreateCall(CGM.getIntrinsic(Intrinsic::xtensa_ae_cend0));
  case Xtensa::BI__builtin_xtensa_ae_cend1:
    return Builder.CreateCall(CGM.getIntrinsic(Intrinsic::xtensa_ae_cend1));
  case Xtensa::BI__builtin_xtensa_ae_setcbegin0:
    return Builder.CreateCall(CGM.getIntrinsic(Intrinsic::xtensa_ae_setcbegin0),
                              {Ops[0]});
  case Xtensa::BI__builtin_xtensa_ae_setcbegin1:
    return Builder.CreateCall(CGM.getIntrinsic(Intrinsic::xtensa_ae_setcbegin1),
                              {Ops[0]});
  case Xtensa::BI__builtin_xtensa_ae_setcend0:
    return Builder.CreateCall(CGM.getIntrinsic(Intrinsic::xtensa_ae_setcend0),
                              {Ops[0]});
  case Xtensa::BI__builtin_xtensa_ae_setcend1:
    return Builder.CreateCall(CGM.getIntrinsic(Intrinsic::xtensa_ae_setcend1),
                              {Ops[0]});
                              
  case Xtensa::BI__builtin_xtensa_ae_srai32:
    ID = Intrinsic::xtensa_ae_srai32; break;
  case Xtensa::BI__builtin_xtensa_ae_slai32:
    ID = Intrinsic::xtensa_ae_slai32; break;
  case Xtensa::BI__builtin_xtensa_ae_slai32s:
    ID = Intrinsic::xtensa_ae_slai32s; break;
  case Xtensa::BI__builtin_xtensa_ae_srai64:
    ID = Intrinsic::xtensa_ae_srai64; break;
  case Xtensa::BI__builtin_xtensa_ae_slai64s:
    ID = Intrinsic::xtensa_ae_slai64s; break;
  case Xtensa::BI__builtin_xtensa_ae_slaa64s:
    ID = Intrinsic::xtensa_ae_slaa64s; break;
  case Xtensa::BI__builtin_xtensa_ae_srai32r:
    ID = Intrinsic::xtensa_ae_srai32r; break;
  case Xtensa::BI__builtin_xtensa_ae_add16:
    ID = Intrinsic::xtensa_ae_add16; break;
  case Xtensa::BI__builtin_xtensa_ae_add16s:
    ID = Intrinsic::xtensa_ae_add16s; break;
  case Xtensa::BI__builtin_xtensa_ae_add24s:
    ID = Intrinsic::xtensa_ae_add24s; break;
  case Xtensa::BI__builtin_xtensa_ae_add32:
    ID = Intrinsic::xtensa_ae_add32; break;
  case Xtensa::BI__builtin_xtensa_ae_add32s:
    ID = Intrinsic::xtensa_ae_add32s; break;
  case Xtensa::BI__builtin_xtensa_ae_add32_hl_lh:
    ID = Intrinsic::xtensa_ae_add32_hl_lh; break;
  case Xtensa::BI__builtin_xtensa_ae_sub16:
    ID = Intrinsic::xtensa_ae_sub16; break;
  case Xtensa::BI__builtin_xtensa_ae_sub16s:
    ID = Intrinsic::xtensa_ae_sub16s; break;
  case Xtensa::BI__builtin_xtensa_ae_sub24s:
    ID = Intrinsic::xtensa_ae_sub24s; break;
  case Xtensa::BI__builtin_xtensa_ae_sub32:
    ID = Intrinsic::xtensa_ae_sub32; break;
  case Xtensa::BI__builtin_xtensa_ae_sub32s:
    ID = Intrinsic::xtensa_ae_sub32s; break;
  case Xtensa::BI__builtin_xtensa_ae_max32:
    ID = Intrinsic::xtensa_ae_max32; break;
  case Xtensa::BI__builtin_xtensa_ae_min32:
    ID = Intrinsic::xtensa_ae_min32; break;
  case Xtensa::BI__builtin_xtensa_ae_abs16s:
    ID = Intrinsic::xtensa_ae_abs16s; break;
  case Xtensa::BI__builtin_xtensa_ae_abs32s:
    ID = Intrinsic::xtensa_ae_abs32s; break;
  case Xtensa::BI__builtin_xtensa_ae_neg16s:
    ID = Intrinsic::xtensa_ae_neg16s; break;
  case Xtensa::BI__builtin_xtensa_ae_neg32s:
    ID = Intrinsic::xtensa_ae_neg32s; break;

  case Xtensa::BI__builtin_xtensa_ae_and16:
    ID = Intrinsic::xtensa_ae_and16; break;
  case Xtensa::BI__builtin_xtensa_ae_or16:
    ID = Intrinsic::xtensa_ae_or16; break;
  case Xtensa::BI__builtin_xtensa_ae_xor16:
    ID = Intrinsic::xtensa_ae_xor16; break;

  // Conversions and Moves
  case Xtensa::BI__builtin_xtensa_ae_cvt16x4:
    ID = Intrinsic::xtensa_ae_cvt16x4; break;
  case Xtensa::BI__builtin_xtensa_ae_cvt32x2f16_10:
    ID = Intrinsic::xtensa_ae_cvt32x2f16_10; break;
  case Xtensa::BI__builtin_xtensa_ae_cvt32x2f16_32:
    ID = Intrinsic::xtensa_ae_cvt32x2f16_32; break;
  case Xtensa::BI__builtin_xtensa_ae_cvt48a32:
    ID = Intrinsic::xtensa_ae_cvt48a32; break;
  case Xtensa::BI__builtin_xtensa_ae_cvtp24a16x2_ll:
    ID = Intrinsic::xtensa_ae_cvtp24a16x2_ll; break;
  case Xtensa::BI__builtin_xtensa_ae_cvtq48a32s:
    ID = Intrinsic::xtensa_ae_cvtq48a32s; break;
  case Xtensa::BI__builtin_xtensa_ae_movad16_0:
    ID = Intrinsic::xtensa_ae_movad16_0; break;
  case Xtensa::BI__builtin_xtensa_ae_movad16_1:
    ID = Intrinsic::xtensa_ae_movad16_1; break;
  case Xtensa::BI__builtin_xtensa_ae_movad16_2:
    ID = Intrinsic::xtensa_ae_movad16_2; break;
  case Xtensa::BI__builtin_xtensa_ae_movad16_3:
    ID = Intrinsic::xtensa_ae_movad16_3; break;
  case Xtensa::BI__builtin_xtensa_ae_movad32_h:
    ID = Intrinsic::xtensa_ae_movad32_h; break;
  case Xtensa::BI__builtin_xtensa_ae_movad32_l:
    ID = Intrinsic::xtensa_ae_movad32_l; break;
  case Xtensa::BI__builtin_xtensa_ae_movda32:
    ID = Intrinsic::xtensa_ae_movda32; break;
  case Xtensa::BI__builtin_xtensa_ae_movda32x2:
    ID = Intrinsic::xtensa_ae_movda32x2; break;
  case Xtensa::BI__builtin_xtensa_ae_movf16x4_fromint64:
    ID = Intrinsic::xtensa_ae_movf16x4_fromint64; break;
  case Xtensa::BI__builtin_xtensa_ae_movf24x2_fromint32x2:
    ID = Intrinsic::xtensa_ae_movf24x2_fromint32x2; break;
  case Xtensa::BI__builtin_xtensa_ae_movint16x4_fromint32x2:
    ID = Intrinsic::xtensa_ae_movint16x4_fromint32x2; break;
  case Xtensa::BI__builtin_xtensa_ae_movint24x2_fromf24x2:
    ID = Intrinsic::xtensa_ae_movint24x2_fromf24x2; break;
  case Xtensa::BI__builtin_xtensa_ae_movint24x2_fromf32x2:
    ID = Intrinsic::xtensa_ae_movint24x2_fromf32x2; break;
  case Xtensa::BI__builtin_xtensa_ae_movint32_fromint16:
    ID = Intrinsic::xtensa_ae_movint32_fromint16; break;
  case Xtensa::BI__builtin_xtensa_ae_movint32_fromint24x2:
    ID = Intrinsic::xtensa_ae_movint32_fromint24x2; break;
  case Xtensa::BI__builtin_xtensa_ae_movint32_fromint64:
    ID = Intrinsic::xtensa_ae_movint32_fromint64; break;
  case Xtensa::BI__builtin_xtensa_ae_sext32x2d16_10:
    ID = Intrinsic::xtensa_ae_sext32x2d16_10; break;
  case Xtensa::BI__builtin_xtensa_ae_sext32x2d16_32:
    ID = Intrinsic::xtensa_ae_sext32x2d16_32; break;
  case Xtensa::BI__builtin_xtensa_ae_trunc16x4f32:
    ID = Intrinsic::xtensa_ae_trunc16x4f32; break;

  // Shifts
  case Xtensa::BI__builtin_xtensa_ae_slaa16s:
    ID = Intrinsic::xtensa_ae_slaa16s; break;
  case Xtensa::BI__builtin_xtensa_ae_slaa32:
    ID = Intrinsic::xtensa_ae_slaa32; break;
  case Xtensa::BI__builtin_xtensa_ae_slaa32s:
    ID = Intrinsic::xtensa_ae_slaa32s; break;
  case Xtensa::BI__builtin_xtensa_ae_slaa64:
    ID = Intrinsic::xtensa_ae_slaa64; break;
  case Xtensa::BI__builtin_xtensa_ae_slai16s:
    ID = Intrinsic::xtensa_ae_slai16s; break;
  case Xtensa::BI__builtin_xtensa_ae_slai24s:
    ID = Intrinsic::xtensa_ae_slai24s; break;
  case Xtensa::BI__builtin_xtensa_ae_slai64:
    ID = Intrinsic::xtensa_ae_slai64; break;
  case Xtensa::BI__builtin_xtensa_ae_sraa16rs:
    ID = Intrinsic::xtensa_ae_sraa16rs; break;
  case Xtensa::BI__builtin_xtensa_ae_sraa32:
    ID = Intrinsic::xtensa_ae_sraa32; break;
  case Xtensa::BI__builtin_xtensa_ae_sraa32rs:
    ID = Intrinsic::xtensa_ae_sraa32rs; break;
  case Xtensa::BI__builtin_xtensa_ae_sraa32s:
    ID = Intrinsic::xtensa_ae_sraa32s; break;
  case Xtensa::BI__builtin_xtensa_ae_sraa64:
    ID = Intrinsic::xtensa_ae_sraa64; break;
  case Xtensa::BI__builtin_xtensa_ae_sraaq56:
    ID = Intrinsic::xtensa_ae_sraaq56; break;


  case Xtensa::BI__builtin_xtensa_ae_zero16:
    ID = Intrinsic::xtensa_ae_zero16; break;
  case Xtensa::BI__builtin_xtensa_ae_zero24:
    ID = Intrinsic::xtensa_ae_zero24; break;
  case Xtensa::BI__builtin_xtensa_ae_zero32:
    ID = Intrinsic::xtensa_ae_zero32; break;
  case Xtensa::BI__builtin_xtensa_ae_zerop48:
    ID = Intrinsic::xtensa_ae_zerop48; break;
  case Xtensa::BI__builtin_xtensa_ae_zero64:
    ID = Intrinsic::xtensa_ae_zero64; break;
  case Xtensa::BI__builtin_xtensa_ae_zeroq56:
    ID = Intrinsic::xtensa_ae_zeroq56; break;
  case Xtensa::BI__builtin_xtensa_ae_round32x2f48sasym:
    ID = Intrinsic::xtensa_ae_round32x2f48sasym; break;
  case Xtensa::BI__builtin_xtensa_ae_round32x2f48ssym:
    ID = Intrinsic::xtensa_ae_round32x2f48ssym; break;
  case Xtensa::BI__builtin_xtensa_ae_sel32_hh:
    ID = Intrinsic::xtensa_ae_sel32_hh; break;
  case Xtensa::BI__builtin_xtensa_ae_sel32_hl:
    ID = Intrinsic::xtensa_ae_sel32_hl; break;
  case Xtensa::BI__builtin_xtensa_ae_sel32_lh:
    ID = Intrinsic::xtensa_ae_sel32_lh; break;
  case Xtensa::BI__builtin_xtensa_ae_sel32_ll:
    ID = Intrinsic::xtensa_ae_sel32_ll; break;
  case Xtensa::BI__builtin_xtensa_ae_selp24_hh:
    ID = Intrinsic::xtensa_ae_selp24_hh; break;
  case Xtensa::BI__builtin_xtensa_ae_selp24_lh:
    ID = Intrinsic::xtensa_ae_selp24_lh; break;

  case Xtensa::BI__builtin_xtensa_ae_pksr32:
    ID = Intrinsic::xtensa_ae_pksr32; break;
  case Xtensa::BI__builtin_xtensa_ae_round16x4f32sasym:
    ID = Intrinsic::xtensa_ae_round16x4f32sasym; break;
  case Xtensa::BI__builtin_xtensa_ae_round16x4f32ssym:
    ID = Intrinsic::xtensa_ae_round16x4f32ssym; break;
  case Xtensa::BI__builtin_xtensa_ae_round24x2f48sasym:
    ID = Intrinsic::xtensa_ae_round24x2f48sasym; break;
  case Xtensa::BI__builtin_xtensa_ae_round24x2f48ssym:
    ID = Intrinsic::xtensa_ae_round24x2f48ssym; break;

  case Xtensa::BI__builtin_xtensa_ae_round32x2f64sasym:
    ID = Intrinsic::xtensa_ae_round32x2f64sasym; break;
  case Xtensa::BI__builtin_xtensa_ae_round32x2f64ssym:
    ID = Intrinsic::xtensa_ae_round32x2f64ssym; break;
  case Xtensa::BI__builtin_xtensa_ae_roundsp16q48x2asym:
    ID = Intrinsic::xtensa_ae_roundsp16q48x2asym; break;
  case Xtensa::BI__builtin_xtensa_ae_roundsp16q48x2sym:
    ID = Intrinsic::xtensa_ae_roundsp16q48x2sym; break;
  case Xtensa::BI__builtin_xtensa_ae_roundsp16f24asym:
    ID = Intrinsic::xtensa_ae_roundsp16f24asym; break;
  case Xtensa::BI__builtin_xtensa_ae_roundsp16f24sym:
    ID = Intrinsic::xtensa_ae_roundsp16f24sym; break;
  case Xtensa::BI__builtin_xtensa_ae_roundsq32f48asym:
    ID = Intrinsic::xtensa_ae_roundsq32f48asym; break;
  case Xtensa::BI__builtin_xtensa_ae_roundsq32f48sym:
    ID = Intrinsic::xtensa_ae_roundsq32f48sym; break;
  case Xtensa::BI__builtin_xtensa_ae_maxabs32s:
    ID = Intrinsic::xtensa_ae_maxabs32s; break;
  case Xtensa::BI__builtin_xtensa_ae_sat16x4:
    ID = Intrinsic::xtensa_ae_sat16x4; break;
  case Xtensa::BI__builtin_xtensa_ae_sat24s:
    ID = Intrinsic::xtensa_ae_sat24s; break;
  case Xtensa::BI__builtin_xtensa_ae_round32f48sasym: {
    llvm::Type *V2I32Ty = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    Value *Op = Builder.CreateBitCast(Ops[0], V2I32Ty);
    Function *F = CGM.getIntrinsic(Intrinsic::xtensa_ae_round32x2f48sasym);
    Value *Call = Builder.CreateCall(F, {Op, Op});
    // Extract low element (index 0) from the v2i32 result
    return Builder.CreateExtractElement(Call, uint64_t(0));
  }
  case Xtensa::BI__builtin_xtensa_ae_round32f48ssym: {
    llvm::Type *V2I32Ty = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    Value *Op = Builder.CreateBitCast(Ops[0], V2I32Ty);
    Function *F = CGM.getIntrinsic(Intrinsic::xtensa_ae_round32x2f48ssym);
    Value *Call = Builder.CreateCall(F, {Op, Op});
    return Builder.CreateExtractElement(Call, uint64_t(0));
  }

  case Xtensa::BI__builtin_xtensa_ae_mulafd32x16x2_fir_hh:
  case Xtensa::BI__builtin_xtensa_ae_mulafd32x16x2_fir_hl:
  case Xtensa::BI__builtin_xtensa_ae_mulafd32x16x2_fir_lh:
  case Xtensa::BI__builtin_xtensa_ae_mulafd32x16x2_fir_ll: {
    unsigned IntrinsicID;
    switch (BuiltinID) {
    case Xtensa::BI__builtin_xtensa_ae_mulafd32x16x2_fir_hh: IntrinsicID = Intrinsic::xtensa_ae_mulafd32x16x2_fir_hh; break;
    case Xtensa::BI__builtin_xtensa_ae_mulafd32x16x2_fir_hl: IntrinsicID = Intrinsic::xtensa_ae_mulafd32x16x2_fir_hl; break;
    case Xtensa::BI__builtin_xtensa_ae_mulafd32x16x2_fir_lh: IntrinsicID = Intrinsic::xtensa_ae_mulafd32x16x2_fir_lh; break;
    case Xtensa::BI__builtin_xtensa_ae_mulafd32x16x2_fir_ll: IntrinsicID = Intrinsic::xtensa_ae_mulafd32x16x2_fir_ll; break;
    default: llvm_unreachable("Unexpected builtin");
    }
    Address BAddr = EmitPointerWithAlignment(E->getArg(0));
    Address AAddr = EmitPointerWithAlignment(E->getArg(1));
    Value *B = Builder.CreateLoad(BAddr);
    Value *A = Builder.CreateLoad(AAddr);
    // Bridge i64 -> v2i32 for accumulator args
    llvm::Type *V2I32Ty = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    llvm::Type *V4I16Ty = llvm::FixedVectorType::get(Builder.getInt16Ty(), 4);
    B = Builder.CreateBitCast(B, V2I32Ty);
    A = Builder.CreateBitCast(A, V2I32Ty);
    // Bridge data args: Ops[2]=d0 (v2i32), Ops[3]=d1 (v2i32), Ops[4]=coefs (v4i16)
    Value *D0 = Builder.CreateBitCast(Ops[2], V2I32Ty);
    Value *D1 = Builder.CreateBitCast(Ops[3], V2I32Ty);
    Value *Coefs = Builder.CreateBitCast(Ops[4], V4I16Ty);
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Call = Builder.CreateCall(F, {B, A, D0, D1, Coefs});
    Value *BOut = Builder.CreateExtractValue(Call, 0);
    Value *AOut = Builder.CreateExtractValue(Call, 1);
    // Bridge v2i32 -> i64 for store back
    Builder.CreateStore(Builder.CreateBitCast(BOut, Builder.getInt64Ty()), BAddr);
    Builder.CreateStore(Builder.CreateBitCast(AOut, Builder.getInt64Ty()), AAddr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  case Xtensa::BI__builtin_xtensa_ae_mula2q32x16_fir_h: {
    llvm::Type *V2I32Ty = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    llvm::Type *V4I16Ty = llvm::FixedVectorType::get(Builder.getInt16Ty(), 4);
    Address AAddr = EmitPointerWithAlignment(E->getArg(0));
    Value *A = Builder.CreateBitCast(Builder.CreateLoad(AAddr), V2I32Ty);
    Value *Op1 = Builder.CreateBitCast(Ops[1], V2I32Ty);
    Value *Op2 = Builder.CreateBitCast(Ops[2], V4I16Ty);
    Function *F = CGM.getIntrinsic(Intrinsic::xtensa_ae_mula2q32x16_fir_h);
    Value *Call = Builder.CreateCall(F, {A, Op1, Op2});
    Builder.CreateStore(Builder.CreateBitCast(Call, Builder.getInt64Ty()), AAddr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  case Xtensa::BI__builtin_xtensa_ae_mulaaf2d32ra_hh_ll: {
    llvm::Type *V2I32Ty = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    Address BAddr = EmitPointerWithAlignment(E->getArg(0));
    Address AAddr = EmitPointerWithAlignment(E->getArg(1));
    Value *B = Builder.CreateBitCast(Builder.CreateLoad(BAddr), V2I32Ty);
    Value *A = Builder.CreateBitCast(Builder.CreateLoad(AAddr), V2I32Ty);
    SmallVector<Value *, 6> Args = {B, A};
    for (unsigned i = 2; i <= 5; ++i)
      Args.push_back(Builder.CreateBitCast(Ops[i], V2I32Ty));
    Function *F = CGM.getIntrinsic(Intrinsic::xtensa_ae_mulaaf2d32ra_hh_ll);
    Value *Call = Builder.CreateCall(F, Args);
    Value *BOut = Builder.CreateExtractValue(Call, 0);
    Value *AOut = Builder.CreateExtractValue(Call, 1);
    Builder.CreateStore(Builder.CreateBitCast(BOut, Builder.getInt64Ty()), BAddr);
    Builder.CreateStore(Builder.CreateBitCast(AOut, Builder.getInt64Ty()), AAddr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  case Xtensa::BI__builtin_xtensa_ae_add64:
    ID = Intrinsic::xtensa_ae_add64; break;
  case Xtensa::BI__builtin_xtensa_ae_add64s:
    ID = Intrinsic::xtensa_ae_add64s; break;
  case Xtensa::BI__builtin_xtensa_ae_sub64:
    ID = Intrinsic::xtensa_ae_sub64; break;

  case Xtensa::BI__builtin_xtensa_ae_and64:
    ID = Intrinsic::xtensa_ae_and64; break;
  case Xtensa::BI__builtin_xtensa_ae_or64:
    ID = Intrinsic::xtensa_ae_or64; break;
  case Xtensa::BI__builtin_xtensa_ae_xor64:
    ID = Intrinsic::xtensa_ae_xor64; break;

  case Xtensa::BI__builtin_xtensa_ae_mul16x4: {
    llvm::Type *V2I32Ty = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    llvm::Type *V4I16Ty = llvm::FixedVectorType::get(Builder.getInt16Ty(), 4);
    Address Q0Addr = EmitPointerWithAlignment(E->getArg(0));
    Address Q1Addr = EmitPointerWithAlignment(E->getArg(1));
    Value *Q0 = Builder.CreateBitCast(Builder.CreateLoad(Q0Addr), V2I32Ty);
    Value *Q1 = Builder.CreateBitCast(Builder.CreateLoad(Q1Addr), V2I32Ty);
    Value *A = Builder.CreateBitCast(Ops[2], V4I16Ty);
    Value *B = Builder.CreateBitCast(Ops[3], V4I16Ty);
    Function *F = CGM.getIntrinsic(Intrinsic::xtensa_ae_mul16x4);
    Value *Call = Builder.CreateCall(F, {Q0, Q1, A, B});
    Value *Q0Out = Builder.CreateExtractValue(Call, 0);
    Value *Q1Out = Builder.CreateExtractValue(Call, 1);
    Builder.CreateStore(Builder.CreateBitCast(Q0Out, Builder.getInt64Ty()), Q0Addr);
    Builder.CreateStore(Builder.CreateBitCast(Q1Out, Builder.getInt64Ty()), Q1Addr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  case Xtensa::BI__builtin_xtensa_ae_mul16:
    ID = Intrinsic::xtensa_ae_mul16; break;

  // --- ae_addandsub32s: multi-return (sum, diff) ---
  case Xtensa::BI__builtin_xtensa_ae_addandsub32s: {
    llvm::Type *V2I32Ty = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    Address SumAddr = EmitPointerWithAlignment(E->getArg(0));
    Address DiffAddr = EmitPointerWithAlignment(E->getArg(1));
    Value *A = Builder.CreateBitCast(Ops[2], V2I32Ty);
    Value *B = Builder.CreateBitCast(Ops[3], V2I32Ty);
    Function *F = CGM.getIntrinsic(Intrinsic::xtensa_ae_addandsub32s);
    Value *Call = Builder.CreateCall(F, {A, B});
    Value *Sum = Builder.CreateExtractValue(Call, 0);
    Value *Diff = Builder.CreateExtractValue(Call, 1);
    Builder.CreateStore(Builder.CreateBitCast(Sum, Builder.getInt64Ty()), SumAddr);
    Builder.CreateStore(Builder.CreateBitCast(Diff, Builder.getInt64Ty()), DiffAddr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  // --- ae_l16x2m_xu: post-update load ---
  case Xtensa::BI__builtin_xtensa_ae_l16x2m_xu:
    ID = Intrinsic::xtensa_ae_l16x2m_xu; IsPostUpdateLoad = true; break;

  // --- Simple passthrough intrinsics ---
  case Xtensa::BI__builtin_xtensa_ae_sel16i:
    ID = Intrinsic::xtensa_ae_sel16i; break;
  case Xtensa::BI__builtin_xtensa_ae_l64_i:
    ID = Intrinsic::xtensa_ae_l64_i; break;
  case Xtensa::BI__builtin_xtensa_ae_s64_i:
    ID = Intrinsic::xtensa_ae_s64_i; break;
  case Xtensa::BI__builtin_xtensa_ae_zalign64:
    ID = Intrinsic::xtensa_ae_zalign64; break;
  case Xtensa::BI__builtin_xtensa_ae_movda16:
    ID = Intrinsic::xtensa_ae_movda16; break;
  case Xtensa::BI__builtin_xtensa_ae_s32x2f24_i:
    ID = Intrinsic::xtensa_ae_s32x2f24_i; break;
  case Xtensa::BI__builtin_xtensa_ae_s32x2f24_ip:
    ID = Intrinsic::xtensa_ae_s32x2f24_ip; IsPostUpdateStore = true; break;
  case Xtensa::BI__builtin_xtensa_ae_clamps16:
    ID = Intrinsic::xtensa_ae_clamps16; break;
  case Xtensa::BI__builtin_xtensa_ae_sext16:
    ID = Intrinsic::xtensa_ae_sext16; break;
  case Xtensa::BI__builtin_xtensa_ae_zext16:
    ID = Intrinsic::xtensa_ae_zext16; break;
  case Xtensa::BI__builtin_xtensa_ae_movab2:
    ID = Intrinsic::xtensa_ae_movab2; break;


  // =========================================================================
  // HiFi5 128-bit Loads (dual-output via pointers + ptr update)
  // =========================================================================
  case Xtensa::BI__builtin_xtensa_ae_la32x2x2_ip:
  case Xtensa::BI__builtin_xtensa_ae_la16x4x2_ip: {
    unsigned IntrinsicID;
    if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_la32x2x2_ip)
      IntrinsicID = Intrinsic::xtensa_ae_la32x2x2_ip;
    else
      IntrinsicID = Intrinsic::xtensa_ae_la16x4x2_ip;

    Address Out0Addr = EmitPointerWithAlignment(E->getArg(0));
    Address Out1Addr = EmitPointerWithAlignment(E->getArg(1));
    // Ops[2] = align (ignored at IR level), Ops[3] = ptr*
    Address PtrAddr = EmitPointerWithAlignment(E->getArg(3));
    Value *Ptr = Builder.CreateLoad(PtrAddr);

    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Call = Builder.CreateCall(F, {Ptr});

    Value *Val0 = Builder.CreateExtractValue(Call, 0);
    Value *Val1 = Builder.CreateExtractValue(Call, 1);
    Value *PtrPost = Builder.CreateExtractValue(Call, 2);

    Builder.CreateStore(Builder.CreateBitCast(Val0, Builder.getInt64Ty()), Out0Addr);
    Builder.CreateStore(Builder.CreateBitCast(Val1, Builder.getInt64Ty()), Out1Addr);
    Builder.CreateStore(PtrPost, PtrAddr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  case Xtensa::BI__builtin_xtensa_ae_l32x2x2_xc:
  case Xtensa::BI__builtin_xtensa_ae_l32x2x2_xc1: {
    unsigned IntrinsicID;
    if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_l32x2x2_xc)
      IntrinsicID = Intrinsic::xtensa_ae_l32x2x2_xc;
    else
      IntrinsicID = Intrinsic::xtensa_ae_l32x2x2_xc1;

    Address Out0Addr = EmitPointerWithAlignment(E->getArg(0));
    Address Out1Addr = EmitPointerWithAlignment(E->getArg(1));
    Address PtrAddr = EmitPointerWithAlignment(E->getArg(2));
    Value *Ptr = Builder.CreateLoad(PtrAddr);

    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Call = Builder.CreateCall(F, {Ptr, Ops[3]});

    Value *Val0 = Builder.CreateExtractValue(Call, 0);
    Value *Val1 = Builder.CreateExtractValue(Call, 1);
    Value *PtrPost = Builder.CreateExtractValue(Call, 2);

    Builder.CreateStore(Builder.CreateBitCast(Val0, Builder.getInt64Ty()), Out0Addr);
    Builder.CreateStore(Builder.CreateBitCast(Val1, Builder.getInt64Ty()), Out1Addr);
    Builder.CreateStore(PtrPost, PtrAddr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  // =========================================================================
  // HiFi5 128-bit Stores (dual-input + ptr update)
  // =========================================================================
  case Xtensa::BI__builtin_xtensa_ae_sa32x2x2_ip:
  case Xtensa::BI__builtin_xtensa_ae_sa16x4x2_ip: {
    unsigned IntrinsicID;
    llvm::Type *VecTy;
    if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_sa32x2x2_ip) {
      IntrinsicID = Intrinsic::xtensa_ae_sa32x2x2_ip;
      VecTy = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    } else {
      IntrinsicID = Intrinsic::xtensa_ae_sa16x4x2_ip;
      VecTy = llvm::FixedVectorType::get(Builder.getInt16Ty(), 4);
    }
    Value *In0 = Builder.CreateBitCast(Ops[0], VecTy);
    Value *In1 = Builder.CreateBitCast(Ops[1], VecTy);
    // Ops[2] = align (ignored), Ops[3] = ptr*
    Address PtrAddr = EmitPointerWithAlignment(E->getArg(3));
    Value *Ptr = Builder.CreateLoad(PtrAddr);

    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *PtrPost = Builder.CreateCall(F, {In0, In1, Ptr});

    Builder.CreateStore(PtrPost, PtrAddr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  case Xtensa::BI__builtin_xtensa_ae_s32x2x2_xc1: {
    llvm::Type *V2I32Ty = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    Value *In0 = Builder.CreateBitCast(Ops[0], V2I32Ty);
    Value *In1 = Builder.CreateBitCast(Ops[1], V2I32Ty);
    Address PtrAddr = EmitPointerWithAlignment(E->getArg(2));
    Value *Ptr = Builder.CreateLoad(PtrAddr);

    Function *F = CGM.getIntrinsic(Intrinsic::xtensa_ae_s32x2x2_xc1);
    Value *PtrPost = Builder.CreateCall(F, {In0, In1, Ptr, Ops[3]});

    Builder.CreateStore(PtrPost, PtrAddr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  // =========================================================================
  // HiFi5 MACs (dual-output via pointers)
  // =========================================================================
  case Xtensa::BI__builtin_xtensa_ae_mula2q32x16_fir_h5: {
    llvm::Type *V2I32Ty = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    llvm::Type *V4I16Ty = llvm::FixedVectorType::get(Builder.getInt16Ty(), 4);
    Address Out0Addr = EmitPointerWithAlignment(E->getArg(0));
    Address Out1Addr = EmitPointerWithAlignment(E->getArg(1));
    Value *Out0 = Builder.CreateBitCast(Builder.CreateLoad(Out0Addr), V2I32Ty);
    Value *Out1 = Builder.CreateBitCast(Builder.CreateLoad(Out1Addr), V2I32Ty);
    Value *D0 = Builder.CreateBitCast(Ops[2], V2I32Ty);
    Value *D1 = Builder.CreateBitCast(Ops[3], V2I32Ty);
    Value *D2 = Builder.CreateBitCast(Ops[4], V2I32Ty);
    Value *Coefs = Builder.CreateBitCast(Ops[5], V4I16Ty);
    Function *F = CGM.getIntrinsic(Intrinsic::xtensa_ae_mula2q32x16_fir_h5);
    Value *Call = Builder.CreateCall(F, {Out0, Out1, D0, D1, D2, Coefs});
    Value *R0 = Builder.CreateExtractValue(Call, 0);
    Value *R1 = Builder.CreateExtractValue(Call, 1);
    Builder.CreateStore(Builder.CreateBitCast(R0, Builder.getInt64Ty()), Out0Addr);
    Builder.CreateStore(Builder.CreateBitCast(R1, Builder.getInt64Ty()), Out1Addr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  case Xtensa::BI__builtin_xtensa_ae_mulf32x2r_hh_ll: {
    llvm::Type *V2I32Ty = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    Address Out0Addr = EmitPointerWithAlignment(E->getArg(0));
    Address Out1Addr = EmitPointerWithAlignment(E->getArg(1));
    Value *In0 = Builder.CreateBitCast(Ops[2], V2I32Ty);
    Value *In1 = Builder.CreateBitCast(Ops[3], V2I32Ty);
    Function *F = CGM.getIntrinsic(Intrinsic::xtensa_ae_mulf32x2r_hh_ll);
    Value *Call = Builder.CreateCall(F, {In0, In1});
    Value *R0 = Builder.CreateExtractValue(Call, 0);
    Value *R1 = Builder.CreateExtractValue(Call, 1);
    Builder.CreateStore(Builder.CreateBitCast(R0, Builder.getInt64Ty()), Out0Addr);
    Builder.CreateStore(Builder.CreateBitCast(R1, Builder.getInt64Ty()), Out1Addr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  case Xtensa::BI__builtin_xtensa_ae_mulf2p32x16x4rs: {
    llvm::Type *V2I32Ty = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    llvm::Type *V4I16Ty = llvm::FixedVectorType::get(Builder.getInt16Ty(), 4);
    Address Out0Addr = EmitPointerWithAlignment(E->getArg(0));
    Address Out1Addr = EmitPointerWithAlignment(E->getArg(1));
    Value *In0 = Builder.CreateBitCast(Ops[2], V2I32Ty);
    Value *In1 = Builder.CreateBitCast(Ops[3], V2I32Ty);
    Value *In2 = Builder.CreateBitCast(Ops[4], V4I16Ty);
    Function *F = CGM.getIntrinsic(Intrinsic::xtensa_ae_mulf2p32x16x4rs);
    Value *Call = Builder.CreateCall(F, {In0, In1, In2});
    Value *R0 = Builder.CreateExtractValue(Call, 0);
    Value *R1 = Builder.CreateExtractValue(Call, 1);
    Builder.CreateStore(Builder.CreateBitCast(R0, Builder.getInt64Ty()), Out0Addr);
    Builder.CreateStore(Builder.CreateBitCast(R1, Builder.getInt64Ty()), Out1Addr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  case Xtensa::BI__builtin_xtensa_ae_mulf2p32x4rs: {
    llvm::Type *V2I32Ty = llvm::FixedVectorType::get(Builder.getInt32Ty(), 2);
    Address Out0Addr = EmitPointerWithAlignment(E->getArg(0));
    Address Out1Addr = EmitPointerWithAlignment(E->getArg(1));
    Value *In0 = Builder.CreateBitCast(Ops[2], V2I32Ty);
    Value *In1 = Builder.CreateBitCast(Ops[3], V2I32Ty);
    Value *In2 = Builder.CreateBitCast(Ops[4], V2I32Ty);
    Value *In3 = Builder.CreateBitCast(Ops[5], V2I32Ty);
    Function *F = CGM.getIntrinsic(Intrinsic::xtensa_ae_mulf2p32x4rs);
    Value *Call = Builder.CreateCall(F, {In0, In1, In2, In3});
    Value *R0 = Builder.CreateExtractValue(Call, 0);
    Value *R1 = Builder.CreateExtractValue(Call, 1);
    Builder.CreateStore(Builder.CreateBitCast(R0, Builder.getInt64Ty()), Out0Addr);
    Builder.CreateStore(Builder.CreateBitCast(R1, Builder.getInt64Ty()), Out1Addr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  default: return nullptr;
  // Circular Base Loads
  case Xtensa::BI__builtin_xtensa_ae_l16_xc:
    ID = Intrinsic::xtensa_ae_l16_xc; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l32_xc:
    ID = Intrinsic::xtensa_ae_l32_xc; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l32_xc1:
    ID = Intrinsic::xtensa_ae_l32_xc1; IsPostUpdateLoad = true; break;

  // Post-update loads
  case Xtensa::BI__builtin_xtensa_ae_l16_ip:
    ID = Intrinsic::xtensa_ae_l16_ip; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l16_xp:
    ID = Intrinsic::xtensa_ae_l16_xp; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l16x4_ip:
    ID = Intrinsic::xtensa_ae_l16x4_ip; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l16x4_xp:
    ID = Intrinsic::xtensa_ae_l16x4_xp; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l32_ip:
    ID = Intrinsic::xtensa_ae_l32_ip; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l32_xp:
    ID = Intrinsic::xtensa_ae_l32_xp; IsPostUpdateLoad = true; break;

  case Xtensa::BI__builtin_xtensa_ae_l16m_xu:
    ID = Intrinsic::xtensa_ae_l16m_xu; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l32f24_xc:
    ID = Intrinsic::xtensa_ae_l32f24_xc; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l32x2f24_ip:
    ID = Intrinsic::xtensa_ae_l32x2f24_ip; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l32x2f24_xc:
    ID = Intrinsic::xtensa_ae_l32x2f24_xc; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l32x2_ip:
    ID = Intrinsic::xtensa_ae_l32x2_ip; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l32x2_xc:
    ID = Intrinsic::xtensa_ae_l32x2_xc; IsPostUpdateLoad = true; break;
  case Xtensa::BI__builtin_xtensa_ae_l32x2_xc1:
    ID = Intrinsic::xtensa_ae_l32x2_xc1; IsPostUpdateLoad = true; break;

  // Alignment & State Loads
  case Xtensa::BI__builtin_xtensa_ae_la64_pp:
  case Xtensa::BI__builtin_xtensa_ae_la128_pp: {
    Address PtrAddr = EmitPointerWithAlignment(E->getArg(0));
    Value *Ptr = Builder.CreateLoad(PtrAddr);

    Function *F;
    if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_la64_pp)
      F = CGM.getIntrinsic(Intrinsic::xtensa_ae_la64_pp);
    else
      F = CGM.getIntrinsic(Intrinsic::xtensa_ae_la128_pp);

    Value *Call = Builder.CreateCall(F, {Ptr});

    Builder.CreateStore(Call, PtrAddr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }
  case Xtensa::BI__builtin_xtensa_ae_la16x4pos_pc:
  case Xtensa::BI__builtin_xtensa_ae_la32x2pos_pc:
  case Xtensa::BI__builtin_xtensa_ae_sa64pos_fp:
  case Xtensa::BI__builtin_xtensa_ae_sa128pos_fp: {
    unsigned IntrinsicID;
    if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_la16x4pos_pc)
      IntrinsicID = Intrinsic::xtensa_ae_la16x4pos_pc;
    else if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_la32x2pos_pc)
      IntrinsicID = Intrinsic::xtensa_ae_la32x2pos_pc;
    else if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_sa64pos_fp)
      IntrinsicID = Intrinsic::xtensa_ae_sa64pos_fp;
    else
      IntrinsicID = Intrinsic::xtensa_ae_sa128pos_fp;

    Value *Ptr = EmitScalarExpr(E->getArg(0));
    Function *F = CGM.getIntrinsic(IntrinsicID);
    Builder.CreateCall(F, {Ptr});

    return llvm::UndefValue::get(CGM.VoidTy);
  }
  case Xtensa::BI__builtin_xtensa_ae_la32x2_ip:
  case Xtensa::BI__builtin_xtensa_ae_la16x4_ip:
  case Xtensa::BI__builtin_xtensa_ae_la24x2_ip:
  case Xtensa::BI__builtin_xtensa_ae_la24_ip: {
    unsigned IntrinsicID;
    if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_la32x2_ip)
      IntrinsicID = Intrinsic::xtensa_ae_la32x2_ip;
    else if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_la16x4_ip)
      IntrinsicID = Intrinsic::xtensa_ae_la16x4_ip;
    else if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_la24x2_ip)
      IntrinsicID = Intrinsic::xtensa_ae_la24x2_ip;
    else
      IntrinsicID = Intrinsic::xtensa_ae_la24_ip;

    Address PtrAddr = EmitPointerWithAlignment(E->getArg(1));
    Value *Ptr = Builder.CreateLoad(PtrAddr);

    Function *F = CGM.getIntrinsic(IntrinsicID);
    Value *Call = Builder.CreateCall(F, {Ptr});

    Value *LoadedVal = Builder.CreateExtractValue(Call, 0, "val");
    Value *UpdatedPtr = Builder.CreateExtractValue(Call, 1, "ptr.post");

    // Bitcast vector result back to i64 for C type compatibility
    if (LoadedVal->getType()->isVectorTy())
      LoadedVal = Builder.CreateBitCast(LoadedVal, Builder.getInt64Ty());

    Builder.CreateStore(UpdatedPtr, PtrAddr);
    return LoadedVal;
  }
  case Xtensa::BI__builtin_xtensa_ae_sa16x4_ip:
  case Xtensa::BI__builtin_xtensa_ae_sa16x4_ic:
  case Xtensa::BI__builtin_xtensa_ae_sa32x2_ip:
  case Xtensa::BI__builtin_xtensa_ae_sa24x2_ip:
  case Xtensa::BI__builtin_xtensa_ae_sa32x2_ic: {
    unsigned IntrinsicID;
    if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_sa16x4_ip)
      IntrinsicID = Intrinsic::xtensa_ae_sa16x4_ip;
    else if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_sa16x4_ic)
      IntrinsicID = Intrinsic::xtensa_ae_sa16x4_ic;
    else if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_sa32x2_ip)
      IntrinsicID = Intrinsic::xtensa_ae_sa32x2_ip;
    else if (BuiltinID == Xtensa::BI__builtin_xtensa_ae_sa24x2_ip)
      IntrinsicID = Intrinsic::xtensa_ae_sa24x2_ip;
    else
      IntrinsicID = Intrinsic::xtensa_ae_sa32x2_ic;

    Address PtrAddr = EmitPointerWithAlignment(E->getArg(2));
    Value *Ptr = Builder.CreateLoad(PtrAddr);

    Function *F = CGM.getIntrinsic(IntrinsicID);
    // Bridge i64 -> vector for the data argument
    Value *DataArg = Ops[0];
    llvm::Type *ExpectedTy = F->getFunctionType()->getParamType(0);
    if (DataArg->getType() != ExpectedTy && DataArg->getType()->isIntegerTy(64))
      DataArg = Builder.CreateBitCast(DataArg, ExpectedTy);
    Value *Call = Builder.CreateCall(F, {DataArg, Ptr});

    Builder.CreateStore(Call, PtrAddr);
    return llvm::UndefValue::get(CGM.VoidTy);
  }

  // Circular Base Stores
  case Xtensa::BI__builtin_xtensa_ae_s16_0_xc:
    ID = Intrinsic::xtensa_ae_s16_0_xc; IsPostUpdateStore = true; break;
  case Xtensa::BI__builtin_xtensa_ae_s16_0_xc1:
    ID = Intrinsic::xtensa_ae_s16_0_xc1; IsPostUpdateStore = true; break;
  case Xtensa::BI__builtin_xtensa_ae_s32_l_xc:
    ID = Intrinsic::xtensa_ae_s32_l_xc; IsPostUpdateStore = true; break;
  case Xtensa::BI__builtin_xtensa_ae_s32_l_xc1:
    ID = Intrinsic::xtensa_ae_s32_l_xc1; IsPostUpdateStore = true; break;
  case Xtensa::BI__builtin_xtensa_ae_s32x2_xc:
    ID = Intrinsic::xtensa_ae_s32x2_xc; IsPostUpdateStore = true; break;
  case Xtensa::BI__builtin_xtensa_ae_s32x2_xc1:
    ID = Intrinsic::xtensa_ae_s32x2_xc1; IsPostUpdateStore = true; break;

  // Post-increment Stores
  case Xtensa::BI__builtin_xtensa_ae_s32_l_ip:
    ID = Intrinsic::xtensa_ae_s32_l_ip; IsPostUpdateStore = true; break;
  case Xtensa::BI__builtin_xtensa_ae_s32x2_ip:
    ID = Intrinsic::xtensa_ae_s32x2_ip; IsPostUpdateStore = true; break;
  case Xtensa::BI__builtin_xtensa_ae_s16_0_ip:
    ID = Intrinsic::xtensa_ae_s16_0_ip; IsPostUpdateStore = true; break;
  case Xtensa::BI__builtin_xtensa_ae_s16x4_ip:
    ID = Intrinsic::xtensa_ae_s16x4_ip; IsPostUpdateStore = true; break;
  case Xtensa::BI__builtin_xtensa_ae_s16_0_xp:
    ID = Intrinsic::xtensa_ae_s16_0_xp; IsPostUpdateStore = true; break;
  case Xtensa::BI__builtin_xtensa_ae_s32_l_xp:
    ID = Intrinsic::xtensa_ae_s32_l_xp; IsPostUpdateStore = true; break;

  // Standard Loads
  case Xtensa::BI__builtin_xtensa_ae_l16_i:
    ID = Intrinsic::xtensa_ae_l16_i;
    break;
  case Xtensa::BI__builtin_xtensa_ae_l32_i:
    ID = Intrinsic::xtensa_ae_l32_i;
    break;
  case Xtensa::BI__builtin_xtensa_ae_l32_x:
    ID = Intrinsic::xtensa_ae_l32_x;
    break;
  case Xtensa::BI__builtin_xtensa_ae_l16x4_x:
    ID = Intrinsic::xtensa_ae_l16x4_x;
    break;

  case Xtensa::BI__builtin_xtensa_ae_l16m_x:
    ID = Intrinsic::xtensa_ae_l16m_x;
    break;
  case Xtensa::BI__builtin_xtensa_ae_l32m_x:
    ID = Intrinsic::xtensa_ae_l32m_x;
    break;
  case Xtensa::BI__builtin_xtensa_ae_l32x2_i:
    ID = Intrinsic::xtensa_ae_l32x2_i;
    break;

  // Standard Stores
  case Xtensa::BI__builtin_xtensa_ae_s16_0_x:
    ID = Intrinsic::xtensa_ae_s16_0_x;
    break;
  case Xtensa::BI__builtin_xtensa_ae_s32_h_i:
    ID = Intrinsic::xtensa_ae_s32_h_i;
    break;
  case Xtensa::BI__builtin_xtensa_ae_s16_i:
    ID = Intrinsic::xtensa_ae_s16_i;
    break;
  case Xtensa::BI__builtin_xtensa_ae_s32_l_i:
    ID = Intrinsic::xtensa_ae_s32_l_i;
    break;
  case Xtensa::BI__builtin_xtensa_ae_s32x2_i:
    ID = Intrinsic::xtensa_ae_s32x2_i;
    break;
  case Xtensa::BI__builtin_xtensa_ae_s16x2m_i:
    ID = Intrinsic::xtensa_ae_s16x2m_i;
    break;
  case Xtensa::BI__builtin_xtensa_ae_s32_l_x:
    ID = Intrinsic::xtensa_ae_s32_l_x;
    break;
  case Xtensa::BI__builtin_xtensa_ae_s32x2_x:
    ID = Intrinsic::xtensa_ae_s32x2_x;
    break;
  }

  // Generic bitcast bridge: builtins use i64 (LLi) for all ae_ types,
  // but LLVM intrinsics use v2i32/v4i16. Bitcast args and results as needed.
  auto BitcastOpsForIntrinsic = [&](Function *F, SmallVectorImpl<Value *> &Ops) {
    llvm::FunctionType *FTy = F->getFunctionType();
    for (unsigned i = 0, e = Ops.size(); i < e; ++i) {
      if (i < FTy->getNumParams()) {
        llvm::Type *ExpectedTy = FTy->getParamType(i);
        if (Ops[i]->getType() != ExpectedTy &&
            Ops[i]->getType()->isIntegerTy(64) &&
            ExpectedTy->isVectorTy()) {
          Ops[i] = Builder.CreateBitCast(Ops[i], ExpectedTy);
        }
      }
    }
  };

  auto BitcastResult = [&](Value *Result) -> Value * {
    if (Result->getType()->isVectorTy()) {
      return Builder.CreateBitCast(Result, Builder.getInt64Ty());
    }
    return Result;
  };

  if (IsPostUpdateLoad) {
    Address PtrAddr = EmitPointerWithAlignment(E->getArg(0));
    Value *Ptr = Builder.CreateLoad(PtrAddr);
    Ops[0] = Ptr;

    Function *F = CGM.getIntrinsic(ID);
    BitcastOpsForIntrinsic(F, Ops);
    Value *Call = Builder.CreateCall(F, Ops);

    Value *LoadedVal = Builder.CreateExtractValue(Call, 0, "val");
    Value *UpdatedPtr = Builder.CreateExtractValue(Call, 1, "ptr.post");

    Builder.CreateStore(UpdatedPtr, PtrAddr);
    return BitcastResult(LoadedVal);
  }

  if (IsPostUpdateStore) {
    Address PtrAddr = EmitPointerWithAlignment(E->getArg(1));
    Value *Ptr = Builder.CreateLoad(PtrAddr);
    Ops[1] = Ptr;

    Function *F = CGM.getIntrinsic(ID);
    BitcastOpsForIntrinsic(F, Ops);
    Value *UpdatedPtr = Builder.CreateCall(F, Ops);

    Builder.CreateStore(UpdatedPtr, PtrAddr);
    return UpdatedPtr;
  }

  Function *F = CGM.getIntrinsic(ID);
  BitcastOpsForIntrinsic(F, Ops);
  Value *Result = Builder.CreateCall(F, Ops);
  return BitcastResult(Result);
}
