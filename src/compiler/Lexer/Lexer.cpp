#include "Lexer.h"
#include "Token.h"
#include <format>
#include <iostream>
#include <regex>

std::regex re_newline("^\\r\\n|\\n\\r|\\n|\\r");
std::regex re_identifier("^[_\\d\\w]+");
std::regex first_opening_long_bracket("^\\[=*\\[");
std::regex re_dec_escape_seq("^[0-9]{1,3}");
std::regex re_hex_escape_seq("^0[xX][0-9a-fA-F]{2}");
std::regex
    re_number("^0[xX][0-9a-fA-F]*(\\.[0-9a-fA-F]*)?([pP][\\+\\-]?[0-9]+)?|^["
              "0-9]*(\\.[0-9]*)?([eE][\\+\\-]?[0-9]+)?");
std::regex re_short_str("(^'(\\\\\\\\|\\\\'|\\\\\\n|\\\\z\\s*|[^'\\n])*')|(^\"("
                        "\\\\\\\\|\\\\\"|\\\\\\n|\\\\z\\s*|[^\"\\n])*\")");

char Lexer::_cur() { return chunk[head]; }
bool Lexer::_is_end() { return head == chunk.size(); }

Lexer::Lexer(std::string &c, std::string &c_name)
    : chunk(c), chunk_name(c_name), line(1), head(0) {}

std::tuple<int, int, std::string> Lexer::next_token() {
  if (next_token_line > 0) {
    auto _line = next_token_line;
    auto _kind = next_token_kind;
    auto _token = next_token_str;
    this->line = this->next_token_line;
    this->next_token_line = 0;
    return {_line, _kind, _token};
  }

  skip_white_spaces();
  if (_is_end()) {
    return {line, 0, "EOF"};
  }

  switch (_cur()) {
  case ';':
    next(1);
    return {line, TOKEN_SEP_SEMI, ";"};
  case ',':
    next(1);
    return {line, TOKEN_SEP_COMMA, ","};
  case '(':
    next(1);
    return {line, TOKEN_SEP_LPAREN, "("};
  case ')':
    next(1);
    return {line, TOKEN_SEP_RPAREN, ")"};
  case ']':
    next(1);
    return {line, TOKEN_SEP_RBRACK, "]"};
  case '{':
    next(1);
    return {line, TOKEN_SEP_LCURLY, "{"};
  case '}':
    next(1);
    return {line, TOKEN_SEP_RCURLY, "}"};
  case '+':
    next(1);
    return {line, TOKEN_OP_ADD, "+"};
  case '-':
    next(1);
    return {line, TOKEN_OP_MINUS, "-"};
  case '*':
    next(1);
    return {line, TOKEN_OP_MUL, "*"};
  case '^':
    next(1);
    return {line, TOKEN_OP_POW, "^"};
  case '%':
    next(1);
    return {line, TOKEN_OP_MOD, "%"};
  case '&':
    next(1);
    return {line, TOKEN_OP_BAND, "&"};
  case '|':
    next(1);
    return {line, TOKEN_OP_BOR, "|"};
  case '#':
    next(1);
    return {line, TOKEN_OP_LEN, "#"};
  case ':':
    if (test("::")) {
      next(2);
      return {line, TOKEN_SEP_LABEL, "::"};
    } else {
      next(1);
      return {line, TOKEN_SEP_COLON, ":"};
    }
  case '/':
    if (test("//")) {
      next(2);
      return {line, TOKEN_OP_IDIV, "//"};
    } else {
      next(1);
      return {line, TOKEN_OP_DIV, "/"};
    }
  case '~':
    if (test("~=")) {
      next(2);
      return {line, TOKEN_OP_NE, "//"};
    } else {
      next(1);
      return {line, TOKEN_OP_WAVE, "/"};
    }
  case '=':
    if (test("==")) {
      next(2);
      return {line, TOKEN_OP_EQ, "=="};
    } else {
      next(1);
      return {line, TOKEN_OP_ASSIGN, "="};
    }
  case '<':
    if (test("<<")) {
      next(2);
      return {line, TOKEN_OP_SHL, "<<"};
    } else if (test("<=")) {
      next(2);
      return {line, TOKEN_OP_LE, "<="};
    } else {
      next(1);
      return {line, TOKEN_OP_LT, "<"};
    }
  case '>':
    if (test(">>")) {
      next(2);
      return {line, TOKEN_OP_SHR, ">>"};
    } else if (test(">=")) {
      next(2);
      return {line, TOKEN_OP_GE, ">="};
    } else {
      next(1);
      return {line, TOKEN_OP_GT, ">"};
    }
  case '.':
    if (test("...")) {
      next(3);
      return {line, TOKEN_VARARG, "..."};
    } else if (test("..")) {
      next(2);
      return {line, TOKEN_OP_CONCAT, ".."};
    } else if (chunk.size() - head == 1 || !is_digit(_cur())) {
      next(1);
      return {line, TOKEN_SEP_DOT, "."};
    }
  case '[':
    if (test("[[") || test("[=")) {
      return {line, TOKEN_STRING, scan_long_string()};
    } else {
      next(1);
      return {line, TOKEN_SEP_LBRACK, "["};
    }
  case '\'':
    return {line, TOKEN_STRING, scan_short_string()};
  case '"':
    return {line, TOKEN_STRING, scan_short_string()};
  default:
    break;
  }

  auto c = _cur();
  if (c == '.' || is_digit(c)) {
    auto token = scan_number();
    return {line, TOKEN_NUMBER, token};
  }
  if (c == '_' || is_latter(c)) {
    auto token = scan_identifier();
    return {line, TOKEN_IDENTIFIER, token};
  }
}

