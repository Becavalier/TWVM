// Copyright 2019 YHSPY. All rights reserved.
#ifndef INCLUDE_ERRORS_H_
#define INCLUDE_ERRORS_H_

#include <string>
#include <unordered_map>

using ::std::string;
using ::std::unordered_map;

#define DECLARE_ENUM_KEY(name, opcode) \
  name,
#define DECLARE_MAP_PAIR(name, value) \
  {Errors::name, value},

#define ITERATE_ALL_ERRORS(V) \
  ITERATE_MISC_ERRORS(V) \
  ITERATE_LOADER_ERRORS(V) \
  ITERATE_CMDLINE_ERRORS(V) \
  ITERATE_RUNTIME_ERRORS(V)
  
#define ITERATE_LOADER_ERRORS(V) \
  V(LOADER_ILLEGAL_END, "illegal ending byte.") \
  V(LOADER_INVALID_OPCODE, "invalid opcode found.") \
  V(LOADER_INVALID_GLOBAL_IMPORT_EXPR, "only immutable imported globals can be used in initializer expressions.") \
  V(LOADER_NOT_DEFAULT_TABLE, "only the default table can be manipulated in MVP.") \
  V(LOADER_NO_TABLE, "no table found to apply the element section.") \
  V(LOADER_UNKNOWN_EXPORT, "unkonwn export type.") \
  V(LOADER_UNKNOWN_IMPORT, "unkonwn import type.") \
  V(LOADER_INVALID_MEM_INDEX, "invalid memory index.") \
  V(LOADER_INVALID_START_FUNC, "the start function must not take any arguments or return value.") \
  V(LOADER_INVALID_MEM_NUM, "only one memory section can be supported in MVP.") \
  V(LOADER_INVALID_TABLE_ELE, "only \"anyfunc\" type can be supported in table section.") \
  V(LOADER_TYPE_MISMATCH, "type section code mismatch.") \
  V(LOADER_UNKNOWN_SECTION, "unidentified section found.") \
  V(LOADER_INVALID_FILE, "can not reading the input file.") \
  V(LOADER_INVALID_WASM_VERSION, "invalid wasm version, expect 0x01.") \
  V(LOADER_INVALID_WASM_MAGIC, "invalid wasm magic word, expect 0x6d736100.") \
  V(LOADER_MEM_ALLOC_ERR, "memory allocating error.") \
  V(LOADER_MEM_ALLOC_SIZE_ERR, "invalid memory allocation size.")

#define ITERATE_CMDLINE_ERRORS(V) \
  V(CMD_NO_FILE, "no input file.") \
  V(CMD_UNKNOWN_OPT, "unknown option.") \
  V(CMD_UNKNOWN_ARG, "unknown argument for this option.") \
  V(CMD_INVALID_SECOND_ARG, "unexpected second argument.") \
  V(CMD_UNEXPECTED_ARG, "couldn\'t find expected argument for this option.") \
  V(CMD_UNEXPECTED_SECOND_POS_ARG, "unexpected second positional argument for this option.\'") \
  V(CMD_UNEXPECTED_POS_ARG, "unexpected positional argument for this option.\'")

#define ITERATE_RUNTIME_ERRORS(V) \
  V(RT_UNREACHABLE_CODE, "unreachable code.") \
  V(RT_ARITY_MISMATCH, "return arity mismatch of the function.") \
  V(RT_INVALID_END, "invalide \"end(0xb)\" condition.") \
  V(RT_INVALID_BRANCH_DEPTH, "invalid branching depth.") \
  V(RT_OPERANDS_NOT_ENOUGH, "operands not enough to be consumed.") \
  V(RT_INVALID_FUNC_INDEX, "invalid function index to be called.") \
  V(RT_MEM_ACCESS_OOB, "memory access out of bound.") \
  V(RT_INVALID_STACK_VAL, "invalid stack on-top value type.") \
  V(RT_OPERANDS_TYPE_MISMATCH, "wrong operands type on the top of the current stack.") \
  V(RT_DIV_BY_ZERO, "division can not by zero.") \
  V(RT_DIV_UNREPRESENTABLE, "the division result can not be representable.")

#define ITERATE_MISC_ERRORS(V) \
  V(MISC_EXPR_NOT_INIT, "initial expression has not been initialized.") \
  V(MISC_INVALID_VALUEFRAME, "invalid type of \"ValueFrame\".")


enum Errors {
  ITERATE_ALL_ERRORS(DECLARE_ENUM_KEY)
};

static unordered_map<Errors, string> errorMapper = {
  ITERATE_ALL_ERRORS(DECLARE_MAP_PAIR)
};

#endif  // INCLUDE_ERRORS_H_
