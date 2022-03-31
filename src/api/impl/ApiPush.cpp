#include <api/LuaState.h>
#include <api/core/LuaClosure.h>
#include <api/core/LuaStack.h>
#include <api/core/LuaTable.h>
#include <api/core/LuaValue.h>
#include <functional>

namespace API {
void LuaState::push_nil() { cur_stack().lock()->push(LuaValue()); }

void LuaState::push_boolean(bool b) { cur_stack().lock()->push(LuaValue(b)); }

void LuaState::push_integer(int64_t n) {
  cur_stack().lock()->push(LuaValue(n));
}

void LuaState::push_number(double n) { cur_stack().lock()->push(LuaValue(n)); }

void LuaState::push_string(char *s) { cur_stack().lock()->push(LuaValue(s)); }

void LuaState::push_c_function(CFunction f) {
  cur_stack().lock()->push(LuaValue(LuaClosure(CFunction(f))));
}

void LuaState::push_global_table() {
  auto &global =
      get_void_ptr_type_ptr<LuaTable>(registry.val_ptr)->get(LUA_RIDX_GLOBALS);
  cur_stack().lock()->push(global);
}

} // namespace API
