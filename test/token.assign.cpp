// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/lexer/token.hpp>
#include <utility>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

struct value
{
    int x, y;
    value(int x = 1, int y = 0) : x(x), y(y) {}
};

struct convertible : value
{
    convertible(int x = 2, int y = 0) : value(x, y) {}
};

TEST_CASE("token<Iterator, Value>::operator=(token<Iterator, UValue> const&)", "[token.cnstr]")
{
    char const lexeme[] = "lexeme";
    eggs::lexers::token<char const*, convertible> const tu(
        1u, lexeme + 0, lexeme + 6);

    eggs::lexers::token<char const*, value> t;
    t = tu;

    CHECK(t.category() == 1u);
    CHECK(t.first == lexeme + 0);
    CHECK(t.second == lexeme + 6);
    CHECK(t.value.x == 2);
    CHECK(t.value.y == 0);

    // sfinae
    {
        constexpr bool assignable =
            std::is_assignable_v<
                eggs::lexers::token<char const*, value>&,
                eggs::lexers::token<char const*, void*> const&>;
        static_assert(assignable == false);
    }
}

TEST_CASE("token<Iterator, Value>::operator=(token<Iterator, UValue>&&)", "[token.cnstr]")
{
    char const lexeme[] = "lexeme";
    eggs::lexers::token<char const*, convertible> tu(
        1u, lexeme + 0, lexeme + 6);

    eggs::lexers::token<char const*, value> t;
    t = std::move(tu);

    CHECK(t.category() == 1u);
    CHECK(t.first == lexeme + 0);
    CHECK(t.second == lexeme + 6);
    CHECK(t.value.x == 2);
    CHECK(t.value.y == 0);

    // sfinae
    {
        constexpr bool assignable =
            std::is_assignable_v<
                eggs::lexers::token<char const*, value>&,
                eggs::lexers::token<char const*, void*>&&>;
        static_assert(assignable == false);
    }
}
