#pragma once

/*
 * New Iteration of the Lexer
 *
 * Updated grammer rules to make them more suitable
 * for configuration scripts.
 *
 */


#include <string>
#include <utility>
#include <vector>

#include "Token2.hpp"
#include "memorymap.hpp"


namespace simlang {

class myLexer2 {
   public:
    myLexer2() : line(0), col(0), length(0) {}


    int LoadFile(const std::string& filename);

    void scan(std::string);
    int ReadToken();

    void Error(const std::string& str);

   private:
    std::string filename;
    std::vector<char> buffer;

    std::string::iterator curr_file_pointer;
    std::string::iterator end_file_pointer;

    int length;
    int line;
    int col;

    std::vector<Token> tokens;

    // bool allocated;
};

}  // namespace simlang
