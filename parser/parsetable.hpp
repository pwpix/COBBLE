#pragma once


#include "symbol.hpp"
#include <cstdint>
#include <map>

struct LL1ParseTable {
    std::map<std::pair<Symbol, Symbol>, uint32_t> table; // use uint instead of Symbol?

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
