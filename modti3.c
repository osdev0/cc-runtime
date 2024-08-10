//===-- modti3.c - Implement __modti3 -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __modti3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

#ifdef CRT_HAS_128BIT

// Returns: a % b

COMPILER_RT_ABI ti_int __modti3(ti_int a, ti_int b) {
  const int N = (int)(sizeof(ti_int) * CHAR_BIT) - 1;
  ti_int s = b >> N;                              // s = b < 0 ? -1 : 0
  tu_int b_u = (tu_int)(b ^ s) + (-s);        // negate if s == -1
  s = a >> N;                                       // s = a < 0 ? -1 : 0
  tu_int a_u = (tu_int)(a ^ s) + (-s);        // negate if s == -1
  tu_int res;
  __udivmodti4(a_u, b_u, &res);
  return (res ^ s) + (-s);                          // negate if s == -1
}

#endif // CRT_HAS_128BIT
