#ifndef PARSEEXP_H_
#define PARSEEXP_H_
#include <vector>

class Lexer;
class ResourceManager;
class Stat;
class Block;
class Exp;

Exp *parse_exp(Lexer *lexer, ResourceManager *p_resource);
std::vector<Exp *> parse_exp_list(Lexer *lexer, ResourceManager *p_resource);
Exp *parse_func_def_exp(Lexer *lexer, ResourceManager *p_resource);
Exp *parse_table_constructor_exp(Lexer *lexer, ResourceManager *p_resource);

#endif