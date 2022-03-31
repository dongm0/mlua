#include "ParseBlock.h"
#include <compiler/AST/Block.h>
#include <compiler/AST/resourcemanager.h>
#include <compiler/Lexer/Lexer.h>
#include <compiler/Lexer/Token.h>

Block *parse(std::string &chunk, std::string &chunk_name) {
  Lexer lexer(chunk, chunk_name);
  auto block = parse_block(&lexer, &ResourceManager::instance());
  lexer.next_token_of_kind(TOKEN_EOF);
  return block;
}