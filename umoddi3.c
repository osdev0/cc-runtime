//===-- umoddi3.c - Implement __umoddi3 -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
// 2024/08/10 - Modified by mintsuki for use inside cc-runtime
//
//===----------------------------------------------------------------------===//
//
// This file implements __umoddi3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

// Returns: a % b

#undef clz
#define clz(a) (sizeof(a) == sizeof(unsigned long long) ? __builtin_clzll(a) : clzsi(a))

// Mostly identical to __udivdi3 but the return values are different.
COMPILER_RT_ABI du_int __umoddi3(du_int n, du_int d) {
  const unsigned N = sizeof(du_int) * CHAR_BIT;
  // d == 0 cases are unspecified.
  unsigned sr = (d ? clz(d) : N) - (n ? clz(n) : N);
  // 0 <= sr <= N - 1 or sr is very large.
  if (sr > N - 1) // n < d
    return n;
  if (sr == N - 1) // d == 1
    return 0;
  ++sr;
  // 1 <= sr <= N - 1. Shifts do not trigger UB.
  du_int r = n >> sr;
  n <<= N - sr;
  du_int carry = 0;
  for (; sr > 0; --sr) {
    r = (r << 1) | (n >> (N - 1));
    n = (n << 1) | carry;
    // Branch-less version of:
    // carry = 0;
    // if (r >= d) r -= d, carry = 1;
    const di_int s = (di_int)(d - r - 1) >> (N - 1);
    carry = s & 1;
    r -= d & s;
  }
  return r;
}
