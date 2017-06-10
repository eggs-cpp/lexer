// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/lexer/lexer.hpp>
#include <type_traits>
#include <utility>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

struct rule
{
    static unsigned int default_constructor_calls;
    static unsigned int destructor_calls;

    rule() { ++default_constructor_calls; }
    ~rule() { ++destructor_calls; }

    template <typename I, typename S>
    I operator()(I first, S last) const { return first; }
};
unsigned int rule::default_constructor_calls = 0;
unsigned int rule::destructor_calls = 0;

struct non_default_constructible_rule : rule
{
    non_default_constructible_rule() = delete;
};

struct list_initializable_rule : rule
{
    list_initializable_rule(int, int, int) {}
};

struct non_copyable_rule : rule
{
    non_copyable_rule() = default;
    non_copyable_rule(non_copyable_rule const&) = delete;
};

struct convertible_rule : rule {};

TEST_CASE("lexer<Rules...>::lexer()", "[lexer.cnstr]")
{
    rule::default_constructor_calls = 0;
    rule::destructor_calls = 0;

    eggs::lexers::lexer<rule> l;

    CHECK(rule::default_constructor_calls == 1);
    CHECK(rule::destructor_calls == 0);

    // sfinae
    {
        constexpr bool default_constructible =
            std::is_default_constructible_v<
                eggs::lexers::lexer<non_default_constructible_rule>>;
        static_assert(default_constructible == false);
    }
}

TEST_CASE("lexer<Rules...>::lexer(Rules const&...)", "[lexer.cnstr]")
{
    eggs::lexers::lexer<list_initializable_rule> l({1, 2, 3});

    // sfinae
    {
        constexpr bool constructible_constructible =
            std::is_constructible_v<
                eggs::lexers::lexer<non_copyable_rule>, non_copyable_rule>;
        static_assert(constructible_constructible == false);
    }
}

TEST_CASE("lexer<Rules...>::lexer(URules...)", "[lexer.cnstr]")
{
    rule r;

    eggs::lexers::lexer<rule> l0(r);
    eggs::lexers::lexer<rule> l1(std::as_const(r));
    eggs::lexers::lexer<rule> l2(std::move(r));
    eggs::lexers::lexer<rule> l3(std::move(std::as_const(r)));

    // explicit
    {
        eggs::lexers::lexer<rule> t(convertible_rule{});

        constexpr bool convertible =
            std::is_convertible_v<
                rule, eggs::lexers::lexer<rule>>;
        static_assert(convertible == false);
    }

    // sfinae
    {
        constexpr bool constructible_less_args =
            std::is_constructible_v<
                eggs::lexers::lexer<rule, rule>, rule>;
        static_assert(constructible_less_args == false);

        constexpr bool constructible_more_args =
            std::is_constructible_v<
                eggs::lexers::lexer<rule>, rule, rule>;
        static_assert(constructible_more_args == false);

        constexpr bool constructible_constructible =
            std::is_constructible_v<
                eggs::lexers::lexer<rule>, int>;
        static_assert(constructible_constructible == false);
    }
}
