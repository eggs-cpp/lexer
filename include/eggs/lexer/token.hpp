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
#include <type_traits>
#include <utility>

namespace eggs { namespace lexers
{
    ///////////////////////////////////////////////////////////////////////////
    //! template <class Iterator, class Value = void>
    //! class token : public std::pair<Iterator, Iterator>, see-below;
    //!
    //! Class template `token` represents a categorized lexeme produced by a
    //! Tokenization Rule, along with its associated value (if any).
    //!
    //! \requires The type `Iterator` shall satisfy ForwardIterator.
    //!
    //! \remarks A token class whose `Value` is not `void` derives from
    //!  `token<Iterator>`, in order to enable slicing the associated value.
    template <typename Iterator, typename Value = void>
    class token
      : public token<Iterator>
    {
    public:
        //! using iterator_type = Iterator;
        using iterator_type = Iterator;

        //! using value_type = Value;
        using value_type = Value;

        //! Value value; // only if `Value` is not `void`
        Value value;

    public:
        //! constexpr token();
        //!
        //! \effects Initializes the category to `no_category`, and
        //!  value-initializes the pair base class subobject and the value.
        //!
        //! \remarks This constructor shall not participate in overload
        //!  resolution unless `std::is_default_constructible_v<Value>`
        //!  is `true`, or `Value` is `void`.
        template <
            typename Depend = void, typename Enable = std::enable_if_t<
                std::is_default_constructible_v<Value>, Depend>>
        constexpr token()
          : token<Iterator>()
          , value()
        {}

        //! template <class ...Args>
        //! constexpr token(std::size_t category, Iterator begin, Iterator end, Args&&... args)
        //!
        //! \preconditions `[begin, end)` shall denote a valid range.
        //!
        //! \effects Initializes the category with the given value,
        //!  initializes the pair base class subobject with the given iterator
        //!  pair denoting a lexeme, and initializes the value from the
        //!  given arguments.
        //!
        //! \remarks This constructor shall not participate in overload
        //!  resolution unless `std::is_constructible_v<Value, Args...>`
        //!  is `true`, or `Value` is `void`.
        template <
            typename ...Args, typename Enable = std::enable_if_t<
                std::is_constructible_v<Value, Args...>>>
        constexpr token(
            std::size_t category, Iterator begin, Iterator end,
            Args&&... args)
          : token<Iterator>(category, begin, end)
          , value(std::forward<Args>(args)...)
        {}

        //! template <class UValue>
        //! constexpr EXPLICIT token(token<Iterator, UValue> const& other)
        //!
        //! \effects Initializes this token with the contents from `other`.
        //!
        //! \remarks This constructor shall not participate in overload
        //!  resolution unless `std::is_constructible_v<Value, UValue const&>`
        //!  is `true`, or `Value` is `void`. The constructor is explicit if
        //!  and only if `std::is_convertible_v<UValue const&, Value>` is
        //!  `false`.
        template <
            typename UValue, std::enable_if_t<
                std::is_constructible_v<Value, UValue const&>
             && std::is_convertible_v<UValue const&, Value>,
                bool> = false>
        constexpr token(token<Iterator, UValue> const& other)
          : token<Iterator>(other)
          , value(other.value)
        {};

        template <
            typename UValue, std::enable_if_t<
                std::is_constructible_v<Value, UValue const&>
             && !std::is_convertible_v<UValue const&, Value>,
                bool> = false>
        constexpr explicit token(token<Iterator, UValue> const& other)
          : token<Iterator>(other)
          , value(other.value)
        {};

        //! template <class UValue>
        //! constexpr EXPLICIT token(token<Iterator, UValue>&& other)
        //!
        //! \effects Initializes this token with the contents from `other`.
        //!
        //! \remarks This constructor shall not participate in overload
        //!  resolution unless `std::is_constructible_v<Value, UValue&&>`
        //!  is `true`, or `Value` is `void`. The constructor is explicit if
        //!  and only if `std::is_convertible_v<UValue&&, Value>` is
        //!  `false`.
        template <
            typename UValue, std::enable_if_t<
                std::is_constructible_v<Value, UValue&&>
             && std::is_convertible_v<UValue&&, Value>,
                bool> = false>
        constexpr token(token<Iterator, UValue>&& other)
          : token<Iterator>(std::move(other))
          , value(std::move(other).value)
        {};

        template <
            typename UValue, std::enable_if_t<
                std::is_constructible_v<Value, UValue&&>
             && !std::is_convertible_v<UValue&&, Value>,
                bool> = false>
        constexpr explicit token(token<Iterator, UValue>&& other)
          : token<Iterator>(std::move(other))
          , value(std::move(other).value)
        {};

        //! token(token const&) = default;
        token(token const&) = default;

        //! token(token&&) = default;
        token(token&&) = default;

        //! template <class UValue>
        //! token& operator=(token<Iterator, UValue> const& other)
        //!
        //! \effects Assigns to this token the contents from `other`.
        //!
        //! \remarks This function shall not participate in overload res
        //!  resolution unless `std::is_assignable_v<Value&, UValue const&>`
        //!  is `true`, or `Value` is `void`.
        template <
            typename UValue, typename Enable = std::enable_if_t<
                std::is_assignable_v<Value&, UValue const&>>>
        token& operator=(token<Iterator, UValue> const& other)
        {
            token<Iterator>::operator=(other);
            value = other.value;
            return *this;
        };

        //! template <class UValue>
        //! token& operator=(token<Iterator, UValue>&& other)
        //!
        //! \effects Assigns to this token the contents from `other`.
        //!
        //! \remarks This function shall not participate in overload
        //!  resolution unless `std::is_assignable_v<Value&, UValue&&>`
        //!  is `true`, or `Value` is `void`.
        template <
            typename UValue, typename Enable = std::enable_if_t<
                std::is_assignable_v<Value&, UValue&&>>>
        token& operator=(token<Iterator, UValue>&& other)
        {
            token<Iterator>::operator=(std::move(other));
            value = std::move(other).value;
            return *this;
        }

        //! token& operator=(token const&) = default;
        token& operator=(token const&) = default;

        //! token& operator=(token&&) = default;
        token& operator=(token&&) = default;

        //! constexpr std::size_t category() const noexcept
        //!
        //! \returns The category of this token.
        using token<Iterator>::category;
    };

    template <typename Iterator>
    class token<Iterator>
      : public std::pair<Iterator, Iterator>
    {
    public:
        //! using iterator_type = Iterator;
        using iterator_type = Iterator;

        //! using value_type = void;
        using value_type = void;

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

        //! constexpr token(std::size_t category, Iterator begin, Iterator end)
        //!
        //! \preconditions `[begin, end)` shall denote a valid range.
        //!
        //! \effects Initializes the category with the given value,
        //!  initializes the pair base class subobject with the given iterator
        //!  pair denoting a lexeme.
        constexpr token(
            std::size_t category, Iterator begin, Iterator end)
          : token::pair(begin, end)
          , _category(category)
        {}

        template <typename UValue>
        constexpr token(token<Iterator, UValue> const& other)
          : token::pair(other)
          , _category(other._category)
        {};

        token(token const&) = default;

        template <typename UValue>
        token& operator=(token<Iterator, UValue> const& other)
        {
            token::pair::operator=(other);
            _category = other._category;
            return *this;
        };

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
}}

#include <eggs/lexer/tokenize.hpp>

#endif /*EGGS_LEXER_TOKEN_HPP*/
