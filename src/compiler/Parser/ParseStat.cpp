#include "ParseBlock.h"
#include "ParseExp.h"
#include "ParsePrefixExp.h"
#include "resourcemanager.h"
#include <compiler/AST/Block.h>
#include <compiler/Lexer/Lexer.h>
#include <compiler/Lexer/Token.h>
#include <typeinfo>

namespace {
Stat *_finish_for_num_stat(Lexer *lexer, int line_of_for, std::string name,
                           ResourceManager *p_resource) {
  lexer->next_token_of_kind(TOKEN_OP_ASSIGN);
  auto init_exp = parse_exp(lexer, p_resource);
  lexer->next_token_of_kind(TOKEN_SEP_COMMA);
  auto limit_exp = parse_exp(lexer, p_resource);
  Exp *step_exp;
  if (lexer->look_ahead() == TOKEN_SEP_COMMA) {
    lexer->next_token();
    step_exp = parse_exp(lexer, p_resource);
  } else {
    step_exp = p_resource->new_exp<IntegerExp>(lexer->token_line(), 1);
  }
  auto [line_of_do, _] = lexer->next_token_of_kind(TOKEN_KW_DO);
  auto block = parse_block(lexer, p_resource);
  lexer->next_token_of_kind(TOKEN_KW_END);
  auto stat = p_resource->new_stat<ForNumStat>(
      line_of_for, line_of_do, name, init_exp, limit_exp, step_exp, block);
  return stat;
}

std::vector<std::string> _finish_name_list(Lexer *lexer, std::string name0,
                                           ResourceManager *p_resource) {
  auto names = std::vector<std::string>{name0};
  while (lexer->look_ahead() == TOKEN_SEP_COMMA) {
    lexer->next_token();
    auto [_, name] = lexer->next_identifier();
    names.push_back(name);
  }
  return names;
}

Stat *_finish_for_in_stat(Lexer *lexer, std::string &name0,
                          ResourceManager *p_resource) {
  auto name_list = _finish_name_list(lexer, name0, p_resource);
  lexer->next_token_of_kind(TOKEN_KW_IN);
  auto exp_list = parse_exp_list(lexer, p_resource);
  auto [line_of_do, _] = lexer->next_token_of_kind(TOKEN_KW_DO);
  auto block = parse_block(lexer, p_resource);
  lexer->next_token_of_kind(TOKEN_KW_END);
  auto stat =
      p_resource->new_stat<ForInStat>(line_of_do, name_list, exp_list, block);
  return stat;
}

Stat *_finish_local_func_def_stat(Lexer *lexer, ResourceManager *p_resource) {
  lexer->next_token_of_kind(TOKEN_KW_FUNCTION);
  auto [_, name] = lexer->next_identifier();
  auto fd_exp = parse_func_def_exp(lexer, p_resource);
  auto stat = p_resource->new_stat<LocalFuncDefStat>(name, fd_exp);
  return stat;
}

Stat *_finish_local_var_decl_stat(Lexer *lexer, ResourceManager *p_resource) {
  auto [_, name0] = lexer->next_identifier();
  auto name_list = _finish_name_list(lexer, name0, p_resource);
  std::vector<Exp *> exp_list;
  if (lexer->look_ahead() == TOKEN_OP_ASSIGN) {
    lexer->next_token();
    exp_list = parse_exp_list(lexer, p_resource);
  }
  auto last_line = lexer->token_line();
  auto stat =
      p_resource->new_stat<LocalVarDecStat>(last_line, name_list, exp_list);
  return stat;
}

Exp *_check_var(Lexer *lexer, Exp *exp, ResourceManager *p_resource) {
  if (typeid(*exp) == typeid(NameExp) ||
      typeid(*exp) == typeid(TableAccessExp)) {
    return exp;
  }
  lexer->next_token_of_kind(-1);
  throw std::runtime_error("unreachable!");
  return nullptr;
}
std::vector<Exp *> _finish_var_list(Lexer *lexer, Exp *var0,
                                    ResourceManager *p_resource) {
  std::vector<Exp *> vars{_check_var(lexer, var0, p_resource)};
  while (lexer->look_ahead() == TOKEN_SEP_COMMA) {
    lexer->next_token();
    auto exp = parse_prefix_exp(lexer, p_resource);
    vars.push_back(_check_var(lexer, exp, p_resource));
  }
  return vars;
}
} // namespace

