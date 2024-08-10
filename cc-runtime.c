#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
//===-- assembly.h - compiler-rt assembler support macros -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// 2024/01/21 - Modified by mintsuki for use inside cc-runtime
//
//===----------------------------------------------------------------------===//
//
// This file defines macros for use in compiler-rt assembler source.
// This file is not part of the interface of this library.
//
//===----------------------------------------------------------------------===//

#ifndef COMPILERRT_ASSEMBLY_H
#define COMPILERRT_ASSEMBLY_H

#define SEPARATOR ;

#define HIDDEN(name) .hidden name
#define LOCAL_LABEL(name) .L_##name
#define FILE_LEVEL_DIRECTIVE
#if defined(__arm__) || defined(__aarch64__)
#define SYMBOL_IS_FUNC(name) .type name,%function
#else
#define SYMBOL_IS_FUNC(name) .type name,@function
#endif
#define CONST_SECTION .section .rodata

#define NO_EXEC_STACK_DIRECTIVE .section .note.GNU-stack,"",%progbits

#if defined(__arm__) || defined(__aarch64__)
#define FUNC_ALIGN                                                             \
  .text SEPARATOR                                                              \
  .balign 16 SEPARATOR
#else
#define FUNC_ALIGN
#endif

// BTI and PAC gnu property note
#define NT_GNU_PROPERTY_TYPE_0 5
#define GNU_PROPERTY_AARCH64_FEATURE_1_AND 0xc0000000
#define GNU_PROPERTY_AARCH64_FEATURE_1_BTI 1
#define GNU_PROPERTY_AARCH64_FEATURE_1_PAC 2

#if defined(__ARM_FEATURE_BTI_DEFAULT)
#define BTI_FLAG GNU_PROPERTY_AARCH64_FEATURE_1_BTI
#else
#define BTI_FLAG 0
#endif

#if __ARM_FEATURE_PAC_DEFAULT & 3
#define PAC_FLAG GNU_PROPERTY_AARCH64_FEATURE_1_PAC
#else
#define PAC_FLAG 0
#endif

#define GNU_PROPERTY(type, value)                                              \
  .pushsection .note.gnu.property, "a" SEPARATOR                               \
  .p2align 3 SEPARATOR                                                         \
  .word 4 SEPARATOR                                                            \
  .word 16 SEPARATOR                                                           \
  .word NT_GNU_PROPERTY_TYPE_0 SEPARATOR                                       \
  .asciz "GNU" SEPARATOR                                                       \
  .word type SEPARATOR                                                         \
  .word 4 SEPARATOR                                                            \
  .word value SEPARATOR                                                        \
  .word 0 SEPARATOR                                                            \
  .popsection

#if BTI_FLAG != 0
#define BTI_C hint #34
#define BTI_J hint #36
#else
#define BTI_C
#define BTI_J
#endif

#if (BTI_FLAG | PAC_FLAG) != 0
#define GNU_PROPERTY_BTI_PAC                                                   \
  GNU_PROPERTY(GNU_PROPERTY_AARCH64_FEATURE_1_AND, BTI_FLAG | PAC_FLAG)
#else
#define GNU_PROPERTY_BTI_PAC
#endif

#if defined(__clang__) || defined(__GCC_HAVE_DWARF2_CFI_ASM)
#define CFI_START .cfi_startproc
#define CFI_END .cfi_endproc
#else
#define CFI_START
#define CFI_END
#endif

#if defined(__arm__)

// Determine actual [ARM][THUMB[1][2]] ISA using compiler predefined macros:
// - for '-mthumb -march=armv6' compiler defines '__thumb__'
// - for '-mthumb -march=armv7' compiler defines '__thumb__' and '__thumb2__'
#if defined(__thumb2__) || defined(__thumb__)
#define DEFINE_CODE_STATE .thumb SEPARATOR
#define DECLARE_FUNC_ENCODING    .thumb_func SEPARATOR
#if defined(__thumb2__)
#define USE_THUMB_2
#define IT(cond)  it cond
#define ITT(cond) itt cond
#define ITE(cond) ite cond
#else
#define USE_THUMB_1
#define IT(cond)
#define ITT(cond)
#define ITE(cond)
#endif // defined(__thumb__2)
#else // !defined(__thumb2__) && !defined(__thumb__)
#define DEFINE_CODE_STATE .arm SEPARATOR
#define DECLARE_FUNC_ENCODING
#define IT(cond)
#define ITT(cond)
#define ITE(cond)
#endif

#if defined(USE_THUMB_1) && defined(USE_THUMB_2)
#error "USE_THUMB_1 and USE_THUMB_2 can't be defined together."
#endif

