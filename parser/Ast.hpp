#pragma once

#include <memory>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

/*
class Node {
    NodeType type;
    std::string_view value;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    std::unique_ptr<Node> children;
};
*/

class AST;

class ASTNode {
    public:
    int token_id;
    virtual ~ASTNode () = default;
};


class NumberNode : public ASTNode {

    public:
    using NumVal = std::variant<int, float>;

    NumberNode () = default;
    explicit NumberNode (const NumVal& v) : value (v) {}


    public:
    NumVal value;
};

class LiteralNode : public ASTNode {
    public:
    LiteralNode () = default;
    explicit LiteralNode (std::string_view s) : value (s) {}
    std::string_view value;
};

class ArrayNode : public ASTNode {
    public:
    std::vector<std::unique_ptr<ASTNode>> value;
};


// std::unique_ptr should be pass-by-value otherwise you won't be able to move
// remove destructor, it silently removes move operations

class IdentifierNode : public ASTNode {
    public:
    IdentifierNode () = default;
    explicit IdentifierNode (std::string_view id) : id (id) {}
    std::string_view id;
};

class NegativeNode : public ASTNode {
    public:
    NegativeNode () = default;
    explicit NegativeNode (std::unique_ptr<NumberNode> val)
    : value (std::move (val)) {}
    std::unique_ptr<NumberNode> value;
};


class FieldAssignmentNode : public ASTNode {

    public:
    using FieldVal =
    std::variant<std::unique_ptr<NumberNode>, std::unique_ptr<LiteralNode>, std::unique_ptr<ArrayNode>, std::unique_ptr<NegativeNode>>;

    FieldAssignmentNode () = default;
    explicit FieldAssignmentNode (std::unique_ptr<IdentifierNode> id, FieldVal val)
    : field_id (std::move (id)), field_value (std::move (val)) {}

    std::unique_ptr<IdentifierNode> field_id;
    FieldVal field_value;
};


class ObjectNode : public ASTNode {
    public:
    // can be by value to maintain consistency with copyable types
    explicit ObjectNode (std::vector<std::unique_ptr<FieldAssignmentNode>> fields)
    : fields (std::move (fields)) {}
    ObjectNode () = default;

    public:
    std::vector<std::unique_ptr<FieldAssignmentNode>> fields;
};

class DeclarationNode : public ASTNode {

    public:
    using DeclValue =
    std::variant<std::monostate, std::unique_ptr<ObjectNode>, std::unique_ptr<NumberNode>, std::unique_ptr<LiteralNode>>;

    DeclarationNode () : decl_name (nullptr) {}
    explicit DeclarationNode (std::unique_ptr<LiteralNode> id, DeclValue val)
    : decl_name (std::move (id)), value (std::move (val)) {}


    public:
    std::unique_ptr<LiteralNode> decl_name;
    DeclValue value;
};


class AST {

    public:
    AST () {};

    void addNode (std::unique_ptr<DeclarationNode> node)
    {
        nodes.emplace_back (std::move (node));
    }


    const auto& getnodes () const noexcept { return nodes; }

    private:
    std::vector<std::unique_ptr<DeclarationNode>> nodes;
};


static std::unique_ptr<DeclarationNode> createDeclarationNode ()
{
    return std::make_unique<DeclarationNode> ();
}

static std::unique_ptr<FieldAssignmentNode> createFieldAssignmentNode ()
{
    return std::make_unique<FieldAssignmentNode> ();
}

static std::unique_ptr<ObjectNode> createObjectNode ()
{
    return std::make_unique<ObjectNode> ();
}

static std::unique_ptr<IdentifierNode> createIdentifierNode ()
{
    return std::make_unique<IdentifierNode> ();
}


void display_ast (const std::vector<std::unique_ptr<DeclarationNode>>& nodes);
