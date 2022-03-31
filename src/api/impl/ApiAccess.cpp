#include <api/LuaState.h>
#include <api/core/LuaClosure.h>
#include <api/core/LuaStack.h>
#include <api/core/LuaValue.h>
#include <math/LuaMath.h>
#include <mlua.h>
namespace API {

std::string_view LuaState::type_name(LValueType tp) {
  switch (tp) {
  case LValueType::LUA_TNONE:
    return "no value";
  case LValueType::LUA_TNIL:
    return "nil";
  case LValueType::LUA_TBOOLEAN:
    return "boolean";
  case LValueType::LUA_TNUMBER:
    return "number";
  case LValueType::LUA_TSTRING:
    return "string";
  case LValueType::LUA_TTABLE:
    return "table";
  case LValueType::LUA_TFUNCTION:
    return "function";
  case LValueType::LUA_TTHREAD:
    return "thread";
  default:
    return "userdata";
  }
}

LValueType LuaState::type(int idx) {
  return type_of(cur_stack().lock()->get(idx));
}
bool LuaState::is_none(int idx) { return type(idx) == LValueType::LUA_TNONE; }
bool LuaState::is_nil(int idx) { return type(idx) == LValueType::LUA_TNIL; }
bool LuaState::is_none_or_nil(int idx) { return is_none(idx) || is_nil(idx); }
bool LuaState::is_boolean(int idx) {
  return type(idx) == LValueType::LUA_TBOOLEAN;
}

bool LuaState::is_integer(int idx) {
  return cur_stack().lock()->get(idx).value_type == CValueType::C_TINT;
}
bool LuaState::is_number(int idx) { return to_number_x(idx).second; }
bool LuaState::is_string(int idx) {
  return type(idx) == LValueType::LUA_TSTRING ||
         type(idx) == LValueType::LUA_TNUMBER;
}
bool LuaState::is_c_function(int idx) {
  auto val = cur_stack().lock()->get(idx);
  if (val.value_type != CValueType::C_TFUNCTION) {
    return false;
  }
  if ((*(LuaClosure *)(val.val_ptr)).c_function) {
    return true;
  } else {
    return false;
  }
}
bool LuaState::to_boolean(int idx) {
  return convert_to_boolean(cur_stack().lock()->get(idx));
}
LInt LuaState::to_integer(int idx) { return to_integer_x(idx).first; }
std::pair<int64_t, bool> LuaState::to_integer_x(int idx) {
  return LMath::convert_to_int(cur_stack().lock()->get(idx));
}
double LuaState::to_number(int idx) { return to_number_x(idx).first; }
std::pair<double, bool> LuaState::to_number_x(int idx) {
  return LMath::convert_to_float(cur_stack().lock()->get(idx));
}

std::string LuaState::to_string(int idx) { return to_string_x(idx).first; }
std::pair<std::string, bool> LuaState::to_string_x(int idx) {
  auto &val = cur_stack().lock()->get(idx);
  if (val.value_type == CValueType::C_TSTRING) {
    return std::make_pair(*(std::string *)val.val_ptr, true);
  } else if (val.value_type == CValueType::C_TINT) {
    auto str = std::to_string(*(LInt *)val.val_ptr);
    cur_stack().lock()->set(idx, LuaValue(str));
    return std::make_pair(str, true);
  } else if (val.value_type == CValueType::C_TREAL) {
    auto str = std::to_string(*(LNumber *)val.val_ptr);
    cur_stack().lock()->set(idx, LuaValue(str));
    return std::make_pair(str, true);
  } else {
    return std::make_pair("", false);
  }
}
CFunction LuaState::to_c_function(int idx) {
  auto val = cur_stack().lock()->get(idx);
  if (val.value_type != CValueType::C_TFUNCTION) {
    return CFunction();
  }
  return get_void_ptr_type_val<LuaClosure>(val.val_ptr).c_function;
}
} // namespace API