#if defined(__ARM_ARCH_4T__) || __ARM_ARCH >= 5
#define ARM_HAS_BX
#endif
#if !defined(__ARM_FEATURE_CLZ) && !defined(USE_THUMB_1) &&  \
    (__ARM_ARCH >= 6 || (__ARM_ARCH == 5 && !defined(__ARM_ARCH_5__)))
#define __ARM_FEATURE_CLZ
#endif

#ifdef ARM_HAS_BX
#define JMP(r) bx r
#define JMPc(r, c) bx##c r
#else
#define JMP(r) mov pc, r
#define JMPc(r, c) mov##c pc, r
#endif

// pop {pc} can't switch Thumb mode on ARMv4T
#if __ARM_ARCH >= 5
#define POP_PC() pop {pc}
#else
#define POP_PC()                                                               \
  pop {ip};                                                                    \
  JMP(ip)
#endif

#if defined(USE_THUMB_2)
#define WIDE(op) op.w
#else
#define WIDE(op) op
#endif
#else // !defined(__arm)
#define DECLARE_FUNC_ENCODING
#define DEFINE_CODE_STATE
#endif

#define GLUE2_(a, b) a##b
#define GLUE(a, b) GLUE2_(a, b)
#define GLUE2(a, b) GLUE2_(a, b)
#define GLUE3_(a, b, c) a##b##c
#define GLUE3(a, b, c) GLUE3_(a, b, c)
#define GLUE4_(a, b, c, d) a##b##c##d
#define GLUE4(a, b, c, d) GLUE4_(a, b, c, d)

#ifndef SYMBOL_NAME
#define SYMBOL_NAME(name) GLUE(__USER_LABEL_PREFIX__, name)
#endif

#ifdef VISIBILITY_HIDDEN
#define DECLARE_SYMBOL_VISIBILITY(name)                                        \
  HIDDEN(SYMBOL_NAME(name)) SEPARATOR
#define DECLARE_SYMBOL_VISIBILITY_UNMANGLED(name) \
  HIDDEN(name) SEPARATOR
#else
#define DECLARE_SYMBOL_VISIBILITY(name)
#define DECLARE_SYMBOL_VISIBILITY_UNMANGLED(name)
#endif

#define DEFINE_COMPILERRT_FUNCTION(name)                                       \
  DEFINE_CODE_STATE                                                            \
  FILE_LEVEL_DIRECTIVE SEPARATOR                                               \
  .globl SYMBOL_NAME(name) SEPARATOR                                           \
  SYMBOL_IS_FUNC(SYMBOL_NAME(name)) SEPARATOR                                  \
  DECLARE_SYMBOL_VISIBILITY(name)                                              \
  DECLARE_FUNC_ENCODING                                                        \
  SYMBOL_NAME(name):

#define DEFINE_COMPILERRT_THUMB_FUNCTION(name)                                 \
  DEFINE_CODE_STATE                                                            \
  FILE_LEVEL_DIRECTIVE SEPARATOR                                               \
  .globl SYMBOL_NAME(name) SEPARATOR                                           \
  SYMBOL_IS_FUNC(SYMBOL_NAME(name)) SEPARATOR                                  \
  DECLARE_SYMBOL_VISIBILITY(name) SEPARATOR                                    \
  .thumb_func SEPARATOR                                                        \
  SYMBOL_NAME(name):

#define DEFINE_COMPILERRT_PRIVATE_FUNCTION(name)                               \
  DEFINE_CODE_STATE                                                            \
  FILE_LEVEL_DIRECTIVE SEPARATOR                                               \
  .globl SYMBOL_NAME(name) SEPARATOR                                           \
  SYMBOL_IS_FUNC(SYMBOL_NAME(name)) SEPARATOR                                  \
  HIDDEN(SYMBOL_NAME(name)) SEPARATOR                                          \
  DECLARE_FUNC_ENCODING                                                        \
  SYMBOL_NAME(name):

#define DEFINE_COMPILERRT_PRIVATE_FUNCTION_UNMANGLED(name)                     \
  DEFINE_CODE_STATE                                                            \
  .globl name SEPARATOR                                                        \
  SYMBOL_IS_FUNC(name) SEPARATOR                                               \
  HIDDEN(name) SEPARATOR                                                       \
  DECLARE_FUNC_ENCODING                                                        \
  name:

#define DEFINE_COMPILERRT_OUTLINE_FUNCTION_UNMANGLED(name)                     \
  DEFINE_CODE_STATE                                                            \
  FUNC_ALIGN                                                                   \
  .globl name SEPARATOR                                                        \
  SYMBOL_IS_FUNC(name) SEPARATOR                                               \
  DECLARE_SYMBOL_VISIBILITY_UNMANGLED(name) SEPARATOR                          \
  DECLARE_FUNC_ENCODING                                                        \
  name:                                                                        \
  SEPARATOR CFI_START                                                          \
  SEPARATOR BTI_C

#define DEFINE_COMPILERRT_FUNCTION_ALIAS(name, target)                         \
  .globl SYMBOL_NAME(name) SEPARATOR                                           \
  SYMBOL_IS_FUNC(SYMBOL_NAME(name)) SEPARATOR                                  \
  DECLARE_SYMBOL_VISIBILITY(name) SEPARATOR                                    \
  .set SYMBOL_NAME(name), SYMBOL_NAME(target) SEPARATOR

#if defined(__ARM_EABI__)
#define DEFINE_AEABI_FUNCTION_ALIAS(aeabi_name, name)                          \
  DEFINE_COMPILERRT_FUNCTION_ALIAS(aeabi_name, name)
#else
#define DEFINE_AEABI_FUNCTION_ALIAS(aeabi_name, name)
#endif

#define END_COMPILERRT_FUNCTION(name)                                          \
  .size SYMBOL_NAME(name), . - SYMBOL_NAME(name)
#define END_COMPILERRT_OUTLINE_FUNCTION(name)                                  \
  CFI_END SEPARATOR                                                            \
  .size SYMBOL_NAME(name), . - SYMBOL_NAME(name)

#endif // COMPILERRT_ASSEMBLY_H
//===-- int_endianness.h - configuration header for compiler-rt -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// 2024/01/21 - Modified by mintsuki for use inside cc-runtime
//
//===----------------------------------------------------------------------===//
//
// This file is a configuration header for compiler-rt.
// This file is not part of the interface of this library.
//
//===----------------------------------------------------------------------===//

#ifndef INT_ENDIANNESS_H
#define INT_ENDIANNESS_H

// Clang and GCC provide built-in endianness definitions.
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define _YUGA_LITTLE_ENDIAN 0
#define _YUGA_BIG_ENDIAN 1
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define _YUGA_LITTLE_ENDIAN 1
#define _YUGA_BIG_ENDIAN 0
#endif // __BYTE_ORDER__

#endif // INT_ENDIANNESS_H
//===-- int_lib.h - configuration header for compiler-rt  -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// 2024/01/21 - Modified by mintsuki for use inside cc-runtime
//
//===----------------------------------------------------------------------===//
//
// This file is not part of the interface of this library.
//
// This file defines various standard types, most importantly a number of unions
// used to access parts of larger types.
//
//===----------------------------------------------------------------------===//

#ifndef INT_TYPES_H
#define INT_TYPES_H


// si_int is defined in Linux sysroot's asm-generic/siginfo.h
#ifdef si_int
#undef si_int
#endif
typedef int32_t si_int;
typedef uint32_t su_int;
#if UINT_MAX == 0xFFFFFFFF
#define clzsi __builtin_clz
#define ctzsi __builtin_ctz
#elif ULONG_MAX == 0xFFFFFFFF
#define clzsi __builtin_clzl
#define ctzsi __builtin_ctzl
#else
#error could not determine appropriate clzsi macro for this system
#endif

typedef int64_t di_int;
typedef uint64_t du_int;

typedef union {
  di_int all;
  struct {
#if _YUGA_LITTLE_ENDIAN
    su_int low;
    si_int high;
#else
    si_int high;
    su_int low;
#endif // _YUGA_LITTLE_ENDIAN
  } s;
} dwords;

typedef union {
  du_int all;
  struct {
#if _YUGA_LITTLE_ENDIAN
    su_int low;
    su_int high;
#else
    su_int high;
    su_int low;
#endif // _YUGA_LITTLE_ENDIAN
  } s;
} udwords;

#if defined(__LP64__) || defined(__wasm__) || defined(__mips64) ||             \
    defined(__SIZEOF_INT128__)
#define CRT_HAS_128BIT
#endif

#ifdef CRT_HAS_128BIT
typedef int ti_int __attribute__((mode(TI)));
typedef unsigned tu_int __attribute__((mode(TI)));

typedef union {
  ti_int all;
  struct {
#if _YUGA_LITTLE_ENDIAN
    du_int low;
    di_int high;
#else
    di_int high;
    du_int low;
#endif // _YUGA_LITTLE_ENDIAN
  } s;
} twords;

typedef union {
  tu_int all;
  struct {
#if _YUGA_LITTLE_ENDIAN
    du_int low;
    du_int high;
#else
    du_int high;
    du_int low;
#endif // _YUGA_LITTLE_ENDIAN
  } s;
} utwords;

