#include "ParseExp.h"
#include "OptimizeExp.h"
#include "ParseBlock.h"
#include "resourcemanager.h"
#include <compiler/AST/Block.h>
#include <compiler/Lexer/Lexer.h>
#include <compiler/Lexer/Token.h>
#include <typeinfo>

namespace {
std::pair<std::vector<std::string>, bool>
_parse_par_list(Lexer *lexer, ResourceManager *p_resource) {
  std::vector<std::string> names;
  bool is_vararg = false;
  auto kind = lexer->look_ahead();
  if (kind == TOKEN_SEP_RPAREN) {
    return {std::vector<std::string>(), false};
  } else if (kind == TOKEN_VARARG) {
    lexer->next_token();
    return {std::vector<std::string>(), false};
  }

  auto [_, name] = lexer->next_identifier();
  names.push_back(name);
  while (lexer->look_ahead() == TOKEN_SEP_COMMA) {
    lexer->next_token();
    if (lexer->look_ahead() == TOKEN_IDENTIFIER) {
      auto [__, _name] = lexer->next_identifier();
      names.push_back(_name);
    } else {
      lexer->next_token_of_kind(TOKEN_VARARG);
      is_vararg = true;
      break;
    }
  }
  return {names, is_vararg};
}

bool _is_field_sep(int kind) {
  return kind == TOKEN_SEP_COMMA || kind == TOKEN_SEP_SEMI;
}
std::pair<Exp *, Exp *> _parse_field(Lexer *lexer,
                                     ResourceManager *p_resource) {
  Exp *k, *v;
  if (lexer->look_ahead() == TOKEN_SEP_LBRACK) {
    lexer->next_token();
    k = parse_exp(lexer, p_resource);
    lexer->next_token_of_kind(TOKEN_SEP_RBRACK);
    lexer->next_token_of_kind(TOKEN_OP_ASSIGN);
    v = parse_exp(lexer, p_resource);
    return {k, v};
  }

  auto exp = parse_exp(lexer, p_resource);
  if (typeid(*exp) == typeid(NameExp)) {
    auto nameexp = dynamic_cast<NameExp *>(exp);
    if (lexer->look_ahead() == TOKEN_OP_ASSIGN) {
      lexer->next_token();
      k = p_resource->new_exp<StringExp>(nameexp->line, nameexp->name);
      v = parse_exp(lexer, p_resource);
      return {k, v};
    }
  }
  return {nullptr, exp};
}

std::pair<std::vector<Exp *>, std::vector<Exp *>>
_parse_field_list(Lexer *lexer, ResourceManager *p_resource) {
  std::vector<Exp *> ks, vs;
  if (lexer->look_ahead() != TOKEN_SEP_RCURLY) {
    auto [k, v] = _parse_field(lexer, p_resource);
    ks.push_back(k);
    ks.push_back(v);

    while (_is_field_sep(lexer->look_ahead())) {
      lexer->next_token();
      if (lexer->look_ahead() != TOKEN_SEP_RCURLY) {
        auto [k, v] = _parse_field(lexer, p_resource);
        ks.push_back(k);
        ks.push_back(v);
      } else {
        break;
      }
    }
  }
  return {ks, vs};
}

Exp *parse_number_exp(Lexer *lexer, ResourceManager *p_resource) {
  auto [line, _, token] = lexer->next_token();
  if (auto [i, ok] = LMath::parse_int(token); ok) {
    return p_resource->new_exp<IntegerExp>(line, i);
  } else if (auto [f, ok] = LMath::parse_float(token); ok) {
    return p_resource->new_exp<FloatExp>(token, f);
  } else {
    throw std::runtime_error(std::string("not a number: ") + token);
  }
}
Exp *parse_exp_0(Lexer *lexer, ResourceManager *p_resource) {
  auto kind = lexer->look_ahead();
  if (kind == TOKEN_VARARG) {
    auto [line, _, __] = lexer->next_token();
    return p_resource->new_exp<VarargExp>(line);
  } else if (kind == TOKEN_KW_NIL) {
    auto [line, _, __] = lexer->next_token();
    return p_resource->new_exp<NilExp>(line);
  } else if (kind == TOKEN_KW_TRUE) {
    auto [line, _, __] = lexer->next_token();
    return p_resource->new_exp<TrueExp>(line);
  } else if (kind == TOKEN_KW_FALSE) {
    auto [line, _, __] = lexer->next_token();
    return p_resource->new_exp<FalseExp>(line);
  } else if (kind == TOKEN_STRING) {
    auto [line, _, token] = lexer->next_token();
    return p_resource->new_exp<StringExp>(line, token);
  } else if (kind == TOKEN_NUMBER) {
    return parse_number_exp(lexer, p_resource);
  } else if (kind == TOKEN_SEP_LCURLY) {
    return parse_table_constructor_exp(lexer, p_resource);
  } else if (kind == TOKEN_KW_FUNCTION) {
    lexer->next_token();
    return parse_func_def_exp(lexer, p_resource);
  } else {
    return parse_prefix_exp(lexer, p_resource);
  }
}
Exp *parse_exp_1(Lexer *lexer, ResourceManager *p_resource) {
  auto exp = parse_exp_0(lexer, p_resource);
  if (lexer->look_ahead() == TOKEN_OP_POW) {
    auto [line, op, _] = lexer->next_token();
    exp = p_resource->new_exp<BinopExp>(line, op, exp,
                                        parse_exp_2(lexer, p_resource));
  }
  return optimize_pow(exp, p_resource);
}
Exp *parse_exp_2(Lexer *lexer, ResourceManager *p_resource) {
  auto kind = lexer->look_ahead();
  if (kind == TOKEN_OP_UNM || kind == TOKEN_OP_BNOT || kind == TOKEN_OP_LEN ||
      kind == TOKEN_OP_NOT) {
    auto [line, op, _] = lexer->next_token();
    auto exp =
        p_resource->new_exp<UnopExp>(line, op, parse_exp_2(lexer, p_resource));
    return optimize_unary_op(exp, p_resource);
  }
  return parse_exp_1(lexer, p_resource);
}
Exp *parse_exp_3(Lexer *lexer, ResourceManager *p_resource) {
  auto exp = parse_exp_2(lexer, p_resource);
  while (true) {
    if (auto kind = lexer->look_ahead();
        kind == TOKEN_OP_MUL || kind == TOKEN_OP_DIV || kind == TOKEN_OP_IDIV ||
        kind == TOKEN_OP_MOD) {
      auto [line, op, _] = lexer->next_token();
      auto arith = p_resource->new_exp<BinopExp>(
          line, op, exp, parse_exp_2(lexer, p_resource));
      exp =
          optimize_arith_binary_op(dynamic_cast<BinopExp *>(arith), p_resource);
    } else {
      return exp;
    }
  }
  return exp;
}
Exp *parse_exp_4(Lexer *lexer, ResourceManager *p_resource) {
  auto exp = parse_exp_3(lexer, p_resource);
  while (true) {
    auto kind = lexer->look_ahead();
    if (kind == TOKEN_OP_ADD || kind == TOKEN_OP_MINUS) {
      auto [line, op, _] = lexer->next_token();
      auto arith = p_resource->new_exp<BinopExp>(
          line, op, exp, parse_exp_3(lexer, p_resource));
      exp =
          optimize_arith_binary_op(dynamic_cast<BinopExp *>(arith), p_resource);
    } else {
      return exp;
    }
  }

  return exp;
}
Exp *parse_exp_5(Lexer *lexer, ResourceManager *p_resource) {
  auto exp = parse_exp_4(lexer, p_resource);
  if (lexer->look_ahead() != TOKEN_OP_CONCAT) {
    return exp;
  }
  auto line = 0;
  std::vector<Exp *> exps{exp};
  while (lexer->look_ahead() == TOKEN_OP_CONCAT) {
    auto [line, _, __] = lexer->next_token();
    exps.push_back(parse_exp_4(lexer, p_resource));
  }
  return p_resource->new_exp<ConcatExp>(line, exps);
}
Exp *parse_exp_6(Lexer *lexer, ResourceManager *p_resource) {
  auto exp = parse_exp_5(lexer, p_resource);
  while (true) {
    auto kind = lexer->look_ahead();
    if (kind == TOKEN_OP_SHL || kind == TOKEN_OP_SHR) {
      auto [line, op, __] = lexer->next_token();
      exp = p_resource->new_exp<BinopExp>(line, op, exp,
                                          parse_exp_5(lexer, p_resource));
      exp =
          optimize_bitwise_binary_op(dynamic_cast<BinopExp *>(exp), p_resource);
    }
  }
  return exp;
}
Exp *parse_exp_7(Lexer *lexer, ResourceManager *p_resource) {
  auto exp = parse_exp_6(lexer, p_resource);
  while (lexer->look_ahead() == TOKEN_OP_BAND) {
    auto [line, op, _] = lexer->next_token();
    exp = p_resource->new_exp<BinopExp>(line, op, exp,
                                        parse_exp_6(lexer, p_resource));
    exp = optimize_bitwise_binary_op(dynamic_cast<BinopExp *>(exp), p_resource);
  }
  return exp;
}
Exp *parse_exp_8(Lexer *lexer, ResourceManager *p_resource) {
  auto exp = parse_exp_7(lexer, p_resource);
  while (lexer->look_ahead() == TOKEN_OP_BXOR) {
    auto [line, op, _] = lexer->next_token();
    exp = p_resource->new_exp<BinopExp>(line, op, exp,
                                        parse_exp_7(lexer, p_resource));
    exp = optimize_bitwise_binary_op(dynamic_cast<BinopExp *>(exp), p_resource);
  }
  return exp;
}

Exp *parse_exp_9(Lexer *lexer, ResourceManager *p_resource) {
  auto exp = parse_exp_8(lexer, p_resource);
  while (lexer->look_ahead() == TOKEN_OP_BOR) {
    auto [line, op, _] = lexer->next_token();
    exp = p_resource->new_exp<BinopExp>(line, op, exp,
                                        parse_exp_8(lexer, p_resource));
    exp = optimize_bitwise_binary_op(dynamic_cast<BinopExp *>(exp), p_resource);
  }
  return exp;
}
Exp *parse_exp_10(Lexer *lexer, ResourceManager *p_resource) {
  auto exp = parse_exp_9(lexer, p_resource);
  while (true) {
    auto kind = lexer->look_ahead();
    if (kind == TOKEN_OP_LT || kind == TOKEN_OP_GT || kind == TOKEN_OP_NE ||
        kind == TOKEN_OP_LE || kind == TOKEN_OP_GE || kind == TOKEN_OP_EQ) {
      auto [line, op, _] = lexer->next_token();
      exp = p_resource->new_exp<BinopExp>(line, op, exp,
                                          parse_exp_9(lexer, p_resource));
    } else {
      return exp;
    }
  }
  return exp;
}
Exp *parse_exp_11(Lexer *lexer, ResourceManager *p_resource) {
  auto exp = parse_exp_10(lexer, p_resource);
  while (lexer->look_ahead() == TOKEN_OP_AND) {
    auto [line, op, _] = lexer->next_token();
    exp = p_resource->new_exp<BinopExp>(line, op, exp,
                                        parse_exp_10(lexer, p_resource));
    exp = optimize_logical_and(dynamic_cast<BinopExp *>(exp), p_resource);
  }
  return exp;
}
Exp *parse_exp_12(Lexer *lexer, ResourceManager *p_resource) {
  auto exp = parse_exp_11(lexer, p_resource);
  while (lexer->look_ahead() == TOKEN_OP_OR) {
    auto [line, op, _] = lexer->next_token();
    exp = p_resource->new_exp<BinopExp>(line, op, exp,
                                        parse_exp_11(lexer, p_resource));
    exp = optimize_logical_or(dynamic_cast<BinopExp *>(exp), p_resource);
  }
  return exp;
}
} // namespace

