// Copyright Steinwurf ApS 2011-2014.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <kodo/default_on_systematic_encoder.hpp>
#include <kodo/has_shallow_symbol_storage.hpp>
#include <kodo/linear_block_decoder_delayed.hpp>
#include <kodo/partial_shallow_symbol_storage.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/shallow_symbol_storage.hpp>

namespace kodo
{
    /// @ingroup fec_stacks
    /// @brief Complete stack implementing a shallow storage delayed RLNC
    ///        decoder.
    ///
    /// The decoder is identical to the full_rlnc_decoder except for
    /// the fact that is uses a shallow storage layer. Furthermore the
    /// decoder uses a delayed Gaussian elimination decoder.  Delayed
    /// here refers to the fact the we will not perform the backwards
    /// substitution until full rank is reached. This has the
    /// advantage that it should overall reduce the necessary number
    /// of finite field operations needed - this is obtained by
    /// reducing the so called fill-in effect. Fill-in refers to the
    /// fact that by performing row operations we may change an
    /// element previously zero into a non-zero element. Fill-in is
    /// also most often an effect seen with sparse codes.
    template<class Field>
    class shallow_full_delayed_rlnc_decoder : public
        // Payload API
        payload_recoder<recoding_stack,
        payload_decoder<
        // Codec Header API
        systematic_decoder<
        symbol_id_decoder<
        // Symbol ID API
        plain_symbol_id_reader<
        // Decoder API
        aligned_coefficients_decoder<
        linear_block_decoder_delayed<
        forward_linear_block_decoder<
        symbol_decoding_status_counter<
        symbol_decoding_status_tracker<
        // Coefficient Storage API
        coefficient_value_access<
        coefficient_storage<
        coefficient_info<
        // Storage API
        mutable_shallow_symbol_storage<
        storage_bytes_used<
        storage_block_info<
        // Finite Field API
        finite_field_math<typename fifi::default_field<Field>::type,
        finite_field_info<Field,
        // Factory API
        final_coder_factory_pool<
        // Final type
        shallow_full_delayed_rlnc_decoder<Field>
        > > > > > > > > > > > > > > > > > > >
    { };
}
