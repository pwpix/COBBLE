#pragma once

#include <cstdint>
#include <string_view>


#define NON_TERMINALS     \
    NT (S, "Start")       \
    NT (E, "Expression")  \
    NT (F, "Name")        \
    NT (T, "Value")       \
    NT (O, "Object")      \
    NT (MB, "FieldBegin") \
    NT (ME, "FieldEnd")   \
    NT (LB, "ListBegin")  \
    NT (LE, "ListEnd")    \
    NT (NUM_NON_TERMINALS, "NumNonTerminals")

#define NT(non_terminal, string) non_terminal,
enum NonTerminals : uint16_t { NON_TERMINALS };
#undef NT

#define NT(non_terminal, string) string,
inline const char* Non_Terminals_Str[] = { NON_TERMINALS };
#undef NT

// Terminals

#define TERMINALS                         \
    TM (ID = NUM_NON_TERMINALS + 1, "ID") \
    TM (NUM, "Number")                    \
    TM (STR, "String")                    \
    TM (EQUAL, "=")                       \
    TM (COMMA, ",")                       \
    TM (COL, ":")                         \
    TM (SEMI, ";")                        \
    TM (PLUS, "+")                        \
    TM (MINUS, "-")                       \
    TM (LBRACE, "{")                      \
    TM (RBRACE, "}")                      \
    TM (RBRACK, "[")                      \
    TM (LBRACK, "]")                      \
    TM (LPAREN, "(")                      \
    TM (RPAREN, ")")                      \
    TM (EMPTY, "empty")                   \
    TM (ENDOF, "end")

#define TM(id, str) id,
enum Terminals : uint16_t { TERMINALS };
#undef TM


struct Token {

    uint16_t token_type     = 0;
    std::string_view lexeme = "";
    uint32_t line           = 0;
    uint32_t col            = 0;
};
