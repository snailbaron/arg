#pragma once

#include <ostream>
#include <string>
#include <type_traits>
#include <variant>

namespace arg::err {

struct RequiredOptionValueNotGiven {
    std::string key;
};

struct UnexpectedArgument {
    std::string argument;
};

struct UnexpectedOptionValueGiven {
    std::string key;
    std::string value;
};

using Error = std::variant<
    RequiredOptionValueNotGiven,
    UnexpectedArgument,
    UnexpectedOptionValueGiven
>;

inline void print(std::ostream& output, const Error& error)
{
    std::visit([&output] (auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same<T, RequiredOptionValueNotGiven>()) {
            output << "option " << arg.key <<
                " requires a value, but it was not provied\n";
        } else if constexpr (std::is_same<T, UnexpectedArgument>()) {
            output << "unexpected argument: " << arg.argument << "\n";
        } else if constexpr (std::is_same<T, UnexpectedOptionValueGiven>()) {
            output << "option " << arg.key <<
                " does not require a value, but " << arg.value <<
                " was provided\n";
        }
    }, error);
}

} // namespace arg::err
