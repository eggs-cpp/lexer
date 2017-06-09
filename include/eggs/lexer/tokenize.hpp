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
#include <variant>

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
    //!   of type (possibly const) `X`, one of the following statements is
    //!   well-formed:
    //!
    //!   - `decltype(first) mark = rule(first, last);`, or
    //!
    //!   - `auto&& [mark, value] = rule(first, last);`.
    //!
    //! - the resulting range `[first, mark)` shall be valid; if non-empty, it
    //!   denotes the lexeme of a demarcated token.
    //!
    //! - given a token `t` of type `token<decltype(first), Value>`, where
    //!   `Value` is `decltype(value)` if one exists or `void` otherwise, then
    //!   if the expression `rule.evaluate(std::move(t))` is well-formed it is
    //!   evaluated with the demarcated token along with its intermediate
    //!   associated value, and its return value (if any) is the associated
    //!   value of the demarcated token. Otherwise, the intermediate `value`
    //!   (if any) is the associated value of the demarcated token.
    namespace detail
    {
        template <typename Rule, typename Iterator, typename Sentinel>
        struct is_tokenization_rule
#if __cpp_lib_is_invocable
          : std::is_invocable<Rule&, Iterator, Sentinel>
#else
          : std::is_callable<Rule&(Iterator&&, Sentinel&&)>
#endif
        {};

        ///////////////////////////////////////////////////////////////////////
        struct empty {};

        template <typename Iterator>
        Iterator get_mark(Iterator mark)
        {
            return mark;
        }

        template <typename Iterator, typename Value>
        Iterator get_mark(std::pair<Iterator, Value>& s)
        {
            return s.first;
        }

        template <typename Iterator>
        empty get_value(Iterator const& /*mark*/) noexcept
        {
            return {};
        }

        template <typename Iterator, typename Value>
        Value&& get_value(std::pair<Iterator, Value>& s) noexcept
        {
            return std::move(s.second);
        }

        template <typename Rule, typename Iterator>
        void _evaluate(
            Rule const& /*rule*/,
            std::size_t /*category*/, Iterator /*first*/, Iterator /*last*/,
            empty&& /*value*/, ...)
        {}

        template <typename Rule, typename Iterator, typename Value>
        Value&& _evaluate(
            Rule const& /*rule*/,
            std::size_t /*category*/, Iterator /*first*/, Iterator /*last*/,
            Value&& value, ...)
        {
            return std::forward<Value>(value);
        }

        template <
            typename Rule, typename Iterator,
            typename Token = lexers::token<Iterator>>
        auto _evaluate(
            Rule const& rule,
            std::size_t category, Iterator first, Iterator last,
            empty&& /*payload*/, int)
         -> decltype(rule.evaluate(std::declval<Token>()))
        {
            return rule.evaluate(Token{category, first, last});
        }

        template <
            typename Rule, typename Iterator, typename Payload,
            typename Token = lexers::token<Iterator, Payload>,
            typename Enable = std::enable_if_t<!std::is_same_v<Payload, empty>>>
        auto _evaluate(
            Rule const& rule,
            std::size_t category, Iterator first, Iterator last,
            Payload&& payload, int)
         -> decltype(rule.evaluate(std::declval<Token>()))
        {
            return rule.evaluate(Token{category, first, last,
                std::forward<Payload>(payload)});
        }

        template <typename Rule, typename Iterator, typename Payload>
        auto evaluate(
            Rule const& rule,
            std::size_t category, Iterator first, Iterator last,
            Payload&& payload)
        {
            return detail::_evaluate(rule, category, first, last,
                std::forward<Payload>(payload), 0);
        }

        ///////////////////////////////////////////////////////////////////////
        template <typename Rule, typename Iterator, typename Sentinel>
        struct tokenization_rule_traits
        {
            using rule = Rule;

#ifdef __cpp_lib_invoke_result
            using Result = std::invoke_result_t<Rule&(Iterator&&, Sentinel&&)>;
#else
            using Result = std::result_of_t<Rule&(Iterator&&, Sentinel&&)>;
#endif
            using payload = std::decay_t<decltype(
                detail::get_value<Iterator>(std::declval<Result&>()))>;

            using value = std::decay_t<decltype(
                detail::evaluate(std::declval<Rule&>(),
                    std::size_t(), std::declval<Iterator>(), std::declval<Iterator>(),
                    std::declval<payload>()))>;
        };
    }

    ///////////////////////////////////////////////////////////////////////
    namespace detail
    {

        template <std::size_t I, typename T>
        struct indexed {};

        template <typename Ts, typename T>
        struct _value_append;

        template <typename ...Ts>
        struct _value_append<
            std::variant<std::monostate, Ts...>, void>
        {
            using type = std::variant<std::monostate, Ts...>;
        };

        template <typename ...Ts, typename T>
        struct _value_append<
            std::variant<std::monostate, Ts...>, T>
        {
            using type = std::variant<std::monostate, Ts..., T>;
        };

        template <typename Rule, typename Payload = empty, typename Value = void>
        struct intermediate_state
        {
            Rule const& rule;
            Payload payload;

            intermediate_state(Rule const& rule, Payload&& payload)
              : rule(rule)
              , payload(std::move(payload))
            {}
        };

        template <typename Ts, std::size_t VI, typename RuleTrait>
        struct _intermediate_state_append;

        template <typename ...Ts, std::size_t VI, typename RuleTrait>
        struct _intermediate_state_append<
            std::variant<Ts...>, VI, RuleTrait>
        {
            using intermediate = intermediate_state<
                typename RuleTrait::rule,
                typename RuleTrait::payload,
                std::conditional_t<
                    std::is_void_v<typename RuleTrait::value>, void,
                    indexed<VI, typename RuleTrait::value>>>;
            using type = std::variant<Ts..., intermediate>;
        };

        template <typename Ts, typename ITs, typename ...RuleTraits>
        struct _tokenization_value;

        template <typename Ts, typename ITs>
        struct _tokenization_value<Ts, ITs>
        {
            using type = Ts;
            using intermediate = ITs;
        };

        template <typename ITs>
        struct _tokenization_value<
            std::variant<std::monostate>, ITs>
        {
            using type = void;
            using intermediate = ITs;
        };

        template <
            typename Ts, typename ITs,
            typename RuleTrait, typename ...RuleTraits>
        struct _tokenization_value<
            Ts, ITs,
            RuleTrait, RuleTraits...
        > : _tokenization_value<
                typename _value_append<Ts, typename RuleTrait::value>::type,
                typename _intermediate_state_append<
                    ITs, std::variant_size_v<Ts>, RuleTrait>::type,
                RuleTraits...>
        {};

        template <typename Iterator, typename Sentinel, typename ...Rules>
        struct tokenization_value
          : _tokenization_value<
                std::variant<std::monostate>,
                std::variant<empty>,
                tokenization_rule_traits<Rules, Iterator, Sentinel>...>
        {};

        ///////////////////////////////////////////////////////////////////////
        template <typename T, std::size_t N>
        static constexpr void _swallow_pack(T const (&)[N]) noexcept {}

        template <std::size_t I>
        using index = std::integral_constant<std::size_t, I>;

        template <typename Iterator, typename Value>
        struct make_token
        {
            using token = lexers::token<Iterator, Value>;

            std::size_t category;
            Iterator first, last;

            token operator()(empty) const
            {
                return token{category, first, last};
            }

            template <typename Ri>
            token operator()(
                intermediate_state<Ri>& /*match*/) const
            {
                return token{category, first, last};
            }

            template <typename Ri, typename Pi>
            token operator()(
                intermediate_state<Ri, Pi>& match) const
            {
                detail::evaluate(match.rule, category, first, last,
                    std::move(match.payload));
                return token{category, first, last};
            }

            template <typename Ri, typename Pi, std::size_t I, typename Vi>
            token operator()(
                intermediate_state<Ri, Pi, indexed<I, Vi>>& match) const
            {
                auto value = detail::evaluate(match.rule, category, first, last,
                    std::move(match.payload));
                return token{category, first, last,
                    std::in_place_index<I>, std::move(value)};
            }
        };

        template <
            typename Value,
            typename Iterator, typename Sentinel,
            std::size_t ...Is, typename ...Rules>
        token<Iterator, Value> _tokenize(
            Iterator first, Sentinel last,
            std::index_sequence<Is...>, Rules const&... rules)
        {
            Iterator mark_iter = first;
            std::size_t mark_length = 0;
            typename detail::tokenization_value<
                Iterator, Sentinel, Rules...>::intermediate mark_category;
            detail::_swallow_pack({[&](auto category, auto const& rule) -> int
            {
                constexpr std::size_t I = decltype(category)::value;

                auto&& result = rule(first, last);

                Iterator const iter = detail::get_mark<Iterator>(result);
                std::size_t const length = std::distance(first, iter);
                if (length > mark_length)
                {
                    mark_iter = iter;
                    mark_length = length;
                    mark_category.template emplace<I + 1>(
                        rule, detail::get_value<Iterator>(result));
                }
                return 0;
            }(index<Is>{}, rules)...});

            std::size_t const category = mark_category.index() - 1;
            return std::visit(
                make_token<Iterator, Value>{category, first, mark_iter},
                mark_category);
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    //! template <class Iterator, class Sentinel, class ...Rules>
    //! token<Iterator, Value> tokenize(Iterator first, Sentinel last, Rules&&... rules)
    //!
    //! Let `Vi` be the type of the associated value of a token demarcated by
    //!  the `i`th rule in `rules`, if any; otherwise, `void`. Let `Vs` be the
    //!  pack comprised by the types `Vi` which are not `void`, if `Vs` is not
    //!  empty then `Value` is `std::variant<std::monostate, Vs...>`,
    //!  otherwise `Value` is `void`.
    //!
    //! \requires The type `Iterator` shall satisfy ForwardIterator. The types
    //!  `Sentinel` and `Iterator` shall satisfy Sentinel. Each type in the
    //!  parameter pack `Rules` shall satisfy TokenizationRule.
    //!
    //! \preconditions `[first, last)` shall denote a valid range.
    //!
    //! \effects Demarcates a token starting at the beginning of the given
    //!  input range `[first, last)` by applying each of the tokenization
    //!  rules in the order they appear in the parameter pack `rules`, then
    //!  selecting the first lexeme produced such that no other lexeme is
    //!  longer.
    //!
    //! \returns The demarcated token, if any; otherwise, an empty token.
    //!
    //! \remarks The category of the resulting token is the 0-based index in
    //!  the parameter pack `rules` of the rule that demarcated it, if any;
    //!  otherwise `token<Iterator>::no_category`. If the demarcating rule
    //!  produced a value, then the associated value of the resulting token
    //!  is an instance of `std::variant` whose active member is the one
    //!  corresponding to that rule, and its value is the produced value;
    //!  otherwise, the associated value (if any) is default constructed.
    template <
        typename Iterator, typename Sentinel,
        typename ...Rules,
        typename Enable = std::enable_if_t<std::conjunction_v<
            detail::is_tokenization_rule<Rules, Iterator, Sentinel>...>>,
        typename Value = typename detail::tokenization_value<
            Iterator, Sentinel, Rules...>::type>
    token<Iterator, Value> tokenize(
        Iterator first, Sentinel last,
        Rules&&... rules)
    {
        std::size_t const category = token<Iterator, Value>::no_category;
        if (first == last)
            return token<Iterator, Value>{category, first, first};

        return detail::_tokenize<Value>(
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
