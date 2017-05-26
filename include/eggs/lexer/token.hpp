//! \file eggs/lexer/token.hpp
// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_LEXER_TOKEN_HPP
#define EGGS_LEXER_TOKEN_HPP

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
        //: std::is_invocable_r<Iterator, Rule&, Iterator, Sentinel>
          : std::is_callable<Rule&(Iterator&&, Sentinel&&), Iterator>
        {};
    }

    ///////////////////////////////////////////////////////////////////////////
    //! template <class Iterator>
    //! class token : public std::pair<Iterator, Iterator>;
    //!
    //! Class template `token` denotes the categorized lexeme produced by a
    //! Tokenization Rule.
    //!
    //! \requires The type `Iterator` shall satisfy ForwardIterator.
    template <typename Iterator>
    class token
      : public std::pair<Iterator, Iterator>
    {
    public:
        //! using iterator = Iterator;
        using iterator = Iterator;

        //! static constexpr std::size_t no_category = std::size_t(-1);
        static constexpr std::size_t no_category = std::size_t(-1);

    public:
        //! constexpr token();
        //!
        //! \effects Initializes the category to `no_category`, and
        //!  value-initializes the pair base class subobject.
        constexpr token()
          : token::pair()
          , _category(no_category)
        {}

        //! constexpr token(std::size_t category, iterator begin, iterator end)
        //!
        //! \preconditions `[begin, end)` shall denote a valid range.
        //!
        //! \effects Initializes the category with the given value, and
        //!  initializes the pair base class subobject with the given iterator
        //!  pair denoting a lexeme.
        constexpr token(std::size_t category, iterator begin, iterator end)
          : token::pair(begin, end)
          , _category(category)
        {}

        token(token const&) = default;
        token& operator=(token const&) = default;

        //! constexpr std::size_t category() const noexcept
        //!
        //! \returns The category of this token.
        constexpr std::size_t category() const noexcept
        {
            return _category;
        }

    private:
        std::size_t _category;
    };

    template <typename Iterator>
    std::size_t const token<Iterator>::no_category;

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
    template <
        typename Iterator, typename Sentinel,
        typename ...Rules,
        typename Enable = std::enable_if_t<std::conjunction_v<
            detail::is_tokenization_rule<Rules, Iterator, Sentinel>...>>>
    token<Iterator> tokenize(
        Iterator first, Sentinel last,
        Rules&&... rules)
    {
        std::size_t mark_category = token<Iterator>::no_category;
        if (first == last)
            return token<Iterator>{mark_category, first, first};

        Iterator mark_iter = first;
        std::size_t mark_length = 0;
        std::size_t category = 0;
        int _sequencer[] = {([&](auto const& rule) -> void {
            Iterator const result = rule(first, last);
            std::size_t const length = std::distance(first, result);
            if (length > mark_length)
            {
                mark_category = category;
                mark_iter = result;
                mark_length = length;
            }
            ++category;
        }(rules), 0)...}; (void)_sequencer;

        return token<Iterator>{mark_category, first, mark_iter};
    }

    template <typename Iterator, typename Sentinel>
    token<Iterator> tokenize(
        Iterator first, Sentinel /*last*/)
    {
        return token<Iterator>{token<Iterator>::no_category, first, first};
    }
}}

#endif /*EGGS_LEXER_TOKEN_HPP*/
