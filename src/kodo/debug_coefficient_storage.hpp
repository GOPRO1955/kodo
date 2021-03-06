// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <iostream>

#include <fifi/fifi_utils.hpp>

namespace kodo
{
    /// @ingroup debug
    /// @ingroup coefficient_storage_layers
    ///
    /// @brief Print functions for coefficient storage
    ///
    /// This layer implements useful functions to print stored coefficients
    template<class SuperCoder>
    class debug_coefficient_storage : public SuperCoder
    {
    public:

        /// @copydoc layer::field_type
        typedef typename SuperCoder::field_type field_type;

        /// @copydoc layer::value_type
        typedef typename field_type::value_type value_type;

    public:

        /// Prints the decoding matrix to the output stream
        /// @param out The output stream to print to
        void print_coefficient_vector_values(std::ostream& out)
        {
            for(uint32_t i = 0; i < SuperCoder::coefficient_vectors(); ++i)
            {
                print_coefficient_vector_value(out, i);
            }
        }

        /// Prints a vector of coefficients
        /// @param out The output stream to print to
        /// @param index The index of the coefficients vector to print
        void print_coefficient_vector_value(std::ostream& out, uint32_t index)
        {
            out << index << ":\t";

            const value_type* c = SuperCoder::coefficient_vector_values(index);

            uint32_t elements = SuperCoder::coefficient_vector_elements();
            for(uint32_t j = 0; j < elements; ++j)
            {
                value_type value = SuperCoder::coefficient_value(c, j);

                static_assert(sizeof(uint32_t) >= sizeof(value_type),
                              "value_type will overflow in this print");

                out << (uint32_t) value << "\t";
            }

            out << std::endl;
        }

    };

}

