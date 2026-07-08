#include "Parser.hpp"
#include "Ast.hpp"
#include "Lexer2.hpp"
#include "Token2.hpp"
#include <charconv>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <unordered_map>
#include <utility>


namespace simlang {

// template <typename T>
// concept ValidAstNodeType = std::derived_from<T, ASTNode>;


// template <ValidAstNodeType T> std::unique_ptr<T> LogError (const char* str) {
//     std::println ("Error: {}", str);
//     return nullptr;
// }

// ----------helpers---------

// https://stackoverflow.com/a/4654718
bool is_numeric (std::string_view str) {
    return !str.empty () &&
    std::find_if (str.begin (), str.end (),
    [] (unsigned char c) { return !std::isdigit (c); }) == str.end ();
}


static std::nullptr_t LogError (const char* str) {
    std::println ("Error: {}", str);
    return nullptr;
}


static std::unique_ptr<IdentifierNode> parseIdentifierExpr (Parser& parser);

// string literal ::= "abc" | "abc123" | "abc123*"
static std::unique_ptr<LiteralNode> parseLiteralExpr (Parser& parser) {

    auto token_type = parser.getTokenType (parser.getCurrentToken ());

    if (token_type != TokenType::STR) {
        return nullptr;
    }

    std::string_view s                  = parser.getTokenStr (parser.getCurrentToken ());
    std::unique_ptr<LiteralNode> result = std::make_unique<LiteralNode> (s);
    return result;
}

// number := 123 | 123.4

static std::unique_ptr<NumberNode> parseNumberExpr (Parser& parser) {


    int token_id       = parser.getCurrentToken ();
    std::string_view s = parser.getTokenStr (token_id);

    std::unique_ptr<NumberNode> result;

    int i{};
    float f{};

    auto [ptr_int, ec_int] = std::from_chars (s.data (), s.data () + s.size (), i);

    if (ec_int == std::errc () && ptr_int == s.data () + s.size ()) {
        result = std::make_unique<NumberNode> (i);
    }

    else {
        auto [ptr_float, ec_float] =
        std::from_chars (s.data (), s.data () + s.size (), f);

        if (ec_float == std::errc () && ptr_float == s.data () + s.size ()) {
            result = std::make_unique<NumberNode> (f);
        }

        else {
            return nullptr;
        }
    }


    return result;
}

static std::unique_ptr<FieldAssignmentNode> parseFieldAssignmentExpr (Parser& parser) {
    auto token_id   = parser.getCurrentToken ();
    auto token_type = parser.getTokenType (token_id);

    std::unique_ptr<FieldAssignmentNode> result;
    std::unique_ptr<IdentifierNode> id;


    if (token_type != TokenType::ID) {
        return nullptr;
    }
    token_id = parser.getNextToken ();
    id       = parseIdentifierExpr (parser);

    if (id == nullptr) {
        return nullptr;
    }

    token_id   = parser.getNextToken ();
    token_type = parser.getTokenType (token_id);
    if (token_type != TokenType::COL) {
        return nullptr;
    }

    token_id   = parser.getNextToken ();
    token_type = parser.getTokenType (token_id);

    std::unique_ptr<ASTNode> s = parseLiteralExpr (parser);

    if (s == nullptr) {
        s = parseNumberExpr (parser);
    }

    if (s == nullptr) {
        std::println ("No value for identifier");
        return nullptr;
    }

    if (auto* num_ptr = dynamic_cast<NumberNode*> (s.get ())) {
        std::unique_ptr<NumberNode> num_node (static_cast<NumberNode*> (s.release ()));
        result = std::make_unique<FieldAssignmentNode> (std::move (id), std::move (num_node));
    }

    else if (auto* lit_ptr = dynamic_cast<NumberNode*> (s.get ())) {
        std::unique_ptr<LiteralNode> lit_node (static_cast<LiteralNode*> (s.release ()));
        result = std::make_unique<FieldAssignmentNode> (std::move (id), std::move (lit_node));
    } else {
        // TODO: LogError
        std::println ("Error: Identifier expected");
        return nullptr;
    }

    return result;
}

static std::unique_ptr<ObjectNode> parseObjectExpr (Parser& parser) {

    auto token_id   = parser.getCurrentToken ();
    auto token_type = parser.getTokenType (token_id);

    if (token_type != TokenType::LBRACE) {
        return nullptr;
    }

    token_id = parser.getNextToken ();
    return nullptr;
}

static std::unique_ptr<IdentifierNode> parseIdentifierExpr (Parser& parser) {


    int token_id    = parser.getCurrentToken ();
    auto token_type = parser.getTokenType (token_id);

    std::unique_ptr<IdentifierNode> result;

    if (token_type != TokenType::ID) {
        return nullptr;
    }

    token_id = parser.getNextToken ();

    std::unique_ptr<LiteralNode> node_name = parseLiteralExpr (parser);


    if (node_name == nullptr) {
        std::println ("Error: missing identifier name");
    } else {
    }
    return nullptr;
}


} // namespace simlang
