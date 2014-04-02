// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>
#include <set>
#include <algorithm>

#include <boost/make_shared.hpp>

#include <gauge/gauge.hpp>
#include <gauge/console_printer.hpp>
#include <gauge/python_printer.hpp>
#include <gauge/csv_printer.hpp>
#include <gauge/json_printer.hpp>

#include <kodo/has_systematic_encoder.hpp>
#include <kodo/set_systematic_off.hpp>
#include <kodo/rlnc/full_vector_codes.hpp>
#include <kodo/rlnc/seed_codes.hpp>
#include <kodo/rs/reed_solomon_codes.hpp>

#include <tables/table.hpp>

#include "codes.hpp"

/// A test block represents an encoder and decoder pair
template<class Encoder, class Decoder>
struct throughput_benchmark : public gauge::time_benchmark
{
    typedef typename Encoder::factory encoder_factory;
    typedef typename Encoder::pointer encoder_ptr;

    typedef typename Decoder::factory decoder_factory;
    typedef typename Decoder::pointer decoder_ptr;

    void init()
    {
        gauge::time_benchmark::init();
    }

    void start()
    {
        //m_encoded_symbols = 0;
        //m_decoded_symbols = 0;
        gauge::time_benchmark::start();
    }

    void stop()
    {
        gauge::time_benchmark::stop();
    }

    double measurement()
    {
        // Get the time spent per iteration
        double time = gauge::time_benchmark::measurement();

        gauge::config_set cs = get_current_configuration();
        std::string type = cs.get_value<std::string>("type");
        uint32_t symbol_size = cs.get_value<uint32_t>("symbol_size");
        uint32_t encoded_symbols = cs.get_value<uint32_t>("encoded_symbols");

        // The number of bytes {en|de}coded
        uint64_t total_bytes = 0;

        if(type == "decoder")
        {
            total_bytes = encoded_symbols * symbol_size;
        }
        else if(type == "encoder")
        {
            total_bytes = encoded_symbols * symbol_size;
        }
        else
        {
            assert(0);
        }

        // The bytes per iteration
        uint64_t bytes =
            total_bytes / gauge::time_benchmark::iteration_count();

        return bytes / time; // MB/s for each iteration
    }

    void store_run(tables::table& results)
    {
        if(!results.has_column("throughput"))
            results.add_column("throughput");

        results.set_value("throughput", measurement());
    }

    bool accept_measurement()
    {
        gauge::config_set cs = get_current_configuration();

        std::string type = cs.get_value<std::string>("type");

        if (type == "decoder")
        {
            // If we are benchmarking a decoder we only accept
            // the measurement if the decoding was successful
            if (!m_decoder->is_complete())
            {
                return false;
            }
            // At this point, the output data should be equal to the input data
            assert(std::equal(m_data_out.begin(), m_data_out.end(),
                              m_data_in.begin()));
        }

        // Force only one iteration
        return true;
        //return gauge::time_benchmark::accept_measurement();
    }

    std::string unit_text() const
    {
        return "MB/s";
    }

    void get_options(gauge::po::variables_map& options)
    {
        auto symbols = options["symbols"].as<std::vector<uint32_t>>();
        auto redundancy = options["redundancy"].as<std::vector<double>>();
        auto symbol_size = options["symbol_size"].as<std::vector<uint32_t>>();
        auto types = options["type"].as<std::vector<std::string>>();

        assert(symbols.size() > 0);
        assert(redundancy.size() > 0);
        assert(symbol_size.size() > 0);
        assert(types.size() > 0);

        for (const auto& s : symbols)
        {
            for (const auto& p : symbol_size)
            {
                for (const auto& r : redundancy)
                {
                    for (const auto& t : types)
                    {
                        gauge::config_set cs;
                        cs.set_value<uint32_t>("symbols", s);
                        cs.set_value<uint32_t>("symbol_size", p);
                        cs.set_value<double>("redundancy", r);
                        cs.set_value<std::string>("type", t);

                        uint32_t encoded = (uint32_t)std::ceil(s * r);
                        cs.set_value<uint32_t>("encoded_symbols", encoded);

                        add_configuration(cs);
                    }
                }
            }
        }
    }

