#include <api/LuaState.h>
#include <api/core/LuaStack.h>
#include <api/core/LuaTable.h>
#include <api/core/LuaValue.h>

namespace API {
void LuaState::new_table() { create_table(0, 0); }
void LuaState::create_table(int n_arr, int n_rec) {
  LuaTable t(n_arr, n_rec);
  cur_stack().lock()->push(std::move(LuaValue(t)));
}

LValueType LuaState::get_table(int idx) {
  auto &t = cur_stack().lock()->get(idx);
  auto &&k = cur_stack().lock()->pop();
  return _get_table(t, k);
}
LValueType LuaState::get_field(int idx, std::string &k) {
  auto &t = cur_stack().lock()->get(idx);
  return _get_table(t, LuaValue(k));
}
LValueType LuaState::get_i(int idx, LInt i) {
  auto &t = cur_stack().lock()->get(idx);
  return _get_table(t, LuaValue(i));
}

LValueType LuaState::_get_table(LuaValue &t, LuaValue &k) {
  if (t.value_type == CValueType::C_TTABLE) {
    auto tbl = (LuaTable *)(t.val_ptr);
    auto &v = tbl->get(k);
    cur_stack().lock()->push(v);
    return type_of(v);
  }
  throw std::runtime_error("not a table!");
}
LValueType LuaState::get_global(std::string &name) {
  auto t = ((LuaTable *)(registry.val_ptr))->get(LUA_RIDX_GLOBALS);
  return _get_table(t, LuaValue(name));
}
} // namespace API
