// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef RULES_HPP
#define RULES_HPP

#include <eggs/lexer/token.hpp>

#include <cctype>
#include <utility>

///////////////////////////////////////////////////////////////////////////////
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

struct unit
{
    template <typename I, typename S>
    I operator()(I first, S last) const
    {
        while (first != last && !std::isspace(*first)) ++first;
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

///////////////////////////////////////////////////////////////////////////////
template <typename Rule, typename Value>
struct rule_with_value
{
    Value value;
    Rule rule = {};

    template <typename I, typename S>
    std::pair<I, Value> operator()(I first, S last) const
    {
        return std::make_pair(rule(first, last), value);
    }
};

template <typename Value>
using number_with_value = rule_with_value<number, Value>;

template <typename Value>
using word_with_value = rule_with_value<word, Value>;

template <typename Value>
using unit_with_value = rule_with_value<unit, Value>;

template <typename Value>
using punct_with_value = rule_with_value<punct, Value>;

///////////////////////////////////////////////////////////////////////////////
template <typename Rule, typename Value = void>
struct rule_with_evaluate
{
    Value value;
    Rule rule = {};

    template <typename I, typename S>
    decltype(auto) operator()(I first, S last) const
    {
        return rule(first, last);
    }

    template <typename I, typename V>
    Value evaluate(eggs::lexers::token<I, V>&& /*token*/) const
    {
        return value;
    }
};

template <typename Rule>
struct rule_with_evaluate<Rule, void>
{
    Rule rule = {};

    template <typename I, typename S>
    decltype(auto) operator()(I first, S last) const
    {
        return rule(first, last);
    }

    template <typename I, typename V>
    void evaluate(eggs::lexers::token<I, V>&& /*token*/) const
    {
        int breakpoint = 3;
    }
};

#endif /*RULES_HPP*/
