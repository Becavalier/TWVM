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
 * i32.wrap_i64
 * i32.trunc_f32_s
 * i32.trunc_f32_u
 * i32.trunc_f64_s
 * i32.trunc_f64_u
 * i64.extend_i32_s
 * i64.extend_i32_u
 * i64.trunc_f32_s
 * i64.trunc_f32_u
 * i64.trunc_f64_s
 * i64.trunc_f64_u
 * f32.convert_i32_s
 * f32.convert_i32_u
 * f32.convert_i64_s
 * f32.convert_i64_u
 * f32.demote_f64
 * f64.convert_i32_s
 * f64.convert_i32_u
 * f64.convert_i64_s
 * f64.convert_i64_u
 * f64.promote_f32
 * i32.reinterpret_f32
 * i64.reinterpret_f64
 * f32.reinterpret_i32
 * f64.reinterpret_i64
 */
TEST(TWVM, Convert) {
  const auto executor = make_unique<Executor>();
 
}



