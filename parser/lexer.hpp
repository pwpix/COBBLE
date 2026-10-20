#pragma once

#include "memorymap.hpp"
#include "token.hpp"
#include <string_view>
#include <vector>

#ifdef _WIN32
#define NEWLINE '\r\n'

#endif

class Lexer {

    public:
    // load a mapped file
    int loadFile (MappedFile::Iterator start, MappedFile::Iterator end);
    void error (const std::string& msg);

    void scan ();
    int tokenize (std::string_view lexeme);

    int checkIfNumber (std::string_view lexeme, Token& new_token);
    int checkIfString (std::string_view lexeme, Token& new_token);
    int checkIfPunctuation (std::string_view lexeme, Token& new_token);
    int checkIfIdentifier (std::string_view lexeme, Token& new_token);
    int checkIfSymbol (std::string_view lexeme, Token& new_token);

    std::vector<Token> get_tokens ()
    {
        return m_tokens;
    }

    private:
    std::string m_filename;

    int m_col    = 0;
    int m_line   = 0;
    int m_length = 0;

    MappedFile ::Iterator _it;
    MappedFile ::Iterator _end;
    bool m_mappedFile = false;

    std::vector<Token> m_tokens;
};
