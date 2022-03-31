#include <api/LuaState.h>
#include <api/core/LuaStack.h>
#include <api/core/LuaValue.h>
#include <math/LuaMath.h>
#include <mlua.h>

namespace API {
auto op_iadd = [](LInt a, LInt b) -> LInt { return a + b; };
auto op_fadd = [](LNumber a, LNumber b) -> LNumber { return a + b; };
auto op_isub = [](LInt a, LInt b) -> LInt { return a - b; };
auto op_fsub = [](LNumber a, LNumber b) -> LNumber { return a - b; };
auto op_imul = [](LInt a, LInt b) -> LInt { return a * b; };
auto op_fmul = [](LNumber a, LNumber b) -> LNumber { return a * b; };
auto op_imod = LMath::int_mod;
auto op_fmod = LMath::float_mod;
LNumber (*op_pow)(LNumber, LNumber) = pow;
auto op_div = [](LNumber a, LNumber b) -> LNumber { return a / b; };
auto op_ifdiv = LMath::int_floor_div;
auto op_ffdiv = LMath::float_floor_div;
auto op_band = [](LInt a, LInt b) -> LInt { return a & b; };
auto op_bor = [](LInt a, LInt b) -> LInt { return a | b; };
auto op_bxor = [](LInt a, LInt b) -> LInt { return a ^ b; };
auto op_shl = [](LInt a, LInt b) -> LInt { return a << b; };
auto op_shr = [](LInt a, LInt b) -> LInt { return a >> b; };
auto op_iunm = [](LInt a, LInt b) -> LInt { return -a; };
auto op_funm = [](LNumber a, LNumber b) -> LNumber { return -a; };
auto op_bnot = [](LInt a, LInt b) -> LInt { return ~a; };

struct Operator {
  LInt (*int_func)(LInt, LInt);
  LNumber (*float_func)(LNumber, LNumber);
};

Operator operators[14] = {
    Operator{op_iadd, op_fadd},   Operator{op_isub, op_fsub},
    Operator{op_imul, op_fmul},   Operator{op_imod, op_fmod},
    Operator{nullptr, op_pow},    Operator{nullptr, op_div},
    Operator{op_ifdiv, op_ffdiv}, Operator{op_band, nullptr},
    Operator{op_bor, nullptr},    Operator{op_bxor, nullptr},
    Operator{op_shl, nullptr},    Operator{op_shr, nullptr},
    Operator{op_iunm, op_funm},   Operator{op_bnot, nullptr}};

namespace {
LuaValue _anonymous_arith(LuaValue &a, LuaValue &b, Operator op) {
  if (op.float_func == nullptr) {
    // bitwise
    if (auto [x, ok] = convert_to_int(a); ok) {
      if (auto [y, ok] = convert_to_int(b); ok) {
        return LuaValue(op.int_func(x, y));
      }
    }
  } else {
    // arith
    if (op.int_func != nullptr) {
      // add, sub, mul, mod, idiv, unm
      if (auto [x, ok] = convert_to_int(a); ok) {
        if (auto [y, ok] = convert_to_int(b); ok) {
          return op.int_func(x, y);
        }
      }
      if (auto [x, ok] = convert_to_number(a); ok) {
        if (auto [y, ok] = convert_to_number(b); ok) {
          return op.float_func(x, y);
        }
      }
    }
  }
  return LuaValue();
}
} // namespace

void LuaState::arith(ArithOp op) {
  LuaValue &&b = cur_stack().lock()->pop();
  LuaValue a;
  if (op != ArithOp::LUA_OPUNM && op != ArithOp::LUA_OPBNOT) {
    a = cur_stack().lock()->pop();
  } else {
    a = b;
  }

  auto _op = operators[(int)op];
  if (auto res = _anonymous_arith(a, b, _op);
      res.value_type != CValueType::C_TNIL) {
    cur_stack().lock()->push(std::move(res));
  } else {
    throw std::runtime_error("arithmetic error!");
  }
}
} // namespace API
