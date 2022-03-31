#include "LuaMath.h"
#include <api/core/LuaValue.h>
#include <cmath>
#include <string>

namespace LMath {
LInt int_floor_div(LInt a, LInt b) {
  if (a * b > 0 || a % b == 0) {
    return a / b;
  } else {
    return a / b - 1;
  }
}

LNumber float_floor_div(LNumber a, LNumber b) { return std::floor(a / b); }

LInt int_mod(LInt a, LInt b) { return a - int_floor_div(a, b) * b; }
LNumber float_mod(LNumber a, LNumber b) { return a - std::floor(a / b) * b; }

// todo: shift

std::pair<LInt, bool> float_to_int(LNumber f) {
  LInt ftmp = (LInt)f;
  return std::make_pair(ftmp, ((LNumber)ftmp) == f);
}
std::pair<LInt, bool> parse_int(std::string &str) {
  LInt res;
  try {
    res = std::stoll(str);
  } catch (std::invalid_argument) {
    return std::make_pair(res, false);
  }
  return std::make_pair(res, true);
}
std::pair<LNumber, bool> parse_float(std::string &str) {
  LNumber res;
  try {
    res = std::stod(str);
  } catch (std::invalid_argument) {
    return std::make_pair(res, false);
  }
  return std::make_pair(res, true);
}
std::pair<LNumber, bool> convert_to_float(API::LuaValue &val) {
  if (val.value_type == API::CValueType::C_TREAL) {
    return std::make_pair(*(LNumber *)(val.val_ptr), true);
  } else if (val.value_type == API::CValueType::C_TINT) {
    return std::make_pair((LNumber) * (LInt *)val.val_ptr, true);
  } else if (val.value_type == API::CValueType::C_TSTRING) {
    return parse_float(*(std::string *)val.val_ptr);
  } else {
    return std::make_pair(0, false);
  }
}

std::pair<LInt, bool> convert_to_int(API::LuaValue &val) {
  if (val.value_type == API::CValueType::C_TREAL) {
    return float_to_int(*(LNumber *)(val.val_ptr));
  } else if (val.value_type == API::CValueType::C_TINT) {
    return std::make_pair(*(LInt *)val.val_ptr, true);
  } else if (val.value_type == API::CValueType::C_TSTRING) {
    auto res = parse_int(*(std::string *)val.val_ptr);
    if (!res.second) {
      auto [f, b] = parse_float(*(std::string *)val.val_ptr);
      return std::make_pair((LInt)f, b);
    } else {
      return res;
    }
  } else {
    return std::make_pair(0, false);
  }
}

} // namespace LMath
