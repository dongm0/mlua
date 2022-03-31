#ifndef LUASTACK_H_
#define LUASTACK_H_
#include "LuaClosure.h"
#include <api/LuaConsts.h>
#include <mlua.h>
#include <vector>

namespace API {
class LuaValue;
class LuaClosure;
struct LuaStack {
  std::vector<LuaValue> slots;
  int top;
  LuaValue *registry;

  LuaClosure closure;
  std::vector<LuaValue> varargs;
  int pc = 0;

  LuaStack(int size, LuaValue *registry);
  void check(int n);
  void push(LuaValue &&val);
  void push(LuaValue &val);
  LuaValue &&pop();
  int abs_index(int idx);
  bool is_valid(int idx);
  LuaValue &get(int idx);
  void set(int idx, LuaValue val);
  void rotate(int idx, int n);
  void push_n(std::vector<LuaValue> &vals, int n);
  std::vector<LuaValue> pop_n(int n);
};
} // namespace API

#endif