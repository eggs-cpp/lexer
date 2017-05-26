// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/lexer/token.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("token<Iterator>::category()", "[token.category]")
{
    eggs::lexers::token<char const*> t(1u, nullptr, nullptr);

    CHECK(t.category() == 1u);

    // constexpr
    {
        constexpr eggs::lexers::token<char const*> t(1u, nullptr, nullptr);

        static_assert(t.category() == 1u);
    }

    // noexcept
    {
        static_assert(noexcept(t.category()));
    }
}