#endif // CRT_HAS_128BIT

#endif // INT_TYPES_H
//===-- int_lib.h - configuration header for compiler-rt  -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// 2024/01/21 - Modified by mintsuki for use inside cc-runtime
//
//===----------------------------------------------------------------------===//
//
// This file is a configuration header for compiler-rt.
// This file is not part of the interface of this library.
//
//===----------------------------------------------------------------------===//

#ifndef INT_LIB_H
#define INT_LIB_H

// Assumption: Signed integral is 2's complement.
// Assumption: Right shift of signed negative is arithmetic shift.
// Assumption: Endianness is little or big (not mixed).

// ABI macro definitions

#if __ARM_EABI__
#ifdef COMPILER_RT_ARMHF_TARGET
#define COMPILER_RT_ABI
#else
#define COMPILER_RT_ABI __attribute__((__pcs__("aapcs")))
#endif
#else
#define COMPILER_RT_ABI
#endif

#define AEABI_RTABI __attribute__((__pcs__("aapcs")))

#define ALWAYS_INLINE __attribute__((always_inline))
#define NOINLINE __attribute__((noinline))
#define NORETURN __attribute__((noreturn))
#define UNUSED __attribute__((unused))

#ifndef SYMBOL_NAME
#define STR(a) #a
#define XSTR(a) STR(a)
#define SYMBOL_NAME(name) XSTR(__USER_LABEL_PREFIX__) #name
#endif

#define COMPILER_RT_ALIAS(name, aliasname) \
  COMPILER_RT_ABI __typeof(name) aliasname __attribute__((__alias__(#name)));

// Include the standard compiler builtin headers we use functionality from.

// Include the commonly used internal type definitions.

// Include internal utility function declarations.

COMPILER_RT_ABI int __paritysi2(si_int a);
COMPILER_RT_ABI int __paritydi2(di_int a);

COMPILER_RT_ABI di_int __divdi3(di_int a, di_int b);
COMPILER_RT_ABI si_int __divsi3(si_int a, si_int b);
COMPILER_RT_ABI su_int __udivsi3(su_int n, su_int d);

COMPILER_RT_ABI su_int __udivmodsi4(su_int a, su_int b, su_int *rem);
COMPILER_RT_ABI du_int __udivmoddi4(du_int a, du_int b, du_int *rem);
#ifdef CRT_HAS_128BIT
COMPILER_RT_ABI int __clzti2(ti_int a);
COMPILER_RT_ABI tu_int __udivmodti4(tu_int a, tu_int b, tu_int *rem);
#endif

#endif // INT_LIB_H
//===-- int_util.h - internal utility functions ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is not part of the interface of this library.
//
// This file defines non-inline utilities which are available for use in the
// library. The function definitions themselves are all contained in int_util.c
// which will always be compiled into any compiler-rt library.
//
//===----------------------------------------------------------------------===//

#ifndef INT_UTIL_H
#define INT_UTIL_H

/// \brief Trigger a program abort (or panic for kernel code).
#define compilerrt_abort() __compilerrt_abort_impl(__FILE__, __LINE__, __func__)

NORETURN void __compilerrt_abort_impl(const char *file, int line,
                                      const char *function);

#define COMPILE_TIME_ASSERT(expr) COMPILE_TIME_ASSERT1(expr, __COUNTER__)
#define COMPILE_TIME_ASSERT1(expr, cnt) COMPILE_TIME_ASSERT2(expr, cnt)
#define COMPILE_TIME_ASSERT2(expr, cnt)                                        \
  typedef char ct_assert_##cnt[(expr) ? 1 : -1] UNUSED

// Force unrolling the code specified to be repeated N times.
#define REPEAT_0_TIMES(code_to_repeat) /* do nothing */
#define REPEAT_1_TIMES(code_to_repeat) code_to_repeat
#define REPEAT_2_TIMES(code_to_repeat)                                         \
  REPEAT_1_TIMES(code_to_repeat)                                               \
  code_to_repeat
#define REPEAT_3_TIMES(code_to_repeat)                                         \
  REPEAT_2_TIMES(code_to_repeat)                                               \
  code_to_repeat
#define REPEAT_4_TIMES(code_to_repeat)                                         \
  REPEAT_3_TIMES(code_to_repeat)                                               \
  code_to_repeat

#define REPEAT_N_TIMES_(N, code_to_repeat) REPEAT_##N##_TIMES(code_to_repeat)
#define REPEAT_N_TIMES(N, code_to_repeat) REPEAT_N_TIMES_(N, code_to_repeat)

#endif // INT_UTIL_H
