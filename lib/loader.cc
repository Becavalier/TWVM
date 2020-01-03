// Copyright 2019 YHSPY. All rights reserved.
#include <fstream>
#include <array>
#include <stdexcept>
#include <iostream>
#include <vector>
#include "lib/loader.h"
#include "lib/include/constants.h"
#include "lib/include/errors.h"
#include "lib/utility.h"
#include "lib/opcode.h"

namespace twvm {
namespace internal {

vector<uint8_t> Loader::buf;
shared_ptr<Reader> Loader::reader = nullptr;
uint32_t Loader::byteCounter = 0;
size_t Loader::currentReaderOffset = 0;

using ::std::endl;
using ::std::hex;
using ::std::ifstream;
using ::std::ios;
using ::std::vector;
using ::std::array;

shared_module_t Loader::init(const ::std::string &fileName) {
  Utility::drawLogoGraphic();
  (Printer::instance() << "- [LOADING PHASE] -\n").debug();
  ifstream in(fileName, ios::binary);
  shared_module_t wasmModule = make_shared<Module>();
  if (in.is_open() && in.good()) {
    reader = make_shared<Reader>(&in);
    // check magic number and verison field;
    validateKeyFields();
  } else {
    Printer::instance().error(Errors::LOADER_INVALID_FILE);
  }
  // parsing start;
  return parse(wasmModule);
}

shared_module_t Loader::init(uint8_t *buffer, size_t len) {
  Utility::drawLogoGraphic();
  (Printer::instance() << "- [LOADING PHASE] -\n").debug();
  shared_module_t wasmModule = make_shared<Module>();
  // use buffer way;
  reader = make_shared<Reader>(buffer, len);
  // check magic number and verison field;
  validateKeyFields();
  // parsing start;
  return parse(wasmModule);
}

shared_module_t Loader::init(vector<uint8_t> buffer) {
  return Loader::init(buffer.data(), buffer.size());
}

void Loader::validateKeyFields() {
  retrieveBytes(charSize * 8);
  if (WRAP_BUF_UINT32() != kWasmMagicWord) {
    Printer::instance().error(Errors::LOADER_INVALID_WASM_MAGIC);
  }
  if (WRAP_BUF_UINT32() != kWasmVersion) {
    Printer::instance().error(Errors::LOADER_INVALID_WASM_VERSION);
  }
}

shared_module_t Loader::parse(const shared_module_t &module) {
  parseSection(module);
  return module;
}

void Loader::parseSection(const shared_module_t &module) {
  while (!reader->hasReachEnd()) {
    const auto sectionCode = WRAP_READER_VARUINT(uint8_t);
    const auto sectionCodeType = static_cast<SectionTypesCode>(sectionCode);

    switch (sectionCodeType) {
      case SectionTypesCode::kUnknownSection: { parseUnkownSection(sectionCode, module); break; }
      case SectionTypesCode::kTypeSection: { parseTypeSection(module); break; }
      case SectionTypesCode::kImportSection: { parseImportSection(module); break; }
      case SectionTypesCode::kFunctionSection: { parseFunctionSection(module); break; }
      case SectionTypesCode::kTableSection: { parseTableSection(module); break; }
      case SectionTypesCode::kMemorySection: { parseMemorySection(module); break; }
      case SectionTypesCode::kStartSection: { parseStartSection(module); break; }
      case SectionTypesCode::kGlobalSection: { parseGlobalSection(module); break; }
      case SectionTypesCode::kExportSection: { parseExportSection(module); break; }
      case SectionTypesCode::kCodeSection: { parseCodeSection(module); break; }
      case SectionTypesCode::kElementSection: { parseElementSection(module); break; }
      case SectionTypesCode::kDataSection: { parseDataSection(module); break; }
      default: {
        Printer::instance().error(Errors::LOADER_UNKNOWN_SECTION);
        break;
      }
    }
  }
  buf.clear();
}

void Loader::parseUnkownSection(uint8_t sectionCode, const shared_module_t &module) {
  // buzzle;
  (Printer::instance().useHexFormat()
    << "customized section code: " << static_cast<int>(sectionCode) << ".\n").warn();
  retrieveBytes(WRAP_READER_VARUINT(uint32_t));
}

void Loader::parseTypeSection(const shared_module_t &module) {
  (Printer::instance() << "parsing type section.\n").debug();
  // payload length;
  retrieveBytes(WRAP_READER_VARUINT(uint32_t));
  // self-counting from "buf";
  const auto entryCount = WRAP_BUF_VARUINT(uint32_t);
  for (uint32_t i = 0; i < entryCount; i++) {
    if (static_cast<ValueTypesCode>(WRAP_BUF_UINT8()) == ValueTypesCode::kFunc) {
      module->getFunctionSig()->emplace_back();
      const auto sig = &module->getFunctionSig()->back();
      const auto paramsCount = WRAP_BUF_VARUINT(uint32_t);
      for (uint32_t j = 0; j < paramsCount; j++) {
        sig->reps.push_back(static_cast<ValueTypesCode>(WRAP_BUF_UINT8()));
      }
      const auto returnCount = WRAP_BUF_VARUINT(uint8_t);
      for (uint32_t j = 0; j < returnCount; j++) {
        sig->reps.push_back(static_cast<ValueTypesCode>(WRAP_BUF_UINT8()));
      }
      sig->index = i;
      sig->paramsCount = paramsCount;
      sig->returnCount = returnCount;
    } else {
      Printer::instance().error(Errors::LOADER_TYPE_MISMATCH);
    }
  }
}

void Loader::parseImportSection(const shared_module_t &module) {
  (Printer::instance() << "parsing import section.\n").debug();
  retrieveBytes(WRAP_READER_VARUINT(uint32_t));
  const auto importCount = WRAP_BUF_VARUINT(uint32_t);
  for (uint32_t i = 0; i < importCount; i++) {
    // set module name;
    const auto moduleName = WRAP_BUF_STRING(WRAP_BUF_VARUINT(uint32_t));

    // set field name;
    const auto fieldName = WRAP_BUF_STRING(WRAP_BUF_VARUINT(uint32_t));
    const auto importType = static_cast<ExternalTypesCode>(WRAP_BUF_UINT8());
    uint32_t index = 0;
    switch (importType) {
      case ExternalTypesCode::kExternalFunction: {
        const auto sigIndex = WRAP_BUF_VARUINT(uint32_t);
        const auto sig = module->getFunctionSig(sigIndex);
        index = module->getFunction()->size();
        module->importedFuncCount++;
        module->getFunction()->push_back({sig, index, sigIndex, {}, {}, 0, true, false});
        break;
      }
      case ExternalTypesCode::kExternalTable: {
        // MVP: only support "anyfunc" (by default);
        const auto tableType = static_cast<ValueTypesCode>(WRAP_BUF_UINT8());
        if (tableType != ValueTypesCode::kFuncRef) {
          Printer::instance().error(Errors::LOADER_INVALID_TABLE_ELE);
        }
        index = module->getTable()->size();
        module->importedTableCount++;

        // insert new element by placement-new && move;
        module->getTable()->emplace_back();
        auto *table = &module->getTable()->back();
        table->imported = true;
        table->type = tableType;
        consumeTableParams(module, table);
        break;
      }
      case ExternalTypesCode::kExternalMemory: {
        consumeMemoryParams(module);
        break;
      }
      case ExternalTypesCode::kExternalGlobal: {
        // TODO(Jason Yu) ;
        break;
      }
      default: {
        Printer::instance().error(Errors::LOADER_UNKNOWN_IMPORT);
      }
    }
    module->getImport()->push_back({moduleName, fieldName, importType, index});
  }
}

void Loader::parseFunctionSection(const shared_module_t &module) {
  (Printer::instance() << "parsing function section.\n").debug();
  retrieveBytes(WRAP_READER_VARUINT(uint32_t));
  const auto declaredFuncCount = WRAP_BUF_VARUINT(uint32_t);
  for (uint32_t i = 0; i < declaredFuncCount; i++) {
    module->getFunction()->emplace_back();
    const auto func = &module->getFunction()->back();
    // indices: uint32_t;
    const auto sigIndex = WRAP_BUF_VARUINT(uint32_t);
    const auto sig = module->getFunctionSig(sigIndex);
    const auto funcIndex = module->getFunction()->size();
    func->sig = sig;
    func->sigIndex = sigIndex;
    func->funcIndex = funcIndex;
  }
}

void Loader::parseTableSection(const shared_module_t &module) {
  (Printer::instance() << "parsing table section.\n").debug();
  retrieveBytes(WRAP_READER_VARUINT(uint32_t));
  const auto tableCount = WRAP_BUF_VARUINT(uint32_t);
  for (uint32_t i = 0; i < tableCount; i++) {
    // MVP: only support "anyfunc" (by default);
    const auto tableType = static_cast<ValueTypesCode>(WRAP_BUF_UINT8());
    if (tableType != ValueTypesCode::kFuncRef) {
      Printer::instance().error(Errors::LOADER_INVALID_TABLE_ELE);
    }

    // insert new element by placement-new && move;
    module->getTable()->emplace_back();
    auto *table = &module->getTable()->back();
    table->type = tableType;
    consumeTableParams(module, table);
  }
}

void Loader::parseMemorySection(const shared_module_t &module) {
  (Printer::instance() << "parsing memory section.\n").debug();
  retrieveBytes(WRAP_READER_VARUINT(uint32_t));
  const auto memeoryCount = WRAP_BUF_VARUINT(uint32_t);
  // determine whether the memory has been initialized via "import";
  if (memeoryCount > 1 || module->getMemory() != nullptr) {
    Printer::instance().error(Errors::LOADER_INVALID_MEM_NUM);
  } else {
    consumeMemoryParams(module);
  }
}

void Loader::parseStartSection(const shared_module_t &module) {
  (Printer::instance() << "parsing start section.\n").debug();
  retrieveBytes(WRAP_READER_VARUINT(uint32_t));
  const auto startFuncIndex = WRAP_BUF_VARUINT(uint32_t);
  // start function: no arguments or return value;
  const auto sig = module->getFunction(startFuncIndex)->sig;
  if (sig->paramsCount != 0 || sig->returnCount != 0) {
    Printer::instance().error(Errors::LOADER_INVALID_START_FUNC);
  } else {
    module->startFuncIndex = startFuncIndex;
    module->hasValidStartFunc = true;
  }
}

void Loader::parseGlobalSection(const shared_module_t &module) {
  (Printer::instance() << "parsing global section.\n").debug();
  retrieveBytes(WRAP_READER_VARUINT(uint32_t));
  const auto globalCount = WRAP_BUF_VARUINT(uint32_t);
  for (uint32_t i = 0; i < globalCount; i++) {
    const auto contentType = static_cast<ValueTypesCode>(WRAP_BUF_UINT8());
    const auto mutability = WRAP_BUF_UINT8() == kWasmTrue;

    // insert new element by placement-new && move;
    module->getGlobal()->emplace_back();
    auto *thisGlobal = &module->getGlobal()->back();
    thisGlobal->type = contentType;
    thisGlobal->mutability = mutability;

    // deal with opcode;
    consumeInitExpr(module, &thisGlobal->init);
  }
}

void Loader::parseExportSection(const shared_module_t &module) {
  (Printer::instance() << "parsing export section.\n").debug();
  retrieveBytes(WRAP_READER_VARUINT(uint32_t));
  const auto exportCount = WRAP_BUF_VARUINT(uint32_t);
  for (uint32_t i = 0; i < exportCount; i++) {
    const auto nameLen = WRAP_BUF_VARUINT(uint32_t);
    const auto name = WRAP_BUF_STRING(nameLen);
    const auto exportType = static_cast<ExternalTypesCode>(WRAP_BUF_UINT8());
    uint32_t index = 0;
    switch (exportType) {
      case ExternalTypesCode::kExternalFunction: {
        index = WRAP_BUF_VARUINT(uint32_t);
        const auto funcIns = module->getFunction(index);
        funcIns->exported = true;
        break;
      }
      case ExternalTypesCode::kExternalTable: {
        index = WRAP_BUF_VARUINT(uint32_t);
        const auto table = module->getTable(index);
        if (table != nullptr) {
          table->exported = true;
        }
        break;
      }
      case ExternalTypesCode::kExternalMemory: {
        index = WRAP_BUF_VARUINT(uint32_t);
        const auto mem = module->getMemory();
        if (index != 0 || mem == nullptr) {
          Printer::instance().error(Errors::LOADER_INVALID_MEM_INDEX);
        } else {
          mem->exported = true;
        }
        break;
      }
      case ExternalTypesCode::kExternalGlobal: {
        index = WRAP_BUF_VARUINT(uint32_t);
        const auto global = module->getGlobal(index);
        if (global != nullptr) {
          global->exported = true;
        }
        break;
      }
      default: {
        Printer::instance().error(Errors::LOADER_UNKNOWN_EXPORT);
      }
    }
    module->getExport()->push_back({name, exportType, index});
  }
}

void Loader::parseCodeSection(const shared_module_t &module) {
  (Printer::instance() << "parsing code section.\n").debug();
  [[maybe_unused]] const auto payloadLen = WRAP_READER_VARUINT(uint32_t);
  const auto bodyCount = WRAP_READER_VARUINT(uint32_t);
  for (uint32_t i = 0; i < bodyCount; i++) {
    const auto bodySize = WRAP_READER_VARUINT(uint32_t);
    retrieveBytes(bodySize);
    // update function body;
    const auto function = module->getFunction(module->importedFuncCount + i);
    // resolve locals;
    const auto localEntryCount = WRAP_BUF_VARUINT(uint32_t);
    for (uint32_t j = 0; j < localEntryCount; j++) {
      const auto localCount = WRAP_BUF_VARUINT(uint32_t);
      const auto localTypeCode = WRAP_BUF_VARUINT(uint8_t);
      const auto type = static_cast<ValueFrameTypes>(localTypeCode);
      for (uint32_t k = 0; k < localCount; k++) {
        function->locals.push_back(type);
      }
    }
    function->codeLen = bodySize - currentReaderOffset;
    uint32_t innerScopeLen = 0;
    for (size_t j = 0; j < function->codeLen; j++) {
      // use TTC by default;
#if defined(OPT_DCT)
      const auto byte = WRAP_BUF_UINT8();
      const auto opcode = static_cast<WasmOpcode>(byte);
      auto codeBucket = &function->code;
      if (innerScopeLen != 0) {
        codeBucket->push_back(byte);
        innerScopeLen--;
        continue;
      }
      // simple DCT (one-time transforming);
      #define DEAL_ONE_VAR_IMME_OPCODE(name, ...) \
        case WasmOpcode::kOpcode##name: { \
          Utility::savePtrIntoBytes<handlerProto>(codeBucket, &OpCode::do##name); \
          auto innerOffset = 1; \
          while(true) { \
            const auto nextVal = WRAP_BUF_UINT8(); \
            codeBucket->push_back(nextVal); \
            if (!(nextVal & 0x80)) { break; } \
            innerOffset++; \
          } \
          j += innerOffset; \
          break; \
        }
      #define DEAL_TWO_VAR_IMME_OPCODE(name, ...) \
        case WasmOpcode::kOpcode##name: { \
          Utility::savePtrIntoBytes<handlerProto>(codeBucket, &OpCode::do##name); \
          auto innerOffset = 1; \
          for (auto k = 0; k < 2; k++) { \
            while(true) { \
              const auto nextVal = WRAP_BUF_UINT8(); \
              codeBucket->push_back(nextVal); \
              if (!(nextVal & 0x80)) { break; } \
              innerOffset++; \
            } \
            j += innerOffset; \
          } \
          break; \
        }
      #define DEAL_NON_VAR_IMME_OPCODE(name, ...) \
        case WasmOpcode::kOpcode##name: { \
          Utility::savePtrIntoBytes<handlerProto>(codeBucket, &OpCode::do##name); break; }
      // keep the raw opcode for identifying purpose;
      codeBucket->push_back(byte);
      switch (opcode) {
        // special cases;
        case WasmOpcode::kOpcodeF32Const: {
          Utility::savePtrIntoBytes<handlerProto>(codeBucket, &OpCode::doF32Const);
          innerScopeLen = f32Size; break; }
        case WasmOpcode::kOpcodeF64Const: {
          Utility::savePtrIntoBytes<handlerProto>(codeBucket, &OpCode::doF64Const);
          innerScopeLen = f64Size; break; }
        ITERATE_OPCODE_NAME_WITH_ONE_VAR_IMME(DEAL_ONE_VAR_IMME_OPCODE)
        ITERATE_OPCODE_NAME_WITH_TWO_VAR_IMME(DEAL_TWO_VAR_IMME_OPCODE)
        ITERATE_OPCODE_NAME_WITH_NON_VAR_IMME(DEAL_NON_VAR_IMME_OPCODE)
        default: {
          Printer::instance().error(Errors::LOADER_INVALID_OPCODE);
        };
      }
#else
      function->code.push_back(WRAP_BUF_UINT8());
#endif
    }
  }
}

// initialize imported/internally-defined table;
void Loader::parseElementSection(const shared_module_t &module) {
  (Printer::instance() << "parsing element section.\n").debug();
  retrieveBytes(WRAP_READER_VARUINT(uint32_t));
  const auto elemCount = WRAP_BUF_VARUINT(uint32_t);
  if (elemCount > 0 && module->getTable()->size() == 0) {
    Printer::instance().error(Errors::LOADER_NO_TABLE);
  }
  for (uint32_t i = 0; i < elemCount; i++) {
    const auto tableIndex = WRAP_BUF_VARUINT(uint32_t);
    if (tableIndex != 0) {
      Printer::instance().error(Errors::LOADER_NOT_DEFAULT_TABLE);
    }
    module->getElement()->emplace_back();
    auto *elem = &module->getElement()->back();
    elem->tableIndex = tableIndex;

    // deal with opcode;
    consumeInitExpr(module, &elem->init);

    // # of the element exprs;
    const auto elemExprCount = WRAP_BUF_VARUINT(uint32_t);
    for (uint32_t j = 0; j < elemExprCount; j++) {
      elem->entities.push_back(module->getFunction(WRAP_BUF_VARUINT(uint32_t)));
    }
  }
}

void Loader::parseDataSection(const shared_module_t &module) {
  (Printer::instance() << "parsing data section.\n").debug();
}

}  // namespace internal
}  // namespace twvm
