#ifndef OPTIMIZAEXP_H_
#define OPTIMIZAEXP_H_
#include <cmath>
#include <compiler/AST/Block.h>
#include <compiler/Lexer/Lexer.h>
#include <compiler/Lexer/Token.h>
#include <math/LuaMath.h>
#include <typeinfo>

namespace {
bool is_false(Exp *exp) {
  if (typeid(*exp) == typeid(FalseExp) || typeid(*exp) == typeid(NilExp)) {
    return true;
  }
  return false;
}

bool is_true(Exp *exp) {
  if (typeid(*exp) == typeid(TrueExp) || typeid(*exp) == typeid(IntegerExp) ||
      typeid(*exp) == typeid(FloatExp) || typeid(*exp) == typeid(StringExp)) {
    return true;
  }
  return false;
}

bool is_vararg_or_func_call(Exp *exp) {
  if (typeid(*exp) == typeid(VarargExp) ||
      typeid(*exp) == typeid(FuncCallExp)) {
    return true;
  }
  return false;
}

std::pair<int64_t, bool> cast_to_int(Exp *exp) {
  if (typeid(*exp) == typeid(IntegerExp)) {
    return {dynamic_cast<IntegerExp *>(exp)->val, true};
  } else if (typeid(*exp) == typeid(FloatExp)) {
    return LMath::float_to_int(dynamic_cast<FloatExp *>(exp)->val);
  } else {
    return {0, false};
  }
}

std::pair<double, bool> cast_to_float(Exp *exp) {
  if (typeid(*exp) == typeid(IntegerExp)) {
    return {(double)dynamic_cast<IntegerExp *>(exp)->val, true};
  } else if (typeid(*exp) == typeid(FloatExp)) {
    return {dynamic_cast<FloatExp *>(exp)->val, true};
  } else {
    return {0, false};
  }
}

Exp *optimize_unm(Exp *exp, ResourceManager *p_resource) {
  if (typeid(*exp) == typeid(IntegerExp)) {
    dynamic_cast<IntegerExp *>(exp)->val =
        -dynamic_cast<IntegerExp *>(exp)->val;
  } else if (typeid(*exp) == typeid(FloatExp)) {
    dynamic_cast<FloatExp *>(exp)->val = -dynamic_cast<FloatExp *>(exp)->val;
  }
  return exp;
}

Exp *optimize_not(Exp *exp, ResourceManager *p_resource) {
  if (typeid(*exp) == typeid(NilExp)) {
    auto tmp = dynamic_cast<NilExp *>(exp);
    return p_resource->new_exp<TrueExp>(tmp->line);
  } else if (typeid(*exp) == typeid(FalseExp)) {
    auto tmp = dynamic_cast<FalseExp *>(exp);
    return p_resource->new_exp<TrueExp>(tmp->line);
  } else if (typeid(*exp) == typeid(TrueExp)) {
    auto tmp = dynamic_cast<TrueExp *>(exp);
    return p_resource->new_exp<FalseExp>(tmp->line);
  } else if (typeid(*exp) == typeid(IntegerExp)) {
    auto tmp = dynamic_cast<IntegerExp *>(exp);
    return p_resource->new_exp<FalseExp>(tmp->line);
  } else if (typeid(*exp) == typeid(FloatExp)) {
    auto tmp = dynamic_cast<FloatExp *>(exp);
    return p_resource->new_exp<FalseExp>(tmp->line);
  } else if (typeid(*exp) == typeid(StringExp)) {
    auto tmp = dynamic_cast<StringExp *>(exp);
    return p_resource->new_exp<FalseExp>(tmp->line);
  }
  return exp;
}

Exp *optimize_bnot(Exp *exp, ResourceManager *p_resource) {
  if (typeid(*exp) == typeid(IntegerExp)) {
    dynamic_cast<IntegerExp *>(exp)->val =
        ~(dynamic_cast<IntegerExp *>(exp)->val);
  } else if (typeid(*exp) == typeid(FloatExp)) {
    if (auto [val, ok] = cast_to_int(exp); ok) {
      return p_resource->new_exp<IntegerExp>(
          dynamic_cast<FloatExp *>(exp)->line, ~val);
    }
  }
  return exp;
}

} // namespace

Exp *optimize_logical_or(BinopExp *exp, ResourceManager *p_resource) {
  if (is_true(exp->exp1)) {
    return exp->exp1;
  }
  if (is_false(exp->exp1) && !is_vararg_or_func_call(exp->exp2)) {
    return exp->exp2;
  }
  return exp;
}

