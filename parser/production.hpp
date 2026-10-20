#pragma once

#include "symbol.hpp"
#include <cstdint>
enum ProductionRules : uint16_t {

    rule_S_id,      // S ::= E ENDOF
    rule_E_id,      // E ::= F = T
    rule_T__num,    // T ::= num
    rule_T__str,    // T ::= str
    rule_T__lbrace, // T ::= O
    rule_T__brack,  // T ::= [ LB LE ]
    rule_F__id,     // F ::= id

    rule_O__lbrace,  // O ::= { MB ME }
    rule_MB__id,     // M ::= E
    rule_ME__rbrace, // MM ::= empty
    rule_ME__comma,  // MM ::= comma E MM

    rule_LB__num,    // LB ::= num
    rule_LE__comma,  // LE ::= , num LE
    rule_LE__rbrack, // LE ::= empty

    num_production_rules,


};


struct ProductionRule {
    int rule;
    Symbol lhs;
    std::vector<Symbol> rhs;
};
