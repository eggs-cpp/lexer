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

TEST_CASE("token<Iterator, Value>::raw()", "[token.raw]")
{
    eggs::lexers::token<char const*, int> tv;

    auto&& t0 = tv.raw();

    CHECK(&t0 == &tv);

    constexpr bool lvalue =
        std::is_same_v<
            eggs::lexers::token<char const*>&,
            decltype(t0)>;
    static_assert(lvalue == true);

    auto&& t1 = std::move(tv).raw();

    CHECK(&t1 == &tv);

    constexpr bool rvalue =
        std::is_same_v<
            eggs::lexers::token<char const*>&&,
            decltype(t1)>;
    static_assert(rvalue == true);

    auto&& t2 = std::as_const(tv).raw();

    CHECK(&t2 == &tv);

    constexpr bool const_lvalue =
        std::is_same_v<
            eggs::lexers::token<char const*> const&,
            decltype(t2)>;
    static_assert(const_lvalue == true);

    auto&& t3 = std::move(std::as_const(tv)).raw();

    CHECK(&t3 == &tv);

    constexpr bool const_rvalue =
        std::is_same_v<
            eggs::lexers::token<char const*> const&&,
            decltype(t3)>;
    static_assert(const_rvalue == true);

    // noexcept
    {
        static_assert(noexcept(tv.raw()));
        static_assert(noexcept(std::move(tv).raw()));
        static_assert(noexcept(std::as_const(tv).raw()));
        static_assert(noexcept(std::move(std::as_const(tv)).raw()));
    }
}

TEST_CASE("token<Iterator>::raw()", "[token.raw]")
{
    eggs::lexers::token<char const*> tv;

    auto&& t0 = tv.raw();

    CHECK(&t0 == &tv);

    constexpr bool lvalue =
        std::is_same_v<
            eggs::lexers::token<char const*>&,
            decltype(t0)>;
    static_assert(lvalue == true);

    auto&& t1 = std::move(tv).raw();

    CHECK(&t1 == &tv);

    constexpr bool rvalue =
        std::is_same_v<
            eggs::lexers::token<char const*>&&,
            decltype(t1)>;
    static_assert(rvalue == true);

    auto&& t2 = std::as_const(tv).raw();

    CHECK(&t2 == &tv);

    constexpr bool const_lvalue =
        std::is_same_v<
            eggs::lexers::token<char const*> const&,
            decltype(t2)>;
    static_assert(const_lvalue == true);

    auto&& t3 = std::move(std::as_const(tv)).raw();

    CHECK(&t3 == &tv);

    constexpr bool const_rvalue =
        std::is_same_v<
            eggs::lexers::token<char const*> const&&,
            decltype(t3)>;
    static_assert(const_rvalue == true);

    // noexcept
    {
        static_assert(noexcept(tv.raw()));
        static_assert(noexcept(std::move(tv).raw()));
        static_assert(noexcept(std::as_const(tv).raw()));
        static_assert(noexcept(std::move(std::as_const(tv)).raw()));
    }
}
