// Copyright 2019 YHSPY. All rights reserved.
#ifndef LOADER_H_
#define LOADER_H_

namespace twvm {
namespace internal {

#define WRAP_BUF_VARUINT(type) \
  Decoder::readVarUint<type>(getAbsReaderEndpoint(), &currentReaderOffset)
#define WRAP_READER_VARUINT(type) \
  Decoder::readVarUint<type>(reader)
#define WRAP_BUF_VARINT(type) \
  Decoder::readVarInt<type>(getAbsReaderEndpoint(), &currentReaderOffset)
#define WRAP_BUF_UINT8() \
  Decoder::readUint8(getAbsReaderEndpoint(), &currentReaderOffset)
#define WRAP_BUF_UINT32() \
  Decoder::readUint32(getAbsReaderEndpoint(), &currentReaderOffset)
#define WRAP_BUF_UINT64() \
  Decoder::readUint64(getAbsReaderEndpoint(), &currentReaderOffset)
#define WRAP_BUF_STRING(strLen) \
  Decoder::decodeName(getAbsReaderEndpoint(), strLen, &currentReaderOffset)

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <ios>
#include <istream>
#include "lib/include/errors.h"
#include "lib/type.h"
#include "lib/module.h"
#include "lib/decoder.h"
#include "lib/opcode.h"
#include "lib/utility.h"

using ::std::vector;
using ::std::string;
using ::std::shared_ptr;
using ::std::make_shared;
using ::std::ifstream;
using ::std::basic_istream;

class Loader {
 private:
  static shared_ptr<Reader> reader;
  static vector<uint8_t> buf;
  static uint32_t byteCounter;
  static size_t currentReaderOffset;

  // analyzer invokers;
  static shared_module_t parse(const shared_module_t&);

  // analyzer helpers;
  static void parseSection(const shared_module_t&);
  static void parseUnkownSection(uint8_t, const shared_module_t&);
  static void parseTypeSection(const shared_module_t&);
  static void parseImportSection(const shared_module_t&);
  static void parseFunctionSection(const shared_module_t&);
  static void parseTableSection(const shared_module_t&);
  static void parseMemorySection(const shared_module_t&);
  static void parseStartSection(const shared_module_t&);
  static void parseGlobalSection(const shared_module_t&);
  static void parseExportSection(const shared_module_t&);
  static void parseCodeSection(const shared_module_t&);
  static void parseElementSection(const shared_module_t&);
  static void parseDataSection(const shared_module_t&);
  static void skipKnownSection(uint8_t, const shared_module_t&);
  static void validateKeyFields();

  static void retrieveBytes(uint32_t count) {
    buf.clear();
    currentReaderOffset = 0;
    while (!reader->hasReachEnd()) {
      buf.push_back(static_cast<uint8_t>(reader->read<>()));
      if (++byteCounter == count) {
        byteCounter = 0;
        break;
      }
    }
  }

  // feeding module pointer directly (due to MVP version);
  static void consumeMemoryParams(const shared_module_t& module) {
    const auto memoryFlags = WRAP_BUF_VARUINT(uint8_t);
    const auto initialPages = WRAP_BUF_VARUINT(uint32_t);
    auto memory = make_shared<WasmMemory>();
    memory->initialPages = initialPages;
    // (0 : no /1: has) maximum field;
    if (memoryFlags == kWasmTrue) {
      const auto maximumPages = WRAP_BUF_VARUINT(uint32_t);
      memory->maximumPages = maximumPages;
      memory->hasMaximumPages = true;
    }
    module->getMemory() = memory;
  }

  static void consumeTableParams(const shared_module_t& module, WasmTable *const table) {
    const auto tableFlags = WRAP_BUF_VARUINT(uint8_t);
    const auto initialSize = WRAP_BUF_VARUINT(uint32_t);
    table->initialSize = initialSize;
    if (tableFlags == kWasmTrue) {
      const auto maximumSize = WRAP_BUF_VARUINT(uint32_t);
      table->maximumSize = maximumSize;
      table->hasMaximumSize = true;
    }
  }

  static void consumeInitExpr(const shared_module_t& module, WasmInitExpr *const  expr) {
    const auto opcode = static_cast<WasmOpcode>(WRAP_BUF_UINT8());

    // MVP: i32.const / i64.const / f32.const / f64.const / get_global;
    switch (opcode) {
      case WasmOpcode::kOpcodeI32Const: {
        expr->kind = InitExprKind::kI32Const;
        expr->val.vI32Const = WRAP_BUF_VARINT(int32_t);
        break;
      }
      case WasmOpcode::kOpcodeI64Const: {
        expr->kind = InitExprKind::kI64Const;
        expr->val.vI64Const = WRAP_BUF_VARINT(int64_t);
        break;
      }
      case WasmOpcode::kOpcodeF32Const: {
        expr->kind = InitExprKind::kF32Const;
        expr->val.vF32Const = WRAP_BUF_UINT32();
        break;
      }
      case WasmOpcode::kOpcodeF64Const: {
        expr->kind = InitExprKind::kF64Const;
        expr->val.vF64Const = WRAP_BUF_UINT64();
        break;
      }
      case WasmOpcode::kOpcodeGlobalSet: {
        const auto globalIndex = WRAP_BUF_VARUINT(uint32_t);
        const auto moduleGlobal = module->getGlobal(globalIndex);
        if (moduleGlobal->mutability || !moduleGlobal->imported) {
          Printer::instance().error(Errors::LOADER_INVALID_GLOBAL_IMPORT_EXPR);
        }
        expr->kind = InitExprKind::kGlobalIndex;
        expr->val.vGlobalIndex = globalIndex;
        break;
      }
      default: {
        Printer::instance().error(Errors::LOADER_INVALID_OPCODE);
        break;
      }
    }
    // "0x0b" ending byte;
    if (static_cast<WasmOpcode>(WRAP_BUF_UINT8()) != WasmOpcode::kOpcodeEnd) {
      Printer::instance().error(Errors::LOADER_ILLEGAL_END);
    }
  }

 public:
  static shared_module_t init(const string&);
  static shared_module_t init(uint8_t*, size_t);
  static shared_module_t init(vector<uint8_t>);

  static inline uint8_t* getAbsReaderEndpoint() {
    return buf.data() + currentReaderOffset;
  }
};

}  // namespace internal
}  // namespace twvm

#endif  // LOADER_H_