    void setup()
    {
        gauge::config_set cs = get_current_configuration();

        uint32_t symbols = cs.get_value<uint32_t>("symbols");
        uint32_t symbol_size = cs.get_value<uint32_t>("symbol_size");
        uint32_t encoded_symbols = cs.get_value<uint32_t>("encoded_symbols");

        // Make the factories fit perfectly otherwise there seems to
        // be problems with memory access i.e. when using a factory
        // with max symbols 1024 with a symbols 16
        m_decoder_factory = std::make_shared<decoder_factory>(
            symbols, symbol_size);

        m_encoder_factory = std::make_shared<encoder_factory>(
            symbols, symbol_size);

        m_decoder_factory->set_symbols(symbols);
        m_decoder_factory->set_symbol_size(symbol_size);

        m_encoder_factory->set_symbols(symbols);
        m_encoder_factory->set_symbol_size(symbol_size);

        m_encoder = m_encoder_factory->build();
        m_decoder = m_decoder_factory->build();

        // Prepare the data buffers
        m_data_in.resize(m_encoder->block_size());
        m_data_out.resize(m_encoder->block_size());
        std::fill_n(m_data_out.begin(), m_data_out.size(), 0);

        for (uint8_t &e : m_data_in)
        {
            e = rand() % 256;
        }

        m_encoder->set_symbols(sak::storage(m_data_in));

        m_decoder->set_symbols(sak::storage(m_data_out));

        // Prepare storage for the encoded payloads
        uint32_t payload_count = encoded_symbols;

        m_payloads.resize(payload_count);
        for (uint32_t i = 0; i < payload_count; ++i)
        {
            m_payloads[i].resize(m_encoder->payload_size());
        }
    }

    void encode_payloads()
    {
        m_encoder->set_symbols(sak::storage(m_data_in));

        // We switch any systematic operations off, because we are only
        // interested in producing coded symbols
        if (kodo::has_systematic_encoder<Encoder>::value)
            kodo::set_systematic_off(m_encoder);

        uint32_t payload_count = m_payloads.size();

        for (uint32_t i = 0; i < payload_count; ++i)
        {
            std::vector<uint8_t> &payload = m_payloads[i];
            m_encoder->encode(&payload[0]);
        }
    }

    void decode_payloads()
    {
        uint32_t payload_count = m_payloads.size();

        for (uint32_t i = 0; i < payload_count; ++i)
        {
            m_decoder->decode(&m_payloads[i][0]);

            if (m_decoder->is_complete())
            {
                return;
            }
        }
    }

    /// Run the encoder
    void run_encode()
    {
        gauge::config_set cs = get_current_configuration();

        uint32_t symbols = cs.get_value<uint32_t>("symbols");
        uint32_t symbol_size = cs.get_value<uint32_t>("symbol_size");

        m_encoder_factory->set_symbols(symbols);
        m_encoder_factory->set_symbol_size(symbol_size);

        // The clock is running
        RUN
        {
            // We have to make sure the encoder is in a "clean" state
            m_encoder->initialize(*m_encoder_factory);

            encode_payloads();
        }
    }

    /// Run the decoder
    void run_decode()
    {
        // Encode some data
        encode_payloads();

        gauge::config_set cs = get_current_configuration();
        uint32_t encoded_symbols = cs.get_value<uint32_t>("encoded_symbols");
        uint32_t symbols = cs.get_value<uint32_t>("symbols");
        uint32_t symbol_size = cs.get_value<uint32_t>("symbol_size");

        // Prepare the data buffer for the decoder
        std::copy(m_data_in.begin(), m_data_in.end(), m_data_out.begin());
        // Randomly delete original symbols that will be restored by processing
        // the encoded symbols
        std::set<uint32_t> erased;
        while (erased.size() < encoded_symbols)
        {
            uint32_t random_symbol = rand() % symbols;
            auto ret = erased.insert(random_symbol);
            // Skip this symbol if it was already included in the erased set
            if (ret.second==false) continue;
            // Zero the symbol
            std::fill_n(m_data_out.begin() + random_symbol * symbol_size,
                symbol_size, 0);
        }

        m_decoder_factory->set_symbols(symbols);
        m_decoder_factory->set_symbol_size(symbol_size);

        // The clock is running
        RUN
        {
            // We have to make sure the decoder is in a "clean" state
            // i.e. no symbols already decoded.
            m_decoder->initialize(*m_decoder_factory);

            m_decoder->set_symbols(sak::storage(m_data_out));

            // Set the existing original symbols
            for (uint32_t i = 0; i < symbols; ++i)
            {
                // Skip the erased symbols
                if (erased.count(i) == 0)
                    m_decoder->decode_symbol(&m_data_out[i * symbol_size], i);
            }

            // Decode the payloads
            decode_payloads();
        }
    }

