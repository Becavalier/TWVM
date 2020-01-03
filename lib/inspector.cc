// Copyright 2019 YHSPY. All rights reserved.
#include <sstream>
#include <string>
#include <vector>
#include "lib/cmdline.h"
#include "lib/utility.h"
#include "lib/inspector.h"
#include "lib/include/macros.h"

namespace twvm {
namespace internal {

using ::std::stringstream;
using ::std::to_string;

void Inspector::inspect(shared_ptr<WasmInstance> wasmIns) {
  auto &printer = Printer::instance();
  // set up display format;
  printer.useHexFormat();

  (printer << '\n').debug();
  (printer << "- [INSPECTING PHASE] -\n").debug();

  // WasmFunctionSig;
  const auto &typeSize = wasmIns->module->types.size();
  (printer
    << "# Signatures (" << typeSize << "): "
    << (typeSize == 0 ? "N/A" : string()) << '\n').debug();
  for (const auto &type : wasmIns->module->types) {
    size_t i = 0;
    const auto reps = type->reps;
    printer << "[";
    printer << type->index << ": ";
    for (; i < type->paramsCount; i++) {
      printer << static_cast<int>(reps[i]) << ' ';
    }
    if (type->paramsCount == 0) { printer << "void"; }
    const auto slotCount = type->paramsCount + type->returnCount;
    for (; i < slotCount; i++) {
      printer << "-> " << static_cast<int>(reps[i]);
    }
    if (type->returnCount == 0) { printer << "void"; }
    printer << "]";
    printer.makeLine();
  }
  printer.printTableView();

  // WasmFunction;
  const auto &funcSize = wasmIns->module->funcs.size();
  (printer
    << "# Functions (" << funcSize << "): "
    << (funcSize == 0 ? "N/A" : string()) << '\n').debug();
  for (const auto &func : wasmIns->module->funcs) {
    printer << "[";
    printer << "sig_index " << func->type->index << " | "
      << "locals(" << func->staticProto->locals.size() << ")";
    for (const auto &type : func->staticProto->locals) {
      printer << static_cast<int>(type) << '\n';
    }
    printer << " | " << "code_size " << func->code->size() << " B]";
    printer.makeLine();
  }
  printer.printTableView();

  // WasmTable;
  const auto &tableSize = wasmIns->module->tables.size();
  (printer
    << "# Tables (" << tableSize << "): "
    << (tableSize == 0 ? "N/A" : string()) << '\n').debug();
  for (const auto &table : wasmIns->module->tables) {
    printer << "[";
    printer << "max_table_size " << table->maxTableSize << "]";
    printer.makeLine();
  }
  printer.printTableView();

  // WasmMemory;
  const auto &memorySize = wasmIns->module->memories.size();
  (printer << "# Memories (" << memorySize << "): " << (memorySize == 0 ? "N/A" : string()) << '\n').debug();
  for (const auto &memory : wasmIns->module->memories) {
    printer << "[";
    printer << "memory_size " << (memory->availableSize() / 1024) << " kib]";
    printer.makeLine();
  }
  printer.printTableView();

  // WasmGlobal;
  const auto &globalSize = wasmIns->module->globals.size();
  (printer
    << "# Globals (" << globalSize << "): "
    << (globalSize == 0 ? "N/A" : string()) << '\n').debug();
  for (const auto &global : wasmIns->module->globals) {
    printer << "[";
    printer << "global_type " << static_cast<int>(global->type) << " | ";
    printer << "mutability " << global->mutability << "]";
    printer.makeLine();
  }
  printer.printTableView();

  // WasmExport;
  const auto &exportSize = wasmIns->module->exports.size();
  (printer
    << "# Exports (" << exportSize << "): "
    << (exportSize == 0 ? "N/A" : string()) << '\n').debug();
  for (const auto &_export : wasmIns->module->exports) {
    printer << "[";
    printer << "export_name \"" << _export.name << "\" | ";
    printer << "export_type " << static_cast<int>(_export.type) << " | ";
    printer << "index " << _export.index << "]";
    printer.makeLine();
  }
  printer.printTableView();
}

}  // namespace internal
}  // namespace twvm
