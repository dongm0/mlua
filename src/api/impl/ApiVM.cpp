#include <api/LuaState.h>
#include <api/core/LuaClosure.h>
#include <api/core/LuaStack.h>
#include <api/core/LuaValue.h>
#include <mlua.h>

namespace API {
// luavm_interface
CSize LuaState::get_pc() { return cur_stack().lock()->pc; }

void LuaState::add_pc(int n) { cur_stack().lock()->pc += n; }

CInt LuaState::fetch() {
  auto i = cur_stack().lock()->closure.proto->code[cur_stack().lock()->pc];
  ++(cur_stack().lock()->pc);
  return i;
}

void LuaState::get_const(int idx) {
  auto c = cur_stack().lock()->closure.proto->constants[idx];
  cur_stack().lock()->push(std::move(c));
}

void LuaState::get_rk(int rk) {
  if (rk > 0xff) {
    this->get_const(rk & 0xff);
  } else {
    this->push_value(rk + 1);
  }
}

int LuaState::register_count() {
  return cur_stack().lock()->closure.proto->max_stack_size;
}

void LuaState::load_vararg(int n) {
  if (n < 0) {
    n = cur_stack().lock()->varargs.size();
  }
  cur_stack().lock()->check(n);
  cur_stack().lock()->push_n(cur_stack().lock()->varargs, n);
}

void LuaState::load_proto(int idx) {
  auto proto = cur_stack().lock()->closure.proto->protos[idx];
  auto closure = LuaClosure(proto);
  cur_stack().lock()->push(LuaValue(closure));
}
} // namespace API
