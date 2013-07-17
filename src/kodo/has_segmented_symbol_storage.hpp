// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "segmented_symbol_storage.hpp"

namespace kodo
{
    /// Type trait helper allows compile time detection of whether an
    /// encoder / decoder contains the segmented_symbol_storage layer
    ///
    /// Example:
    ///
    /// typedef kodo::full_rlnc8_encoder encoder_t;
    ///
    /// if(kodo::has_segmented_symbol_storage<encoder_t>::value)
    /// {
    ///     // Do something here
    /// }
    ///
    template<class T>
    struct has_segmented_symbol_storage
    {
        template<class U>
        static uint8_t test(const kodo::segmented_symbol_storage<U> *);

        static uint32_t test(...);

        static const bool value = sizeof(test(static_cast<T*>(0))) == 1;
    };


}


