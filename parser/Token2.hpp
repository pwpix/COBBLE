#pragma once

/*
 * Updated Token.hpp
 * Re-definition of grammer rules.
 *
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace simlang {


#define MASTER_LIST     \
    X(ID, "ID")         \
    X(DECL, "!--")      \
    X(ASSIGN, "=")      \
    X(PLUS, "+")        \
    X(MINUS, "-")       \
    X(SEMI, ";")        \
    X(COL, ":")         \
    X(COMMA, ",")       \
    X(LBRACE, "{")      \
    X(RBRACE, "}")      \
    X(SYMBOL, "SYMBOL") \
    X(NUM, "NUMBER")    \
    X(STR, "STRING")    \
    X(INDENT, "    ")   \
    X(NEWLINE, "\n")

#define X(Token, String) Token,
enum class TokenType : uint8_t { MASTER_LIST };
#undef X

struct Token {
    using Type = TokenType;
    using Lexeme = std::string_view;
    Type type;
    Lexeme str;
};

/*
struct Token {
    using Type = TokenType;

    Type type;
    uint64_t start_offset;
    size_t length;
};
*/

#define X(Token, String) String,
inline const char* TOKEN_STRINGS[] = {MASTER_LIST};
#undef X

inline std::vector<std::string> keywords{"object", "filename"};

}  // namespace simlang
