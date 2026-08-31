#include "table_driven/parser3.hpp"

#include "table_driven/stack.hpp"
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


void driver ()
{
    Stack<int> stack;
    int dummy_next_input_symbol = 1;

    int next_input_symbol = dummy_next_input_symbol;

    // TODO: replace vector with token stream from lexer!
    std::vector<int> dummy_token_buffer;

    // Driver Procedure
    while (!stack.empty ()) {

        auto top = stack.top ();

        if (is_non_terminal (top)) {
            auto action = parse_table (top, next_input_symbol);
            // assert (action >= 0 && "Not a valid production");
            if (action >= 0) {

                // pop top symbol
                stack.pop ();

                // push RHS of production. We need to start from the symbol at the right.
                // Assuming the symbols are arranged in RTL
                auto production = ProductionTable[action];
                for (const auto& p : production)
                    stack.push (p);
            }
        }

        else if (next_input_symbol == top) {
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

} // namespace simlang
