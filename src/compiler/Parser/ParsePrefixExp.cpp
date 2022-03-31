#include "ParsePrefixExp.h"
#include "OptimizeExp.h"
#include "ParseBlock.h"
#include "ParseExp.h"
#include "resourcemanager.h"
#include <compiler/AST/Block.h>
#include <compiler/Lexer/Lexer.h>
#include <compiler/Lexer/Token.h>
#include <typeinfo>

Exp *_parse_name_exp(Lexer *lexer, ResourceManager *p_resource) {
  if (lexer->look_ahead() == TOKEN_SEP_COLON) {
    lexer->next_token();
    auto [line, name] = lexer->next_identifier();
    return p_resource->new_exp<StringExp>(line, name);
  }
  return nullptr;
}

std::vector<Exp *> _parse_args(Lexer *lexer, ResourceManager *p_resource) {
  std::vector<Exp *> args;
  auto kind = lexer->look_ahead();
  if (kind == TOKEN_SEP_LPAREN) {
    lexer->next_token();
    if (lexer->look_ahead() != TOKEN_SEP_RPAREN) {
      args = parse_exp_list(lexer, p_resource);
    }
    lexer->next_token_of_kind(TOKEN_SEP_RPAREN);
  } else if (kind == TOKEN_SEP_LCURLY) {
    args = std::vector<Exp *>{parse_table_constructor_exp(lexer, p_resource)};
  } else {
    auto [line, str] = lexer->next_token_of_kind(TOKEN_STRING);
    args = std::vector<Exp *>{p_resource->new_exp<StringExp>(line, str)};
  }
  return args;
}

Exp *_finish_func_call_exp(Lexer *lexer, Exp *prefix_exp,
                           ResourceManager *p_resource) {
  auto name_exp = _parse_name_exp(lexer, p_resource);
  auto line = lexer->token_line();
  auto args = _parse_args(lexer, p_resource);
  auto last_line = lexer->token_line();
  return p_resource->new_exp<FuncCallExp>(line, last_line, prefix_exp, name_exp,
                                          args);
}

Exp *parse_parens_exp(Lexer *lexer, ResourceManager *p_resource) {
  lexer->next_token_of_kind(TOKEN_SEP_LPAREN);
  auto exp = parse_exp(lexer, p_resource);
  lexer->next_token_of_kind(TOKEN_SEP_RPAREN);

  if (typeid(*exp) == typeid(VarargExp) ||
      typeid(*exp) == typeid(FuncCallExp) || typeid(*exp) == typeid(NameExp) ||
      typeid(*exp) == typeid(TableAccessExp)) {
    return p_resource->new_exp<ParensExp>(exp);
  }
  return exp;
}

Exp *_finish_prefix_exp(Lexer *lexer, Exp *exp, ResourceManager *p_resource) {
  while (true) {
    auto kind = lexer->look_ahead();
    if (kind == TOKEN_SEP_LBRACK) {
      lexer->next_token();
      auto key_exp = parse_exp(lexer, p_resource);
      lexer->next_token_of_kind(TOKEN_SEP_RBRACK);
      exp = p_resource->new_exp<TableAccessExp>(lexer->token_line(), exp,
                                                key_exp);
    } else if (kind == TOKEN_SEP_DOT) {
      lexer->next_token();
      auto [line, name] = lexer->next_identifier();
      auto key_exp = p_resource->new_exp<StringExp>(line, name);
      exp = p_resource->new_exp<TableAccessExp>(line, exp, key_exp);
    } else if (kind == TOKEN_SEP_LPAREN || kind == TOKEN_SEP_LCURLY ||
               kind == TOKEN_STRING) {
      exp = _finish_func_call_exp(lexer, exp, p_resource);
    } else {
      return exp;
    }
  }
  return exp;
}

Exp *parse_prefix_exp(Lexer *lexer, ResourceManager *p_resource) {
  Exp *exp;
  if (lexer->look_ahead() == TOKEN_IDENTIFIER) {
    auto [line, name] = lexer->next_identifier();
    exp = p_resource->new_exp<NameExp>(line, name);
  } else {
    exp = parse_parens_exp(lexer, p_resource);
  }
  return _finish_prefix_exp(lexer, exp, p_resource);
}