Exp *parse_exp(Lexer *lexer, ResourceManager *p_resource) {
  return parse_exp_12(lexer, p_resource);
}

Exp *parse_func_def_exp(Lexer *lexer, ResourceManager *p_resource) {
  auto line = lexer->token_line();
  lexer->next_token_of_kind(TOKEN_SEP_LPAREN);
  auto [par_list, is_vararg] = _parse_par_list(lexer, p_resource);
  lexer->next_token_of_kind(TOKEN_SEP_RPAREN);
  auto block = parse_block(lexer, p_resource);
  auto [last_line, _] = lexer->next_token_of_kind(TOKEN_KW_END);
  return p_resource->new_exp<FunDefExp>(line, last_line, par_list, is_vararg,
                                        block);
}
Exp *parse_prefix_exp(Lexer *lexer, ResourceManager *p_resource);

std::vector<Exp *> parse_exp_list(Lexer *lexer, ResourceManager *p_resource) {
  std::vector<Exp *> ret;
  for (int i = 0; i < 4; ++i) {
    Exp *exp = p_resource->new_exp<Exp>();
    ret.push_back(exp);
  }
  ret.push_back(parse_exp(lexer, p_resource));
  while (lexer->look_ahead() == TOKEN_SEP_COMMA) {
    lexer->next_token();
    ret.push_back(parse_exp(lexer, p_resource));
  }
  return ret;
}

Exp *parse_table_constructor_exp(Lexer *lexer, ResourceManager *p_resource) {
  auto line = lexer->token_line();
  lexer->next_token_of_kind(TOKEN_SEP_LCURLY);
  auto [key_exps, val_exps] = _parse_field_list(lexer, p_resource);
  lexer->next_token_of_kind(TOKEN_SEP_RCURLY);
  auto last_line = lexer->token_line();
  return p_resource->new_exp<TableConstructExp>(line, last_line, key_exps,
                                                val_exps);
}