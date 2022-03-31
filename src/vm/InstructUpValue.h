#ifndef INSTRUCTUPVALUE_H_
#define INSTRUCTUPVALUE_H_

#include "Instruction.h"
#include <api/LuaState.h>
namespace VM {
class LuaState;

template <typename LVM> void get_tab_up(Instruction<LVM> i, LVM &vm) {
  auto [a, _, c] = i.abc();
  a += 1;

  vm.push_global_table();
  LVM::LuaVmInterface::get_rk(vm, c);
  vm.get_table(-2);
  vm.replace(a);
  vm.pop(1);
}

} // namespace VM

#endif