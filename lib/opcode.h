// Copyright 2019 YHSPY. All rights reserved.
#ifndef OPCODE_H_
#define OPCODE_H_

namespace twvm {
namespace internal {

#define DECLARE_MEMBER_HANDLER(name, opcode) \
  static handlerProto do##name;

#define DECLARE_NAMED_ENUM(name, opcode) \
  kOpcode##name = opcode,

#define ITERATE_ALL_OPCODE(V) \
  ITERATE_CONTROL_OPCODE(V) \
  ITERATE_MISC_OPCODE(V) \
  ITERATE_LOAD_MEM_OPCODE(V) \
  ITERATE_STORE_MEM_OPCODE(V) \
  ITERATE_MISC_MEM_OPCODE(V) \
  ITERATE_SIMPLE_OPCODE(V) \

#define ITERATE_CONTROL_OPCODE(V) \
  V(Unreachable, 0x00) \
  V(Nop, 0x01) \
  V(Block, 0x02) \
  V(Loop, 0x03) \
  V(If, 0x04) \
  V(Else, 0x05) \
  V(End, 0x0b) \
  V(Br, 0x0c) \
  V(BrIf, 0x0d) \
  V(BrTable, 0x0e) \
  V(Return, 0x0f)

#define ITERATE_MISC_OPCODE(V) \
  V(Call, 0x10) \
  V(CallIndirect, 0x11) \
  V(Drop, 0x1a) \
  V(Select, 0x1b) \
  V(LocalGet, 0x20) \
  V(LocalSet, 0x21) \
  V(LocalTee, 0x22) \
  V(GlobalGet, 0x23) \
  V(GlobalSet, 0x24) \
  V(I32Const, 0x41) \
  V(I64Const, 0x42) \
  V(F32Const, 0x43) \
  V(F64Const, 0x44) \

#define ITERATE_LOAD_MEM_OPCODE(V) \
  V(I32LoadMem, 0x28) \
  V(I64LoadMem, 0x29) \
  V(F32LoadMem, 0x2a) \
  V(F64LoadMem, 0x2b) \
  V(I32LoadMem8S, 0x2c) \
  V(I32LoadMem8U, 0x2d) \
  V(I32LoadMem16S, 0x2e) \
  V(I32LoadMem16U, 0x2f) \
  V(I64LoadMem8S, 0x30) \
  V(I64LoadMem8U, 0x31) \
  V(I64LoadMem16S, 0x32) \
  V(I64LoadMem16U, 0x33) \
  V(I64LoadMem32S, 0x34) \
  V(I64LoadMem32U, 0x35)

#define ITERATE_STORE_MEM_OPCODE(V) \
  V(I32StoreMem, 0x36) \
  V(I64StoreMem, 0x37) \
  V(F32StoreMem, 0x38) \
  V(F64StoreMem, 0x39) \
  V(I32StoreMem8, 0x3a) \
  V(I32StoreMem16, 0x3b) \
  V(I64StoreMem8, 0x3c) \
  V(I64StoreMem16, 0x3d) \
  V(I64StoreMem32, 0x3e)

#define ITERATE_MISC_MEM_OPCODE(V) \
  V(MemorySize, 0x3f) \
  V(MemoryGrow, 0x40)

#define ITERATE_SIMPLE_OPCODE(V) \
  V(I32Eqz, 0x45) \
  V(I32Eq, 0x46) \
  V(I32Ne, 0x47) \
  V(I32LtS, 0x48) \
  V(I32LtU, 0x49) \
  V(I32GtS, 0x4a) \
  V(I32GtU, 0x4b) \
  V(I32LeS, 0x4c) \
  V(I32LeU, 0x4d) \
  V(I32GeS, 0x4e) \
  V(I32GeU, 0x4f) \
  V(I64Eqz, 0x50) \
  V(I64Eq, 0x51) \
  V(I64Ne, 0x52) \
  V(I64LtS, 0x53) \
  V(I64LtU, 0x54) \
  V(I64GtS, 0x55) \
  V(I64GtU, 0x56) \
  V(I64LeS, 0x57) \
  V(I64LeU, 0x58) \
  V(I64GeS, 0x59) \
  V(I64GeU, 0x5a) \
  V(F32Eq, 0x5b) \
  V(F32Ne, 0x5c) \
  V(F32Lt, 0x5d) \
  V(F32Gt, 0x5e) \
  V(F32Le, 0x5f) \
  V(F32Ge, 0x60) \
  V(F64Eq, 0x61) \
  V(F64Ne, 0x62) \
  V(F64Lt, 0x63) \
  V(F64Gt, 0x64) \
  V(F64Le, 0x65) \
  V(F64Ge, 0x66) \
  V(I32Clz, 0x67) \
  V(I32Ctz, 0x68) \
  V(I32Popcnt, 0x69) \
  V(I32Add, 0x6a) \
  V(I32Sub, 0x6b) \
  V(I32Mul, 0x6c) \
  V(I32DivS, 0x6d) \
  V(I32DivU, 0x6e) \
  V(I32RemS, 0x6f) \
  V(I32RemU, 0x70) \
  V(I32And, 0x71) \
  V(I32Or, 0x72) \
  V(I32Xor, 0x73) \
  V(I32Shl, 0x74) \
  V(I32ShrS, 0x75) \
  V(I32ShrU, 0x76) \
  V(I32Rol, 0x77) \
  V(I32Ror, 0x78) \
  V(I64Clz, 0x79) \
  V(I64Ctz, 0x7a) \
  V(I64Popcnt, 0x7b) \
  V(I64Add, 0x7c) \
  V(I64Sub, 0x7d) \
  V(I64Mul, 0x7e) \
  V(I64DivS, 0x7f) \
  V(I64DivU, 0x80) \
  V(I64RemS, 0x81) \
  V(I64RemU, 0x82) \
  V(I64And, 0x83) \
  V(I64Or, 0x84) \
  V(I64Xor, 0x85) \
  V(I64Shl, 0x86) \
  V(I64ShrS, 0x87) \
  V(I64ShrU, 0x88) \
  V(I64Rol, 0x89) \
  V(I64Ror, 0x8a) \
  V(F32Abs, 0x8b) \
  V(F32Neg, 0x8c) \
  V(F32Ceil, 0x8d) \
  V(F32Floor, 0x8e) \
  V(F32Trunc, 0x8f) \
  V(F32NearestInt, 0x90) \
  V(F32Sqrt, 0x91) \
  V(F32Add, 0x92) \
  V(F32Sub, 0x93) \
  V(F32Mul, 0x94) \
  V(F32Div, 0x95) \
  V(F32Min, 0x96) \
  V(F32Max, 0x97) \
  V(F32CopySign, 0x98) \
  V(F64Abs, 0x99) \
  V(F64Neg, 0x9a) \
  V(F64Ceil, 0x9b) \
  V(F64Floor, 0x9c) \
  V(F64Trunc, 0x9d) \
  V(F64NearestInt, 0x9e) \
  V(F64Sqrt, 0x9f) \
  V(F64Add, 0xa0) \
  V(F64Sub, 0xa1) \
  V(F64Mul, 0xa2) \
  V(F64Div, 0xa3) \
  V(F64Min, 0xa4) \
  V(F64Max, 0xa5) \
  V(F64CopySign, 0xa6) \
  V(I32WrapI64, 0xa7) \
  V(I32TruncF32S, 0xa8) \
  V(I32TruncF32U, 0xa9) \
  V(I32TruncF64S, 0xaa) \
  V(I32TruncF64U, 0xab) \
  V(I64ExtendI32S, 0xac) \
  V(I64ExtendI32U, 0xad) \
  V(I64TruncF32S, 0xae) \
  V(I64TruncF32U, 0xaf) \
  V(I64TruncF64S, 0xb0) \
  V(I64TruncF64U, 0xb1) \
  V(F32SConvertI32, 0xb2) \
  V(F32UConvertI32, 0xb3) \
  V(F32SConvertI64, 0xb4) \
  V(F32UConvertI64, 0xb5) \
  V(F32DemoteF64, 0xb6) \
  V(F64SConvertI32, 0xb7) \
  V(F64UConvertI32, 0xb8) \
  V(F64SConvertI64, 0xb9) \
  V(F64UConvertI64, 0xba) \
  V(F64PromoteF32, 0xbb) \
  V(I32ReinterpretF32, 0xbc) \
  V(I64ReinterpretF64, 0xbd) \
  V(F32ReinterpretI32, 0xbe) \
  V(F64ReinterpretI64, 0xbf)

#include <memory>
#include <vector>
#include <functional>
#include "lib/frames.h"
#include "lib/utility.h"
#include "lib/decoder.h"
#include "lib/instances.h"
#include "lib/executor.h"

// ahead declare;
struct WasmInstance;
class Executor;

using ::std::shared_ptr;
using ::std::vector;
using shared_wasm_t = shared_ptr<WasmInstance>;
using handlerProto = void (shared_wasm_t&, Executor*);
using ::std::function;

enum class WasmOpcode {
  ITERATE_ALL_OPCODE(DECLARE_NAMED_ENUM)
};

// opcode classifications (make sure to except unknown opcodes);
#define ITERATE_OPCODE_NAME_WITH_ONE_VAR_IMME(V) \
  V(Block) \
  V(Loop) \
  V(BrIf) \
  V(If) \
  V(Call) \
  V(CallIndirect) \
  V(LocalGet) \
  V(LocalSet) \
  V(LocalTee) \
  V(GlobalGet) \
  V(GlobalSet) \
  V(I32Const) \
  V(I64Const)

#define ITERATE_OPCODE_NAME_WITH_TWO_VAR_IMME(V) \
  ITERATE_LOAD_MEM_OPCODE(V) \
  ITERATE_STORE_MEM_OPCODE(V)

#define ITERATE_OPCODE_NAME_WITH_NON_VAR_IMME(V) \
  ITERATE_SIMPLE_OPCODE(V) \
  V(Nop) \
  V(Return) \
  V(Else) \
  V(End) \
  V(Drop) \
  V(Select) \


class OpCode {
 private:
  template <typename T>
  static void storeMemarg(
    shared_wasm_t &wasmIns,
    Executor *executor,
    const function<void(const int32_t, WasmMemoryInstance *const&, const T)>&);

