//===-- lib/mulsf3.c - Single-precision multiplication ------------*- C -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements single-precision soft-float multiplication
// with the IEEE-754 default rounding (to nearest, ties to even).
//
//===----------------------------------------------------------------------===//

#ifndef CC_RUNTIME_NO_FLOAT

#define SINGLE_PRECISION
#include "fp_mul_impl.inc"

COMPILER_RT_ABI fp_t __mulsf3(fp_t a, fp_t b) { return __mulXf3__(a, b); }

#if defined(__ARM_EABI__)
#if defined(COMPILER_RT_ARMHF_TARGET)
AEABI_RTABI fp_t __aeabi_fmul(fp_t a, fp_t b) { return __mulsf3(a, b); }
#else
COMPILER_RT_ALIAS(__mulsf3, __aeabi_fmul)
#endif
#endif

#endif
