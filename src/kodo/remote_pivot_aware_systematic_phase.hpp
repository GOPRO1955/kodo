// Copyright Steinwurf Apes 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdint>
#include <boost/dynamic_bitset.hpp>

namespace kodo
{

    /// @todo docs
    /// @ingroup encoder_layers
    ///
    /// @brief This class is used to detect whether the encoder is in
    /// the systematic phase i.e. whether the next symbol to encode
    /// should be a uncoded systematic symbol. This is done by
    /// tracking which symbols has already been sent systematically
    /// and which symbols are currently available in the storage
    /// layers
    template<class SuperCoder>
    class remote_pivot_aware_systematic_phase : public SuperCoder
    {
    public:

        /// @return true if we are in the systematic phase (i.e. there
        /// are systematic packet to send) otherwise false
        bool in_systematic_phase() const
        {
            if(single_symbol_available())
            {
                return true;
            }
            else
            {
                return SuperCoder::in_systematic_phase();
            }
        }

        /// @return The index of the next symbol to be sent in a
        uint32_t next_systematic_symbol() const
        {
            if(single_symbol_available())
            {
                uint32_t next_symbol = 0;
                bool next_symbol_found = false;

                for(uint32_t i = 0; i < SuperCoder::symbols(); ++i)
                {
                    bool is_remote_pivot =
                        SuperCoder::remote_is_symbol_pivot(i);

                    bool is_pivot = SuperCoder::is_symbol_pivot(i);

                    if(!is_remote_pivot && is_pivot)
                    {
                        next_symbol_found = true;
                        next_symbol = i;
                        break;
                    }
                }

                assert(next_symbol_found);
                return next_symbol;

            }
            else
            {
                return SuperCoder::next_systematic_symbol();
            }

        }

    protected:

        /// @todo docs
        bool single_symbol_available() const
        {
            return SuperCoder::remote_rank() + 1 == SuperCoder::rank();
        }

    };

}

