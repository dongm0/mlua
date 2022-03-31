#ifndef LUAVALUE_H_
#define LUAVALUE_H_

#include <api/LuaConsts.h>
#include <cstring>
#include <mlua.h>
#include <stdexcept>

namespace API {
class LuaTable;
class LuaClosure;

class LuaValue {
public:
  CValueType value_type;
  void *val_ptr;

  LuaValue();
  LuaValue(bool b);
  LuaValue(LInt i);
  LuaValue(LNumber num);
  LuaValue(char *str);
  LuaValue(std::string &str);
  LuaValue(LuaTable &table);
  LuaValue(LuaClosure &closure);
  LuaValue(const LuaValue &rhs);
  LuaValue(LuaValue &&rhs);

  ~LuaValue();

  LuaValue &operator=(const LuaValue &rhs);
  LuaValue &operator=(LuaValue &&rhs);
  bool operator==(const LuaValue &rhs) const;

private:
  void _allocate_val_ptr(CValueType type, void *new_val_ptr);
  void _construct_val_ptr(CValueType type, void *new_val_ptr);
  void _move_construct_val_ptr(CValueType type, void *new_val_ptr);
  void _deallocate_val_ptr();
};

class LuaValueHash {
public:
  size_t operator()(const LuaValue &val) const;
};

LValueType type_of(const LuaValue &val);
bool convert_to_boolean(const LuaValue &val);
std::pair<LInt, bool> convert_to_int(const LuaValue &val);
std::pair<LNumber, bool> convert_to_number(const LuaValue &val);

template <typename Ty> inline Ty get_void_ptr_type_val(const void *ptr) {
  return *(Ty *)ptr;
}
template <typename Ty> inline Ty *get_void_ptr_type_ptr(void *ptr) {
  return (Ty *)ptr;
}
template <typename Ty> inline Ty &&get_void_ptr_type_right_ref(void *ptr) {
  return std::move(*(Ty *)ptr);
}
} // namespace API
#endif