#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_
#include <iostream>
#include <mlua.h>
#include <string_view>
#include <tuple>

namespace VM {
const uint32_t MAXARG_BX = (1 << 18) - 1;
const uint32_t MAXARG_SBX = MAXARG_BX >> 1;

template <typename LVM> struct Instruction {
  uint32_t code;

  Instruction(CInt _c) : code(_c) {}

  uint32_t op_code() { return code & 0x3f; }
  std::tuple<uint32_t, uint32_t, uint32_t> abc() {
    return {(code >> 6) & 0xff, (code >> 23) & 0x1ff, (code >> 14) & 0x1ff};
  }
  std::pair<uint32_t, uint32_t> abx() {
    return {(code >> 6) & 0xff, code >> 14};
  }
  std::pair<uint32_t, int32_t> asbx() {
    auto [a, bx] = abx();
    return {a, (int32_t)(bx)-MAXARG_SBX};
  }
  uint32_t ax() { return code >> 6; }
  std::string_view name() { return opcodes[op_code()].name; }
  Byte op_mode() { return opcodes[op_code()].op_mode; }
  Byte arg_b_mode() { return opcodes[op_code()].arg_b_mode; }
  Byte arg_c_mode() { return opcodes[op_code()].arg_c_mode; }

  void execute(LVM &vm) {
    auto action = opcodes[this->op_code()].action;
    if (action) {
      action(*this, vm);
    } else {
      throw std::runtime_error(this->name().data());
    }
    // print_stack(vm);
  }
};
} // namespace VM
#endif