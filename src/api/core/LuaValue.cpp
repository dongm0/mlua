#include "LuaValue.h"
#include "LuaClosure.h"
#include "LuaTable.h"
#include <functional>

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
LuaValue::LuaValue() : value_type(CValueType::C_TNIL) {
  _allocate_val_ptr(value_type, 0);
}
LuaValue::LuaValue(bool b) : value_type(CValueType::C_TBOOLEAN) {
  _allocate_val_ptr(value_type, &b);
  _construct_val_ptr(value_type, &b);
}
LuaValue::LuaValue(LInt i) : value_type(CValueType::C_TINT) {
  _allocate_val_ptr(value_type, (void *)&i);
  _construct_val_ptr(value_type, (void *)&i);
}
LuaValue::LuaValue(LNumber num) : value_type(CValueType::C_TREAL) {
  _allocate_val_ptr(value_type, (void *)&num);
  _construct_val_ptr(value_type, (void *)&num);
}
LuaValue::LuaValue(char *str) : value_type(CValueType::C_TSTRING) {
  auto cppstr = std::string(str);
  _allocate_val_ptr(value_type, (void *)&cppstr);
  _construct_val_ptr(value_type, (void *)&cppstr);
}
LuaValue::LuaValue(std::string &str) : value_type(CValueType::C_TSTRING) {
  _allocate_val_ptr(value_type, (void *)&str);
  _construct_val_ptr(value_type, (void *)&str);
}
LuaValue::LuaValue(LuaTable &table) : value_type(CValueType::C_TTABLE) {
  _allocate_val_ptr(value_type, (void *)&table);
  _construct_val_ptr(value_type, (void *)&table);
}
LuaValue::LuaValue(LuaClosure &closure) : value_type(CValueType::C_TFUNCTION) {
  _allocate_val_ptr(value_type, (void *)&closure);
  _construct_val_ptr(value_type, (void *)&closure);
}

LuaValue::LuaValue(const LuaValue &rhs) : value_type(rhs.value_type) {
  _allocate_val_ptr(value_type, rhs.val_ptr);
  _construct_val_ptr(value_type, rhs.val_ptr);
}

LuaValue::LuaValue(LuaValue &&rhs) : value_type(rhs.value_type) {
  _allocate_val_ptr(value_type, rhs.val_ptr);
  _move_construct_val_ptr(value_type, rhs.val_ptr);
  // val_ptr = rhs.val_ptr;
  // rhs.value_type = CValueType::C_TNONE;
  // rhs.val_ptr = nullptr;
}

LuaValue::~LuaValue() { _deallocate_val_ptr(); }

bool LuaValue::operator==(const LuaValue &rhs) const {
  if (value_type != rhs.value_type) {
    return false;
  }

  if (value_type == CValueType::C_TINT) {
    return *(LInt *)(val_ptr) == *(LInt *)(rhs.val_ptr);
  } else if (value_type == CValueType::C_TREAL) {
    return *(LNumber *)(val_ptr) == *(LNumber *)(rhs.val_ptr);
  } else if (value_type == CValueType::C_TSTRING) {
    return *(std::string *)(val_ptr) == *(std::string *)(rhs.val_ptr);
  } else if (value_type == CValueType::C_TBOOLEAN) {
    return (*(Byte *)(val_ptr)&0x01) ^ (*(Byte *)(rhs.val_ptr) & 0x01);
  } else {
    throw std::runtime_error("todo!");
    return false;
  }
}

LuaValue &LuaValue::operator=(const LuaValue &rhs) {
  _deallocate_val_ptr();
  value_type = rhs.value_type;
  _allocate_val_ptr(value_type, rhs.val_ptr);
  _construct_val_ptr(value_type, rhs.val_ptr);
  return *this;
}

LuaValue &LuaValue::operator=(LuaValue &&rhs) {
  _deallocate_val_ptr();
  value_type = rhs.value_type;
  _allocate_val_ptr(value_type, rhs.val_ptr);
  _move_construct_val_ptr(value_type, rhs.val_ptr);
  return *this;
}

