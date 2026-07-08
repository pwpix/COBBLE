#include "Lexer2.hpp"

#include <_mingw_mac.h>

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <print>
#include <string>
#include <string_view>

#include "Token2.hpp"
#include "windows/memorymap.hpp"


namespace simlang {

#define NEWLINE '\r\n'

int MyLexer2::loadFile (MappedFile::Iterator it, MappedFile::Iterator end) {
    // null check
    if (!it)
        return 0;

    _it          = it;
    _end         = end;
    m_mappedFile = true;

    return 1;
}

void MyLexer2::scan () {
    uint8_t space_count{ 0 };
    MappedFile::Iterator start = _it;
    int res{ -1 };
    while (_it != _end) {
        unsigned char ch = static_cast<unsigned char> (*_it);


        if (ch == '\n') {
            this->newline ();
            m_col    = 0;
            m_length = 0;


        } else if (ch == '\r') {
            if (m_length > 0) {
                res = tokenize (std::string_view (start, start + m_length));
            }
            m_length = 0;

        }
        // -1234 : MINUS NUM
        else if (std::isdigit (ch) && m_length == 1) {
            if (static_cast<unsigned char> (*start) == '-') {
                res      = tokenize (std::string_view (start, start + 1));
                m_length = 0;
                start    = _it;
            }
        }

        else if (std::isspace (ch)) {
            space_count++;

            if (space_count == 4) {
                // add token indent
            }
            if (m_length > 0) {
                res = tokenize (std::string_view (start, start + m_length));
            }
            m_length = 0;

        } else if (ch == '\0') {
            // End of file
            break;
        } else {
            if (m_length == 0) {
                start = _it;
            }
            m_length++;
        }

        if (res == 0) {
            break;
        }
        this->column ();
        ++_it;
    }
    if (m_length > 0) {
        res = tokenize (std::string_view (start, start + m_length));
    }

    // token EOF
    tokens.emplace_back (
    Token{ .type = TokenType::Token_EOF, .str = std::string_view{ "\0" } });
}

int MyLexer2::tokenize (std::string_view lexeme) {
    Token newToken;

    std::println ("{}", lexeme);

    int res{ 0 };

    if (lexeme.starts_with ('"') && lexeme.ends_with ('"')) {
        newToken.type = TokenType::STR;
        res           = 1;
    } else if (lexeme == "--!") {
        newToken.type = TokenType::DECL;
        res           = 1;
    } else {
        res = checkIfPunctuation (lexeme, newToken);
    }


    if (!res) {
        res = checkIfNumber (lexeme, newToken);
    }

    if (!res) {
        // TODO:is symbol

        // if (std::isalpha(static_cast<unsigned char>(lexeme.at(0))) &&
        //     lexeme.size() < 50) {
        //     newToken.type = TokenType::ID;

        res = checkIfSymbol (lexeme, newToken);

        if (!res) {
            error ("Invalid symbol at line:" + std::to_string (m_line) +
            " col: " + std::to_string (m_col) + " " + std::string (lexeme));
            return 0;
        }
    }


    newToken.str = lexeme;
    tokens.emplace_back (newToken);
    return 1;
}

int MyLexer2::checkIfPunctuation (std::string_view lexeme, Token& newToken) {
    if (lexeme == "=") {
        newToken.type = TokenType::ASSIGN;
        return 1;
    }

    else if (lexeme == ",") {
        newToken.type = TokenType::COMMA;
        return 1;
    }

    else if (lexeme == ":") {
        newToken.type = TokenType::COL;
        return 1;
    }

    else if (lexeme == ";") {
        newToken.type = TokenType::SEMI;
        return 1;
    }

    else if (lexeme == "{") {
        newToken.type = TokenType::LBRACE;
        return 1;
    }

    else if (lexeme == "}") {
        newToken.type = TokenType::RBRACE;
        return 1;
    }

    else if (lexeme == "-") {
        newToken.type = TokenType::MINUS;
        return 1;
    }
    return 0;
}

int MyLexer2::checkIfNumber (std::string_view lexeme, Token& newToken) {
    bool found_char;
    for (auto ch : lexeme) {
        if (!std::isdigit (static_cast<unsigned char> (ch))) {
            found_char = true;
        }
    }
    if (!found_char) {
        newToken.type = TokenType::NUM;
        return 1;
    }
    return 0;
}

int MyLexer2::checkIfSymbol (std::string_view lexeme, Token& newToken) {
    bool found_char{ false };
    if (!std::isalpha (static_cast<unsigned char> (lexeme.at (0))) || lexeme.length () > 50) {
        return 0;
    }
    for (auto ch : lexeme) {
        if (!std::isalnum (static_cast<unsigned char> (ch)) && ch != '_') {
            found_char = true;
            break;
        }
    }
    if (!found_char) {
        newToken.type = TokenType::ID;
        return 1;
    }
    return 0;
}

void MyLexer2::error (const std::string& str) { std::println ("{}", str); }

} // namespace simlang
