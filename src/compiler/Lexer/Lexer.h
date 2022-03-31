#ifndef LEXER_H_
#define LEXER_H_
#include <string>
#include <regex>

struct Lexer {
  std::string chunk;
  std::string chunk_name;
  int line;
  int head;

  std::string next_token_str = "";
  int next_token_kind = 0;
  int next_token_line = 0;

  Lexer(std::string &chunk, std::string &chunk_name);
  std::tuple<int, int, std::string> next_token();
  std::pair<int, std::string> next_token_of_kind(int kind);
  std::pair<int, std::string> next_identifier();
  void skip_white_spaces();
  void next(int n);
  bool test(std::string);
  bool is_white_space(char);
  bool is_new_line(char);
  void skip_comment();
  std::string scan_long_string();
  std::string scan_short_string();
  std::string scan_number();
  std::string scan_identifier();
  bool is_digit(char);
  bool is_latter(char);
  template <typename... Args> void error(std::string f, const Args &...a);
  std::string escape(std::string &str);
  int look_ahead();
  int token_line();

  std::string scan(std::regex &re);

  char _cur();
  bool _is_end();
};

#endif