void Lexer::skip_white_spaces() {
  while (!_is_end()) {
    if (test("--")) {
      skip_comment();
    } else if (test("\r\n") || test("\n\r")) {
      next(2);
      line += 1;
    } else if (is_new_line(_cur())) {
      next(1);
      line += 1;
    } else if (is_white_space(_cur())) {
      next(1);
    } else {
      break;
    };
  }
}

bool Lexer::test(std::string s) {
  if (chunk.size() - head < s.size()) {
    return false;
  }
  return chunk.substr(head, s.size()) == s;
}

void Lexer::next(int n) { head += n; }

bool Lexer::is_white_space(char c) {
  if (c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' ||
      c == ' ') {
    return true;
  }
  return false;
}

bool Lexer::is_new_line(char c) { return (c == '\r' || c == '\n'); }

void Lexer::skip_comment() {
  next(2);
  if (test("[")) {

    if (std::regex_search(chunk.begin() + head, chunk.end(),
                          first_opening_long_bracket)) {
      scan_long_string();
      return;
    }
  } else {
    while (!this->_is_end() && !is_new_line(_cur())) {
      next(1);
    }
  }
}

template <typename... Args> void Lexer::error(std::string f, const Args &...a) {
  std::cout << f;
}

std::string Lexer::scan_long_string() {
  std::smatch m;
  // std::regex_match(chunk.begin() + head, chunk.end(), m, long_bracket);
  std::regex_search(chunk.cbegin() + head, chunk.cend(), m,
                    first_opening_long_bracket);
  if (m.empty()) {
    error("invalid long string delimiter near {}"); //
  }
  auto cl_bracket = m[0].str();
  std::for_each(cl_bracket.begin(), cl_bracket.end(), [=](char &c) {
    if (c == '[')
      c = ']';
  });
  auto pos = chunk.find(cl_bracket, head + m.position(0));
  if (pos == std::string::npos) {
    error("unfinished long string or comment");
  }

  std::string _tmpstr(chunk.begin() + head + m.position(0) + cl_bracket.size(),
                      chunk.begin() + pos);
  next(pos + cl_bracket.size() - head);

  auto str = std::regex_replace(_tmpstr, re_newline, "\n");
  int linenum = 0;
  std::for_each(str.begin(), str.end(), [&](char c) {
    if (c == '\n')
      linenum += 1;
  });
  line += linenum;
  if (str.size() > 0 && str[0] == '\n') {
    str.erase(str.begin());
  }
  return str;
}

