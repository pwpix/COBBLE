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
#include <vector>

namespace simlang {

// template <typename T>
// concept ValidAstNodeType = std::derived_from<T, ASTNode>;

// template <ValidAstNodeType T> std::unique_ptr<T> LogError (const char* str) {
//     std::println ("Error: {}", str);
//     return nullptr;
// }

// ----------helpers---------

// https://stackoverflow.com/a/4654718
bool is_numeric (std::string_view str)
{
    return !str.empty () &&
    std::find_if (str.begin (), str.end (), [] (unsigned char c) { return !std::isdigit (c); }) == str.end ();
}

static std::nullptr_t LogError (const char* str)
{
    std::println ("Error: {}", str);
    return nullptr;
}

static std::unique_ptr<IdentifierNode> parseIdentifierExpr (Parser& parser);

// string literal ::= "abc" | "abc123" | "abc123*"
static std::unique_ptr<LiteralNode> parseLiteralExpr (Parser& parser)
{

    auto token_type = parser.getTokenType (parser.getCurrentToken ());

    if (token_type != TokenType::STR) {
        return nullptr;
    }

    std::string_view s                  = parser.getTokenStr (parser.getCurrentToken ());
    std::unique_ptr<LiteralNode> result = std::make_unique<LiteralNode> (s);
    return result;
}

// number := 123 | 123.4

static std::unique_ptr<NumberNode> parseNumberExpr (Parser& parser)
{

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


// Field ::= id : str
//           id : num
//           id : array
//
static std::unique_ptr<FieldAssignmentNode> parseFieldAssignmentExpr (Parser& parser)
{
    auto token_id   = parser.getCurrentToken ();
    auto token_type = parser.getTokenType (token_id);

    std::unique_ptr<FieldAssignmentNode> result;
    std::unique_ptr<IdentifierNode> id;

    id = parseIdentifierExpr (parser);

    if (id == nullptr) {
        return nullptr;
    }

    token_id   = parser.getNextToken ();
    token_type = parser.getTokenType (token_id);
    if (token_type != TokenType::COL) {
        return nullptr;
    }

    token_id   = parser.advance ();
    token_type = parser.getTokenType (token_id);


    // id : str
    std::unique_ptr<LiteralNode> lit_expr = parseLiteralExpr (parser);

    if (lit_expr != nullptr) {
        result = std::make_unique<FieldAssignmentNode> (std::move (id), std::move (lit_expr));
    }

    else {
        // id : num
        std::unique_ptr<NumberNode> num_expr = parseNumberExpr (parser);
        if (num_expr != nullptr) {
            result = std::make_unique<FieldAssignmentNode> (std::move (id), std::move (num_expr));
        }
        else {
            return LogError ("Error: Expected a literal or number expression for identifier");
        }
    }


    return result;
}

// Object Literal ::= {
//                      field
//                      field
//                    }

static std::unique_ptr<ObjectNode> parseObjectExpr (Parser& parser)
{

    auto token_id   = parser.getCurrentToken ();
    auto token_type = parser.getTokenType (token_id);

    if (token_type != TokenType::LBRACE) {
        return LogError ("Error: Missing { in object declaration");
    }

    std::vector<std::unique_ptr<FieldAssignmentNode>> fields;

    while (true) {
        token_id   = parser.advance ();
        token_type = parser.getTokenType (token_id);

        if (token_type == TokenType::RBRACE) {
            break;
        }

        std::unique_ptr<FieldAssignmentNode> field = parseFieldAssignmentExpr (parser);

        if (field == nullptr) {
            return LogError ("Error: expected '}' or a field identifier");
        }
        fields.emplace_back (std::move (field));
    }


    token_type = parser.getTokenType (token_id);

    std::unique_ptr<ObjectNode> result = std::make_unique<ObjectNode> (std::move (fields));

    return result;
}

static std::unique_ptr<IdentifierNode> parseIdentifierExpr (Parser& parser)
{

    int token_id    = parser.getCurrentToken ();
    auto token_type = parser.getTokenType (token_id);
    auto token_line = parser.getTokenLine (token_id);

    std::unique_ptr<IdentifierNode> result;

    if (token_type != TokenType::ID) {
        std::string err = std::string ("Error: Line: ") + std::to_string (token_line) + std::string (" Expected Identifier");
        return LogError (err.c_str ());
    }

    auto str = parser.getTokenStr (token_id);
    result   = std::make_unique<IdentifierNode> (str);
    return result;
}


//
// decl ::= decl id name { body }
static std::unique_ptr<DeclarationNode> parseDeclExpr (Parser& parser)
{

    int token_id    = parser.getCurrentToken ();
    auto token_type = parser.getTokenType (token_id);

    std::unique_ptr<DeclarationNode> result;
    std::unique_ptr<LiteralNode> decl_name;
    std::unique_ptr<IdentifierNode> decl_id;
    std::unique_ptr<ASTNode> expr;


    if (token_type != TokenType::DECL) {
        return LogError ("Error: Expected declaration");
    }

    token_id = parser.advance ();

    // id
    decl_id = parseIdentifierExpr (parser);
    if (!decl_id) {
        return LogError ("Error: expected identifier after declaration");
    }

    token_id = parser.advance ();

    // name
    decl_name = parseLiteralExpr (parser);
    if (!decl_name) {
        return LogError ("Error: expected a declaration name");
    }

    std::string_view name_str = decl_name->value;


    auto it = SYMBOL_TABLE.find (std::string (decl_name->value));
    if (it != SYMBOL_TABLE.end ()) {
        std::string err = std::string ("Error: multiple definitions of ") + std::string (decl_name->value) + std::string (" of type: ") + std::string (decl_id->id);
        return LogError (err.c_str ());
    }

    SYMBOL_TABLE[std::string (decl_name->value)] = decl_id.get ();

    // expression/body
    token_id   = parser.advance ();
    token_type = parser.getTokenType (token_id);

    // type: '{' object literal
    if (token_type == TokenType::LBRACE) {

        std::unique_ptr<ObjectNode> obj_expr = parseObjectExpr (parser);
        if (obj_expr != nullptr) {
            result = std::make_unique<DeclarationNode> (std::move (decl_name), std::move (obj_expr));
        }

        else {
            return LogError ("Error: expected a declaration body");
        }
    }
    // type: primitive value
    else {
        std::unique_ptr<LiteralNode> lit_expr = parseLiteralExpr (parser);
        if (lit_expr != nullptr) {
            result = std::make_unique<DeclarationNode> (std::move (decl_name), std::move (lit_expr));
        }

        else {
            std::unique_ptr<NumberNode> num_expr = parseNumberExpr (parser);
            if (num_expr != nullptr) {
                result = std::make_unique<DeclarationNode> (std::move (decl_name), std::move (num_expr));
            }

            else {
                return LogError ("Error: expected a primitive expression or a declaration body");
            }
        }
    }

    // TODO: advance function
    // advance to next expression
    token_id = parser.advance ();

    return result;
}


AST parse (Parser& parser)
{

    AST ast;
    auto token_id   = parser.getNextToken ();
    auto token_type = parser.getTokenType (token_id);
    while (true) {
        token_id   = parser.getCurrentToken ();
        token_type = parser.getTokenType (token_id);
        // std::println ("{}", static_cast<int> (token_type));
        if (token_type == TokenType::Token_EOF) {
            std::println ("End of file");
            break;
        }
        auto node = parseDeclExpr (parser);
        if (!node)
            return ast;
        ast.addNode (std::move (node));
    }
    return ast;
}


} // namespace simlang
