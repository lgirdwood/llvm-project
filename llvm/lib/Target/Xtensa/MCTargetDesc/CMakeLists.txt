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

# Config-driven regeneration of the AE FLIX-bundle encoding table.
#
# XtensaMCCodeEmitter.cpp encodes the bundle-only Audio Engine (HiFi) opcodes
# from a table (ace15_enc.inc) generated from the target core's Tensilica libisa
# data module.  The checked-in ace15_enc.inc targets the ACE15 core and is used
# by default.  To retarget (or reproduce) the table from another core, pass:
#
#   -DXTENSA_MODULES_C=<path to that core's xtensa-modules.c>   (per-config half)
#   -DXTENSA_LIBISA_DIR=<dir containing xtensa-isa.c,           (generic MIT half)
#                        xtensa-isa.h, xtensa-isa-internal.h>
#
# When XTENSA_MODULES_C is set, CMake builds the host enumerator, runs it to
# produce xt_table.tsv, joins that with the LLVM .td asm strings via emit-inc.py
# to regenerate ace15_enc.inc in the build tree, and points the emitter at it.
# See MCTargetDesc/gen/README.md.
if(XTENSA_MODULES_C)
  if(NOT XTENSA_LIBISA_DIR)
    message(FATAL_ERROR
      "XTENSA_MODULES_C is set but XTENSA_LIBISA_DIR (directory containing "
      "xtensa-isa.c, xtensa-isa.h, xtensa-isa-internal.h) is not.")
  endif()

  set(_xtgen ${CMAKE_CURRENT_SOURCE_DIR}/gen)
  set(_xttool ${CMAKE_CURRENT_BINARY_DIR}/xtensa-gen-ace-enc)
  set(_xttsv  ${CMAKE_CURRENT_BINARY_DIR}/xt_table.tsv)
  set(_xtinc  ${CMAKE_CURRENT_BINARY_DIR}/ace15_enc.inc)

  # Build the enumerator with the host C compiler, out-of-band of the LLVM
  # target flags (third-party C, warnings silenced).
  add_custom_command(
    OUTPUT ${_xttool}
    COMMAND ${CMAKE_C_COMPILER} -w
            -I${_xtgen} -I${XTENSA_LIBISA_DIR}
            ${XTENSA_LIBISA_DIR}/xtensa-isa.c
            ${XTENSA_MODULES_C}
            ${_xtgen}/gen-ace-enc.c
            -o ${_xttool}
    DEPENDS ${_xtgen}/gen-ace-enc.c ${XTENSA_MODULES_C}
    COMMENT "Building Xtensa AE encoding-table enumerator"
    VERBATIM)

  add_custom_command(
    OUTPUT ${_xttsv}
    COMMAND ${_xttool} > ${_xttsv}
    DEPENDS ${_xttool}
    COMMENT "Enumerating Xtensa AE opcode encodings"
    VERBATIM)

  add_custom_command(
    OUTPUT ${_xtinc}
    COMMAND ${Python3_EXECUTABLE} ${_xtgen}/emit-inc.py
            ${CMAKE_CURRENT_SOURCE_DIR}/.. ${_xttsv} ${_xtinc}
    DEPENDS ${_xttsv} ${_xtgen}/emit-inc.py
    COMMENT "Generating ace15_enc.inc from libisa"
    VERBATIM)

  add_custom_target(XtensaGenEncInc DEPENDS ${_xtinc})
  add_dependencies(LLVMXtensaDesc XtensaGenEncInc)
  set_source_files_properties(XtensaMCCodeEmitter.cpp PROPERTIES
    OBJECT_DEPENDS ${_xtinc}
    COMPILE_DEFINITIONS "XTENSA_ENC_INC=\"${_xtinc}\"")
endif()
