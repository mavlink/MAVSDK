#include "overloaded.hpp"
#include <gtest/gtest.h>
#include <string>
#include <variant>

using namespace mavsdk;

TEST(Overloaded, SelectsMatchingOverload)
{
    using Var = std::variant<int, std::string, double>;

    const auto visit = [](const Var& v) {
        return std::visit(
            overloaded{
                [](int x) { return std::string("i:") + std::to_string(x); },
                [](const std::string& s) { return std::string("s:") + s; },
                [](double d) { return std::string("d:") + std::to_string(static_cast<int>(d)); },
            },
            v);
    };

    EXPECT_EQ(visit(Var{7}), "i:7");
    EXPECT_EQ(visit(Var{std::string("mav")}), "s:mav");
    EXPECT_EQ(visit(Var{3.9}), "d:3");
}

TEST(Overloaded, AutoFallbackCoversUnexpectedTypeShape)
{
    // Single auto catch-all doesn't compile without exact matches; here two
    // overloads — specific int and generic fallback — from public docs pattern.
    int selected = 0;
    overloaded{
        [&](int) { selected = 1; },
        [&](auto) { selected = 2; },
    }(42);
    EXPECT_EQ(selected, 1);

    selected = 0;
    overloaded{
        [&](int) { selected = 1; },
        [&](auto) { selected = 2; },
    }(std::string("x"));
    EXPECT_EQ(selected, 2);
}
