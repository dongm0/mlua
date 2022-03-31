#include "api/LuaState.h"
#include "mlua.h"
#include <chunk/BinTrunk.h>
#include <compiler/Lexer.h>
#include <compiler/Token.h>
#include <format>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vm/OpCodes.h>
/*
static size_t get_bin_file_size(std::ifstream &fin) {
  auto head = fin.tellg();
  fin.seekg(0, std::ios::end);
  auto tail = fin.tellg();
  auto size = tail - head;
  fin.seekg(0, std::ios::beg);
  return size;
}

int print(API::LuaState &ls) { auto n_args = ls.get_top();
  for (int i = 1; i <= n_args; ++i) {
    if (ls.is_boolean(i)) {
      std::cout << (ls.to_boolean(i) ? "True" : "False") << "\n";
    } else if (ls.is_string(i)) {
      std::cout << ls.to_string(i) << "\n";
    } else if (ls.is_integer(i)) {
      std::cout << ls.to_integer(i) << "\n";
    } else if (ls.is_number(i)) {
      std::cout << ls.to_number(i) << "\n";
    } else if (ls.is_nil(i)) {
      std::cout << "nil\n";
    } else {
      std::cout << ls.type_name(ls.type(i)) << "\n";
    }
  }
  return 0;
}

int main(int argc, char **argv) {
  std::string ifname;
  std::string chunk_name("chunk");
#ifdef _DEBUG
  ifname = "luac.out";
#else
  if (argc <= 1) {
    return 1;
  } else {
    ifname = argv[1];
  }
#endif

  std::ifstream fin;
  fin.open(ifname, std::ios::binary);
  auto size = get_bin_file_size(fin);
  Byte *raw_data = new Byte[size + 1];
  fin.read((char *)raw_data, size);
  auto ls = API::LuaState();
  ls.state_register(std::string("print"), print);
  ls.load(raw_data, chunk_name, std::string("b"));
  ls.call(0, 0);
}
*/

static size_t get_bin_file_size(std::ifstream &fin) {
  auto head = fin.tellg();
  fin.seekg(0, std::ios::end);
  auto tail = fin.tellg();
  auto size = tail - head;
  fin.seekg(0, std::ios::beg);
  return size;
}

std::string kind_token_library(int kind) {
  if (kind < TOKEN_SEP_SEMI) {
    return "other";
  } else if (kind <= TOKEN_SEP_RCURLY) {
    return "separator";
  } else if (kind <= TOKEN_OP_NOT) {
    return "operator";
  } else if (kind <= TOKEN_KW_WHILE) {
    return "keyword";
  } else if (kind == TOKEN_IDENTIFIER) {
    return "identifier";
  } else if (kind == TOKEN_NUMBER) {
    return "number";
  } else if (kind == TOKEN_STRING) {
    return "string";
  } else {
    return "other";
  }
}

void test_lexer(std::string chunk, std::string chunk_name) {
  auto lexer = Lexer(chunk, chunk_name);
  while (true) {
    auto [line, kind, token] = lexer.next_token();
    std::cout << line << " " << kind_token_library(kind) << " " << token
              << "\n";
    if (kind == TOKEN_EOF) {
      break;
    }
  }
}

int main(int argc, char **argv) {
  std::string ifname;
#ifdef _DEBUG
  ifname = "test.lua";
#else
  if (argc <= 1) {
    return 1;
  } else {
    ifname = argv[1];
  }
#endif

  std::ifstream fin;
  fin.open(ifname);
  std::ostringstream tmp;
  tmp << fin.rdbuf();
  std::string raw_data = tmp.str();
  //auto size = get_bin_file_size(fin);
  //Byte *raw_data = new Byte[size + 1];
  //fin.read((char *)raw_data, size);
  test_lexer(raw_data, ifname);
}