#ifndef INSTRUCTOPERATOR_H_
#define INSTRUCTOPERATOR_H_
#include "Instruction.h"
#include <api/LuaState.h>
namespace VM {
namespace {
template <typename LVM>
void _anonymous_binary_arith(Instruction<LVM> i, LVM &vm, API::ArithOp op) {
  auto [a, b, c] = i.abc();
  a += 1;
  LVM::LuaVmInterface::get_rk(vm, b);
  LVM::LuaVmInterface::get_rk(vm, c);
  vm.arith(op);
  vm.replace(a);
}

template <typename LVM>
void _anonymous_unary_arith(Instruction<LVM> i, LVM &vm, API::ArithOp op) {
  auto [a, b, _useless] = i.abc();
  a += 1, b += 1;
  vm.push_value(b);
  vm.arith(op);
  vm.replace(a);
}

template <typename LVM>
void _anonymous_compare(Instruction<LVM> i, LVM &vm, API::CompareOp op) {
  auto [a, b, c] = i.abc();
  LVM::LuaVmInterface::get_rk(vm, b);
  LVM::LuaVmInterface::get_rk(vm, c);
  if (vm.compare(-2, -1, op) != (a != 0)) {
    LVM::LuaVmInterface::add_pc(vm, 1);
  }
  vm.pop(2);
}

} // namespace

template <typename LVM> void add(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPADD);
}
template <typename LVM> void sub(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPSUB);
}
template <typename LVM> void mul(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPMUL);
}
template <typename LVM> void mod(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPMOD);
}
template <typename LVM> void pow(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPPOW);
}
template <typename LVM> void div(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPDIV);
}
template <typename LVM> void idiv(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPIDIV);
}
template <typename LVM> void band(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPBAND);
}
template <typename LVM> void bor(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPBOR);
}
template <typename LVM> void bxor(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPBXOR);
}
template <typename LVM> void shl(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPSHL);
}
template <typename LVM> void shr(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPSHR);
}
template <typename LVM> void unm(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPUNM);
}
template <typename LVM> void bnot(Instruction<LVM> i, LVM &vm) {
  _anonymous_binary_arith(i, vm, API::ArithOp::LUA_OPBNOT);
}
template <typename LVM> void eq(Instruction<LVM> i, LVM &vm) {
  _anonymous_compare(i, vm, API::CompareOp::LUA_OPEQ);
}
template <typename LVM> void lt(Instruction<LVM> i, LVM &vm) {
  _anonymous_compare(i, vm, API::CompareOp::LUA_OPLT);
}
template <typename LVM> void le(Instruction<LVM> i, LVM &vm) {
  _anonymous_compare(i, vm, API::CompareOp::LUA_OPLE);
}
template <typename LVM> void not(Instruction<LVM> i, LVM &vm) {
  auto [a, b, _useless] = i.abc();
  a += 1, b += 1;
  vm.push_boolean(!vm.to_boolean(b));
  vm.replace(a);
}

template <typename LVM> void concat(Instruction<LVM> i, LVM &vm) {
  auto [a, b, c] = i.abc();
  a += 1, b += 1, c += 1;
  auto n = c - b + 1;
  vm.check_stack(n);
  for (auto i = b; i <= c; ++i) {
    vm.push_value(i);
  }
  vm.concat(n);
  vm.replace(a);
}

template <typename LVM> void test_set(Instruction<LVM> i, LVM &vm) {
  auto [a, b, c] = i.abc();
  a += 1, b += 1;
  if (vm.to_boolean(b) == (c != 0)) {
    vm.copy(b, a);
  } else {
    LVM::LuaVmInterface::add_pc(vm, 1);
  }
}

template <typename LVM> void test(Instruction<LVM> i, LVM &vm) {
  auto [a, _useless, c] = i.abc();
  a += 1;
  if (vm.to_boolean(a) != (c != 0)) {
    LVM::LuaVmInterface::add_pc(vm, 1);
  }
}

template <typename LVM> void len(Instruction<LVM> i, LVM &vm) {
  auto [a, b, _useless] = i.abc();
  a += 1, b += 1;
  vm.len(b);
  vm.replace(a);
}

} // namespace VM
#endif