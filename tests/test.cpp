#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <arg.hpp>

TEST_CASE()
{
    auto f = arg::flag("-v")
        .help("some flag");
    auto x = arg::option<int>("-x", "--value")
        .required()
        .help("some value");
    auto y = arg::option<int>("-y", "--another-value")
        .def(10)
        .metavar("SOMETHING")
        .help("another value");
    auto z = arg::value()
        .metavar("PATH")
}
