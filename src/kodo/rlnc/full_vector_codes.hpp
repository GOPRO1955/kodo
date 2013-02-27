// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_RLNC_FULL_VECTOR_CODES_HPP
#define KODO_RLNC_FULL_VECTOR_CODES_HPP

#include <cstdint>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/make_shared.hpp>

#include <fifi/fifi_utils.hpp>
#include <fifi/default_field_impl.hpp>
#include <fifi/is_binary.hpp>

#include "../final_coder_factory_pool.hpp"
#include "../final_coder_factory.hpp"
#include "../finite_field_math.hpp"
#include "../zero_symbol_encoder.hpp"
#include "../systematic_encoder.hpp"
#include "../non_systematic_encoder.hpp"
#include "../systematic_decoder.hpp"
#include "../storage_bytes_used.hpp"
#include "../storage_block_info.hpp"
#include "../partial_shallow_symbol_storage.hpp"
#include "../deep_symbol_storage.hpp"
#include "../generators/block.hpp"
#include "../generators/block_cache.hpp"
#include "../generators/block_cache_lookup.hpp"
#include "../payload_encoder.hpp"
#include "../payload_decoder.hpp"
#include "../align_coefficient_decoder.hpp"
#include "../align_symbol_id_encoder.hpp"
#include "../symbol_id_encoder.hpp"
#include "../symbol_id_decoder.hpp"
#include "../random_uniform_symbol_id.hpp"
#include "../coefficient_storage.hpp"
#include "../coefficient_info.hpp"

#include "../linear_block_encoder.hpp"
#include "../linear_block_decoder.hpp"
#include "../linear_block_decoder_delayed.hpp"
#include "../linear_block_vector_storage.hpp"
#include "../linear_block_vector_generator.hpp"

#include "../../kodo_debug/full_vector_decoder_debug.hpp"

#include "full_vector_encoder.hpp"
#include "full_vector_decoder.hpp"
#include "full_vector_recoder.hpp"
#include "full_vector_payload_recoder.hpp"
#include "full_vector_systematic_recoder.hpp"

namespace kodo
{

    /// @copydoc full_vector_encoder
    template<class Field>
    class full_rlnc_encoder
        : public // Payload Codec API
                 payload_encoder<
                 // Codec Header API
                 systematic_encoder<
                 symbol_id_encoder<
                 // Symbol ID API
                 random_uniform_symbol_id<
                 // Codec API
                 zero_symbol_encoder<
                 linear_block_encoder<
                 coefficient_info<
                 // Finite Field Math API
                 finite_field_math<fifi::default_field_impl,
                 // Symbol Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Factory API
                 final_coder_factory_pool<full_rlnc_encoder<Field>, Field>
                     > > > > > > > > > > >
    {};

    /// Intermediate layer utilized by the re-coding functionality
    /// of a RLNC decoder. This allows us to re-use layers from the
    /// RLNC encoder to build a RLNC recoder
    template<class SuperCoder>
    class recode_proxy
        : public payload_encoder<
                 non_systematic_encoder<
                 align_symbol_id_encoder<
                 zero_symbol_encoder<SuperCoder
                     > > > >
    {};

    /// @copydoc full_vector_decoder
    template<class Field>
    class full_rlnc_decoder
        : public full_vector_recoder<recode_proxy, random_uniform,
                 // Payload API
                 payload_decoder<
                 // Codec Header API
                 systematic_decoder<
                 symbol_id_decoder<
                 // Symbol ID API
                 random_uniform_symbol_id<
                 // Codec API
                 align_coefficient_decoder<
                 full_vector_decoder<
                 linear_block_decoder<
                 linear_block_vector_storage<
                 coefficient_storage<
                 coefficient_info<
                 // Finite Field Math API
                 finite_field_math<fifi::default_field_impl,
                 // Storage API
                 deep_symbol_storage<
                 storage_bytes_used<
                 storage_block_info<
                 // Factory API
                 final_coder_factory_pool<full_rlnc_decoder<Field>, Field>
                     > > > > > > > > > > > > > > >
    {};


    /// A RLNC decoder with delayed backwards substitute. The decoder decodes
    /// according to a full encoding vector.
    // template<class Field>
    // class full_rlnc_decoder_delayed
    //     : public full_vector_recoder<recode_proxy, random_uniform,
    //              payload_decoder<
    //              systematic_decoder<
    //              align_symbol_id_decoder<
    //              full_vector_decoder<
    //              linear_block_decoder_delayed<
    //              linear_block_decoder<
    //              linear_block_vector_storage<
    //              finite_field_math<fifi::default_field_impl,
    //              deep_symbol_storage<
    //              storage_bytes_used<
    //              storage_block_info<
    //              final_coder_factory_pool<full_rlnc_decoder_delayed<Field>, Field>
    //                  > > > > > > > > > > > >
    // {};
}

#endif