    void run_benchmark()
    {
        gauge::config_set cs = get_current_configuration();

        std::string type = cs.get_value<std::string>("type");

        if (type == "encoder")
        {
            run_encode();
        }
        else if (type == "decoder")
        {
            run_decode();
        }
        else
        {
            assert(0);
        }
    }

protected:

    /// The decoder factory
    std::shared_ptr<decoder_factory> m_decoder_factory;

    /// The encoder factory
    std::shared_ptr<encoder_factory> m_encoder_factory;

    /// The encoder to use
    encoder_ptr m_encoder;

    /// The decoder to use
    decoder_ptr m_decoder;

    /// The input data
    std::vector<uint8_t> m_data_in;

    /// The output data
    std::vector<uint8_t> m_data_out;

    /// Storage for encoded symbols
    std::vector< std::vector<uint8_t> > m_payloads;
};


/// A test block represents an encoder and decoder pair
template<class Encoder, class Decoder>
struct sparse_throughput_benchmark :
    public throughput_benchmark<Encoder,Decoder>
{
public:

    /// The type of the base benchmark
    typedef throughput_benchmark<Encoder,Decoder> Super;

    /// We need access to the encoder built to adjust the average number of
    /// nonzero symbols
    using Super::m_encoder;

public:

    void get_options(gauge::po::variables_map& options)
    {
        auto symbols = options["symbols"].as<std::vector<uint32_t> >();
        auto redundancy = options["redundancy"].as<std::vector<double> >();
        auto symbol_size = options["symbol_size"].as<std::vector<uint32_t> >();
        auto types = options["type"].as<std::vector<std::string> >();
        auto density = options["density"].as<std::vector<double> >();

        assert(symbols.size() > 0);
        assert(redundancy.size() > 0);
        assert(symbol_size.size() > 0);
        assert(types.size() > 0);
        assert(density.size() > 0);

        for (const auto& s : symbols)
        {
            for (const auto& r : redundancy)
            {
                for (const auto& p : symbol_size)
                {
                    for (const auto& t : types)
                    {
                        for (const auto& d: density)
                        {
                            gauge::config_set cs;
                            cs.set_value<uint32_t>("symbols", s);
                            cs.set_value<uint32_t>("symbol_size", p);
                            cs.set_value<double>("redundancy", r);
                            cs.set_value<std::string>("type", t);

                            uint32_t encoded = (uint32_t)std::ceil(s * r);
                            cs.set_value<uint32_t>("encoded_symbols", encoded);

                            // Add the calculated density
                            cs.set_value<double>("density", d);

                            Super::add_configuration(cs);
                        }
                    }
                }
            }
        }
    }

    void setup()
    {
        Super::setup();

        gauge::config_set cs = Super::get_current_configuration();
        double symbols = cs.get_value<double>("density");
        m_encoder->set_density(symbols);
    }
};


/// Using this macro we may specify options. For specifying options
/// we use the boost program options library. So you may additional
/// details on how to do it in the manual for that library.
BENCHMARK_OPTION(throughput_options)
{
    gauge::po::options_description options;

    std::vector<uint32_t> symbols;
    symbols.push_back(16);
    symbols.push_back(32);
    symbols.push_back(64);

    auto default_symbols =
        gauge::po::value<std::vector<uint32_t>>()->default_value(
            symbols, "")->multitoken();

    std::vector<double> redundancy;
    redundancy.push_back(0.5);

    auto default_redundancy =
        gauge::po::value<std::vector<double>>()->default_value(
            redundancy, "")->multitoken();

    std::vector<uint32_t> symbol_size;
    symbol_size.push_back(1000000);

    auto default_symbol_size =
        gauge::po::value<std::vector<uint32_t>>()->default_value(
            symbol_size, "")->multitoken();

    std::vector<std::string> types;
    types.push_back("encoder");
    types.push_back("decoder");

    auto default_types =
        gauge::po::value<std::vector<std::string> >()->default_value(
            types, "")->multitoken();

    options.add_options()
        ("symbols", default_symbols, "Set the number of symbols");

    options.add_options()
        ("redundancy", default_redundancy, "Set the ratio of repair symbols");

    options.add_options()
        ("symbol_size", default_symbol_size, "Set the symbol size in bytes");

    options.add_options()
        ("type", default_types, "Set type [encoder|decoder]");

    gauge::runner::instance().register_options(options);
}