std::string Lexer::scan_short_string() {

  std::smatch m;
  if (std::regex_search(chunk.cbegin() + head, chunk.cend(), m, re_short_str)) {
    next(m[0].second - m[0].first);
    auto _tmp = m[0].str();
    std::string _content(_tmp.begin() + 1, _tmp.end() - 1);
    if (_content.find('\\') != std::string::npos) {
      auto str = std::regex_replace(_content, re_newline, "\n");
      int linenum = 0;
      std::for_each(str.begin(), str.end(), [&](char c) {
        if (c == '\n')
          linenum += 1;
      });
      line += linenum;
      _content = escape(str);
    }
    return _content;
  }
  error("unfinished short string!");
  return "";
}

std::string Lexer::escape(std::string &str) {
  std::string buf;

  std::smatch m;
  int st = 0;
  while (st < str.size()) {
    if (str[st] != '\\') {
      buf.push_back(str[st]);
      st += 1;
      continue;
    }
    if (str.size() - st == 1) {
      error("unfinished string");
    }
    switch (str[st + 1]) {
    case 'a':
      buf.push_back('\a');
      st += 2;
      continue;
    case 'b':
      buf.push_back('\b');
      st += 2;
      continue;
    case 'f':
      buf.push_back('\f');
      st += 2;
      continue;
    case 'n':
      buf.push_back('\n');
      st += 2;
      continue;
    case '\n':
      buf.push_back('\n');
      st += 2;
      continue;
    case 'r':
      buf.push_back('\r');
      st += 2;
      continue;
    case 't':
      buf.push_back('\t');
      st += 2;
      continue;
    case 'v':
      buf.push_back('\v');
      st += 2;
      continue;
    case '"':
      buf.push_back('"');
      st += 2;
      continue;
    case '\'':
      buf.push_back('\'');
      st += 2;
      continue;
    case '\\':
      buf.push_back('\\');
      st += 2;
      continue;
    case '0':
      if (std::regex_search(str.cbegin() + st, str.cend(), m,
                            re_dec_escape_seq)) {
        auto _num = m[0].str().substr(1);
        int _d = std::stoi(_num, nullptr, 10);
        if (_d < 0xff) {
          buf.push_back((char)_d);
          st += (m[0].second - m[0].first);
          continue;
        }
        error("decimal escape too large near {}", _num);
      }
      continue;
    case '1':
      if (std::regex_search(str.cbegin() + st, str.cend(), m,
                            re_dec_escape_seq)) {
        auto _num = m[0].str().substr(1);
        int _d = std::stoi(_num, nullptr, 10);
        if (_d < 0xff) {
          buf.push_back((char)_d);
          st += (m[0].second - m[0].first);
          continue;
        }
        error("decimal escape too large near {}", _num);
      }
      continue;
    case '2':
      if (std::regex_search(str.cbegin() + st, str.cend(), m,
                            re_dec_escape_seq)) {
        auto _num = m[0].str().substr(1);
        int _d = std::stoi(_num, nullptr, 10);
        if (_d < 0xff) {
          buf.push_back((char)_d);
          st += (m[0].second - m[0].first);
          continue;
        }
        error("decimal escape too large near {}", _num);
      }
      continue;
    case '3':
      if (std::regex_search(str.cbegin() + st, str.cend(), m,
                            re_dec_escape_seq)) {
        auto _num = m[0].str().substr(1);
        int _d = std::stoi(_num, nullptr, 10);
        if (_d < 0xff) {
          buf.push_back((char)_d);
          st += (m[0].second - m[0].first);
          continue;
        }
        error("decimal escape too large near {}", _num);
      }
      continue;
    case '4':
      if (std::regex_search(str.cbegin() + st, str.cend(), m,
                            re_dec_escape_seq)) {
        auto _num = m[0].str().substr(1);
        int _d = std::stoi(_num, nullptr, 10);
        if (_d < 0xff) {
          buf.push_back((char)_d);
          st += (m[0].second - m[0].first);
          continue;
        }
        error("decimal escape too large near {}", _num);
      }
      continue;
    case '5':
      if (std::regex_search(str.cbegin() + st, str.cend(), m,
                            re_dec_escape_seq)) {
        auto _num = m[0].str().substr(1);
        int _d = std::stoi(_num, nullptr, 10);
        if (_d < 0xff) {
          buf.push_back((char)_d);
          st += (m[0].second - m[0].first);
          continue;
        }
        error("decimal escape too large near {}", _num);
      }
      continue;
    case '6':
      if (std::regex_search(str.cbegin() + st, str.cend(), m,
                            re_dec_escape_seq)) {
        auto _num = m[0].str().substr(1);
        int _d = std::stoi(_num, nullptr, 10);
        if (_d < 0xff) {
          buf.push_back((char)_d);
          st += (m[0].second - m[0].first);
          continue;
        }
        error("decimal escape too large near {}", _num);
      }
      continue;
    case '7':
      if (std::regex_search(str.cbegin() + st, str.cend(), m,
                            re_dec_escape_seq)) {
        auto _num = m[0].str().substr(1);
        int _d = std::stoi(_num, nullptr, 10);
        if (_d < 0xff) {
          buf.push_back((char)_d);
          st += (m[0].second - m[0].first);
          continue;
        }
        error("decimal escape too large near {}", _num);
      }
      continue;
    case '8':
      if (std::regex_search(str.cbegin() + st, str.cend(), m,
                            re_dec_escape_seq)) {
        auto _num = m[0].str().substr(1);
        int _d = std::stoi(_num, nullptr, 10);
        if (_d < 0xff) {
          buf.push_back((char)_d);
          st += (m[0].second - m[0].first);
          continue;
        }
        error("decimal escape too large near {}", _num);
      }
      continue;
    case '9':
      if (std::regex_search(str.cbegin() + st, str.cend(), m,
                            re_dec_escape_seq)) {
        auto _num = m[0].str().substr(1);
        int _d = std::stoi(_num, nullptr, 10);
        if (_d < 0xff) {
          buf.push_back((char)_d);
          st += (m[0].second - m[0].first);
          continue;
        }
        error("decimal escape too large near {}", _num);
      }
      continue;
    case 'x':
      if (std::regex_search(str.cbegin() + st, str.cend(), m,
                            re_hex_escape_seq)) {
        auto _num = m[0].str().substr(2);
        int _d = std::stoi(_num, nullptr, 16);
        buf.push_back((char)_d);
        st += (m[0].second - m[0].first);
        continue;
      }
      continue;
    case 'u':
      throw std::runtime_error("unicode unsupported");
      continue;
    case 'z':
      st += 2;
      while (st < str.size() && is_white_space(str[0])) {
        st += 1;
      }
    default:
      error("invalid escape sequence near {}", str[st + 1]);
      continue;
    }
  }
  return buf;
}

bool Lexer::is_digit(char c) { return c >= '0' && c <= '9'; }
bool Lexer::is_latter(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

std::string Lexer::scan_number() { return scan(re_number); }

std::string Lexer::scan_identifier() { return scan(re_identifier); }

std::string Lexer::scan(std::regex &re) {
  std::smatch m;
  if (std::regex_search(chunk.cbegin() + head, chunk.cend(), m, re)) {
    next(m[0].second - m[0].first);
    return m[0].str();
  }
  throw std::runtime_error("unreachable!");
  return std::string();
}

int Lexer::look_ahead() {
  if (next_token_line > 0) {
    return next_token_kind;
  }
  auto current_line = line;
  auto [line, kind, token] = next_token();
  this->line = current_line;
  this->next_token_line = line;
  this->next_token_kind = kind;
  this->next_token_str = token;
  return kind;
}

std::pair<int, std::string> Lexer::next_token_of_kind(int kind) {
  auto [_line, _kind, _token] = next_token();
  if (_kind != kind) {
    error("syntax error near {}", _token);
  }
  return {_line, _token};
}

std::pair<int, std::string> Lexer::next_identifier() {
  return next_token_of_kind(TOKEN_IDENTIFIER);
}

int Lexer::token_line() { return line; }