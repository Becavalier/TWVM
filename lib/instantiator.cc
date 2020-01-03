// Copyright 2019 YHSPY. All rights reserved.
#include <algorithm>
#include <string>
#include <memory>
#include "lib/instantiator.h"
#include "lib/store.h"
#include "lib/decoder.h"
#include "lib/utility.h"

namespace twvm {
namespace internal {

using ::std::find_if;
using ::std::hex;
using ::std::showbase;
using ::std::to_string;
using ::std::make_shared;

const shared_ptr<WasmInstance> Instantiator::instantiate(shared_module_t module) {
  (Printer::instance() << '\n').debug();
  (Printer::instance() << "- [INSTANTIATING PHASE] -\n").debug();

  // produce store, stack and module instance;
  (Printer::instance() << "instantiating (store, stack and instances).\n").debug();
  const auto store = make_shared<Store>();
  const auto stack = make_shared<Stack>();
  const auto moduleInst = make_shared<WasmModuleInstance>();
  const auto staticFuncTypes = module->getFunctionSig();

  // keep the static module alive;
  moduleInst->staticModuleRef = module;
  for (auto &i : (*staticFuncTypes)) {
    moduleInst->types.push_back(&i);
  }

  // memory instance;
  (Printer::instance() << "store: creating memory instances.\n").debug();
  const auto staticMemory = module->getMemory();
  if (staticMemory) {
    // we can not use "push_back" here, -
    // since the destructor will be called when the temp value is copied by default copy-constructor -
    // (even for move-constructor, we didn't use ::std::move), and the memory we allocated will be lost.
    // so, only allow the way of "placement-new" here.
    store->memoryInsts.emplace_back(staticMemory->initialPages, staticMemory->maximumPages);
    moduleInst->memories.push_back(&store->memoryInsts.back());
    // TODO(Jason Yu): init data section;
  }
  
  // function instances;
  (Printer::instance() << "store: creating function instances.\n").debug();
  const auto staticFunctions = module->getFunction();
  for (auto &i : *staticFunctions) {
    store->functionInsts.emplace_back();
    const auto ins = &store->functionInsts.back();
    ins->code = &i.code;
    ins->type = i.sig;
    ins->module = moduleInst;
    ins->staticProto = &i;
  }
  // We need to perform this loop separately, since -
  // the address of the vector elements are not stable due to the "resize" of each "*_back";
  for (auto &i : store->functionInsts) {
    moduleInst->funcs.push_back(&i);
  }

  // global instances;
  (Printer::instance() << "store: creating global instances.\n").debug();
  const auto staticGlobal = module->getGlobal();
  for (auto &i : *staticGlobal) {
    // skip platform-hosting imported global;
    if (i.type != ValueTypesCode::kFunc) {
      store->globalInsts.push_back({i.type, i.init.toRTValue(), i.mutability});
    }
  }
  for (auto &i : store->globalInsts) {
    moduleInst->globals.push_back(&i);
  }

  // table instances;
  (Printer::instance() << "store: creating table instances.\n").debug();
  const auto staticTable = module->getTable();
  for (auto &i : *staticTable) {
    store->tableInsts.push_back({i.maximumSize});
    const auto tableInst = &store->tableInsts.back();
    for (auto &j : *module->getElement()) {
      // MVP: use default element section;
      if (j.tableIndex == 0) {
        for (const auto p : j.entities) {
          tableInst->funcIndices.push_back(p);
        }
      }
    }
  }
  for (auto &i : store->tableInsts) {
    moduleInst->tables.push_back(&i);
  }

  // export instances;
  (Printer::instance() << "store: creating export instances.\n").debug();
  const auto staticExport = module->getExport();
  for (auto &i : *staticExport) {
    moduleInst->exports.push_back({i.name, i.type, i.index});
  }

  // global Wasm instance;
  const auto wasmIns = make_shared<WasmInstance>();
  wasmIns->module = moduleInst;
  wasmIns->store = store;
  wasmIns->stack = stack;

  // setup start point;
  if (module->hasValidStartFunc) {
    const uint32_t startFunctionIndex = module->startFuncIndex;
    const auto wasmFunc = &store->functionInsts.at(startFunctionIndex);
    wasmIns->startPoint = make_shared<PosPtr>(startFunctionIndex, wasmFunc->code);
    stack->activationStack->emplace({
      wasmFunc,
      stack->valueStack->size(),
      stack->labelStack->size()});
  } else {
    const auto exportItB = module->getExport()->begin();
    const auto exportItE = module->getExport()->end();
    const auto it = find_if(exportItB, exportItE, [](const WasmExport &exportEntity) -> auto {
      return exportEntity.name == "main"
        && exportEntity.type == ExternalTypesCode::kExternalFunction;
    });
    if (it != exportItE) {
      const auto wasmFunc = &store->functionInsts.at(it->index);
      wasmIns->startPoint = make_shared<PosPtr>(it->index, wasmFunc->code);
      wasmIns->startEntry = false;
      stack->activationStack->emplace({
        wasmFunc,
        stack->valueStack->size(),
        stack->labelStack->size()});
    }
  }

  return wasmIns;
}

}  // namespace internal
}  // namespace twvm
