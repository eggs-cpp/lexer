//! \file eggs/lexer/tokenize.hpp
// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_LEXER_TOKENIZE_HPP
#define EGGS_LEXER_TOKENIZE_HPP

#include <eggs/lexer/token.hpp>

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace eggs { namespace lexers
{
    ///////////////////////////////////////////////////////////////////////////
    //! The _TokenizationRule_ concept specifies the requirements of a type
    //! that demarcates tokens on a given input range.
    //!
    //! A type `X` satisfies TokenizationRule if and only if:
    //!
    //! - it is a function object type,
    //!
    //! - given a range `[first, last)`, where `first` and `last` are of some
    //!   ForwardIterator and Sentinel type respectively, and an lvalue `rule`
    //!   of type (possibly const) `X`, the following expression is valid:
    //!
    //!       decltype(first) mark = rule(first, last);
    //!
    //! - the resulting range `[first, mark)` shall be valid; if non-empty, it
    //!   denotes the lexeme of a demarcated token.
    namespace detail
    {
        template <typename Rule, typename Iterator, typename Sentinel>
        struct is_tokenization_rule
#if __cpp_lib_is_invocable
          : std::is_invocable_r<Iterator, Rule&, Iterator, Sentinel>
#else
          : std::is_callable<Rule&(Iterator&&, Sentinel&&), Iterator>
#endif
        {};
    }

    ///////////////////////////////////////////////////////////////////////////
    //! template <class Iterator, class Sentinel, class ...Rules>
    //! token<Iterator> tokenize(Iterator first, Sentinel last, Rules&&... rules)
    //!
    //! \requires The type `Iterator` shall satisfy ForwardIterator. The types
    //!  `Sentinel` and `Iterator` shall satisfy Sentinel. Each type in the
    //!  parameter pack `Rules` shall satisfy TokenizationRule.
    //!
    //! \preconditions `[first, last)` shall denote a valid range.
    //!
    //! \effects Demarcates a token starting at the beginning of the given
    //!  input range `[first, last)` by applying each of the tokenization
    //!  rules in the order they appear in the parameter pack `rules`, and
    //!  selecting the first lexeme produced such that no other lexeme is
    //!  longer.
    //!
    //! \returns The demarcated token, if any; otherwise, an empty token.
    //!
    //! \remarks The category of the resulting token is the 0-based index in
    //!  the parameter pack `rules` of the rule that demarcated it.
    namespace detail
    {
        template <typename T, std::size_t N>
        static constexpr void _swallow_pack(T const (&)[N]) noexcept {}

        template <std::size_t I>
        using index = std::integral_constant<std::size_t, I>;

        template <
            typename Iterator, typename Sentinel,
            std::size_t ...Is, typename ...Rules>
        token<Iterator> _tokenize(
            Iterator first, Sentinel last,
            std::index_sequence<Is...>, Rules const&... rules)
        {
            std::size_t mark_category = token<Iterator>::no_category;
            Iterator mark_iter = first;
            std::size_t mark_length = 0;
            detail::_swallow_pack({[&](auto category, auto const& rule) -> int
            {
                Iterator const iter = rule(first, last);

                std::size_t const length = std::distance(first, iter);
                if (length > mark_length)
                {
                    mark_category = category;
                    mark_iter = iter;
                    mark_length = length;
                }
                return 0;
            }(index<Is>{}, rules)...});

            return token<Iterator>{mark_category, first, mark_iter};
        }
    }

    template <
        typename Iterator, typename Sentinel,
        typename ...Rules,
        typename Enable = std::enable_if_t<std::conjunction_v<
            detail::is_tokenization_rule<Rules, Iterator, Sentinel>...>>>
    token<Iterator> tokenize(
        Iterator first, Sentinel last,
        Rules&&... rules)
    {
        std::size_t const mark_category = token<Iterator>::no_category;
        if (first == last)
            return token<Iterator>{mark_category, first, first};

        return detail::_tokenize(
            first, last, std::index_sequence_for<Rules...>{}, rules...);
    }

    template <typename Iterator, typename Sentinel>
    token<Iterator> tokenize(
        Iterator first, Sentinel /*last*/)
    {
        return token<Iterator>{token<Iterator>::no_category, first, first};
    }
}}

#endif /*EGGS_LEXER_TOKENIZE_HPP*/
