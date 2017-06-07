// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/lexer/lexer.hpp>
#include <iterator>
#include <type_traits>
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

    constexpr bool no_value =
        std::is_same_v<
            eggs::lexers::token<char const*>,
            decltype(l)::token<char const*>>;
    static_assert(no_value == true);

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

    // value
    {
        eggs::lexers::lexer<number_with_value<int>, word_with_value<int>, punct> l{
            number_with_value<int>{42}, word_with_value<int>{43}, punct{}};

        char const input[] = "123abc!";

        std::vector<decltype(l)::token<char const*>> ts;
        auto const last = l(
            input + 0, input + sizeof(input) - 1,
            std::back_inserter(ts));

        REQUIRE(ts.size() == 2u);
        REQUIRE(ts[0].value.index() == 2u);
        CHECK(std::get<2>(ts[0].value) == 43);
        REQUIRE(ts[1].value.index() == 0u);
    }
}
