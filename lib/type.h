// Copyright 2019 YHSPY. All rights reserved.
#ifndef TYPE_H_
#define TYPE_H_

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include "lib/include/macros.h"
#include "lib/utility.h"

namespace twvm {
namespace internal {

using ::std::string;
using ::std::vector;

constexpr size_t kSpecMaxWasmMemoryPages = 65536;
constexpr uint32_t kWasmMagicWord = 0x6d736100;
constexpr uint32_t kWasmVersion = 0x01;
constexpr uint8_t kWasmTrue = 1;
constexpr uint8_t kWasmFalse = 0;

// basic types;
enum class ValueTypesCode : int8_t {
  kVoid = 0x40,  // block_type;
  kI32 = 0x7f,
  kI64 = 0x7e,
  kF32 = 0x7d,
  kF64 = 0x7c,
  kFunc = 0x60,
  kFuncRef = 0x70,
};

// import/export;
enum class ExternalTypesCode : uint8_t {
  kExternalFunction = 0,
  kExternalTable = 1,
  kExternalMemory = 2,
  kExternalGlobal = 3,
  kExternalException = 4,
};

// sections;
enum class SectionTypesCode : uint8_t {
  kUnknownSection = 0,
  kTypeSection = 1,
  kImportSection = 2,
  kFunctionSection = 3,
  kTableSection = 4,
  kMemorySection = 5,
  kGlobalSection = 6,
  kExportSection = 7,
  kStartSection = 8,
  kElementSection = 9,
  kCodeSection = 10,
  kDataSection = 11,
  kDataCountSection = 12,
  kExceptionSection = 13,
};

// stack frame types;
enum class ValueFrameTypes : uint8_t {
  kI32Value = 0x7f,
  kI64Value = 0x7e,
  kF32Value = 0x7d,
  kF64Value = 0x7c,
};

enum class StackFrameTypes : uint8_t {
  kValues = 0,
  kLabels,
  kActivations,
};

enum class InitExprKind : uint8_t {
  kNone,
  kGlobalIndex,
  kI32Const,
  kI64Const,
  kF32Const,
  kF64Const,
};

/* runtime types */
// here we directly use "int32_t", "int64_t", "float", "double" to present 4 kinds of Wasm values;
union RTValue {
  int32_t i32;
  int64_t i64;
  float f32;
  double f64;
};

enum class OpcodeMeta : uint8_t {
  EndOffset,
  BranchOfset,
};

struct PosPtr {
  PosPtr(
    uint32_t index,
    vector<uint8_t> *pc,
    const size_t offset = -1) : index(index), pc(pc), offset(offset) {}
  uint32_t index;
  vector<uint8_t> *pc;
  const size_t offset;
};

/* compound types */
// for initial value of global section, offset of data/elements segment;
struct WasmInitExpr {
  WasmInitExpr() = default;
  InitExprKind kind = InitExprKind::kNone;
  union {
    // constants;
    int32_t vI32Const;
    int64_t vI64Const;
    float vF32Const;
    double vF64Const;
    // op: get_global (refer to an immutable import);
    uint32_t vGlobalIndex;
  } val;
  RTValue toRTValue() {
    RTValue rtVal;
    if (kind != InitExprKind::kGlobalIndex) {
      switch (kind) {
        case InitExprKind::kF32Const: { rtVal.f32 = val.vF32Const; break; }
        case InitExprKind::kF64Const: { rtVal.f64 = val.vF64Const; break; }
        case InitExprKind::kI32Const: { rtVal.i32 = val.vI32Const; break; }
        case InitExprKind::kI64Const: { rtVal.i64 = val.vI64Const; break; }
        default: {
          Printer::instance().error(Errors::MISC_EXPR_NOT_INIT);
        }
      }
    } else  {
      // deal with platform-hosted imported "global";
    }
    return rtVal;
  }
};

/* static sections types */
struct WasmFunctionSig {
  SET_STRUCT_MOVE_ONLY(WasmFunctionSig);
  uint32_t index = 0;
  size_t paramsCount = 0;
  size_t returnCount = 0;
  vector<ValueTypesCode> reps;
  // TODO(Jason Yu) reduce vector copying overhead;
  inline vector<ValueTypesCode> getParamTypes() {
    return vector<ValueTypesCode>(begin(reps), begin(reps) + paramsCount);
  }
  inline vector<ValueTypesCode> getReturnTypes() {
    return vector<ValueTypesCode>(begin(reps) + paramsCount, end(reps));
  }
};

// wasm indirect call table;
struct WasmFunction {
  SET_STRUCT_MOVE_ONLY(WasmFunction);
  WasmFunctionSig* sig = nullptr;
  uint32_t funcIndex = 0;
  uint32_t sigIndex = 0;
  vector<ValueFrameTypes> locals = {};
  vector<uint8_t> code;
  size_t codeLen = 0;
  bool imported = false;
  bool exported = false;
};

struct WasmTable {
  SET_STRUCT_MOVE_ONLY(WasmTable);
  ValueTypesCode type = ValueTypesCode::kFuncRef;
  uint32_t initialSize = 0;
  uint32_t maximumSize = 0;
  bool hasMaximumSize = false;
  bool imported = false;
  bool exported = false;
};

template <typename T = WasmFunction>
struct WasmElement {
  SET_STRUCT_MOVE_ONLY(WasmElement<T>);
  uint32_t tableIndex = 0;  // default;
  ExternalTypesCode type = ExternalTypesCode::kExternalException;  // anyfunc;
  WasmInitExpr init = {};  // offset in table (initialization expr);
  vector<T*> entities = {};
};

struct WasmMemory {
  SET_STRUCT_MOVE_ONLY(WasmMemory);
  uint32_t initialPages = 0;
  uint32_t maximumPages = 0;
  bool hasMaximumPages = false;
  bool imported = false;
  bool exported = false;
};

struct WasmGlobal {
  SET_STRUCT_MOVE_ONLY(WasmGlobal);
  ValueTypesCode type = ValueTypesCode::kVoid;
  bool mutability = false;
  WasmInitExpr init = {};  // initialization expr;
  bool imported = false;
  bool exported = false;
};

struct WasmExport {
  string name;
  ExternalTypesCode type;
  uint32_t index = 0;
};

struct WasmImport {
  string moduleName;
  string fieldName;
  ExternalTypesCode kind;
  uint32_t index = 0;
};

}  // namespace internal
}  // namespace twvm

#endif  // TYPE_H_
