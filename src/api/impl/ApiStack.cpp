#include <api/LuaState.h>
#include <api/core/LuaClosure.h>
#include <api/core/LuaStack.h>
#include <api/core/LuaTable.h>
#include <api/core/LuaValue.h>
#include <mlua.h>

namespace API {
int LuaState::get_top() { return cur_stack().lock()->top; }

int LuaState::abs_index(int idx) { return cur_stack().lock()->abs_index(idx); }

bool LuaState::check_stack(int n) {
  cur_stack().lock()->check(n);
  return true;
}

void LuaState::pop(int n) {
  for (int i = 0; i < n; ++i) {
    cur_stack().lock()->pop();
  }
}

void LuaState::copy(int from_idx, int to_idx) {
  cur_stack().lock()->set(to_idx, cur_stack().lock()->get(from_idx));
}

void LuaState::push_value(int idx) {
  cur_stack().lock()->push(cur_stack().lock()->get(idx));
}

void LuaState::replace(int idx) {
  cur_stack().lock()->set(idx, cur_stack().lock()->pop());
}

void LuaState::insert(int idx) { rotate(idx, 1); }

void LuaState::remove(int idx) {
  rotate(idx, -1);
  cur_stack().lock()->pop();
}

void LuaState::rotate(int idx, int n) { cur_stack().lock()->rotate(idx, n); }

void LuaState::set_top(int idx) {
  auto new_top = cur_stack().lock()->abs_index(idx);
  if (new_top < 0) {
    throw std::runtime_error("stack underflow!");
  }
  auto n = cur_stack().lock()->top - new_top;
  if (n > 0) {
    for (int i = 0; i < n; ++i) {
      cur_stack().lock()->pop();
    }
  } else if (n < 0) {
    for (int i = 0; i > n; --i) {
      cur_stack().lock()->push(LuaValue());
    }
  }
}

} // namespace API