void LuaValue::_allocate_val_ptr(CValueType type, void *new_val_ptr) {
  switch (type) {
  case CValueType::C_TNONE:
    break;
  case CValueType::C_TNIL:
    break;
  case CValueType::C_TLIGHTUSERDATA:
    throw std::runtime_error("to do!");
  case CValueType::C_TBOOLEAN:
    val_ptr = (void *)(new Byte);
    break;
  case CValueType::C_TINT:
    val_ptr = (void *)(new LInt);
    break;
  case CValueType::C_TREAL:
    val_ptr = (void *)(new LNumber);
    break;
  case CValueType::C_TSTRING:
    val_ptr = (void *)(new std::string());
    break;
  case CValueType::C_TTABLE:
    val_ptr = (void *)(new LuaTable(0, 0));
    break;
  case CValueType::C_TFUNCTION:
    val_ptr = (void *)(new LuaClosure());
    break;
  default:
    throw std::runtime_error("to do!");
  }
}
void LuaValue::_construct_val_ptr(CValueType type, void *new_val_ptr) {
  switch (type) {
  case CValueType::C_TNONE:
    break;
  case CValueType::C_TNIL:
    break;
  case CValueType::C_TLIGHTUSERDATA:
    throw std::runtime_error("to do!");
  case CValueType::C_TBOOLEAN:
    *(Byte *)(val_ptr) = get_void_ptr_type_val<Byte>(new_val_ptr);
    break;
  case CValueType::C_TINT:
    *(LInt *)(val_ptr) = get_void_ptr_type_val<LInt>(new_val_ptr);
    break;
  case CValueType::C_TREAL:
    *(LNumber *)(val_ptr) = get_void_ptr_type_val<LNumber>(new_val_ptr);
    break;
  case CValueType::C_TSTRING:
    *(std::string *)(val_ptr) = get_void_ptr_type_val<std::string>(new_val_ptr);
    break;
  case CValueType::C_TTABLE:
    *(LuaTable *)(val_ptr) = get_void_ptr_type_val<LuaTable>(new_val_ptr);
    break;
  case CValueType::C_TFUNCTION:
    *(LuaClosure *)(val_ptr) = get_void_ptr_type_val<LuaClosure>(new_val_ptr);
    break;
  default:
    throw std::runtime_error("to do!");
  }
}
void LuaValue::_move_construct_val_ptr(CValueType type, void *new_val_ptr) {
  switch (type) {
  case CValueType::C_TNONE:
    break;
  case CValueType::C_TNIL:
    break;
  case CValueType::C_TLIGHTUSERDATA:
    throw std::runtime_error("to do!");
  case CValueType::C_TBOOLEAN:
    *(Byte *)(val_ptr) = get_void_ptr_type_right_ref<Byte>(new_val_ptr);
    break;
  case CValueType::C_TINT:
    *(LInt *)(val_ptr) = get_void_ptr_type_right_ref<LInt>(new_val_ptr);
    break;
  case CValueType::C_TREAL:
    *(LNumber *)(val_ptr) = get_void_ptr_type_right_ref<LNumber>(new_val_ptr);
    break;
  case CValueType::C_TSTRING:
    *(std::string *)(val_ptr) =
        get_void_ptr_type_right_ref<std::string>(new_val_ptr);
    break;
  case CValueType::C_TTABLE:
    *(LuaTable *)(val_ptr) = get_void_ptr_type_right_ref<LuaTable>(new_val_ptr);
    break;
  case CValueType::C_TFUNCTION:
    *(LuaClosure *)(val_ptr) =
        get_void_ptr_type_right_ref<LuaClosure>(new_val_ptr);
    break;
  default:
    throw std::runtime_error("to do!");
  }
}
void LuaValue::_deallocate_val_ptr() {
  switch (value_type) {
  case CValueType::C_TNONE:
    break;
  case CValueType::C_TNIL:
    break;
  case CValueType::C_TLIGHTUSERDATA:
    throw std::runtime_error("to do!");
  case CValueType::C_TBOOLEAN:
    delete (Byte *)val_ptr;
    break;
  case CValueType::C_TINT:
    delete (LInt *)val_ptr;
    break;
  case CValueType::C_TREAL:
    delete (LNumber *)val_ptr;
    break;
  case CValueType::C_TSTRING:
    delete (std::string *)val_ptr;
    break;
  case CValueType::C_TTABLE:
    delete (LuaTable *)val_ptr;
    break;
  case CValueType::C_TFUNCTION:
    delete (LuaClosure *)val_ptr;
    break;
  default:
    throw std::runtime_error("to do!");
  }
}

