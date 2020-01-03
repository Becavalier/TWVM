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
 * unreachable
 * nop
 * block
 * loop
 * if
 * else
 * end
 * br
 * br_if
 * br_table
 * return
 * call
 * call_indirect
 * drop
 * select
 */
TEST(TWVM, Control) {
  const auto executor = make_unique<Executor>();
 
}



