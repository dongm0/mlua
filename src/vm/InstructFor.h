#ifndef INSTRUCTFOR_H_
#define INSTRUCTFOR_H_
#include "Instruction.h"
#include <api/LuaState.h>
#include <api/core/LuaValue.h>

namespace VM {

template <typename LVM> void for_prep(Instruction<LVM> i, LVM &vm) {
  auto [a, sbx] = i.asbx();
  a += 1;

  vm.push_value(a);
  vm.push_value(a + 2);
  vm.arith(API::ArithOp::LUA_OPSUB);
  vm.replace(a);
  LVM::LuaVmInterface::add_pc(vm, sbx);
}

template <typename LVM> void for_loop(Instruction<LVM> i, LVM &vm) {
  auto [a, sbx] = i.asbx();
  a += 1;

  vm.push_value(a + 2);
  vm.push_value(a);
  vm.arith(API::ArithOp::LUA_OPADD);
  vm.replace(a);

  auto is_positive = (vm.to_number(a + 2) >= 0);
  if ((is_positive && vm.compare(a, a + 1, API::CompareOp::LUA_OPLE)) ||
      (!is_positive && vm.compare(a + 1, a, API::CompareOp::LUA_OPLE))) {
    LVM::LuaVmInterface::add_pc(vm, sbx);
    vm.copy(a, a + 3);
  }
}
} // namespace VM

#endif