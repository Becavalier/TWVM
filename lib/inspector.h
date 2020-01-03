// Copyright 2019 YHSPY. All rights reserved.
#ifndef INSPECTOR_H_
#define INSPECTOR_H_

#include <memory>
#include "lib/instantiator.h"

namespace twvm {
namespace internal {

using ::std::shared_ptr;

// validate the structure of "WasmInstance" before execution;
class Inspector {
 public:
  static void inspect(shared_ptr<WasmInstance>);
};

}  // namespace internal
}  // namespace twvm

#endif  // INSPECTOR_H_
