#ifndef INSTRUCTTABLE_H_
#define INSTRUCTTABLE_H_

#include "Instruction.h"
#include "fpb.h"
#include <api/LuaState.h>
namespace VM {
const int64_t LFIELDS_PER_FLUSH = 50;
class LuaState;

template <typename LVM> void new_table(Instruction<LVM> i, LVM &vm) {
  auto [a, b, c] = i.abc();
  a += 1;
  vm.create_table(fb_to_int(b), fb_to_int(c));
  vm.replace(a);
}

template <typename LVM> void get_table(Instruction<LVM> i, LVM &vm) {
  auto [a, b, c] = i.abc();
  a += 1, b += 1;
  LVM::LuaVmInterface::get_rk(vm, c);
  vm.get_table(b);
  vm.replace(a);
}
template <typename LVM> void set_table(Instruction<LVM> i, LVM &vm) {
  auto [a, b, c] = i.abc();
  a += 1;
  LVM::LuaVmInterface::get_rk(vm, b);
  LVM::LuaVmInterface::get_rk(vm, c);
  vm.set_table(a);
}
template <typename LVM> void set_list(Instruction<LVM> i, LVM &vm) {
  auto [a, b, c] = i.abc();
  a += 1;
  if (c > 0) {
    c = c - 1;
  } else {
    c = Instruction<LVM>(LVM::LuaVmInterface::fetch(vm)).ax();
  }
  auto idx = (LInt)(c * LFIELDS_PER_FLUSH);
  for (uint32_t j = 1; j <= b; ++j) {
    ++idx;
    vm.push_value(a + j);
    vm.set_i(a, idx);
  }

  auto b_is_zero = (b == 0);
  if (b_is_zero) {
    b = vm.to_integer(-1) - a - 1;
    vm.pop(1);
  }
  if (b_is_zero) {
    for (auto j = LVM::LuaVmInterface::register_count(vm) + 1;
         j <= vm.get_top(); ++j) {
      ++idx;
      vm.push_value(j);
      vm.set_i(a, idx);
    }
    vm.set_top(LVM::LuaVmInterface::register_count(vm));
  }
}
} // namespace VM

#endif