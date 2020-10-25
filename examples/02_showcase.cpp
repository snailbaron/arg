#include <arg.hpp>

int main(int argc, char* argv[])
{
    auto parser = arg::Parser{};
    auto i = parser.option<int>()
        .keys("-i", "--integer");
    auto u = parser.option<unsigned>()
        .keys("-u", "--unsigned");
    parser.parse(argc, argv);
}
