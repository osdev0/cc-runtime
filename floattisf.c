//===-- floattisf.c - Implement __floattisf -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __floattisf for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#ifndef CC_RUNTIME_NO_FLOAT

#include "int_lib.h"

#ifdef CRT_HAS_128BIT

#define SRC_I128
#define DST_SINGLE
#include "int_to_fp_impl.inc"

// Returns: convert a to a float, rounding toward even.

// Assumption: float is a IEEE 32 bit floating point type
//             ti_int is a 128 bit integral type

// seee eeee emmm mmmm mmmm mmmm mmmm mmmm

COMPILER_RT_ABI float __floattisf(ti_int a) { return __floatXiYf__(a); }

#endif // CRT_HAS_128BIT

#endif
