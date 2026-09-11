#include "table_driven/parser3.hpp"

#include "table_driven/stack.hpp"
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace simlang {

static std::string getKeywordString (uint16_t id_nt)
{
#define NT(id, str)  \
    if (id == id_nt) \
        return std::string (str);
    NON_TERMINALS
#undef NT
    else return std::string ("nt_id_not_valid");
}

static bool is_non_terminal (int id)
{
    return
#define NT(val, str) (static_cast<uint16_t> (val) == static_cast<uint16_t> (id)) ||
    NON_TERMINALS
#undef NT
    false;
}

static Symbol get_non_terminal (int val)
{
    if (val == S)
        return Symbol{ S, false };
    else if (val == E)
        return Symbol (E, false);
    else if (val == F)
        return Symbol (F, false);
    else if (val == T)
        return Symbol (T, false);
    else if (val == O)
        return Symbol (O, false);
    else if (val == MB)
        return Symbol (MB, false);
    else if (val == ME)
        return Symbol (ME, false);
    else if (val == LB)
        return Symbol (LB, false);
    else if (val == LE)
        return Symbol (LE, false);
    else
        std::runtime_error ("No symbol found");
}

static Symbol get_terminal (int val)
{

    if (val == ID)
        return Symbol{ ID, true };
    else if (val == NUM)
        return Symbol{ NUM, true };
    else if (val == STR)
        return Symbol{ STR, true };
    else if (val == EQUAL)
        return Symbol{ EQUAL, true };
    else if (val == COMMA)
        return Symbol{ COMMA, true };
    else if (val == LBRACE)
        return Symbol{ LBRACE, true };
    else if (val == RBRACE)
        return Symbol{ RBRACE, true };
    else if (val == LBRACK)
        return Symbol{ LBRACK, true };
    else if (val == RBRACK)
        return Symbol{ RBRACK, true };
    else if (val == LPAREN)
        return Symbol{ LPAREN, true };
    else if (val == RPAREN)
        return Symbol{ RPAREN, true };
    else if (val == EMPTY)
        return Symbol{ EMPTY, true };
    else if (val == ENDOF)
        return Symbol{ ENDOF, true };


    else
        throw std::runtime_error ("No symbol found");
}

static ProductionRule get_production (int rule)
{

    if (rule == rule_S__id) {
        return ProductionRule{
            rule_S__id,
            get_non_terminal (S),
            { get_non_terminal (E),
            get_terminal (ENDOF) }
        };
    }

    else if (rule == rule_E__id) {
        return ProductionRule{
            rule_E__id,
            get_non_terminal (E),
            { get_non_terminal (F),
            get_terminal (EQUAL),
            get_non_terminal (T) }
        };
    }

    else if (rule == rule_T__num) {
        return ProductionRule{
            rule_T__num,
            get_non_terminal (T),
            { get_terminal (NUM) }
        };
    }

    else if (rule == rule_T__str) {
        return ProductionRule{
            rule_T__str,
            get_non_terminal (T),
            { get_terminal (STR) }
        };
    }

    else if (rule == rule_T__lbrace) {
        return ProductionRule{
            rule_T__lbrace,
            get_non_terminal (T),
            { get_non_terminal (O) }
        };
    }

    else if (rule == rule_T__brack) {
        return ProductionRule{
            rule_T__brack,
            get_non_terminal (T),
            { get_terminal (LBRACK),
            get_terminal (LB),
            get_terminal (LE),
            get_terminal (RBRACK) }
        };
    }

    else if (rule == rule_F__id) {
        return ProductionRule{
            rule_F__id,
            get_non_terminal (F),
            { get_terminal (ID) }
        };
    }

    else if (rule == rule_O__lbrace) {
        return ProductionRule{
            rule_O__lbrace,
            get_non_terminal (O),
            { get_terminal (LPAREN),
            get_terminal (MB),
            get_terminal (ME),
            get_terminal (RPAREN) }
        };
    }


    else if (rule == rule_MB__id) {
        return ProductionRule{
            rule_MB__id,
            get_terminal (MB),
            { get_non_terminal (E) }
        };
    }


    else if (rule == rule_ME_rbrace) {
        return ProductionRule{
            rule_ME_rbrace,
            get_terminal (ME),
            { get_terminal (EMPTY) }
        };
    }

    else if (rule == rule_ME_comma) {
        return ProductionRule{
            rule_ME_rbrace,
            get_terminal (ME),
            { get_terminal (COMMA),
            get_non_terminal (E),
            get_terminal (ME) }
        };
    }

    else if (rule == rule_LB__num) {
        return ProductionRule{
            rule_LB__num,
            get_terminal (LB),
            { get_terminal (NUM) }
        };
    }

    else if (rule == rule_LE__comma) {
        return ProductionRule{
            rule_LE__comma,
            get_terminal (LE),
            { get_terminal (COMMA),
            get_terminal (NUM),
            get_terminal (LE) }
        };
    }

    else if (rule == rule_LE__rbrack) {
        return ProductionRule{
            rule_LE__rbrack,
            get_terminal (LE),
            { get_terminal (EMPTY) }
        };
    }

    else {
        return ProductionRule{ -1 };
    }
}

static LL1ParseTable init_parse_table ()
{
    LL1ParseTable table;

    table.set (get_non_terminal (S), get_terminal (ID), rule_S__id);
    table.set (get_non_terminal (E), get_terminal (ID), rule_E__id);
    table.set (get_non_terminal (T), get_terminal (NUM), rule_T__num);
    table.set (get_non_terminal (T), get_terminal (STR), rule_T__str);
    table.set (get_non_terminal (F), get_terminal (ID), rule_F__id);

    table.set (get_non_terminal (O), get_terminal (LBRACE), rule_O__lbrace);
    table.set (get_non_terminal (MB), get_terminal (ID), rule_MB__id);
    table.set (get_non_terminal (ME), get_terminal (RBRACE), rule_ME_rbrace);
    table.set (get_non_terminal (ME), get_terminal (COMMA), rule_ME_comma);

    table.set (get_non_terminal (LB), get_terminal (NUM), rule_LB__num);
    table.set (get_non_terminal (LE), get_terminal (COMMA), rule_LE__comma);
    table.set (get_non_terminal (LE), get_terminal (RBRACK), rule_LE__rbrack);

    return table;
}

void driver (std::vector<Token> tokens)
{

    LL1ParseTable table = init_parse_table ();

    Stack<Symbol> stack;
    int dummy_next_input_symbol = 1;

    uint16_t next_input_symbol = dummy_next_input_symbol;

    // TODO: replace vector with token stream from lexer!
    std::vector<int> dummy_token_buffer;

    stack.push ({ S, false });

    // Driver Procedure
    while (!stack.empty ()) {

        auto top = stack.top ();

        if (!top.is_terminal) {
            auto action = table.get_production (top, Symbol{ next_input_symbol, true });
            //  assert (action >= 0 && "Not a valid production");
            if (action >= 0) {

                // pop top symbol
                stack.pop ();

                // push RHS of production. We need to start from the symbol at the right.
                // Assuming the symbols are arranged in RTL
                auto production = get_production (action);
                for (const auto& p : production.rhs)
                    stack.push (p);
            }
        }

        else if (next_input_symbol == top.id) {
            // match terminal symbol in input
            stack.pop ();

            // get next terminal symbol in input
            next_input_symbol = dummy_token_buffer[0];
            if (next_input_symbol < 0) {
                break;
            }
        }

        else {
            std::println ("ERROR::PARSER::SYNTAX ERROR");
            break;
        }
    }
}

void Parser3::parse ()
{
    driver (tokens);
}

} // namespace simlang
