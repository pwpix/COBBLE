#pragma once

/*
 * New Iteration of the Lexer
 *
 * Updated grammer rules to make them more suitable
 * for configuration scripts.
 *
 */


#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Token2.hpp"
#include "windows/memorymap.hpp"


namespace simlang {

class MyLexer2 {
   public:
    MyLexer2() : m_line(1), m_col(0), m_length(0) {}


    int loadFile(const std::string& filename);

    int loadFile(MappedFile::Iterator start, MappedFile::Iterator end);

    void scan();

    int tokenize(std::string_view lexeme);

    // int readToken();

    void error(const std::string& str);


    int checkIfString(std::string_view lexeme);
    int checkIfPunctuation(std::string_view lexeme, Token& newToken);
    int checkIfDeclaration(std::string_view lexeme);
    int checkIfNumber(std::string_view lexeme, Token& newToken);
    int checkIfSymbol(std::string_view lexeme, Token& newToken);


   private:
    std::string m_filename;
    std::vector<char> buffer;

    void column() { m_col++; }

    void newline() { m_line++; }

    // std::string::iterator curr_file_pointer;
    // std::string::iterator end_file_pointer;

    int m_length;
    int m_line;
    int m_col;

    bool m_mappedFile;
    MappedFile::Iterator _it;
    MappedFile::Iterator _end;

    std::vector<Token> tokens;

    // bool allocated;
};

}  // namespace simlang
