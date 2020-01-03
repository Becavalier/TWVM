// Copyright 2019 YHSPY. All rights reserved.
#ifndef INSTANCES_H_
#define INSTANCES_H_

#include <memory>
#include <string>
#include <vector>
#include "lib/type.h"
#include "lib/store.h"
#include "lib/stack.h"
#include "lib/include/macros.h"
#include "lib/include/constants.h"
#include "lib/include/errors.h"
#include "lib/utility.h"
#include "lib/module.h"

namespace twvm {
namespace internal {

using ::std::string;
using ::std::vector;
using ::std::shared_ptr;

class Stack;
struct Store;
struct WasmModuleInstance;

class WasmMemoryInstance {
 public:
  SET_STRUCT_MOVE_ONLY(WasmMemoryInstance);
  WasmMemoryInstance(uint32_t initMemSize = 1, uint32_t maxMemSize = 0) : maxMemSize(maxMemSize) {
    if (initMemSize > 0 && (maxMemSize == 0 || initMemSize <= maxMemSize)) {
      // allocate space (multiple of Wasm page);
      if ((data = static_cast<uint8_t*>(calloc(initMemSize * WASM_PAGE_SIZE, uint8Size)))) {
        currentMemSize = initMemSize;
      } else {
        Printer::instance().error(Errors::LOADER_MEM_ALLOC_ERR);
      }
    } else {
      Printer::instance().error(Errors::LOADER_MEM_ALLOC_SIZE_ERR);
    }
  }
  ~WasmMemoryInstance() {
    free(data);
  }

  inline const auto availableSize() const {
    return currentMemSize * WASM_PAGE_SIZE;
  }

  inline const auto maxSize() const {
    return maxMemSize;
  }

  inline const auto& rawDataBuf() {
    return data;
  }
  
  // memory -> stack;
  template <typename T>
  T load(uint32_t offset) {
    if (offset + sizeof(T) <= availableSize()) {
      return *reinterpret_cast<T*>(data + offset);
    } else {
      Printer::instance().error(Errors::RT_MEM_ACCESS_OOB);
      // unreachable;
      return false;
    }
  }

  // stack -> memory;
  template <typename T>
  void store(uint32_t offset, T val) {
    // bound check;
    if (offset + sizeof(T) <= availableSize()) {
      *(reinterpret_cast<T*>(data + offset)) = val;
    } else {
      Printer::instance().error(Errors::RT_MEM_ACCESS_OOB);
    }
  }

  void inspect(uint32_t size = 20) const {
    for (uint32_t i = 0; i < size; i++) {
      cout << static_cast<int>(*(data + i)) << ' ';
    }
  }

  // expand the maxmium capacity of the memory by Wasm pages;
  const size_t grow(const size_t &pages) {
    if (maxMemSize != 0 && currentMemSize + pages > maxMemSize) {
      return -1;
    } else {
      const auto previousMemSize = currentMemSize;
      currentMemSize = currentMemSize + pages;
      // TODO(Jason Yu): relloc;
      return previousMemSize;
    }
  }

 private:
  // 64k per pages;
  uint32_t maxMemSize = 0;
  uint32_t currentMemSize = 0;
  uint8_t* data = nullptr;
  const WasmMemory *staticMemory;
};

struct WasmGlobalInstance {
  SET_STRUCT_MOVE_ONLY(WasmGlobalInstance);
  ValueTypesCode type;
  RTValue val;
  bool mutability;
};

struct WasmTableInstance {
  SET_STRUCT_MOVE_ONLY(WasmTableInstance);
  uint32_t maxTableSize;
  vector<WasmFunction*> funcIndices;
};

struct WasmFuncInstance {
  SET_STRUCT_MOVE_ONLY(WasmFuncInstance);
  WasmFunctionSig* type;
  WasmFunction* staticProto;
  shared_ptr<WasmModuleInstance> module;
  vector<uint8_t> *code;
};

struct WasmExportInstance {
  SET_STRUCT_MOVE_ONLY(WasmExportInstance);
  string name;
  ExternalTypesCode type;
  uint32_t index = 0;
};

struct WasmModuleInstance {
  SET_STRUCT_MOVE_ONLY(WasmModuleInstance);
  shared_module_t staticModuleRef = nullptr;
  vector<WasmFunctionSig*> types;
  vector<WasmFuncInstance*> funcs;
  vector<WasmTableInstance*> tables;
  vector<WasmMemoryInstance*> memories;
  vector<WasmGlobalInstance*> globals;
  vector<WasmExportInstance> exports;
};

struct WasmInstance {
  SET_STRUCT_MOVE_ONLY(WasmInstance);
  // start function, or main function;
  shared_ptr<PosPtr> startPoint = nullptr;
  bool startEntry = true;
  shared_ptr<WasmModuleInstance> module;
  shared_ptr<Store> store;
  shared_ptr<Stack> stack;
};

}  // namespace internal
}  // namespace twvm

#endif  // INSTANCES_H_
