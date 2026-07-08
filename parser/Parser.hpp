#pragma once


#include <cstddef>
#include <memory>
#include <vector>

#include "Ast.hpp"
#include "Token2.hpp"

namespace simlang {

static std::nullptr_t LogError (const char* str);

class Parser {

    public:
    Parser (std::vector<Token> tokens) : tokens (std::move (tokens)) {}

    TokenType getTokenType (int id) {

        return id < tokens.size () ? tokens.at (id).type : TokenType::Token_NONE;
    }

    size_t getCurrentToken () const noexcept { return currentToken; }

    size_t getNextToken () {
        ++currentToken;
        return currentToken;
    }

    auto getTokenStr (int id) { return tokens.at (id).str; }

    private:
    DeclarationNode* parseDeclaration (size_t index);
    int currentToken{ -1 };

    std::vector<Token> tokens;
};


} // namespace simlang
