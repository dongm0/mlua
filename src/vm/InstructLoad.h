#ifndef INSTRUCTLOAD_H_
#define INSTRUCTLOAD_H_

#include "Instruction.h"
namespace VM {
template <typename LVM> void load_nil(Instruction<LVM> i, LVM &vm) {
  auto [a, b, _useless] = i.abc();
  a += 1;
  vm.push_nil();
  for (auto i = a; i <= a + b; ++i) {
    vm.copy(-1, i);
  }
  vm.pop(1);
}

template <typename LVM> void load_bool(Instruction<LVM> i, LVM &vm) {
  auto [a, b, c] = i.abc();
  a += 1;
  vm.push_boolean(b != 0);
  vm.replace(a);
  if (c != 0) {
    LVM::LuaVmInterface::add_pc(vm, 1);
  }
}

template <typename LVM> void load_k(Instruction<LVM> i, LVM &vm) {
  auto [a, bx] = i.abx();
  a += 1;
  LVM::LuaVmInterface::get_const(vm, bx);
  vm.replace(a);
}

template <typename LVM> void load_kx(Instruction<LVM> i, LVM &vm) {
  auto [a, _useless] = i.abx();
  a += 1;
  auto ax = Instruction<LVM>(LVM::LuaVmInterface::fetch(vm)).ax();
  LVM::LuaVmInterface::get_const(vm, ax);
  vm.replace(a);
}
} // namespace VM
#endif