#ifndef INSTRUCTMISC_H_
#define INSTRUCTMISC_H_
#include "Instruction.h"
namespace VM {
template <typename LVM> void move(Instruction<LVM> i, LVM &vm) {
  auto [a, b, _useless] = i.abc();
  a += 1, b += 1;
  vm.copy(b, a);
}

template <typename LVM> void jmp(Instruction<LVM> i, LVM &vm) {
  auto [a, sbx] = i.asbx();
  LVM::LuaVmInterface::add_pc(vm, sbx);
  if (a != 0) {
    throw std::runtime_error("todo!");
  }
}
} // namespace VM

#endif