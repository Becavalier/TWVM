// Copyright 2019 YHSPY. All rights reserved.
#ifndef FRAMES_H_
#define FRAMES_H_

#include <cstring>
#include <vector>
#include <memory>
#include <ostream>
#include <type_traits>
#include <utility>
#include "lib/include/constants.h"
#include "lib/include/errors.h"
#include "lib/include/macros.h"
#include "lib/type.h"
#include "lib/utility.h"

namespace twvm {
namespace internal {

using ::std::memcpy;
using ::std::memcmp;
using ::std::vector;
using ::std::shared_ptr;
using ::std::ostream;
using ::std::is_same;
using ::std::move;

struct WasmFuncInstance;

class ValueFrame {
 public:
  bool initialized = false;
  SET_STRUCT_MOVE_ONLY(ValueFrame);
  ValueFrame(ValueFrameTypes type) : bitPattern{} {}
  ValueFrame(const ValueFrame *other) : type(other->type), isValueZero(other->isValueZero) {
    // copy "bitPattern";
    memcpy(bitPattern, other->bitPattern, WASM_VALUE_BIT_PATTERN_WIDTH);
    initialized = true;
  }

#define DEFINE_VALUEFRAME_TYPE_SPECIFIC_METHODS(name, localtype, ctype) \
  ValueFrame(ctype v) : type(localtype), bitPattern{} { \
    isValueZero = (v == static_cast<ctype>(0)); \
    Utility::writeUnalignedValue<ctype>(reinterpret_cast<uintptr_t>(bitPattern), v); \
    initialized = true; \
  } \
  inline const ctype to##name() const { \
    return Utility::readUnalignedValue<ctype>(reinterpret_cast<uintptr_t>(bitPattern)); \
  }
  ITERATE_WASM_VAL_TYPE(DEFINE_VALUEFRAME_TYPE_SPECIFIC_METHODS)

  const bool operator==(const ValueFrame& other) const {
    return type == other.type &&
      !memcmp(bitPattern, other.bitPattern, WASM_VALUE_BIT_PATTERN_WIDTH);
  }

  inline ValueTypesCode getValueType() const {
    return static_cast<ValueTypesCode>(type);
  }

  template <typename T>
  inline void resetValue(T v) {
    if constexpr (is_same<T, int32_t>::value) {
      type = ValueFrameTypes::kI32Value;
    } else if constexpr (is_same<T, int64_t>::value) {
      type = ValueFrameTypes::kI64Value;
    } else if constexpr (is_same<T, float>::value) {
      type = ValueFrameTypes::kF32Value;
    } else if constexpr (is_same<T, double>::value) {
      type = ValueFrameTypes::kF64Value;
    } else {
      Printer::instance().error(Errors::MISC_INVALID_VALUEFRAME);
    }
    isValueZero = (v == static_cast<T>(0));
    Utility::writeUnalignedValue<T>(reinterpret_cast<uintptr_t>(bitPattern), v);
  }

  void outputValue(ostream &out) const {
    switch (type) {
      case ValueFrameTypes::kF32Value: { out << toF32(); break; }
      case ValueFrameTypes::kF64Value: { out << toF64(); break; }
      case ValueFrameTypes::kI32Value: { out << toI32(); break; }
      case ValueFrameTypes::kI64Value: { out << toI64(); break; }
      default: break;
    }
  }

  inline const auto isZero() const {
    return isValueZero;
  }

  template <typename T>
  inline const T resolveValue() {
    return Utility::readUnalignedValue<T>(reinterpret_cast<uintptr_t>(bitPattern));
  }

 private:
  ValueFrameTypes type;
  bool isValueZero;
  uint8_t bitPattern[WASM_VALUE_BIT_PATTERN_WIDTH];
};

class LabelFrame {
 public:
  SET_STRUCT_MOVE_ONLY(LabelFrame);
  shared_ptr<PosPtr> end;
  shared_ptr<PosPtr> branch;
  shared_ptr<PosPtr> start;

  LabelFrame(
    ValueTypesCode resultType,
    size_t valueStackHeight) : resultType(resultType), valueStackHeight(valueStackHeight) {}

  inline const auto getResultType() const { return resultType; }
  inline const auto getValueStackHeight() const { return valueStackHeight; }

 private:
  // for "block", "loop" and "if";
  ValueTypesCode resultType;
  // determine the # of returning arity;
  size_t valueStackHeight = 0;
};

class ActivationFrame {
 public:
  SET_STRUCT_MOVE_ONLY(ActivationFrame);
  const WasmFuncInstance *pFuncIns = nullptr;
  vector<ValueFrame*> locals = {};
  shared_ptr<PosPtr> leaveEntry;

  ActivationFrame(
    const WasmFuncInstance *pFuncIns,
    size_t valueStackHeight,
    size_t labelStackHeight,
    shared_ptr<PosPtr> leaveEntry = nullptr,
    vector<ValueFrame*> inputLocals = {}) :
    pFuncIns(pFuncIns),
    leaveEntry(leaveEntry),
    valueStackHeight(valueStackHeight),
    labelStackHeight(labelStackHeight) {
      if (locals.size() != 0) {
        locals = move(inputLocals);
      }
    }

  inline const auto getValueStackHeight() const { return valueStackHeight; }
  inline const auto getLabelStackHeight() const { return labelStackHeight; }

 private:
  // determine the # of returning arity;
  size_t valueStackHeight = 0;
  // determine whether we reach the "end" of the function;
  size_t labelStackHeight = 0;
};

}  // namespace internal
}  // namespace twvm

#endif  // FRAMES_H_
