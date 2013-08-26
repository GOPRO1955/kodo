// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <iostream>

#include "has_print_cached_symbol_coefficients.hpp"

namespace kodo
{

    /// @ingroup generic_api
    /// Generic function overload for cases where print_decoder_state is part
    /// of a stack. @see layer::print_cached_symbol_coefficients(std::ostream&) const
    /// @param t The stack to query
    /// @param out The output stream where we will write the debug info
    template<class T>
    inline void print_cached_symbol_coefficients(const T& t, std::ostream& out)
    {
        print_cached_symbol_coefficients<has_print_cached_symbol_coefficients<T>::value>(t,out);
    }

    /// @ingroup generic_api
    /// @copydoc print_cached_symbol_coefficients(const T&, std::ostream&)
    template<class T>
    inline void print_cached_symbol_coefficients(const boost::shared_ptr<T>& t,
                                         std::ostream& out)
    {
        print_cached_symbol_coefficients(*t, out);
    }

    /// @ingroup generic_api
    /// @copydoc print_cached_symbol_coefficients(const T&)
    template<bool what, class T>
    inline void print_cached_symbol_coefficients(const T& t, std::ostream& out,
                                         char (*)[what] = 0)
    {
        // Explanation for the char (*)[what] here:
        // http://stackoverflow.com/a/6917354/1717320
        t.print_cached_symbol_coefficients(out);
    }

    /// @ingroup generic_api
    /// @copydoc print_cached_symbol_coefficients(const T&)
    template<bool what, class T>
    inline void print_cached_symbol_coefficients(const T& t, std::ostream& out,
                                         char (*)[!what] = 0)
    {
        (void) t;
        (void) out;

        // We do the assert here - to make sure that this call is not
        // silently ignored in cases where the stack does not have the
        // print_cached_symbol_coefficients() function. However, this assert can
        // be avoided by using the has_print_cached_symbol_coefficients
        assert(0);
    }

}


