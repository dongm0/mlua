#include "LuaStack.h"
#include "LuaClosure.h"
#include "LuaTable.h"
#include <api/core/LuaValue.h>
#include <string>

namespace LMath {
LInt int_floor_div(LInt a, LInt b);
LNumber float_floor_div(LNumber a, LNumber b);
LInt int_mod(LInt a, LInt b);
LNumber float_mod(LNumber a, LNumber b);

std::pair<LInt, bool> float_to_int(LNumber f);
std::pair<LInt, bool> parse_int(std::string &str);
std::pair<LNumber, bool> parse_float(std::string &str);
std::pair<LNumber, bool> convert_to_float(API::LuaValue &val);
std::pair<LInt, bool> convert_to_int(API::LuaValue &val);
} // namespace LMath

namespace API {
LuaStack::LuaStack(int size, LuaValue *registry) {
  slots = std::vector<LuaValue>(size);
  top = 0;
  this->registry = registry;
}

void LuaStack::check(int n) {
  if (top + n > slots.size()) {
    slots.resize(top + n);
  }
}

void LuaStack::push(LuaValue &val) {
  if (top >= slots.size()) {
    throw std::runtime_error("stack overflow!");
  } else {
    slots[top++] = val;
  }
}

void LuaStack::push(LuaValue &&val) {
  if (top >= slots.size()) {
    throw std::runtime_error("stack overflow!");
  } else {
    slots[top++] = std::move(val);
  }
}

LuaValue &&LuaStack::pop() {
  if (top < 1) {
    throw std::runtime_error("stack underflow!");
  } else {
    top--;
    return std::move(slots[top]);
  }
}

int LuaStack::abs_index(int idx) {
  if (idx <= LUA_REGISTRYINDEX) {
    return idx;
  }
  if (idx >= 0) {
    return idx;
  } else {
    return idx + top + 1;
  }
}

bool LuaStack::is_valid(int idx) {
  if (idx == LUA_REGISTRYINDEX)
    return true;
  auto abs_idx = abs_index(idx);
  return abs_idx > 0 && abs_idx <= top;
}

LuaValue &LuaStack::get(int idx) {
  if (idx == LUA_REGISTRYINDEX) {
    return *registry;
  }

  if (is_valid(idx)) {
    auto abs_idx = abs_index(idx);
    return slots[abs_idx - 1];
  }
  throw std::runtime_error("invalid index!");
  return slots[0];
  // return LuaValue();
}

void LuaStack::set(int idx, LuaValue val) {
  if (idx == LUA_REGISTRYINDEX) {
    *registry = val;
    return;
  }

  if (is_valid(idx)) {
    auto abs_idx = abs_index(idx);
    slots[abs_idx - 1] = val;
    return;
  }
  throw std::runtime_error("invalid index! ");
}

void LuaStack::rotate(int idx, int n) {
  auto abs_idx = abs_index(idx);
  int rotate_len = top + 1 - abs_idx;
  if (n < 0) {
    n = rotate_len - (-n) % rotate_len;
  } else if (n >= rotate_len) {
    n = n % rotate_len;
  }
  if (n == 0) {
    return;
  }
  std::rotate(slots.begin() + abs_idx - 1,
              slots.begin() + abs_idx - 1 + rotate_len - n,
              slots.begin() + abs_idx - 1 + rotate_len);
}
}