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
#include <kodo/sparse_uniform_generator.hpp>

namespace kodo
{
    /// @ingroup fec_stacks
    /// @brief Complete stack implementing a shallow storage sparse RLNC
    ///        decoder.
    ///
    /// The decoder is identical to the full_rlnc_decoder except for
    /// the fact that is uses a shallow storage layer. Furthermore the
    /// RLNC encoder using a density based random generator, which can be
    /// used to control the density i.e. the number of non-zero elements in
    /// the encoding vector.
    template<class Field>
    class shallow_sparse_full_rlnc_encoder : public
        // Payload Codec API
        payload_encoder<
        // Codec Header API
        default_on_systematic_encoder<
        symbol_id_encoder<
        // Symbol ID API
        plain_symbol_id_writer<
        // Coefficient Generator API
        sparse_uniform_generator<
        // Encoder API
        encode_symbol_tracker<
        zero_symbol_encoder<
        linear_block_encoder<
        storage_aware_encoder<
        // Coefficient Storage API
        coefficient_value_access<
        coefficient_info<
        // Symbol Storage API
        partial_shallow_symbol_storage<
        storage_bytes_used<
        storage_block_info<
        // Finite Field API
        finite_field_math<typename fifi::default_field<Field>::type,
        finite_field_info<Field,
        // Factory API
        final_coder_factory_pool<
        // Final type
        shallow_sparse_full_rlnc_encoder<Field
        > > > > > > > > > > > > > > > > > >
    { };
}
