//! \file eggs/lexer/lexer.hpp
// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_LEXER_LEXER_HPP
#define EGGS_LEXER_LEXER_HPP

#include <eggs/lexer/token.hpp>

#include <cassert>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace eggs { namespace lexers
{
    namespace detail
    {
        template <typename T, typename ...Ts>
        struct index_of;

        template <typename T, typename ...Ts>
        struct index_of<T, T, Ts...>
          : std::integral_constant<std::size_t, 0>
        {};

        template <typename T, typename U, typename ...Ts>
        struct index_of<T, U, Ts...>
          : std::integral_constant<std::size_t, 1 + index_of<T, Ts...>::value>
        {};
    }

    ///////////////////////////////////////////////////////////////////////////
    //! template <class ...Rules>
    //! class lexer;
    //!
    //! Class template `lexer` represents a lexical analyzer constituted by an
    //! ordered set of Tokenization Rules.
    //!
    //! \requires `sizeof...(Rules)` shall not be zero. Each type in the
    //!  parameter pack `Rules` shall satisfy the requirements of
    //!  TokenizationRule.
    template <typename ...Rules>
    class lexer
    {
        static_assert(sizeof...(Rules) > 0);

    public:
        //! constexpr lexer() = default;
        //!
        //! \effects Value-initializes each tokenization rule.
        //!
        //! \remarks This constructor shall not participate in overload
        //!  resolution unless `(is_default_constructible_v<Rules> && ...)` is
        //!  `true`.
        template <
            typename Depend = void,
            typename Enable = std::enable_if_t<std::conjunction_v<
                std::is_default_constructible<Rules>...>, Depend>>
        constexpr lexer()
          : _rules()
        {}

        //! template <class ...URules>
        //! explicit lexer(URules&&... rules)
        //!
        //! \effects Initializes each tokenization rule with the corresponding
        //!  value in `std::forward<URules>(rules)`.
        //!
        //! \remarks This constructor shall not participate in overload
        //!  resolution unless `sizeof...(Rules) == sizeof...(URules)` and
        //!  `(is_constructible_v<Rules, URules> && ...)` is `true`.
        template <
            typename ...URules,
            typename Enable = std::enable_if_t<std::conjunction_v<
                std::is_constructible<Rules, URules>...>>>
        explicit lexer(URules&&... rules)
          : _rules(std::forward<URules>(rules)...)
        {}

        lexer(lexer const&) = default;
        lexer(lexer&&) = default;

        lexer& operator=(lexer const&) = default;
        lexer& operator=(lexer&&) = default;

        //! template <class Rule>
        //! static constexpr std::size_t category_of() noexcept
        //!
        //! \requires `Rule` shall occur exactly once in the parameter pack
        //!  `Rules`.
        //!
        //! \returns The category corresponding to the given tokenization rule.
        template <typename Rule>
        static constexpr std::size_t category_of() noexcept
        {
            return detail::index_of<Rule, Rules...>::value;
        };

        //! template <class Iterator, class Sentinel, class OutputIterator>
        //! Iterator operator()(Iterator first, Sentinel last, OutputIterator result) const
        //!
        //! \requires The type `Iterator` shall satisfy ForwardIterator. The
        //!  types `Sentinel` and `Iterator` shall satisfy Sentinel. The type
        //!  `OutputIterator` shall satisfy OutputIterator. The expression
        //!  `*result = token<Iterator>{}` shall be valid.
        //!
        //! \preconditions `[first, last)` shall denote a valid range.
        //!
        //! \effects Copies into `[result, ...)` tokens demarcated by the
        //!  tokenization rules of the lexer in the given input range
        //!  `[first, last)` until an invalid token is found or the range is
        //!  exhausted.
        //!
        //! \returns An iterator denoting the start of the range that produced
        //!  an invalid token, if one was found; otherwise, an iterator
        //!  denoting the end of the input range.
        //!
        //! \remarks Tokens are demarcated as if by successive calls to
        //!  `tokenize`, where the order of the tokenization rules corresponds
        //!  to their order in the parameter pack `Rules`.
        template <
            typename Iterator, typename Sentinel,
            typename OutputIterator>
        Iterator operator()(
            Iterator first, Sentinel last,
            OutputIterator result) const
        {
            return _tokenize(
                std::make_index_sequence<sizeof...(Rules)>{},
                first, last, result);
        }

    private:
        template <
            std::size_t ...Is,
            typename Iterator, typename Sentinel,
            typename OutputIterator>
        Iterator _tokenize(
            std::index_sequence<Is...>,
            Iterator first, Sentinel last,
            OutputIterator result) const
        {
            while (first != last)
            {
                token<Iterator> token =
                    tokenize(first, last, std::get<Is>(_rules)...);
                if (token.category() == token.no_category)
                    break;

                assert(token.first != token.second && "lexeme cannot be empty");
                first = token.second;
                *result++ = std::move(token);
            }
            return first;
        }

    private:
        std::tuple<Rules...> _rules;
    };

    //! template <class Rule, class Lexer>
    //! struct category_of;
    //!
    //! \requires `Lexer` shall be an instance of `lexer`.
    //!
    //! \remarks Specializations shall meet the `UnaryTypeTrait` requirements
    //!  with a `BaseCharacteristic` of `std::integral_constant<std::size_t, C>`
    //!  for some `C` that uniquely corresponds to `Rule` for the given `Lexer`.
    template <typename Rule, typename Lexer>
    struct category_of;

    template <typename Rule, typename ...Rules>
    struct category_of<Rule, lexer<Rules...>>
      : detail::index_of<Rule, Rules...>
    {};
}}

#endif /*EGGS_LEXER_LEXER_HPP*/
