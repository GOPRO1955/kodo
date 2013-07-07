// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

/// @file test_forward_linear_block_decoder.cpp Unit tests for the
///       kodo::forward_linear_block_decoder

#include <cstdint>
#include <gtest/gtest.h>

#include <kodo/forward_linear_block_decoder.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>

#include <kodo/debug_linear_block_decoder.hpp>

#include "basic_api_test_helper.hpp"

namespace kodo
{
    template<class Field>
    class test_forward_stack
        : public // Payload API
                 // Codec Header API
                 // Symbol ID API
                 // Codec API
                 debug_linear_block_decoder<
                 linear_block_decoder<
                 // Coefficient Storage API
                 coefficient_storage<
                 coefficient_info<
                 // Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Finite Field API
                 finite_field_math<typename fifi::default_field<Field>::type,
                 finite_field_info<Field,
                 // Factory API
                 final_coder_factory_pool<
                 // Final type
                 test_forward_stack<Field>
                     > > > > > > > > > >
    { };

}


/// Run the tests typical coefficients stack
TEST(TestForwardLinearBlockDecoder, test_decoder)
{
    typedef fifi::binary field_type;

    kodo::test_forward_stack<field_type>::factory f(8, 1600);

    auto d = f.build();

    EXPECT_EQ(d->coefficients_size(), 1U);

    uint8_t coefficients[1];

    // Create an encoding vector looking like this: 01000010
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 1, 1U);
    fifi::set_value<field_type>(coefficients, 6, 1U);

    // Create a dummy symbol
    std::vector<uint8_t> symbol = random_vector(d->symbol_size());

    d->decode_symbol(&symbol[0], coefficients);
    d->print_decoder_state(std::cout);

    EXPECT_EQ(d->rank(), 1U);
    EXPECT_TRUE(d->symbol_pivot(6));

    // Create an encoding vector looking like this: 10000010
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 6, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    d->print_decoder_state(std::cout);

    EXPECT_EQ(d->rank(), 2U);
    EXPECT_TRUE(d->symbol_pivot(6));
    EXPECT_TRUE(d->symbol_pivot(1));

    // Create an encoding vector looking like this: 11100010
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 1, 1U);
    fifi::set_value<field_type>(coefficients, 2, 1U);
    fifi::set_value<field_type>(coefficients, 6, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    d->print_decoder_state(std::cout);

    EXPECT_EQ(d->rank(), 3U);
    EXPECT_TRUE(d->symbol_pivot(6));
    EXPECT_TRUE(d->symbol_pivot(1));
    EXPECT_TRUE(d->symbol_pivot(2));

    // Create an encoding vector looking like this: 11100010
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 1, 1U);
    fifi::set_value<field_type>(coefficients, 2, 1U);
    fifi::set_value<field_type>(coefficients, 3, 1U);
    fifi::set_value<field_type>(coefficients, 4, 1U);
    fifi::set_value<field_type>(coefficients, 5, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    d->print_decoder_state(std::cout);

    // Create an encoding vector looking like this: 11100010
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 2, 1U);
    fifi::set_value<field_type>(coefficients, 3, 1U);
    fifi::set_value<field_type>(coefficients, 4, 1U);
    fifi::set_value<field_type>(coefficients, 5, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    d->print_decoder_state(std::cout);

        // Create an encoding vector looking like this: 11100010
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 1, 1U);
    fifi::set_value<field_type>(coefficients, 2, 1U);
    fifi::set_value<field_type>(coefficients, 3, 1U);
    fifi::set_value<field_type>(coefficients, 5, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    d->print_decoder_state(std::cout);

        // Create an encoding vector looking like this: 11100010
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 0, 1U);
    fifi::set_value<field_type>(coefficients, 3, 1U);


    d->decode_symbol(&symbol[0], coefficients);
    d->print_decoder_state(std::cout);


    // Create an encoding vector looking like this: 11100010
    coefficients[0] = 0;
    fifi::set_value<field_type>(coefficients, 3, 1U);
    fifi::set_value<field_type>(coefficients, 4, 1U);
    fifi::set_value<field_type>(coefficients, 7, 1U);

    d->decode_symbol(&symbol[0], coefficients);
    d->print_decoder_state(std::cout);



    EXPECT_EQ(d->rank(), 3U);
    EXPECT_TRUE(d->symbol_pivot(6));
    EXPECT_TRUE(d->symbol_pivot(1));
    EXPECT_TRUE(d->symbol_pivot(2));


}



