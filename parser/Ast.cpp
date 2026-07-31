
#include "Ast.hpp"
#include <memory>
#include <print>
#include <variant>


void display_ast (const std::vector<std::unique_ptr<DeclarationNode>>& nodes)
{
    for (auto& node : nodes) {
        std::println ("Root:");
        std::println ("{}", node->decl_name->value);

        std::visit ([&node] (auto&& arg) {
            using T = std::decay_t<decltype (arg)>;
            if constexpr (std::is_same_v<T, std::unique_ptr<NumberNode>>) {
                std::print ("  Number node ");
                std::visit ([&node] (auto&& arg2) {
                    std::println ("{}", arg2);
                },
                arg->value);
            }
            else if constexpr (std::is_same_v<T, std::unique_ptr<LiteralNode>>) {
                std::println ("   literal {}", arg->value);
            }
            else if constexpr (std::is_same_v<T, std::unique_ptr<ObjectNode>>) {
                std::println ("  object \n     fields");
                for (auto& field : arg->fields) {
                    std::print ("       name {} value ", field->field_id->id);
                    auto& fv = field->field_value;
                    std::visit ([&fv] (auto&& arg3) {
                        if constexpr (std::is_same_v<std::decay_t<decltype (arg3)>, std::unique_ptr<LiteralNode>>) {
                            std::print ("{}", arg3->value);
                        }
                        std::println ();
                    },
                    fv);
                }
                std::println ();
            }
            else if constexpr (std::is_same_v<T, std::monostate>) {
            }
            else
                static_assert (false, "non-exhaustive visitor!");
        },
        node->value);
    }
}
