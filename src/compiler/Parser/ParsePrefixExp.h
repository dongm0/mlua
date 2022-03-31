#ifndef PARSEPREFIXEXP_H_
#define PARSEPREFIXEXP_H_
#include <vector>

class Lexer;
class ResourceManager;
class Stat;
class Block;
class Exp;
Exp *parse_prefix_exp(Lexer *lexer, ResourceManager *p_resource);
#endif