#include <api/LuaState.h>
#include <api/core/LuaClosure.h>
#include <api/core/LuaStack.h>
#include <api/core/LuaTable.h>
#include <api/core/LuaValue.h>
#include <mlua.h>
#include <string>

namespace API {
std::weak_ptr<LuaStack> LuaState::cur_stack() {
  if (_frame.empty()) {
    throw std::runtime_error("frame empty!");
  }
  return std::weak_ptr<LuaStack>(_frame.back());
}

void LuaStack::push_n(std::vector<LuaValue> &vals, int n) {
  if (n < 0) {
    n = vals.size();
  }
  for (int i = 0; i < n; ++i) {
    if (i < vals.size()) {
      push(vals[i]);
    } else {
      push(LuaValue());
    }
  }
}

std::vector<LuaValue> LuaStack::pop_n(int n) {
  std::vector<LuaValue> res;
  res.resize(n);
  for (int i = n - 1; i >= 0; --i) {
    res[i] = pop();
  }
  return res;
}

LuaState::LuaState() {
  registry = LuaValue(LuaTable(0, 0));
  ((LuaTable *)(registry.val_ptr))
      ->put(LUA_RIDX_GLOBALS, LuaValue(LuaTable(0, 0)));
  push_lua_stack(std::make_shared<LuaStack>(LuaStack(20, &registry)));
}

void LuaState::push_lua_stack(std::shared_ptr<LuaStack> stack) {
  _frame.push_back(stack);
}
void LuaState::pop_lua_stack() { _frame.pop_back(); }

} // namespace API