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
 * i32.const
 * i64.const
 * f32.const
 * f64.const
 * local.get
 * local.set
 * local.tee
 * global.get
 * global.set
 */
TEST(TWVM, Others) {
  const auto executor = make_unique<Executor>();
 
}



