#include "LuaTable.h"
#include "LuaValue.h"

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
namespace {
LuaValue _annoymous_float_to_int(const LuaValue &key) {
  if (key.value_type == CValueType::C_TREAL) {
    auto [f, b] = LMath::float_to_int(*(LNumber *)(key.val_ptr));
    if (b) {
      return LuaValue(f);
    }
  }
  return key;
}
} // namespace

void LuaTable::_shrink_array() {
  for (int i = tab_arr.size() - 1; i >= 0; --i) {
    if (tab_arr[i].value_type == CValueType::C_TNIL) {
      tab_arr.pop_back();
    }
  }
}
void LuaTable::_expand_array() {
  for (int64_t i = tab_arr.size() + 1;; ++i) {
    auto val_i = LuaValue(i);
    if (tab_map.count(val_i)) {
      tab_map.erase(val_i);
      tab_arr.push_back(std::move(val_i));
    } else {
      break;
    }
  }
}

LuaTable::LuaTable(int n_arr, int n_rec) {
  tab_arr = std::vector<LuaValue>();
  tab_map = std::unordered_map<LuaValue, LuaValue, LuaValueHash>();
  if (n_arr > 0)
    tab_arr.resize(n_arr);
  if (n_rec > 0)
    tab_map.reserve(n_rec);
}

LuaValue &LuaTable::get(const LuaValue &key) {
  auto key1 = _annoymous_float_to_int(key);
  if (key1.value_type == CValueType::C_TINT) {
    auto idx = *(LInt *)(key1.val_ptr);
    if (idx >= 1 && idx <= (LInt)tab_arr.size()) {
      return tab_arr[idx - 1];
    }
  }
  if (tab_map.count(key)) {
    auto &val = tab_map[key];
    return val;
  }
  return LuaValue();
}

void LuaTable::put(LuaValue key, LuaValue val) {
  if (key.value_type == CValueType::C_TNIL) {
    throw std::runtime_error("table index in nil!");
  }
  if (key.value_type == CValueType::C_TREAL) {
    if (std::isnan(*(LNumber *)key.val_ptr)) {
      throw std::runtime_error("table index is nan!");
    }
  }

  auto key1 = _annoymous_float_to_int(key);
  if (key1.value_type == CValueType::C_TINT) {
    auto idx = *(LInt *)(key1.val_ptr);
    auto arr_len = (LInt)tab_arr.size();
    if (idx >= 1 && idx <= arr_len) {
      if (idx <= arr_len) {
        tab_arr[idx - 1] = val;
        if (val.value_type == CValueType::C_TNIL) {
          _shrink_array();
        }
        return;
      }
    } else if (idx == arr_len + 1) {
      tab_map.erase(key1);
      if (val.value_type != CValueType::C_TNIL) {
        tab_arr.push_back(val);
        _expand_array();
      }
      return;
    }
  }

  if (val.value_type != CValueType::C_TNIL) {
    tab_map.insert(std::make_pair(key, val));
  } else {
    tab_map.erase(key);
  }
}

LuaTable::LuaTable(const LuaTable &rhs) {
  tab_arr = rhs.tab_arr;
  tab_map = rhs.tab_map;
}
LuaTable::LuaTable(LuaTable &&rhs) {
  tab_arr = std::move(rhs.tab_arr);
  tab_map = std::move(rhs.tab_map);
}

LuaTable &LuaTable::operator=(const LuaTable &rhs) {
  tab_arr = rhs.tab_arr;
  tab_map = rhs.tab_map;
  return *this;
}
LuaTable &LuaTable::operator=(LuaTable &&rhs) {
  tab_arr = std::move(rhs.tab_arr);
  tab_map = std::move(rhs.tab_map);
  return *this;
}

LInt LuaTable::len() { return tab_arr.size(); }
} // namespace API