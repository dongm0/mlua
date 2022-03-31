#ifndef INSTRUCTCALL_H_
#define INSTRUCTCALL_H_
#include "Instruction.h"
#include <api/LuaState.h>
#include <api/core/LuaValue.h>

namespace VM {
namespace {
template <typename LVM>
int _annoymous_push_func_and_args(int a, int b, LVM &vm) {
  if (b >= 1) {
    vm.check_stack(b);
    for (int i = a; i < a + b; ++i) {
      vm.push_value(i);
    }
    return b - 1;
  } else {
    _annoymous_fix_stack(a, vm);
    return vm.get_top() - LVM::LuaVmInterface::register_count(vm) - 1;
  }
}
template <typename LVM> void _annoymous_pop_results(int a, int c, LVM &vm) {
  if (c == 1) {
  } else if (c > 1) {
    for (int i = a + c - 2; i >= a; --i) {
      vm.replace(i);
    }
  } else {
    vm.check_stack(1);
    vm.push_integer((LInt)a);
  }
}
template <typename LVM> void _annoymous_fix_stack(int a, LVM &vm) {
  auto x = int(vm.to_integer(-1));
  vm.pop(1);

  vm.check_stack(x - a);
  for (int i = a; i < x; ++i) {
    vm.push_value(i);
  }
  vm.rotate(LVM::LuaVmInterface::register_count(vm) + 1, x - a);
}
} // namespace
template <typename LVM> void closure(Instruction<LVM> i, LVM &vm) {
  auto [a, bx] = i.abx();
  a += 1;
  LVM::LuaVmInterface::load_proto(vm, bx);
  vm.replace(a);
}

template <typename LVM> void call(Instruction<LVM> i, LVM &vm) {
  auto [a, b, c] = i.abc();
  a += 1;
  auto n_args = _annoymous_push_func_and_args(a, b, vm);
  vm.call(n_args, c - 1);
  _annoymous_pop_results(a, c, vm);
}

template <typename LVM> void vm_return(Instruction<LVM> i, LVM &vm) {
  auto [a, b, c] = i.abc();
  a += 1;

  if (b == 1) {
  } else if (b > 1) {
    vm.check_stack(b - 1);
    for (int i = a; i <= a + b - 2; ++i) {
      vm.push_value(i);
    }
  } else {
    _annoymous_fix_stack(a, vm);
  }
}

template <typename LVM> void vararg(Instruction<LVM> i, LVM &vm) {
  auto [a, b, c] = i.abc();
  a += 1;
  if (b != 1) {
    LVM::LuaVmInterface::load_vararg(vm, b - 1);
    _annoymous_pop_results(a, b, vm);
  }
}

template <typename LVM> void tail_call(Instruction<LVM> i, LVM &vm) {
  auto [a, b, _useless] = i.abc();
  a += 1;
  int c = 0;
  auto n_args = _annoymous_push_func_and_args(a, b, vm);
  vm.call(n_args, c - 1);
  _annoymous_pop_results(a, c, vm);
}

template <typename LVM> void self(Instruction<LVM> i, LVM &vm) {
  auto [a, b, c] = i.abc();
  a += 1, b += 1;

  vm.copy(b, a + 1);
  LVM::LuaVmInterface::get_rk(vm, c);
  vm.get_table(b);
  vm.replace(a);
}
} // namespace VM
#endif