#include <cassert>
#include <cstdint>
#include <map>
#include <print>
#include <stdexcept>
#include <vector>

#include "stack.hpp"
#include "table_driven/stack.hpp"

namespace simlang {

static std::nullptr_t LogError (const char* str);

// Non terminals

#define NON_TERMINALS      \
    NT (S, "Start")        \
    NT (D, "Decl")         \
    NT (F, "Name")         \
    NT (T, "Value")        \
    NT (O, "Object")       \
    NT (MB, "Field_Begin") \
    NT (ME, "Field_End")   \
    NT (LB, "ListBegin")   \
    NT (LE, "ListEnd")     \
    NT (NUM_NON_TERMINALS, "Num_Non_Terminals")

#define NT(non_terminal, string) non_terminal,
enum NonTerminals : uint16_t { NON_TERMINALS };
#undef NT

#define NT(non_terminal, string) string,
inline const char* Non_Terminals_Str[] = { NON_TERMINALS };
#undef NT

// Terminals

enum Terminals : uint16_t {
    id = NUM_NON_TERMINALS + 1,
    num,
    str,
    equal,
    comma,
    lbrace,
    rbrace,
    lbrack,
    rbrack,
    lparen,
    rparen,
    empty,
    endof
};


// rule_X__YY : X is the symbol we have at top of stack, and YY is the lookahead symbol
// rule_S__id => S ::= Production: 01, Symbols: { E, ENDOF }
//              top: S, lookahead: id
enum ProductionRules : uint16_t {

    rule_S__id,     // S ::= E ENDOF
    rule_E__id,     // E ::= F = T
    rule_T__num,    // T ::= num
    rule_T__str,    // T ::= str
    rule_T__lbrace, // T ::= O
    rule_T__brack,  // T ::= [ LB LE ]
    rule_F__id,     // F::= id

    rule_O__lbrace, // O ::= { MB ME }
    rule_MB__id,    // M ::= E
    rule_ME_rbrace, // MM ::= empty
    rule_ME_comma,  // MM ::= comma E MM

    rule_LB__num,    // LB ::= num
    rule_LE__comma,  // LE ::= , num LE
    rule_LE__rbrack, // LE ::= empty
};

std::vector<std::vector<int>> ProductionTable{};

// TODO:
int parse_table (int top, int lookahead)
{
    return -1;
}


// TODO:
void driver ();


/*
class Parser {

    public:
    explicit Parser (std::vector<Token> tokens_) : tokens (std::move (tokens_)) {}

    TokenType getTokenType (int id)
    {

        return id >= 0 && id < tokens.size () ? tokens.at (id).type : TokenType::Token_NONE;
    }

    size_t getCurrentToken () const noexcept { return currentToken; }

    size_t advance ()
    {
        ++currentToken;
        return currentToken;
    }

    size_t getNextToken ()
    {
        ++currentToken;
        return currentToken;
    }

    auto getTokenStr (int id) { return tokens.at (id).str; }
uint32_t getTokenLine (int id) { return tokens.at (id).line; }

    private:
    DeclarationNode* parseDeclaration (size_t index);
    int currentToken{ -1 };

    std::vector<Token> tokens;
};
*/


} // namespace simlang
