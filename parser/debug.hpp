#pragma once
#include <cstddef>
#include <print>
#include <string_view>
#include <utility>

#include "Token2.hpp"


template <typename T>
constexpr std::string_view type_name() {
#if defined(__clang__) || defined(__GNUC__)
    return __PRETTY_FUNCTION__;
#elif defined(_MSVC_VER)
    return __FUNCSIG__;
#else
    return "Unknown Type";
#endif
}

template <typename T, auto... E>
struct Inspector {
    static void describe(const T& obj) {
        [&obj, ... args = E]() {
            auto print_helper = [](const auto& member) {
                if constexpr (std::is_enum_v<std::decay_t<decltype(member)>>) {
                    return simlang::TOKEN_STRINGS[std::to_underlying(member)];
                } else {
                    return member;
                }
            };
            ((std::println("Type {} : Value {}",
                           type_name<decltype(obj.*args)>(),
                           print_helper(obj.*args))),
             ...);
            std::println();
        }();
    }
};
