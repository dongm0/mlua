#include "ParseBlock.h"
#include "ParseExp.h"
#include "ParseStat.h"
#include <compiler/AST/Block.h>
#include <compiler/AST/resourcemanager.h>
#include <compiler/Lexer/Lexer.h>
#include <compiler/Lexer/Token.h>
#include <typeinfo>

namespace {
bool _is_return_or_block_end(int token_kind) {
  if (token_kind == TOKEN_KW_RETURN || token_kind == TOKEN_EOF ||
      token_kind == TOKEN_KW_END || token_kind == TOKEN_KW_ELSE ||
      token_kind == TOKEN_KW_ELSEIF || token_kind == TOKEN_KW_UNTIL) {
    return true;
  }
  return false;
}
} // namespace

std::vector<Stat *> parse_stats(Lexer *lexer, ResourceManager *p_resource) {
  std::vector<Stat *> ret;
  for (int i = 0; i < 8; ++i) {
    auto stat = p_resource->new_stat<Stat>();
    ret.push_back(stat);
  }

  while (!_is_return_or_block_end(lexer->look_ahead())) {
    auto stat = parse_stat(lexer, p_resource);
    if (typeid(*stat) != typeid(EmptyStat)) {
      ret.push_back(stat);
    }
  }
  return ret;
}
std::vector<Exp *> parse_ret_exps(Lexer *lexer, ResourceManager *p_resource) {
  std::vector<Exp *> ret;
  if (lexer->look_ahead() != TOKEN_KW_RETURN) {
    return ret;
  }

  lexer->next_token();
  auto _kind = lexer->look_ahead();
  if (_kind == TOKEN_EOF || _kind == TOKEN_KW_ELSEIF ||
      _kind == TOKEN_KW_UNTIL) {
    return ret;
  } else if (_kind == TOKEN_SEP_SEMI) {
    lexer->next_token();
    return ret;
  } else {
    auto exps = parse_exp_list(lexer, p_resource);
    if (lexer->look_ahead() == TOKEN_SEP_SEMI) {
      lexer->next_token();
    }
    return exps;
  }
}