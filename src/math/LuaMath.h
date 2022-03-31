#ifndef LUAMATH_H_
#define LUAMATH_H_
#include <cmath>
#include <mlua.h>
#include <string>

namespace API {
class LuaValue;
}

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

#endif