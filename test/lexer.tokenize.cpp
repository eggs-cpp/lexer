// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/lexer/lexer.hpp>
#include <string>
#include <type_traits>
#include <variant>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "rules.hpp"

TEST_CASE("lexer<Rules...>::tokenize(Iterator, Sentinel)", "[lexer.tokenize]")
{
    char const input[] = "123abc!";

    eggs::lexers::lexer<number, word> l;

    auto const t = l.tokenize(
        input + 0, input + sizeof(input) - 1);

    CHECK(t.category() == l.category_of<word>());
    CHECK(t.first == input + 0);
    CHECK(t.second == input + 6);

    constexpr bool no_value =
        std::is_same_v<
            eggs::lexers::token<char const*> const,
            decltype(t)>;
    static_assert(no_value == true);

    // no match
    {
        char const input[] = "!";

        auto const t = l.tokenize(
            input + 0, input + sizeof(input) - 1);

        CHECK(t.category() == t.no_category);
        CHECK(t.first == input + 0);
        CHECK(t.second == input + 0);
    }

    // empty input
    {
        char const input[] = "";

        auto const t = l.tokenize(
            input + 0, input + sizeof(input) - 1);

        CHECK(t.category() == t.no_category);
        CHECK(t.first == input + 0);
        CHECK(t.second == input + 0);
    }

    // first longest match
    {
        char const input[] = "123abc !";

        eggs::lexers::lexer<number, unit, word> l;

        auto const t = l.tokenize(
            input + 0, input + sizeof(input) - 1);

        CHECK(t.category() == l.category_of<unit>());
        CHECK(t.first == input + 0);
        CHECK(t.second == input + 6);
    }
}
