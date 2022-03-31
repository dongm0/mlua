#ifndef TOKEN_H_
#define TOKEN_H_

#include <map>
constexpr int TOKEN_EOF = 0;                 // end-of-file
constexpr int TOKEN_VARARG = 1;              // ...
constexpr int TOKEN_SEP_SEMI = 2;            // ;
constexpr int TOKEN_SEP_COMMA = 3;           // ,
constexpr int TOKEN_SEP_DOT = 4;             // .
constexpr int TOKEN_SEP_COLON = 5;           // :
constexpr int TOKEN_SEP_LABEL = 6;           // ::
constexpr int TOKEN_SEP_LPAREN = 7;          // (
constexpr int TOKEN_SEP_RPAREN = 8;          // )
constexpr int TOKEN_SEP_LBRACK = 9;          // [
constexpr int TOKEN_SEP_RBRACK = 10;         // ]
constexpr int TOKEN_SEP_LCURLY = 11;         // {
constexpr int TOKEN_SEP_RCURLY = 12;         // }
constexpr int TOKEN_OP_ASSIGN = 13;          // =
constexpr int TOKEN_OP_MINUS = 14;           // - (sub or unm)
constexpr int TOKEN_OP_WAVE = 15;            // ~ (bnot or bxor)
constexpr int TOKEN_OP_ADD = 16;             // +
constexpr int TOKEN_OP_MUL = 17;             // *
constexpr int TOKEN_OP_DIV = 18;             // /
constexpr int TOKEN_OP_IDIV = 19;            // //
constexpr int TOKEN_OP_POW = 20;             // ^
constexpr int TOKEN_OP_MOD = 21;             // %
constexpr int TOKEN_OP_BAND = 22;            // &
constexpr int TOKEN_OP_BOR = 23;             // |
constexpr int TOKEN_OP_SHR = 24;             // >>
constexpr int TOKEN_OP_SHL = 25;             // <<
constexpr int TOKEN_OP_CONCAT = 26;          // ..
constexpr int TOKEN_OP_LT = 27;              // <
constexpr int TOKEN_OP_LE = 28;              // <=
constexpr int TOKEN_OP_GT = 29;              // >
constexpr int TOKEN_OP_GE = 30;              // >=
constexpr int TOKEN_OP_EQ = 31;              // ==
constexpr int TOKEN_OP_NE = 32;              // ~=
constexpr int TOKEN_OP_LEN = 33;             // #
constexpr int TOKEN_OP_AND = 33;             // and
constexpr int TOKEN_OP_OR = 34;              // or
constexpr int TOKEN_OP_NOT = 35;             // not
constexpr int TOKEN_KW_BREAK = 36;           // break
constexpr int TOKEN_KW_DO = 37;              // do
constexpr int TOKEN_KW_ELSE = 38;            // else
constexpr int TOKEN_KW_ELSEIF = 39;          // elseif
constexpr int TOKEN_KW_END = 40;             // end
constexpr int TOKEN_KW_FALSE = 41;           // false
constexpr int TOKEN_KW_FOR = 42;             // for
constexpr int TOKEN_KW_FUNCTION = 43;        // function
constexpr int TOKEN_KW_GOTO = 44;            // goto
constexpr int TOKEN_KW_IF = 45;              // if
constexpr int TOKEN_KW_IN = 46;              // in
constexpr int TOKEN_KW_LOCAL = 47;           // local
constexpr int TOKEN_KW_NIL = 48;             // nil
constexpr int TOKEN_KW_REPEAT = 49;          // repeat
constexpr int TOKEN_KW_RETURN = 50;          // return
constexpr int TOKEN_KW_THEN = 51;            // then
constexpr int TOKEN_KW_TRUE = 52;            // true
constexpr int TOKEN_KW_UNTIL = 53;           // until
constexpr int TOKEN_KW_WHILE = 54;           // while
constexpr int TOKEN_IDENTIFIER = 55;         // identifier
constexpr int TOKEN_NUMBER = 56;             // number literal
constexpr int TOKEN_STRING = 57;             // string literal
constexpr int TOKEN_OP_UNM = TOKEN_OP_MINUS; // unary minus
constexpr int TOKEN_OP_SUB = TOKEN_OP_MINUS;
constexpr int TOKEN_OP_BNOT = TOKEN_OP_WAVE;
constexpr int TOKEN_OP_BXOR = TOKEN_OP_WAVE;

static std::map<std::string, int> keywords{
    std::pair{"and", TOKEN_OP_AND},
    std::pair{"break", TOKEN_KW_BREAK},
    std::pair{"do", TOKEN_KW_DO},
    std::pair{"else", TOKEN_KW_ELSE},
    std::pair{"elseif", TOKEN_KW_ELSEIF},
    std::pair{"end", TOKEN_KW_END},
    std::pair{"false", TOKEN_KW_FALSE},
    std::pair{"for", TOKEN_KW_FOR},
    std::pair{"function", TOKEN_KW_FUNCTION},
    std::pair{"goto", TOKEN_KW_GOTO},
    std::pair{"if", TOKEN_KW_IF},
    std::pair{"in", TOKEN_KW_IN},
    std::pair{"local", TOKEN_KW_LOCAL},
    std::pair{"nil", TOKEN_KW_NIL},
    std::pair{"not", TOKEN_OP_NOT},
    std::pair{"or", TOKEN_OP_OR},
    std::pair{"repeat", TOKEN_KW_REPEAT},
    std::pair{"return", TOKEN_KW_RETURN},
    std::pair{"then", TOKEN_KW_THEN},
    std::pair{"true", TOKEN_KW_TRUE},
    std::pair{"until", TOKEN_KW_UNTIL},
    std::pair{"while", TOKEN_KW_WHILE}};

#endif