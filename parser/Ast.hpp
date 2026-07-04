#pragma once

#include <memory>
#include <string_view>
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




class ASTNode {
   public:
    virtual ~ASTNode();
};



class ValuePairNode : public ASTNode {
   public:
    ~ValuePairNode() {}
};

class NumberNode : ASTNode {
   public:
    std::variant<int, float, double, long> value;
    ~NumberNode() {}
};

class LiteralNode : public ASTNode {
   public:
    std::string_view value;
    ~LiteralNode() {}
};

class ArrayNode : public ASTNode {
   public:
    std::vector<std::variant<std::string_view, int, float, double, long>> value;
    ~ArrayNode() {}
};

class IdentifierNode : public ASTNode {
   public:
    std::string_view value;
    ~IdentifierNode() {}
};


class FieldAssignmentNode : public ASTNode {
   public:
    std::unique_ptr<IdentifierNode> field_name;
    std::variant<std::unique_ptr<NumberNode>, std::unique_ptr<LiteralNode>,
                 std::unique_ptr<ArrayNode>>
        value;

    ~FieldAssignmentNode() {}
};


class NegativeNode : public ASTNode {
   public:
    std::unique_ptr<NumberNode> value;
    ~NegativeNode() {}
};


class ObjectNode : public ASTNode {
   public:
    std::vector<FieldAssignmentNode> fields;
    ~ObjectNode() {}
};

class DeclarationNode : public ASTNode {
   public:
    std::unique_ptr<IdentifierNode> decl_name;
    std::variant<std::unique_ptr<ObjectNode>, std::unique_ptr<NumberNode>,
                 std::unique_ptr<LiteralNode>>
        value;
    ~DeclarationNode() {}
};
