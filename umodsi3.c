//===-- umodsi3.c - Implement __umodsi3 -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __umodsi3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#include "int_lib.h"

// Returns: a % b

#undef clz
#define clz(a) (sizeof(a) == sizeof(unsigned long long) ? __builtin_clzll(a) : clzsi(a))

// Mostly identical to __udivsi3 but the return values are different.
COMPILER_RT_ABI su_int __umodsi3(su_int n, su_int d) {
  const unsigned N = sizeof(su_int) * CHAR_BIT;
  // d == 0 cases are unspecified.
  unsigned sr = (d ? (unsigned)clz(d) : N) - (n ? (unsigned)clz(n) : N);
  // 0 <= sr <= N - 1 or sr is very large.
  if (sr > N - 1) // n < d
    return n;
  if (sr == N - 1) // d == 1
    return 0;
  ++sr;
  // 1 <= sr <= N - 1. Shifts do not trigger UB.
  su_int r = n >> sr;
  n <<= N - sr;
  su_int carry = 0;
  for (; sr > 0; --sr) {
    r = (r << 1) | (n >> (N - 1));
    n = (n << 1) | carry;
    // Branch-less version of:
    // carry = 0;
    // if (r >= d) r -= d, carry = 1;
    const si_int s = (si_int)(d - r - 1) >> (N - 1);
    carry = s & 1;
    r -= d & s;
  }
  return r;
}
