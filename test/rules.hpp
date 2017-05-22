// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef RULES_HPP
#define RULES_HPP

#include <cctype>

struct number
{
    template <typename I, typename S>
    I operator()(I first, S last) const {
        while (first != last && std::isdigit(*first)) ++first;
        return first;
    }
};

struct word
{
    template <typename I, typename S>
    I operator()(I first, S last) const
    {
        while (first != last && std::isalnum(*first)) ++first;
        return first;
    }
};

struct punct
{
    template <typename I, typename S>
    I operator()(I first, S /*last*/) const
    {
        if (std::ispunct(*first)) ++first;
        return first;
    }
};

#endif /*RULES_HPP*/
