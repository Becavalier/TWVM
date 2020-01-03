// Copyright 2019 YHSPY. All rights reserved.
#ifndef TWVM_H_
#define TWVM_H_

#include <chrono>
#include "lib/include/errors.h"
#include "lib/include/macros.h"
#include "lib/include/constants.h"
#include "lib/utility.h"
#include "lib/loader.h"
#include "lib/module.h"
#include "lib/executor.h"
#include "lib/stack.h"
#include "lib/instantiator.h"

using ::std::chrono::high_resolution_clock;
using ::std::chrono::duration_cast;
using ::std::chrono::microseconds;

const auto calcTimeInterval(decltype(high_resolution_clock::now()) &previous) {
  const auto stop = high_resolution_clock::now();
  const auto duration = duration_cast<microseconds>(stop - previous);
  previous = stop;
  return duration.count() / 1000.0;
}

#endif  // TWVM_H_
