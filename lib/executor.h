// Copyright 2019 YHSPY. All rights reserved.
#ifndef LIB_EXECUTOR_H_
#define LIB_EXECUTOR_H_

#include <memory>
#include <functional>
#include <vector>
#include <unordered_map>
#include <utility>
#include "lib/type.h"
#include "lib/cache.h"
#include "lib/common/opcode.h"
#include "lib/common/constants.h"
#include "lib/instances/ins-wasm.h"
#include "lib/structures/struct-frames.h"

using std::shared_ptr;
using std::function;
using std::vector;
using std::unordered_map;
using std::move;

#define ITERATE_OPERANDS_VALUE_TYPES(V) \
  V(i32, int32_t) \
  V(u32, uint32_t) \
  V(i64, int64_t) \
  V(u64, uint64_t) \
  V(f32, float) \
  V(f64, double)

#define DECLARE_CONSTANT_POOL(name, type) \
  unordered_map<type, ValueFrame> name##ConstantPool = {};

#define ACTION_CLEAR_CONSTANT_POOL(name, ...) \
  name##ConstantPool.clear();

#define DECLARE_CONSTANT_POOL_SETTERS(name, type) \
  auto checkUpConstant(type val) { \
    return &((name##ConstantPool.try_emplace(val, val).first->second)); \
  }

#define DECLARE_CONSTANT_POOL_DEBUGGER(name, type) \
  void name##ConstantPoolDebug(ostringstream &oss) { \
    unordered_map<type, ValueFrame>::size_type counter = 0; \
    for (const auto& val : name##ConstantPool) { \
      val.second.outputValue(oss); \
      if (++counter != name##ConstantPool.size()) { \
        oss << ", "; \
      } \
    } \
  }

#define DECLARE_CACHE_OPERATIONS(name, type) \
  type name##UseImmesCache(function<void(size_t*, type*)> accessor) { \
    const auto opcodeStaticOffset = innerOffset; \
    const auto& result = cache->name##GetValueCache( \
      contextIndex, \
      opcodeStaticOffset); \
    type immediate = 0; \
    if (result.empty()) { \
      size_t step = 0; \
      accessor(&step, &immediate); \
      cache->name##SetValueCache(contextIndex, opcodeStaticOffset, immediate, step); \
    } else {\
      immediate = result[DEFAULT_ELEMENT_INDEX]; \
      innerOffset += (result[DEFAULT_ELEMENT_INDEX + 1]); \
    } \
    return immediate; \
  }

// core execution logic;
class Executor {
 private:
  bool runningStatus = true;
  shared_ptr<WasmInstance> currentWasmIns = nullptr;
  ITERATE_OPERANDS_VALUE_TYPES(DECLARE_CONSTANT_POOL)
  ValueFrame lastRunningResult = {};
  void resetExecutionEngine(ValueFrame *result) {
    // keep running result.
    lastRunningResult = move(*result);
    // release shared_ptr.
    currentWasmIns = nullptr;
    // reset cache.
    cache->reset();
    // reset constant pool.
    ITERATE_OPERANDS_VALUE_TYPES(ACTION_CLEAR_CONSTANT_POOL)
    // reset params.
    pc = nullptr;
    innerOffset = -1;
    contextIndex = -1;
    // re-start engine.
    runningStatus = true;
  }

 public:
  vector<uint8_t> *pc = nullptr;
  size_t innerOffset = -1;
  uint32_t contextIndex = -1;
  shared_ptr<Cache> cache = make_shared<Cache>();
  const bool execute(shared_ptr<WasmInstance>);
  const void crawler(const uint8_t*, size_t, const function<bool(WasmOpCode, size_t)> &callback = nullptr);
  const bool checkStackState(shared_ptr<WasmInstance>);

  ITERATE_OPERANDS_VALUE_TYPES(DECLARE_CONSTANT_POOL_SETTERS)
  ITERATE_OPERANDS_VALUE_TYPES(DECLARE_CONSTANT_POOL_DEBUGGER)
  ITERATE_IMMEDIATES_VALUE_TYPES(DECLARE_CACHE_OPERATIONS)

  int64_t int64UseMetaCache(OpcodeMeta type, function<void(int64_t*)> accessor) {
    const auto opcodeStaticOffset = innerOffset;
    const auto cacheVal = cache->int64GetMetaCache(contextIndex, opcodeStaticOffset, type);
    int64_t value = 0;
    if (cacheVal == 0) {
      accessor(&value);
      cache->int64SetMetaCache(contextIndex, opcodeStaticOffset, type, value);
    } else { value = cacheVal; }
    return value;
  }

  const auto& uint32UseMemargCache(function<void(uint32_t*, uint32_t*, size_t*)> accessor) {
    const auto opcodeStaticOffset = innerOffset;
    auto& argsVal = cache->uint32GetMemargCache(contextIndex, opcodeStaticOffset);
    if (argsVal.empty()) {
      size_t step = 0;
      uint32_t align = 0, offset = 0;
      accessor(&align, &offset, &step);
      // prevent from copy-constructing.
      cache->uint32SetMemargCache(contextIndex, opcodeStaticOffset, align, offset, step);
    }
    return argsVal;
  }

  const uint8_t* absAddr() const {
    return pc->data() + innerOffset;
  }

  void switchStatus(bool flag) {
    runningStatus = flag;
  }

  template <typename T>
  const T inspectRunningResult() {
    if (lastRunningResult.initialized) {
      return lastRunningResult.resolveValue<T>();
    } else {
      (Printer::instance() << "No existing running result found.\n").warn();
      return T{};
    }
  }

  const uint8_t* forward_(size_t step = 1) {
    innerOffset += step;
    return absAddr();
  }
};

#endif  // LIB_EXECUTOR_H_