namespace {
inline LValueType _anonymous_ctype_to_ltype(CValueType vtype) {
  switch (vtype) {
  case CValueType::C_TNONE:
    return LValueType::LUA_TNONE;
  case CValueType::C_TNIL:
    return LValueType::LUA_TNIL;
  case CValueType::C_TBOOLEAN:
    return LValueType::LUA_TBOOLEAN;
  case CValueType::C_TLIGHTUSERDATA:
    return LValueType::LUA_TLIGHTUSERDATA;
  case CValueType::C_TINT:
    return LValueType::LUA_TNUMBER;
  case CValueType::C_TREAL:
    return LValueType::LUA_TNUMBER;
  case CValueType::C_TTABLE:
    return LValueType::LUA_TTABLE;
  case CValueType::C_TFUNCTION:
    return LValueType::LUA_TFUNCTION;
  case CValueType::C_TSTRING:
    return LValueType::LUA_TSTRING;
  case CValueType::C_TUSERDATA:
    return LValueType::LUA_TUSERDATA;
  default:
    return LValueType::LUA_TTHREAD;
  }
}
} // namespace

LValueType type_of(const LuaValue &val) {
  return _anonymous_ctype_to_ltype(val.value_type);
}

bool convert_to_boolean(const LuaValue &val) {
  switch (val.value_type) {
  case CValueType::C_TNIL:
    return false;
  case CValueType::C_TBOOLEAN:
    return *(Byte *)val.val_ptr == 1;
  default:
    return true;
  }
}

// todo: modify
std::pair<LInt, bool> convert_to_int(const LuaValue &val) {
  switch (val.value_type) {
  case CValueType::C_TREAL:
    return std::make_pair(*(LNumber *)val.val_ptr, true);
  case CValueType::C_TINT:
    return std::make_pair((LNumber) * (LInt *)val.val_ptr, true);
  case CValueType::C_TSTRING:
    return LMath::parse_int(*(std::string *)val.val_ptr);
  default:
    return std::make_pair(0, false);
  }
}

std::pair<LNumber, bool> convert_to_number(const LuaValue &val) {
  switch (val.value_type) {
  case CValueType::C_TREAL:
    return std::make_pair(*(LNumber *)val.val_ptr, true);
  case CValueType::C_TINT:
    return std::make_pair((LNumber) * (LInt *)val.val_ptr, true);
  case CValueType::C_TSTRING:
    return LMath::parse_float(*(std::string *)val.val_ptr);
  default:
    return std::make_pair(0, false);
  }
}

size_t LuaValueHash::operator()(const LuaValue &val) const {
  if (val.value_type == CValueType::C_TNONE ||
      val.value_type == CValueType::C_TNIL) {
    throw std::runtime_error("unsupported!");
    return -1;
  } else if (val.value_type == CValueType::C_TINT) {
    return std::hash<LInt>()(get_void_ptr_type_val<LInt>(val.val_ptr));
  } else if (val.value_type == CValueType::C_TBOOLEAN) {
    return std::hash<bool>()(!(get_void_ptr_type_val<Byte>(val.val_ptr) == 0));
  } else if (val.value_type == CValueType::C_TREAL) {
    return std::hash<LNumber>()(get_void_ptr_type_val<LNumber>(val.val_ptr));
  } else if (val.value_type == CValueType::C_TSTRING) {
    return std::hash<std::string>()(
        get_void_ptr_type_val<std::string>(val.val_ptr));
  } else {
    throw std::runtime_error("todo!");
    return -1;
  }
}

// std::pair<std::string, bool> convert_to_string(const LuaValue &val) {}

} // namespace API