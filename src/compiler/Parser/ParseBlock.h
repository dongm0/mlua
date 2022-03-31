#ifndef PARSEBLOCK_H_
#define PARSEBLOCK_H_
#include <vector>

class Lexer;
class ResourceManager;
class Stat;
class Block;
class Exp;

Block *parse_block(Lexer *lexer, ResourceManager *p_resource);

std::vector<Stat *> parse_stats(Lexer *lexer, ResourceManager *p_resource);

std::vector<Exp *> parse_ret_exps(Lexer *lexer, ResourceManager *p_resource);

#endif