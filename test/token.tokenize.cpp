// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/lexer/tokenize.hpp>
#include <type_traits>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "rules.hpp"

TEST_CASE("tokenize(Iterator, Sentinel, Rules&&...)", "[token.tokenize]")
{
    char const input[] = "123abc!";

    auto const t = eggs::lexers::tokenize(
        input + 0, input + sizeof(input) - 1,
        number{}, word{});

    CHECK(t.category() == 1u);
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

        auto const t = eggs::lexers::tokenize(
            input + 0, input + sizeof(input) - 1,
            number{}, word{});

        CHECK(t.category() == t.no_category);
        CHECK(t.first == input + 0);
        CHECK(t.second == input + 0);
    }

    // empty input
    {
        char const input[] = "";

        auto const t = eggs::lexers::tokenize(
            input + 0, input + sizeof(input) - 1,
            number{}, word{});

        CHECK(t.category() == t.no_category);
        CHECK(t.first == input + 0);
        CHECK(t.second == input + 0);
    }

    // first longest match
    {
        char const input[] = "123abc !";

        auto const t = eggs::lexers::tokenize(
            input + 0, input + sizeof(input) - 1,
            number{}, unit{}, word{});

        CHECK(t.category() == 1u);
        CHECK(t.first == input + 0);
        CHECK(t.second == input + 6);
    }

    // value
    {
        char const input[] = "123abc!";

        auto const t0 = eggs::lexers::tokenize(
            input + 0, input + sizeof(input) - 1,
            number_with_value<int>{42},
            word_with_value<int>{43});

        REQUIRE(t0.value == 43);

        auto const t1 = eggs::lexers::tokenize(
            input + 0, input + sizeof(input) - 1,
            number{},
            word_with_value<int>{43});

        REQUIRE(t1.value.index() == 1u);
        CHECK(std::get<1>(t1.value) == 43);

        auto const t2 = eggs::lexers::tokenize(
            input + 0, input + sizeof(input) - 1,
            number_with_value<int>{42},
            word{});

        REQUIRE(t2.value.index() == 0u);

        // evaluate
        {
            char const input[] = "123abc!";

            auto const t0 = eggs::lexers::tokenize(
                input + 0, input + sizeof(input) - 1,
                rule_with_evaluate<number>{},
                rule_with_evaluate<word>{});

            constexpr bool no_value =
                std::is_same_v<
                    eggs::lexers::token<char const*> const,
                    decltype(t0)>;
            static_assert(no_value == true);

            auto const t1 = eggs::lexers::tokenize(
                input + 0, input + sizeof(input) - 1,
                rule_with_evaluate<number, int>{42},
                rule_with_evaluate<word>{});

            REQUIRE(t1.value.index() == 0u);

            auto const t2 = eggs::lexers::tokenize(
                input + 0, input + sizeof(input) - 1,
                rule_with_evaluate<number, int>{42},
                rule_with_evaluate<word, int>{43});

            REQUIRE(t2.value == 43);

            auto const t3 = eggs::lexers::tokenize(
                input + 0, input + sizeof(input) - 1,
                rule_with_evaluate<number, int>{42},
                rule_with_evaluate<word_with_value<int>>{{43}});

            REQUIRE(t3.value.index() == 0u);

            auto const t4 = eggs::lexers::tokenize(
                input + 0, input + sizeof(input) - 1,
                rule_with_evaluate<number, int>{42},
                rule_with_evaluate<word_with_value<int>, std::string>{"43", {43}});

            REQUIRE(t4.value.index() == 2u);
            CHECK(std::get<2>(t4.value) == "43");
        }
    }
}

TEST_CASE("tokenize(Iterator, Sentinel)", "[token.tokenize]")
{
    char const input[] = "";

    auto const t = eggs::lexers::tokenize(
        input + 0, input + sizeof(input) - 1);

    CHECK(t.category() == t.no_category);
    CHECK(t.first == input + 0);
    CHECK(t.second == input + 0);

    constexpr bool no_value =
        std::is_same_v<
            eggs::lexers::token<char const*> const,
            decltype(t)>;
    static_assert(no_value == true);
}
