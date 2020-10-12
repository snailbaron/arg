#include <arg.hpp>

#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    auto string = arg::option<std::string>()
        .keys("-s", "--string")
        .markRequired()
        .help("a string to print");
    auto number = arg::option<int>()
        .keys("-n", "--number")
        .defaultValue(3)
        .help("number of times to print the string");
    arg::parse(argc, argv);

    for (int i = 0; i < number; i++) {
        std::cout << string << "\n";
    }
}
