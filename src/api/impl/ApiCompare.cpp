#include <api/LuaState.h>
#include <api/core/LuaStack.h>
#include <api/core/LuaValue.h>
#include <math/LuaMath.h>
#include <mlua.h>

namespace API {

namespace {
bool _anonymous_eq(LuaValue &a, LuaValue &b) {
  if (a.value_type == CValueType::C_TNIL) {
    return b.value_type == CValueType::C_TNIL;
  } else if (a.value_type == CValueType::C_TBOOLEAN) {
    if (b.value_type == CValueType::C_TBOOLEAN) {
      return (bool)*(Byte *)a.val_ptr == (bool)*(Byte *)b.val_ptr;
    } else {
      return false;
    }
  } else if (a.value_type == CValueType::C_TSTRING) {
    if (b.value_type == CValueType::C_TSTRING) {
      return *(std::string *)a.val_ptr == *(std::string *)b.val_ptr;
    } else {
      return false;
    }
  } else if (a.value_type == CValueType::C_TINT) {
    if (b.value_type == CValueType::C_TINT) {
      return *(LInt *)a.val_ptr == *(LInt *)b.val_ptr;
    } else if (b.value_type == CValueType::C_TREAL) {
      return (LNumber) * (LInt *)a.val_ptr == *(LNumber *)b.val_ptr;
    } else {
      return false;
    }
  } else if (a.value_type == CValueType::C_TREAL) {
    if (b.value_type == CValueType::C_TINT) {
      return *(LNumber *)a.val_ptr == (LNumber) * (LInt *)b.val_ptr;
    } else if (b.value_type == CValueType::C_TREAL) {
      return *(LNumber *)a.val_ptr == *(LNumber *)b.val_ptr;
    } else {
      return false;
    }
  } else {
    throw std::runtime_error("todo!");
    return false;
  }
}
bool _anonymous_lt(LuaValue &a, LuaValue &b) {
  if (a.value_type == CValueType::C_TSTRING) {
    if (b.value_type == CValueType::C_TSTRING) {
      return *(std::string *)a.val_ptr < *(std::string *)b.val_ptr;
    }
  } else if (a.value_type == CValueType::C_TINT) {
    if (b.value_type == CValueType::C_TINT) {
      return *(LInt *)a.val_ptr < *(LInt *)b.val_ptr;
    } else if (b.value_type == CValueType::C_TREAL) {
      return (LNumber) * (LInt *)a.val_ptr < *(LNumber *)b.val_ptr;
    }
  } else if (a.value_type == CValueType::C_TREAL) {
    if (b.value_type == CValueType::C_TINT) {
      return *(LNumber *)a.val_ptr < (LNumber) * (LInt *)b.val_ptr;
    } else if (b.value_type == CValueType::C_TREAL) {
      return *(LNumber *)a.val_ptr < *(LNumber *)b.val_ptr;
    }
  }
  throw std::runtime_error("comparison error!");
  return false;
}
bool _anonymous_le(LuaValue &a, LuaValue &b) {
  if (a.value_type == CValueType::C_TSTRING) {
    if (b.value_type == CValueType::C_TSTRING) {
      return *(std::string *)a.val_ptr <= *(std::string *)b.val_ptr;
    }
  } else if (a.value_type == CValueType::C_TINT) {
    if (b.value_type == CValueType::C_TINT) {
      return *(LInt *)a.val_ptr <= *(LInt *)b.val_ptr;
    } else if (b.value_type == CValueType::C_TREAL) {
      return (LNumber) * (LInt *)a.val_ptr <= *(LNumber *)b.val_ptr;
    }
  } else if (a.value_type == CValueType::C_TREAL) {
    if (b.value_type == CValueType::C_TINT) {
      return *(LNumber *)a.val_ptr <= (LNumber) * (LInt *)b.val_ptr;
    } else if (b.value_type == CValueType::C_TREAL) {
      return *(LNumber *)a.val_ptr <= *(LNumber *)b.val_ptr;
    }
  }
  throw std::runtime_error("comparison error!");
  return false;
}
} // namespace
bool LuaState::compare(int idx1, int idx2, CompareOp op) {
  auto a = cur_stack().lock()->get(idx1);
  auto b = cur_stack().lock()->get(idx2);
  if (op == CompareOp::LUA_OPEQ) {
    return _anonymous_eq(a, b);
  } else if (op == CompareOp::LUA_OPLE) {
    return _anonymous_le(a, b);
  } else {
    return _anonymous_lt(a, b);
  }
}
} // namespace API