Stat *parse_stat(Lexer *lexer, ResourceManager *p_resource) {
  auto _kind = lexer->look_ahead();
  if (_kind == TOKEN_SEP_SEMI)
    return parse_empty_stat(lexer, p_resource);
  else if (_kind == TOKEN_KW_BREAK)
    return parse_break_stat(lexer, p_resource);
  else if (_kind == TOKEN_SEP_LABEL)
    return parse_label_stat(lexer, p_resource);
  else if (_kind == TOKEN_KW_GOTO)
    return parse_go_to_stat(lexer, p_resource);
  else if (_kind == TOKEN_KW_DO)
    return parse_do_stat(lexer, p_resource);
  else if (_kind == TOKEN_KW_WHILE)
    return parse_while_stat(lexer, p_resource);
  else if (_kind == TOKEN_KW_REPEAT)
    return parse_repeat_stat(lexer, p_resource);
  else if (_kind == TOKEN_KW_IF)
    return parse_if_stat(lexer, p_resource);
  else if (_kind == TOKEN_KW_FOR)
    return parse_for_stat(lexer, p_resource);
  else if (_kind == TOKEN_KW_FUNCTION)
    return parse_func_def_stat(lexer, p_resource);
  else if (_kind == TOKEN_KW_LOCAL)
    return parse_local_assign_or_func_def_stat(lexer, p_resource);
  else
    return parse_assign_or_func_call_stat(lexer, p_resource);
}
Stat *parse_empty_stat(Lexer *lexer, ResourceManager *p_resource) {
  lexer->next_token_of_kind(TOKEN_SEP_SEMI);
  auto stat = p_resource->new_stat<EmptyStat>();
  return stat;
}
Stat *parse_break_stat(Lexer *lexer, ResourceManager *p_resource) {
  lexer->next_token_of_kind(TOKEN_KW_BREAK);
  auto stat = p_resource->new_stat<BreakStat>(lexer->token_line());
  return stat;
}
Stat *parse_label_stat(Lexer *lexer, ResourceManager *p_resource) {
  lexer->next_token_of_kind(TOKEN_SEP_LABEL);
  auto [_, name] = lexer->next_identifier();
  auto stat = p_resource->new_stat<LabelStat>(name);
  lexer->next_token_of_kind(TOKEN_SEP_LABEL);
  return stat;
}
Stat *parse_go_to_stat(Lexer *lexer, ResourceManager *p_resource) {
  lexer->next_token_of_kind(TOKEN_KW_GOTO);
  auto [_, name] = lexer->next_identifier();
  auto stat = p_resource->new_stat<GotoStat>(name);
  return stat;
}
Stat *parse_do_stat(Lexer *lexer, ResourceManager *p_resource) {
  lexer->next_token_of_kind(TOKEN_KW_DO);
  auto block = parse_block(lexer, p_resource);
  auto stat = p_resource->new_stat<DoStat>(block);
  lexer->next_token_of_kind(TOKEN_KW_END);
  return stat;
}
Stat *parse_while_stat(Lexer *lexer, ResourceManager *p_resource) {
  lexer->next_token_of_kind(TOKEN_KW_WHILE);
  auto exp = parse_exp(lexer, p_resource);
  lexer->next_token_of_kind(TOKEN_KW_DO);
  auto block = parse_block(lexer, p_resource);
  auto stat = p_resource->new_stat<WhileStat>(exp, block);
  return stat;
}
Stat *parse_repeat_stat(Lexer *lexer, ResourceManager *p_resource) {
  lexer->next_token_of_kind(TOKEN_KW_REPEAT);
  auto block = parse_block(lexer, p_resource);
  lexer->next_token_of_kind(TOKEN_KW_UNTIL);
  auto exp = parse_exp(lexer, p_resource);
  auto stat = p_resource->new_stat<RepeatStat>(exp, block);
  return stat;
}
Stat *parse_if_stat(Lexer *lexer, ResourceManager *p_resource) {
  std::vector<Block *> blocks;
  std::vector<Exp *> exps;
  for (int i = 0; i < 4; ++i) {
    Exp *exp = p_resource->new_exp<Exp>();
    Block *block = p_resource->new_block();
  }

  lexer->next_token_of_kind(TOKEN_KW_IF);
  exps.push_back(parse_exp(lexer, p_resource));
  lexer->next_token_of_kind(TOKEN_KW_THEN);
  blocks.push_back(parse_block(lexer, p_resource));

  while (lexer->look_ahead() == TOKEN_KW_ELSEIF) {
    lexer->next_token();
    exps.push_back(parse_exp(lexer, p_resource));
    lexer->next_token();
    blocks.push_back(parse_block(lexer, p_resource));
  }

  if (lexer->look_ahead() == TOKEN_KW_ELSE) {
    lexer->next_token();
    auto trueexp = p_resource->new_exp<TrueExp>(lexer->token_line());
    exps.push_back(trueexp);
    blocks.push_back(parse_block(lexer, p_resource));
  }

  lexer->next_token_of_kind(TOKEN_KW_END);
  auto stat = p_resource->new_stat<IfStat>(exps, blocks);
  return stat;
}
Stat *parse_for_stat(Lexer *lexer, ResourceManager *p_resource) {
  auto [line_of_for, _] = lexer->next_token_of_kind(TOKEN_KW_FOR);
  auto [_, name] = lexer->next_identifier();
  if (lexer->look_ahead() == TOKEN_OP_ASSIGN) {
    return _finish_for_num_stat(lexer, line_of_for, name, p_resource);
  } else {
    return _finish_for_in_stat(lexer, name, p_resource);
  }
}
std::pair<Exp *, bool> _parse_func_name(Lexer *lexer,
                                        ResourceManager *p_resource) {
  bool has_colon = false;
  auto [line, name] = lexer->next_identifier();
  Exp *exp = p_resource->new_exp<NameExp>(line, name);
  while (lexer->look_ahead() == TOKEN_SEP_DOT) {
    lexer->next_token();
    auto [_line, _name] = lexer->next_identifier();
    auto idx = p_resource->new_exp<StringExp>(line, name);
    exp = p_resource->new_exp<TableAccessExp>(line, exp, idx);
  }
  if (lexer->look_ahead() == TOKEN_SEP_COLON) {
    lexer->next_token();
    auto [_line, _name] = lexer->next_identifier();
    auto idx = p_resource->new_exp<StringExp>(line, name);
    exp = p_resource->new_exp<TableAccessExp>(line, exp, idx);
    has_colon = true;
  }
  return {exp, has_colon};
}
Stat *parse_func_def_stat(Lexer *lexer, ResourceManager *p_resource) {
  lexer->next_token_of_kind(TOKEN_KW_FUNCTION);
  auto [fn_exp, has_colon] = _parse_func_name(lexer, p_resource);
  auto fd_exp = parse_func_def_exp(lexer, p_resource);
  auto tmp = dynamic_cast<FunDefExp *>(fd_exp);
  if (has_colon) {
    tmp->par_list.insert(tmp->par_list.begin(), "self");
  }
  return p_resource->new_stat<AssignStat>(tmp->line, std::vector<Exp *>{fn_exp},
                                          std::vector<Exp *>{fd_exp});
}
Stat *parse_local_assign_or_func_def_stat(Lexer *lexer,
                                          ResourceManager *p_resource) {
  lexer->next_token_of_kind(TOKEN_KW_LOCAL);
  if (lexer->look_ahead() == TOKEN_KW_FUNCTION) {
    return _finish_local_func_def_stat(lexer, p_resource);
  } else {
    return _finish_local_var_decl_stat(lexer, p_resource);
  }
}

Stat *parse_assign_stat(Lexer *lexer, Exp *var0, ResourceManager *p_resource) {
  auto var_list = _finish_var_list(lexer, var0, p_resource);
  lexer->next_token_of_kind(TOKEN_OP_ASSIGN);
  auto exp_list = parse_exp_list(lexer, p_resource);
  auto last_line = lexer->token_line();
  auto stat = p_resource->new_stat<AssignStat>(last_line, var_list, exp_list);
  return stat;
}
Stat *parse_assign_or_func_call_stat(Lexer *lexer,
                                     ResourceManager *p_resource) {
  auto pre_fix_exp = parse_prefix_exp(lexer, p_resource);
  if (typeid(*pre_fix_exp) == typeid(FuncCallExp)) {
    auto tmp = dynamic_cast<FuncCallExp *>(pre_fix_exp);
    return p_resource->new_stat<FuncCallStat>(
        tmp->line, tmp->last_line, tmp->prefix_exp, tmp->name_exp, tmp->args);
  } else {
    return parse_assign_stat(lexer, pre_fix_exp, p_resource);
  }
}
