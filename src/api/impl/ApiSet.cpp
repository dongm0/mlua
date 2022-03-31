#include <api/LuaState.h>
#include <api/core/LuaStack.h>
#include <api/core/LuaTable.h>
#include <api/core/LuaValue.h>

namespace API {

void LuaState::set_table(int idx) {
  auto &t = cur_stack().lock()->get(idx);
  auto &&v = cur_stack().lock()->pop();
  auto &&k = cur_stack().lock()->pop();
  _set_table(t, k, v);
}
void LuaState::set_field(int idx, std::string &k) {
  auto &t = cur_stack().lock()->get(idx);
  auto &&v = cur_stack().lock()->pop();
  _set_table(t, LuaValue(k), v);
}
void LuaState::set_i(int idx, LInt i) {
  auto &t = cur_stack().lock()->get(idx);
  auto &&v = cur_stack().lock()->pop();
  _set_table(t, LuaValue(i), v);
}

void LuaState::_set_table(LuaValue &t, LuaValue &k, LuaValue &v) {
  if (t.value_type == CValueType::C_TTABLE) {
    auto tbl = (LuaTable *)(t.val_ptr);
    tbl->put(k, v);
    return;
  }
  throw std::runtime_error("not a table!");
}

void LuaState::set_global(std::string &name) {
  auto &t = ((LuaTable *)registry.val_ptr)->get(LUA_RIDX_GLOBALS);
  auto &&v = cur_stack().lock()->pop();
  _set_table(t, LuaValue(name), v);
}

void LuaState::state_register(std::string &name, CFunction f) {
  push_c_function(f);
  set_global(name);
}

} // namespace API
