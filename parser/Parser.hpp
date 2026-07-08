#pragma once


#include <cstddef>
#include <unordered_map>
#include <vector>

#include "Ast.hpp"
#include "Token2.hpp"

namespace simlang {

static std::nullptr_t LogError (const char* str);

inline static std::unordered_map<std::string, IdentifierNode*> SYMBOL_TABLE;

class Parser {

    public:
    explicit Parser (std::vector<Token> tokens_) : tokens (std::move (tokens_)) {}

    TokenType getTokenType (int id)
    {

        return id >= 0 && id < tokens.size () ? tokens.at (id).type : TokenType::Token_NONE;
    }

    size_t getCurrentToken () const noexcept { return currentToken; }

    size_t getNextToken ()
    {
        ++currentToken;
        return currentToken;
    }

    auto getTokenStr (int id) { return tokens.at (id).str; }
    uint32_t getTokenLine (int id) { return tokens.at (id).line; }

    private:
    DeclarationNode* parseDeclaration (size_t index);
    int currentToken{ -1 };

    std::vector<Token> tokens;
};


AST parse (Parser& parser);


} // namespace simlang
