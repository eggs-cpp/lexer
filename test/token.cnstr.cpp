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
    static unsigned int default_constructor_calls;
    static unsigned int destructor_calls;

    int x, y;
    value(int x = 1, int y = 0) : x(x), y(y) { ++default_constructor_calls; }
    ~value() { ++destructor_calls; }
};
unsigned int value::default_constructor_calls = 0;
unsigned int value::destructor_calls = 0;

struct constexpr_value
{
    int x, y;
    constexpr constexpr_value(int x = 1, int y = 0) : x(x), y(y) {}
};

struct non_default_constructible_value : value
{
    non_default_constructible_value() = delete;
};

TEST_CASE("token<Iterator, Value>::token()", "[token.cnstr]")
{
    value::default_constructor_calls = 0;
    value::destructor_calls = 0;

    eggs::lexers::token<char const*, value> t;

    CHECK(value::default_constructor_calls == 1);
    CHECK(value::destructor_calls == 0);

    CHECK(t.category() == t.no_category);
    CHECK(t.first == nullptr);
    CHECK(t.second == nullptr);
    CHECK(t.value.x == 1);
    CHECK(t.value.y == 0);

    // constexpr
    {
        constexpr eggs::lexers::token<char const*, constexpr_value> t;

        static_assert(t.category() == t.no_category);
        static_assert(t.first == nullptr);
        static_assert(t.second == nullptr);
        static_assert(t.value.x == 1);
        static_assert(t.value.y == 0);
    }

    // sfinae
    {
        constexpr bool default_constructible =
            std::is_default_constructible_v<
                eggs::lexers::token<char const*, non_default_constructible_value>>;
        static_assert(default_constructible == false);
    }
}

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

TEST_CASE("token<Iterator, Value>::token(std::size_t, Iterator, Iterator, Args&&...)", "[token.cnstr]")
{
    char const lexeme[] = "lexeme";

    eggs::lexers::token<char const*, value> t0(
        1u, lexeme + 0, lexeme + 6);

    CHECK(t0.category() == 1u);
    CHECK(t0.first == lexeme + 0);
    CHECK(t0.second == lexeme + 6);
    CHECK(t0.value.x == 1);
    CHECK(t0.value.y == 0);

    eggs::lexers::token<char const*, value> t1(
        1u, lexeme + 0, lexeme + 6, 42);

    CHECK(t1.category() == 1u);
    CHECK(t1.first == lexeme + 0);
    CHECK(t1.second == lexeme + 6);
    CHECK(t1.value.x == 42);
    CHECK(t1.value.y == 0);

    eggs::lexers::token<char const*, value> t2(
        1u, lexeme + 0, lexeme + 6, 42, 43);

    CHECK(t2.category() == 1u);
    CHECK(t2.first == lexeme + 0);
    CHECK(t2.second == lexeme + 6);
    CHECK(t2.value.x == 42);
    CHECK(t2.value.y == 43);

    // constexpr
    {
        static constexpr char lexeme[] = "lexeme";

        constexpr eggs::lexers::token<char const*, constexpr_value> t0(
            1u, lexeme + 0, lexeme + 6);

        static_assert(t0.category() == 1u);
        static_assert(t0.first == lexeme + 0);
        static_assert(t0.second == lexeme + 6);
        static_assert(t0.value.x == 1);
        static_assert(t0.value.y == 0);

        constexpr eggs::lexers::token<char const*, constexpr_value> t1(
            1u, lexeme + 0, lexeme + 6, 42);

        static_assert(t1.category() == 1u);
        static_assert(t1.first == lexeme + 0);
        static_assert(t1.second == lexeme + 6);
        static_assert(t1.value.x == 42);
        static_assert(t1.value.y == 0);

        constexpr eggs::lexers::token<char const*, constexpr_value> t2(
            1u, lexeme + 0, lexeme + 6, 42, 43);

        static_assert(t2.category() == 1u);
        static_assert(t2.first == lexeme + 0);
        static_assert(t2.second == lexeme + 6);
        static_assert(t2.value.x == 42);
        static_assert(t2.value.y == 43);
    }

    // sfinae
    {
        constexpr bool constructible_more_args =
            std::is_constructible_v<
                eggs::lexers::token<char const*, value>, int, int, int, int>;
        static_assert(constructible_more_args == false);

        constexpr bool constructible_constructible =
            std::is_constructible_v<
                eggs::lexers::token<char const*, value>, void*>;
        static_assert(constructible_constructible == false);
    }
}

TEST_CASE("token<Iterator>::token(std::size_t, Iterator, Iterator)", "[token.cnstr]")
{
    char const lexeme[] = "lexeme";

    eggs::lexers::token<char const*> t(
        1u, lexeme + 0, lexeme + 6);

    CHECK(t.category() == 1u);
    CHECK(t.first == lexeme + 0);
    CHECK(t.second == lexeme + 6);

    // constexpr
    {
        static constexpr char lexeme[] = "lexeme";

        constexpr eggs::lexers::token<char const*> t(
            1u, lexeme + 0, lexeme + 6);

        static_assert(t.category() == 1u);
        static_assert(t.first == lexeme + 0);
        static_assert(t.second == lexeme + 6);
    }

    // sfinae
    {
        constexpr bool constructible_constructible =
            std::is_constructible_v<
                eggs::lexers::token<char const*>, int>;
        static_assert(constructible_constructible == false);
    }
}


TEST_CASE("token<Iterator>::token(token<Iterator, Value>)", "[token.cnstr]")
{
    char const lexeme[] = "lexeme";

    eggs::lexers::token<char const*, value> const tv0(
        1u, lexeme + 0, lexeme + 6);
    eggs::lexers::token<char const*> t0(tv0);

    CHECK(t0.category() == 1u);
    CHECK(t0.first == lexeme + 0);
    CHECK(t0.second == lexeme + 6);

    eggs::lexers::token<char const*, value> tv1(
        1u, lexeme + 0, lexeme + 6);
    eggs::lexers::token<char const*> t1(std::move(tv1));

    CHECK(t1.category() == 1u);
    CHECK(t1.first == lexeme + 0);
    CHECK(t1.second == lexeme + 6);

    // constexpr
    {
        static constexpr char lexeme[] = "lexeme";

        constexpr eggs::lexers::token<char const*> tv(1u, lexeme + 0, lexeme + 6);
        constexpr eggs::lexers::token<char const*> t(tv);

        static_assert(t.category() == 1u);
        static_assert(t.first == lexeme + 0);
        static_assert(t.second == lexeme + 6);
    }

    // slicing
    {
    char const lexeme[] = "lexeme";

    eggs::lexers::token<char const*, value> tv(
        1u, lexeme + 0, lexeme + 6);
    eggs::lexers::token<char const*>& t(tv);

    CHECK(&t == &tv);
    }
}
