
#include <concepts>
#include <cstddef>
#include <cstring>
#include <memory>
#include <print>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include "Lexer2.hpp"
#include "Token2.hpp"
#include "parser/Lexer2.hpp"
#include "parser/Parser.hpp"
#include "parser/windows/memorymap.hpp"
#include "windows/memorymap.hpp"


struct A {
    int a;
};

int main (int argc, char* argv[])
{
    using namespace simlang;
    MappedFile file ("test.txt");

    MappedFile::Iterator it  = file.Begin ();
    MappedFile::Iterator end = file.End ();

    simlang::MyLexer2 lexer;
    lexer.loadFile (it, end);
    lexer.scan ();

    std::vector<Token> tokens = lexer.getTokens ();
    int debug                 = 0;

    simlang::Parser parser (tokens);

    AST ast = parse (parser);

    display_ast (ast.getnodes ());


    std::println ("Ok");

    return 0;
}
