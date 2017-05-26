// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/lexer/token.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("token<Iterator>::token()", "[token.cnstr]")
{
    eggs::lexers::token<char const*> t;

    CHECK(t.category() == t.no_category);
    CHECK(t.first == nullptr);
    CHECK(t.second == nullptr);

    // constexpr
    {
        constexpr eggs::lexers::token<char const*> t;

        static_assert(t.category() == t.no_category);
        static_assert(t.first == nullptr);
        static_assert(t.second == nullptr);
    }
}

TEST_CASE("token<Iterator>::token(std::size_t, iterator, iterator)", "[token.cnstr]")
{
    char const lexeme[] = "lexeme";

    eggs::lexers::token<char const*> t(1u, lexeme + 0, lexeme + 6);

    CHECK(t.category() == 1u);
    CHECK(t.first == lexeme + 0);
    CHECK(t.second == lexeme + 6);

    // constexpr
    {
        static constexpr char lexeme[] = "lexeme";

        constexpr eggs::lexers::token<char const*> t(1u, lexeme + 0, lexeme + 6);

        static_assert(t.category() == 1u);
        static_assert(t.first == lexeme + 0);
        static_assert(t.second == lexeme + 6);
    }
}
