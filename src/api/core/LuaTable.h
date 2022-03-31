#ifndef LUATABLE_H_
#define LUATABLE_H_

#include "LuaValue.h"
#include <cmath>
#include <mlua.h>
#include <unordered_map>
namespace API {

class LuaValue;
class LuaValueHash;

class LuaTable {
public:
  std::vector<LuaValue> tab_arr;
  std::unordered_map<LuaValue, LuaValue, LuaValueHash> tab_map;

private:
  void _shrink_array();
  void _expand_array();

public:
  LuaTable(int n_arr, int n_rec);
  LuaTable(const LuaTable &rhs);
  LuaTable(LuaTable &&rhs);

  LuaTable &operator=(const LuaTable &rhs);
  LuaTable &operator=(LuaTable &&rhs);

  LuaValue &get(const LuaValue &key);
  void put(LuaValue key, LuaValue val);
  LInt len();
};

} // namespace API

#endif