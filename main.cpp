#include "lexer.hpp"
#include "parser/lexer.hpp"
#include "parser/memorymap.hpp"
#include <iostream>
#include <print>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

int main (int argc, char* argv[])
{

    MappedFile f ("f.txt");
    MappedFile::Iterator i = f.Begin ();
    MappedFile::Iterator e = f.End ();

    Lexer l;

    l.loadFile (i, e);

    l.scan ();
    std::vector<Token> tokens = l.get_tokens ();

    for (auto t : tokens) {
        std::cout << t.lexeme << '\n';
    }
    return 0;
}
