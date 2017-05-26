// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/lexer/lexer.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "rules.hpp"

TEST_CASE("lexer<Rules...>::category_of<Rule>()", "[lexer.category_of]")
{
    eggs::lexers::lexer<number, word> l;

    CHECK(l.category_of<number>() == 0u);
    CHECK(l.category_of<word>() == 1u);

    // constexpr
    {
        constexpr eggs::lexers::lexer<number, word> l;

        static_assert(l.category_of<number>() == 0u);
        static_assert(l.category_of<word>() == 1u);
    }

    // noexcept
    {
        static_assert(noexcept(l.category_of<number>()));
    }
}

TEST_CASE("category_of<Rule, Lexer>", "[lexer.category_of]")
{
    using lexer = eggs::lexers::lexer<number, word>;

    constexpr bool base_characteristic_number =
        std::is_base_of_v<
            std::integral_constant<std::size_t, 0>,
            eggs::lexers::category_of<number, lexer>>;
    static_assert(base_characteristic_number == true);

    constexpr bool base_characteristic_word =
        std::is_base_of_v<
            std::integral_constant<std::size_t, 1>,
            eggs::lexers::category_of<word, lexer>>;
    static_assert(base_characteristic_word == true);
}