Exp *optimize_logical_and(BinopExp *exp, ResourceManager *p_resource) {
  if (is_false(exp->exp1)) {
    return exp->exp1;
  }
  if (is_true(exp->exp1) && !is_vararg_or_func_call(exp->exp2)) {
    return exp->exp2;
  }
  return exp;
}

Exp *optimize_bitwise_binary_op(BinopExp *exp, ResourceManager *p_resource) {
  if (auto [i, ok] = cast_to_int(exp->exp1); ok) {
    if (auto [j, ok] = cast_to_int(exp->exp2); ok) {
      if (exp->op == TOKEN_OP_BAND) {
        return p_resource->new_exp<IntegerExp>(exp->line, i & j);
      } else if (exp->op == TOKEN_OP_BOR) {
        return p_resource->new_exp<IntegerExp>(exp->line, i | j);
      } else if (exp->op == TOKEN_OP_BXOR) {
        return p_resource->new_exp<IntegerExp>(exp->line, i ^ j);
      } else if (exp->op == TOKEN_OP_SHL) {
        return p_resource->new_exp<IntegerExp>(exp->line, i << j);
      } else if (exp->op == TOKEN_OP_SHR) {
        return p_resource->new_exp<IntegerExp>(exp->line, i >> j);
      }
    }
  }
  return exp;
}

Exp *optimize_arith_binary_op(BinopExp *exp, ResourceManager *p_resource) {
  if (auto [i, ok] = cast_to_int(exp->exp1); ok) {
    if (auto [j, ok] = cast_to_int(exp->exp2); ok) {
      if (exp->op == TOKEN_OP_ADD) {
        return p_resource->new_exp<IntegerExp>(exp->line, i + j);
      } else if (exp->op == TOKEN_OP_MINUS) {
        return p_resource->new_exp<IntegerExp>(exp->line, i - j);
      } else if (exp->op == TOKEN_OP_MUL) {
        return p_resource->new_exp<IntegerExp>(exp->line, i * j);
      } else if (exp->op == TOKEN_OP_IDIV && j != 0) {
        return p_resource->new_exp<IntegerExp>(exp->line,
                                               LMath::int_floor_div(i, j));
      } else if (exp->op == TOKEN_OP_MOD) {
        return p_resource->new_exp<IntegerExp>(exp->line, LMath::int_mod(i, j));
      }
    }
  }
  if (auto [i, ok] = cast_to_float(exp->exp1); ok) {
    if (auto [j, ok] = cast_to_float(exp->exp2); ok) {
      if (exp->op == TOKEN_OP_ADD) {
        return p_resource->new_exp<FloatExp>(exp->line, i + j);
      } else if (exp->op == TOKEN_OP_MINUS) {
        return p_resource->new_exp<FloatExp>(exp->line, i - j);
      } else if (exp->op == TOKEN_OP_MUL) {
        return p_resource->new_exp<FloatExp>(exp->line, i * j);
      } else if (exp->op == TOKEN_OP_DIV && j != 0) {
        return p_resource->new_exp<FloatExp>(exp->line, i / j);
      } else if (exp->op == TOKEN_OP_IDIV && j != 0) {
        return p_resource->new_exp<FloatExp>(exp->line,
                                             LMath::float_floor_div(i, j));
      } else if (exp->op == TOKEN_OP_MOD) {
        return p_resource->new_exp<FloatExp>(exp->line, LMath::float_mod(i, j));
      } else if (exp->op == TOKEN_OP_POW) {
        return p_resource->new_exp<FloatExp>(exp->line, pow(i, j));
      }
    }
  }
  return exp;
}

Exp *optimize_pow(Exp *exp, ResourceManager *p_resource) {
  if (typeid(*exp) == typeid(BinopExp) &&
      dynamic_cast<BinopExp *>(exp)->op == TOKEN_OP_POW) {
    BinopExp *tmp = dynamic_cast<BinopExp *>(exp);
    tmp->exp2 = optimize_pow(tmp->exp2, p_resource);
    return optimize_arith_binary_op(tmp, p_resource);
  }
  return exp;
}

Exp *optimize_unary_op(Exp *exp, ResourceManager *p_resource) {
  if (typeid(*exp) == typeid(UnopExp)) {
    BinopExp *tmp = dynamic_cast<BinopExp *>(exp);
    if (tmp->op == TOKEN_OP_UNM) {
      return optimize_unm(tmp, p_resource);
    } else if (tmp->op == TOKEN_OP_NOT) {
      return optimize_not(tmp, p_resource);
    } else if (tmp->op == TOKEN_OP_BNOT) {
      return optimize_bnot(tmp, p_resource);
    }
  }
  return exp;
}

#endif