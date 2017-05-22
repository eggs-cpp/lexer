// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/lexer/token.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "rules.hpp"

TEST_CASE("tokenize(Iterator, Sentinel, Rules&&...)", "[token.tokenize]")
{
    char const input[] = "123abc!";

    auto const t = eggs::lexers::tokenize(
        input + 0, input + sizeof(input) - 1,
        number{}, word{});

    CHECK(t.first == input + 0);
    CHECK(t.second == input + 6);

    // no match
    {
        char const input[] = "!";

        auto const t = eggs::lexers::tokenize(
            input + 0, input + sizeof(input) - 1,
            number{}, word{});

        CHECK(t.first == input + 0);
        CHECK(t.second == input + 0);
    }

    // empty input
    {
        char const input[] = "";

        auto const t = eggs::lexers::tokenize(
            input + 0, input + sizeof(input) - 1,
            number{}, word{});

        CHECK(t.first == input + 0);
        CHECK(t.second == input + 0);
    }
}

TEST_CASE("tokenize(Iterator, Sentinel)", "[token.tokenize]")
{
    char const input[] = "";

    auto const t = eggs::lexers::tokenize(
        input + 0, input + sizeof(input) - 1);

    CHECK(t.first == input + 0);
    CHECK(t.second == input + 0);
}
