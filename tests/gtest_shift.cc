#include <memory>
#include <iostream>
#include <vector>
#include "gtest/gtest.h"
#include "tests/macros.h"
#include "lib/loader.h"
#include "lib/executor.h"
#include "lib/instantiator.h"
#include "lib/inspector.h"

using ::std::make_unique;
using ::std::cout;
using ::std::endl;
using ::std::vector;

#define START_BYTES \
  0, 0x61, 0x73, 0x6d, 0x1, 0, 0, 0

/**
 * Case ISAs:
 * 
 * i32.shl / i64.shl
 * i32.shr_s / i64.shr_s
 * i32.shr_u / i64.shr_u
 * i32.rotl / i64.rotl
 * i32.rotr / i64.rotr
 */
TEST(TWVM, Shift) {
  const auto executor = make_unique<Executor>();
 
}