  static void retrieveMemarg(
    shared_wasm_t &wasmIns,
    Executor *executor,
    const function<void(const int32_t, WasmMemoryInstance *const&)>&);

  template <typename T>
  static void retrieveDoubleRTVals(
    shared_wasm_t &wasmIns,
    Executor *executor,
    const function<void(const shared_ptr<Stack::ValueFrameStack>&, ValueFrame *const&, ValueFrame *const&)>&);
    
  template <typename T>
  static void retrieveSingleRTVal(
    shared_wasm_t &wasmIns,
    Executor *executor,
    const function<void(const shared_ptr<Stack::ValueFrameStack>&, ValueFrame *const&)>&);
 public:
  static uint32_t calcOpCodeEntityLen(const uint8_t* buf, WasmOpcode opcode) {
    #define OPCODE_CASE(name, ...) \
      case WasmOpcode::kOpcode##name:
    switch (opcode) {
      case WasmOpcode::kOpcodeF32Const: { return f32Size; }
      case WasmOpcode::kOpcodeF64Const: { return f64Size; }
      ITERATE_OPCODE_NAME_WITH_ONE_VAR_IMME(OPCODE_CASE) {
        return Decoder::calcPassBytes(buf);
      }
      // "memory_immediate";
      ITERATE_OPCODE_NAME_WITH_TWO_VAR_IMME(OPCODE_CASE) {
        return Decoder::calcPassBytes(buf, 2);
      }
      case WasmOpcode::kOpcodeBrTable: {
        // 
        return 0;
        break;
      }
      default: {
        return 0;
      }
    }
    return 0;
  }
  ITERATE_ALL_OPCODE(DECLARE_MEMBER_HANDLER)
};

}  // namespace internal
}  // namespace twvm

#endif  // OPCODE_H_
