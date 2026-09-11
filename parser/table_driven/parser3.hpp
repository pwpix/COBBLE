#include <cassert>
#include <cstdint>
#include <map>
#include <print>
#include <stdexcept>
#include <vector>

#include "Token2.hpp"
#include "stack.hpp"
#include "table_driven/stack.hpp"

namespace simlang {

static std::nullptr_t LogError (const char* str);

// Non terminals

#define NON_TERMINALS      \
    NT (S, "Start")        \
    NT (E, "Decl")         \
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

#define TERMINALS                         \
    TM (ID = NUM_NON_TERMINALS + 1, "ID") \
    TM (NUM, "NUM")                       \
    TM (STR, "STR")                       \
    TM (EQUAL, "=")                       \
    TM (COMMA, ",")                       \
    TM (LBRACE, "{")                      \
    TM (RBRACE, "}")                      \
    TM (RBRACK, "[")                      \
    TM (LBRACK, "]")                      \
    TM (LPAREN, "(")                      \
    TM (RPAREN, ")")                      \
    TM (EMPTY, "empty")                   \
    TM (ENDOF, "END")

#define TM(id, str) id,
enum Terminals : uint16_t { TERMINALS };
#undef TM


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
    num_production_rules,
};


// TODO:
void driver ();

struct Symbol {
    uint16_t id;
    bool is_terminal;

    bool operator< (const Symbol& other) const
    {
        return std::tie (id, is_terminal) < std::tie (other.id, other.is_terminal);
    }
    bool operator== (const Symbol& other) const
    {
        return id == other.id && is_terminal == other.is_terminal;
    }
};

struct ProductionRule {
    int rule;
    Symbol lhs;
    std::vector<Symbol> rhs;
};

struct LL1ParseTable {
    std::map<std::pair<Symbol, Symbol>, uint32_t> table;

    void set (Symbol non_terminal, Symbol terminal, uint32_t rule)
    {
        table[{ non_terminal, terminal }] = rule;
    }

    int get_production (Symbol top, Symbol next_input_symbol)
    {
        auto it = table.find ({ top, next_input_symbol });
        if (it == table.end ())
            return -1;
        return it->second;
    }
};

class Parser3 {

    public:
    explicit Parser3 (std::vector<Token> _tokens) : tokens (tokens) {}
    void parse ();

    private:
    std::vector<Token> tokens;
};


} // namespace simlang