BENCHMARK_OPTION(sparse_density_options)
{
    gauge::po::options_description options;

    std::vector<double> density;
    density.push_back(0.5);

    auto default_density =
        gauge::po::value<std::vector<double> >()->default_value(
            density, "")->multitoken();

    options.add_options()
        ("density", default_density, "Set the density of the sparse codes");

    gauge::runner::instance().register_options(options);
}

//------------------------------------------------------------------
// FullRLNC
//------------------------------------------------------------------

typedef throughput_benchmark<
    kodo::full_rlnc_encoder_shallow<fifi::binary>,
    kodo::full_rlnc_decoder_shallow<fifi::binary>>
    setup_rlnc_throughput;

BENCHMARK_F(setup_rlnc_throughput, FullRLNC, Binary, 5)
{
    run_benchmark();
}

typedef throughput_benchmark<
    kodo::full_rlnc_encoder_shallow<fifi::binary8>,
    kodo::full_rlnc_decoder_shallow<fifi::binary8>>
    setup_rlnc_throughput8;

BENCHMARK_F(setup_rlnc_throughput8, FullRLNC, Binary8, 5)
{
    run_benchmark();
}

//------------------------------------------------------------------
// BackwardFullRLNC
//------------------------------------------------------------------

typedef throughput_benchmark<
    kodo::full_rlnc_encoder_shallow<fifi::binary>,
    kodo::backward_full_rlnc_decoder_shallow<fifi::binary>>
    setup_backward_rlnc_throughput;

BENCHMARK_F(setup_backward_rlnc_throughput, BackwardFullRLNC, Binary, 5)
{
    run_benchmark();
}

typedef throughput_benchmark<
    kodo::full_rlnc_encoder_shallow<fifi::binary8>,
    kodo::backward_full_rlnc_decoder_shallow<fifi::binary8>>
    setup_backward_rlnc_throughput8;

BENCHMARK_F(setup_backward_rlnc_throughput8, BackwardFullRLNC, Binary8, 5)
{
    run_benchmark();
}

//------------------------------------------------------------------
// FullDelayedRLNC
//------------------------------------------------------------------

typedef throughput_benchmark<
   kodo::full_rlnc_encoder_shallow<fifi::binary>,
   kodo::full_delayed_rlnc_decoder_shallow<fifi::binary>>
   setup_delayed_rlnc_throughput;

BENCHMARK_F(setup_delayed_rlnc_throughput, FullDelayedRLNC, Binary, 5)
{
   run_benchmark();
}

typedef throughput_benchmark<
   kodo::full_rlnc_encoder_shallow<fifi::binary8>,
   kodo::full_delayed_rlnc_decoder_shallow<fifi::binary8>>
   setup_delayed_rlnc_throughput8;

BENCHMARK_F(setup_delayed_rlnc_throughput8, FullDelayedRLNC, Binary8, 5)
{
   run_benchmark();
}

//------------------------------------------------------------------
// SparseFullRLNC
//------------------------------------------------------------------

typedef sparse_throughput_benchmark<
    kodo::sparse_full_rlnc_encoder_shallow<fifi::binary>,
    kodo::full_rlnc_decoder_shallow<fifi::binary>>
    setup_sparse_rlnc_throughput;

BENCHMARK_F(setup_sparse_rlnc_throughput, SparseFullRLNC, Binary, 5)
{
    run_benchmark();
}

typedef sparse_throughput_benchmark<
    kodo::sparse_full_rlnc_encoder_shallow<fifi::binary8>,
    kodo::full_rlnc_decoder_shallow<fifi::binary8>>
    setup_sparse_rlnc_throughput8;

BENCHMARK_F(setup_sparse_rlnc_throughput8, SparseFullRLNC, Binary8, 5)
{
    run_benchmark();
}

int main(int argc, const char* argv[])
{
    srand(static_cast<uint32_t>(time(0)));

    gauge::runner::add_default_printers();
    gauge::runner::run_benchmarks(argc, argv);

    return 0;
}
