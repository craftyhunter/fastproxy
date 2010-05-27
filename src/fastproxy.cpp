#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#include <iostream>
#include <algorithm>
#include <boost/asio.hpp>
#include <boost/system/linux_error.hpp>
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/log/filters.hpp>

#include "common.hpp"
#include "proxy.hpp"

using namespace boost::asio;
namespace logging = boost::log;

namespace po = boost::program_options;

po::variables_map parse_config(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")("listen", po::value<boost::uint16_t>(), "listening port");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("listen"))
    {
        std::cout << desc << std::endl;
        exit(1);
    }

    return vm;
}

void init_logging()
{
    logging::add_common_attributes();
    logging::init_log_to_console
    (
            std::clog,
            logging::keywords::format = "[%TimeStamp%]: %Channel%: %_%"
    );
//    boost::log::core::get()->set_filter
//    (
//        boost::log::filters::attr<boost::log::trivial::severity_level>("Severity") >= boost::log::trivial::debug
//    );
}

int main(int argc, char* argv[])
{
    po::variables_map vm = parse_config(argc, argv);
    init_logging();

    ip::tcp::endpoint inbound(ip::tcp::v4(), vm["listen"].as<boost::uint16_t> ());

    io_service io;
    proxy p(io, inbound);
    p.start_accept();

    io.run();

    return 0;
}
