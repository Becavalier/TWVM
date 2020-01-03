// Copyright 2019 YHSPY. All rights reserved.
#ifndef STACK_H_
#define STACK_H_

#include <memory>
#include <cstring>
#include <utility>
#include <vector>
#include "lib/frames.h"
#include "lib/include/constants.h"
#include "lib/utility.h"
#include "lib/type.h"

namespace twvm {
namespace internal {

using ::std::shared_ptr;
using ::std::make_shared;
using ::std::dec;
using ::std::forward;
using ::std::vector;

// use vector to simulate stack, then we can have the ability of random-access,
// and high-efficient element accessing (without "stack->deque");
template <typename T>
class StackContainer {
 public:
  inline void popN(size_t n) {
    if (n > 0 && n <= size()) {
      for (uint32_t i = 0; i < n; i++) {
        container.pop_back();
      }
    }
  }
  inline void pop() { container.pop_back(); }
  inline void erase(size_t start, size_t height) {
    container.erase(end(container) - start - height, end(container) - start);
  }
  inline void push(T&& v) { container.push_back(forward<T>(v)); }
  inline void emplace(T&& v) { container.emplace_back(forward<T>(v)); }
  // back index, start from 0;
  inline auto& top(size_t i = 0) { return at(size() - 1 - i); }
  inline auto& at(size_t i) { return container.at(i); }
  inline vector<T*> topN(size_t n) {
    vector<T*> t;
    if (n > 0 && n <= size()) {
      bool stop = false;
      for (uint32_t i = size() - 1; (i >= size() - n) && !stop; i--) {
        if (i == 0) { stop = true; }
        t.push_back(&at(i));
      }
    }
    return t;
  }
  inline const auto size() const { return container.size(); }
  inline const auto& data() { return container; }

 private:
  // stack: (bottom) [head ... back] (top);
  vector<T> container = {};
};

// for saving "Values" / "Labels" / "Activations";
class Stack {
 public:
  // in order to reduce the overhead from casting between parent and child types -
  // caused by "dynamic_cast" and "static_cast", we'd better store these three kinds of Frames -
  // separately.
  using ValueFrameStack = StackContainer<ValueFrame*>;
  using LabelFrameStack = StackContainer<LabelFrame>;
  using ActivationFrameStack = StackContainer<ActivationFrame>;
  shared_ptr<ValueFrameStack> valueStack = make_shared<ValueFrameStack>();
  shared_ptr<LabelFrameStack> labelStack = make_shared<LabelFrameStack>();
  shared_ptr<ActivationFrameStack> activationStack = make_shared<ActivationFrameStack>();
};

}  // namespace internal
}  // namespace twvm

#endif  // STACK_H_
