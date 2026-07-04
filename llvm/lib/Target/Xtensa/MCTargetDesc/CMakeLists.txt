add_llvm_component_library(LLVMXtensaDesc
  XtensaAsmBackend.cpp
  XtensaELFObjectWriter.cpp
  XtensaInstPrinter.cpp
  XtensaMCAsmInfo.cpp
  XtensaMCCodeEmitter.cpp
  XtensaMCTargetDesc.cpp
  XtensaTargetStreamer.cpp

  LINK_COMPONENTS
  MC
  Support
  XtensaInfo

  ADD_TO_COMPONENT
  Xtensa
 )

# Config-driven regeneration of the AE FLIX-bundle encoding tables.
#
# XtensaMCCodeEmitter.cpp encodes the bundle-only Audio Engine (HiFi) opcodes
# from per-core tables (<core>_enc.inc) generated from that core's Tensilica
# libisa data module.  The checked-in tables (ACE15 = HiFi4, TGL = cavs2.5
# HiFi3) are used by default.  To retarget (or reproduce) a table, pass the
# generic libisa half once plus the per-core data module:
#
#   -DXTENSA_LIBISA_DIR=<dir with xtensa-isa.c, xtensa-isa.h,      (generic MIT)
#                        xtensa-isa-internal.h>
#   -DXTENSA_MODULES_C_ACE15=<ACE15 core xtensa-modules.c>         (per-config)
#   -DXTENSA_MODULES_C_TGL=<Tiger Lake/cavs2.5 core xtensa-modules.c>
#
# When a -DXTENSA_MODULES_C_<CORE> is set, CMake builds the host enumerator,
# runs it to produce <core>.tsv, joins that with the LLVM .td asm strings via
# emit-inc.py to regenerate <core>_enc.inc in the build tree, and points the
# emitter's XTENSA_ENC_INC_<CORE> include at it.  See MCTargetDesc/gen/README.md.
set(_xtgen ${CMAKE_CURRENT_SOURCE_DIR}/gen)

# xtensa_gen_enc(<macro-suffix> <out-basename> <modules-c-path>)
function(xtensa_gen_enc suffix base modules_c)
  if(NOT XTENSA_LIBISA_DIR)
    message(FATAL_ERROR
      "XTENSA_MODULES_C_${suffix} is set but XTENSA_LIBISA_DIR (directory with "
      "xtensa-isa.c, xtensa-isa.h, xtensa-isa-internal.h) is not.")
  endif()
  string(TOLOWER "${suffix}" _lc)
  set(_tool ${CMAKE_CURRENT_BINARY_DIR}/xtensa-gen-enc-${_lc})
  set(_tsv  ${CMAKE_CURRENT_BINARY_DIR}/${_lc}.tsv)
  set(_inc  ${CMAKE_CURRENT_BINARY_DIR}/${base})

  # Build the enumerator with the host C compiler, out-of-band of the LLVM
  # target flags (third-party C, warnings silenced).
  add_custom_command(
    OUTPUT ${_tool}
    COMMAND ${CMAKE_C_COMPILER} -w
            -I${_xtgen} -I${XTENSA_LIBISA_DIR}
            ${XTENSA_LIBISA_DIR}/xtensa-isa.c ${modules_c}
            ${_xtgen}/gen-ace-enc.c -o ${_tool}
    DEPENDS ${_xtgen}/gen-ace-enc.c ${modules_c}
    COMMENT "Building Xtensa AE encoding-table enumerator (${_lc})"
    VERBATIM)
  add_custom_command(
    OUTPUT ${_tsv}
    COMMAND ${_tool} > ${_tsv}
    DEPENDS ${_tool}
    COMMENT "Enumerating Xtensa AE opcode encodings (${_lc})"
    VERBATIM)
  add_custom_command(
    OUTPUT ${_inc}
    COMMAND ${Python3_EXECUTABLE} ${_xtgen}/emit-inc.py
            ${CMAKE_CURRENT_SOURCE_DIR}/.. ${_tsv} ${_inc}
    DEPENDS ${_tsv} ${_xtgen}/emit-inc.py
    COMMENT "Generating ${base} from libisa"
    VERBATIM)
  add_custom_target(XtensaGenEnc_${suffix} DEPENDS ${_inc})
  add_dependencies(LLVMXtensaDesc XtensaGenEnc_${suffix})
  set_property(SOURCE XtensaMCCodeEmitter.cpp APPEND PROPERTY
    OBJECT_DEPENDS ${_inc})
  set_property(SOURCE XtensaMCCodeEmitter.cpp APPEND PROPERTY
    COMPILE_DEFINITIONS "XTENSA_ENC_INC_${suffix}=\"${_inc}\"")
endfunction()

if(XTENSA_MODULES_C_ACE15)
  xtensa_gen_enc(ACE15 ace15_enc.inc ${XTENSA_MODULES_C_ACE15})
endif()
if(XTENSA_MODULES_C_TGL)
  xtensa_gen_enc(TGL tgl_enc.inc ${XTENSA_MODULES_C_TGL})
endif()
if(XTENSA_MODULES_C_ACE30)
  xtensa_gen_enc(ACE30 ace30_enc.inc ${XTENSA_MODULES_C_ACE30})
endif()
if(XTENSA_MODULES_C_ACE40)
  xtensa_gen_enc(ACE40 ace40_enc.inc ${XTENSA_MODULES_C_ACE40})
endif()
