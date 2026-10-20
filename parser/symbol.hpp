#pragma once

#include <cstdint>

struct Symbol {
    uint16_t id;
    bool is_terminal;

    bool operator< (const Symbol& other) const
    {
        return std::tie (id, is_terminal) < std::tie (other.id, other.is_terminal);
    }

    bool operator== (const Symbol& other) const
    {
        return id == other.id && other.is_terminal;
    }
};
