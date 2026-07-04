/* Minimal shim so the Tensilica libisa sources (xtensa-isa.c / xtensa-modules.c)
 * compile as a standalone host tool without the full binutils/GCC headers. */
#ifndef ANSIDECL_STUB_H
#define ANSIDECL_STUB_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define ATTRIBUTE_UNUSED __attribute__((__unused__))
#endif
