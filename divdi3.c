//===-- divdi3.c - Implement __divdi3 -------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// 2024/08/10 - Modified by mintsuki for use inside cc-runtime
//
//===----------------------------------------------------------------------===//
//
// This file implements __divdi3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

// Returns: a / b

COMPILER_RT_ABI di_int __divdi3(di_int a, di_int b) {
  const int N = (int)(sizeof(di_int) * CHAR_BIT) - 1;
  di_int s_a = a >> N;                            // s_a = a < 0 ? -1 : 0
  di_int s_b = b >> N;                            // s_b = b < 0 ? -1 : 0
  du_int a_u = (du_int)(a ^ s_a) + (-s_a);    // negate if s_a == -1
  du_int b_u = (du_int)(b ^ s_b) + (-s_b);    // negate if s_b == -1
  s_a ^= s_b;                                       // sign of quotient
  return (__udivmoddi4(a_u, b_u, (du_int *)0) ^ s_a) + (-s_a);   // negate if s_a == -1
}
