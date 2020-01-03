// Copyright 2019 YHSPY. All rights reserved.
#ifndef CACHE_H_
#define CACHE_H_

#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <vector>
#include "lib/type.h"
#include "lib/utility.h"

namespace twvm {
namespace internal {

using ::std::unordered_map;
using ::std::string;
using ::std::vector;

#define DECLARE_CACHE_CONTAINER(name, type) \
  unordered_map<uint64_t, vector<type>> name##Map = {};

#define ACTION_CLEAR_ALL_CACHE(name, ...) \
  name##Map.clear();

#define DECLARE_CACHE_SET_METHODS(name, type) \
  void name##SetValueCache(uint32_t index, size_t offset, type v, size_t step) { \
    name##Map[hashLoc(index, offset)] = {v, static_cast<type>(step)}; \
  }
#define DECLARE_CACHE_GET_METHODS(name, type) \
  const auto& name##GetValueCache(uint32_t index, size_t offset) { \
    return name##Map[hashLoc(index, offset)]; \
  }

#define ITERATE_IMMEDIATES_VALUE_TYPES(V) \
  V(uint8, uint8_t) \
  V(uint32, uint32_t) \
  V(uint64, uint64_t) \
  V(int32, int32_t) \
  V(int64, int64_t) \
  V(float, float) \
  V(double, double)

// keep the runtime immediates in memory, and then can be re-used;
class Cache {
 private:
  // id + offset;
  ITERATE_IMMEDIATES_VALUE_TYPES(DECLARE_CACHE_CONTAINER)
  // meta cache container;
  unordered_map<uint64_t, unordered_map<OpcodeMeta, int64_t>> metaContainer = {};
  // "memarg" cache container;
  unordered_map<uint64_t, vector<uint32_t>> memargContainer = {};

  inline uint64_t hashLoc(uint32_t index, size_t offset) const {
    // avaiable size: 131072;
    return index * (2 << 16) + offset;
  }

 public:
  ITERATE_IMMEDIATES_VALUE_TYPES(DECLARE_CACHE_SET_METHODS)
  ITERATE_IMMEDIATES_VALUE_TYPES(DECLARE_CACHE_GET_METHODS)

  inline void int64SetMetaCache(uint32_t index, size_t offset, OpcodeMeta key, int64_t val) {
    metaContainer[hashLoc(index, offset)][key] = val;
  }

  inline auto int64GetMetaCache(uint32_t index, size_t offset, OpcodeMeta key) {
    return metaContainer[hashLoc(index, offset)][key];
  }

  inline auto& uint32GetMemargCache(uint32_t index, size_t offset) {
    return memargContainer[hashLoc(index, offset)];
  }

  inline void uint32SetMemargCache(uint32_t index, size_t offset, uint32_t memAlign, uint32_t memOffset, size_t step) {
    memargContainer[hashLoc(index, offset)] = {memAlign, memOffset, static_cast<uint32_t>(step)};
  }

  void reset() {
    ITERATE_IMMEDIATES_VALUE_TYPES(ACTION_CLEAR_ALL_CACHE)
    metaContainer.clear();
    memargContainer.clear();
  }
};

}  // namespace internal
}  // namespace twvm

#endif  // CACHE_H_
