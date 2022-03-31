#ifndef PARSESTAT_H_
#define PARSESTAT_H_
#include <vector>

class Lexer;
class ResourceManager;
class Stat;
class Block;
class Exp;
Stat *parse_stat(Lexer *lexer, ResourceManager *p_resource);
Stat *parse_empty_stat(Lexer *lexer, ResourceManager *p_resource);
Stat *parse_break_stat(Lexer *lexer, ResourceManager *p_resource);
Stat *parse_label_stat(Lexer *lexer, ResourceManager *p_resource);
Stat *parse_go_to_stat(Lexer *lexer, ResourceManager *p_resource);
Stat *parse_do_stat(Lexer *lexer, ResourceManager *p_resource);
Stat *parse_while_stat(Lexer *lexer, ResourceManager *p_resource);
Stat *parse_repeat_stat(Lexer *lexer, ResourceManager *p_resource);
Stat *parse_if_stat(Lexer *lexer, ResourceManager *p_resource);
Stat *parse_for_stat(Lexer *lexer, ResourceManager *p_resource);
Stat *parse_func_def_stat(Lexer *lexer, ResourceManager *p_resource);
Stat *parse_local_assign_or_func_def_stat(Lexer *lexer,
                                          ResourceManager *p_resource);
Stat *parse_assign_or_func_call_stat(Lexer *lexer, ResourceManager *p_resource);

#endif