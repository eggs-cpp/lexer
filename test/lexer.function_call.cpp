// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/lexer/lexer.hpp>
#include <iterator>
#include <vector>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "rules.hpp"

TEST_CASE("lexer<Rules...>::operator()(Iterator, Sentinel, OutputIterator)", "[lexer.function_call]")
{
    eggs::lexers::lexer<number, word, punct> l;

    char const input[] = "123abc!";

    std::vector<eggs::lexers::token<char const*>> ts;
    auto const last = l(
        input + 0, input + sizeof(input) - 1,
        std::back_inserter(ts));

    CHECK(last == input + 7);
    REQUIRE(ts.size() == 2u);
    CHECK(ts[0].category() == l.category_of<word>());
    CHECK(ts[0].first == input + 0);
    CHECK(ts[0].second == input + 6);
    CHECK(ts[1].category() == l.category_of<punct>());
    CHECK(ts[1].first == input + 6);
    CHECK(ts[1].second == input + 7);

    // invalid match
    {
        char const input[] = "123abc !";

        std::vector<eggs::lexers::token<char const*>> ts;
        auto const last = l(
            input + 0, input + sizeof(input) - 1,
            std::back_inserter(ts));

        CHECK(last == input + 6);
        REQUIRE(ts.size() == 1u);
        CHECK(ts[0].category() == l.category_of<word>());
        CHECK(ts[0].first == input + 0);
        CHECK(ts[0].second == input + 6);
    }

    // empty input
    {
        char const input[] = "";

        std::vector<eggs::lexers::token<char const*>> ts;
        auto const last = l(
            input + 0, input + sizeof(input) - 1,
            std::back_inserter(ts));

        CHECK(last == input + 0);
        CHECK(ts.size() == 0u);
    }

    // first longest match
    {
        eggs::lexers::lexer<number, unit, word> l;

        char const input[] = "123abc !";

        std::vector<eggs::lexers::token<char const*>> ts;
        auto const last = l(
            input + 0, input + sizeof(input) - 1,
            std::back_inserter(ts));

        CHECK(last == input + 6);
        REQUIRE(ts.size() == 1u);
        CHECK(ts[0].category() == l.category_of<unit>());
        CHECK(ts[0].first == input + 0);
        CHECK(ts[0].second == input + 6);
    }
}
