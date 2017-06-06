//! \file eggs/lexer.hpp
// Eggs.Lexer
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2017
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_LEXER_HPP
#define EGGS_LEXER_HPP

#include <eggs/lexer/lexer.hpp>
#include <eggs/lexer/token.hpp>
#include <eggs/lexer/tokenize.hpp>

namespace eggs
{
    //! template <class Iterator>
    //! using lexical_token = lexers::token<Iterator>;
    template <typename Iterator>
    using lexical_token = lexers::token<Iterator>;

    //! using lexers::tokenize;
    using lexers::tokenize;

    //! using lexers::lexer;
    using lexers::lexer;
}

#endif /*EGGS_LEXER_HPP*/
