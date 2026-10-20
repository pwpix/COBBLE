#include "lexer.hpp"

#include "memorymap.hpp"
#include "token.hpp"
#include <cctype>
#include <cstdint>
#include <print>
#include <string>
#include <string_view>


int Lexer::loadFile (MappedFile::Iterator it, MappedFile::Iterator end)
{
    // null check
    if (!it)
        return 0;
    _it          = it;
    _end         = end;
    m_mappedFile = true;
    return 1;
}

// Convert mapped file char to unsigned char
static unsigned char iter_to_uchar (MappedFile::Iterator it)
{
    return static_cast<unsigned char> (*it);
}

void Lexer::scan ()
{
    uint8_t space_count = 0;

    MappedFile::Iterator start = _it;
    int r                      = -1;

    while (_it != _end) {

        unsigned char ch = iter_to_uchar (_it);

        if (ch == '\n') {
            m_line++;
            m_col    = 0;
            m_length = 0;
        }
        else if (ch == '\r') {
            if (m_length > 0) {
                r = tokenize (std::string_view (start, start + m_length));
            }
            m_length = 0;
        }
        // negative integers
        else if (std::isdigit (ch) && m_length == 1) {
            if (iter_to_uchar (start) == '-') {
                r     = tokenize (std::string_view (start, start + 1));
                start = _it;
            }
            else {
                m_length++;
            }
        }

        else if (ch == ',') {
            // tokenize the previous item
            if (m_length > 0) {
                r = tokenize (std::string_view (start, start + m_length));
            }
            // tokemize the comma
            r        = tokenize (std::string_view (_it, _it + 1));
            m_length = 0;
        }
        else if (std::isspace (ch)) {
            space_count++;

            // tokenize it when its followed by a space
            if (m_length > 0) {
                r = tokenize (std::string_view (start, start + m_length));
            }
            m_length = 0;
        }

        else if (ch == '\0') {
            break; // end of file
        }
        else {
            // point to next token
            if (m_length == 0) {
                start = _it;
            }
            m_length++;
        }
        if (r == 0) {
            break;
        }
        m_col++;
        ++_it;
    }

    if (m_length > 0) {
        r = tokenize (std::string_view (start, start + m_length));
    }

    m_tokens.emplace_back (
    Token{ .token_type = Terminals::ENDOF, .lexeme = std::string_view{}, .line = static_cast<uint32_t> (m_line) });
}


int Lexer::tokenize (std::string_view lexeme)
{
    Token new_token{ .lexeme = lexeme, .line = static_cast<uint32_t> (m_line) };

    int r = 0;

    if (checkIfString (lexeme, new_token)) {
        r = 1;
    }
    else if (checkIfNumber (lexeme, new_token)) {
        r = 1;
    }
    else if (checkIfPunctuation (lexeme, new_token)) {
        r = 1;
    }
    else if (checkIfSymbol (lexeme, new_token)) {
        r = 1;
    }

    if (r == 0) {
        error ("Invalid symbol at line:" + std::to_string (m_line) +
        " col: " + std::to_string (m_col) + " " + std::string (lexeme));
        return 0;
    }

    m_tokens.emplace_back (new_token);

    return 1;
}

int Lexer::checkIfString (std::string_view lexeme, Token& new_token)
{
    if (lexeme.starts_with ('"') && lexeme.ends_with ('"')) {
        new_token.token_type = Terminals::STR;
        return 1;
    }
    return 0;
}

int Lexer::checkIfPunctuation (std::string_view lexeme, Token& new_token)
{
    if (lexeme == "=") {
        new_token.token_type = Terminals::EQUAL;
        return 1;
    }

    else if (lexeme == ",") {
        new_token.token_type = Terminals::COMMA;
        return 1;
    }

    else if (lexeme == ":") {
        new_token.token_type = Terminals::COL;
        return 1;
    }

    else if (lexeme == ";") {
        new_token.token_type = Terminals::SEMI;
        return 1;
    }

    else if (lexeme == "{") {
        new_token.token_type = Terminals::LBRACE;
        return 1;
    }

    else if (lexeme == "}") {
        new_token.token_type = Terminals::RBRACE;
        return 1;
    }

    else if (lexeme == "-") {
        new_token.token_type = Terminals::MINUS;
        return 1;
    }
    return 0;
}


int Lexer::checkIfNumber (std::string_view lexeme, Token& new_token)
{
    uint8_t found_char = 0xFF;

    for (auto ch : lexeme) {
        if (!std::isdigit (static_cast<unsigned char> (ch)) || ch == '.')
            found_char = ~found_char;
        break;
    }

    if (found_char & 0xFF) {
        new_token.token_type = Terminals::NUM;
        return 1;
    }
    return 0;
}

int Lexer::checkIfSymbol (std::string_view lexeme, Token& new_token)
{
    bool found_char = false;

    if (!std::isalpha (static_cast<unsigned char> (lexeme.at (0))) || lexeme.length () > 50) {
        return 0;
    }
    for (auto ch : lexeme) {
        if (std::isalnum (static_cast<unsigned char> (ch)) || ch == '_') {
            found_char = true;
            break;
        }
    }
    if (found_char) {
        new_token.token_type = Terminals::ID;
        return 1;
    }
    return 0;
}


void Lexer::error (const std::string& str)
{
    std::println ("{}", str);
}
