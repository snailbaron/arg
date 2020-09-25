#pragma once

#include "arg/adapters.hpp"
#include "arg/arguments.hpp"

#include <memory>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace arg {

struct Config {
    bool allowKeyValueSyntax = true;
    std::string keyValueSeparator = "=";
    bool allowArgumentPacking = true;
    std::string packPrefix = "-";
};

class Parser {
public:
    void attach(Flag flag)
    {
        _options.push_back(std::make_unique<FlagAdapter>(std::move(flag)));
    }

    void attach(MultiFlag multiFlag)
    {
        _options.push_back(
            std::make_unique<MultiFlagAdapter>(std::move(multiFlag)));
    }

    template <class T>
    void attach(Option<T> option)
    {
        _options.push_back(
            std::make_unique<OptionAdapter<T>>(std::move(option)));
    }

    template <class T>
    void attach(MultiOption<T> multiOption)
    {
        _options.push_back(
            std::make_unique<MultiOptionAdapter<T>>(std::move(multiOption)));
    }

    template <class T>
    void attach(Value<T> value)
    {
        _arguments.push_back(std::make_unique<ValueAdapter<T>>(std::move(value)));
    }

    template <class T>
    void attach(MultiValue<T> multiValue)
    {
        _arguments.push_back(
            std::make_unique<MultiValueAdapter<T>>(std::move(multiValue)));
    }

    template <class... Ts>
    Flag flag(Ts&&... keys)
    {
        return addArgumentWithKeys<Flag>(std::forward<Ts>(keys)...);
    }

    template <class... Ts>
    MultiFlag multiFlag(Ts&&... keys)
    {
        return addArgumentWithKeys<MultiFlag>(std::forward<Ts>(keys)...);
    }

    template <class T, class... Ts>
    Option<T> option(Ts&&... keys)
    {
        return addArgumentWithKeys<Option<T>>(std::forward<Ts>(keys)...);
    }

    template <class T, class... Ts>
    MultiOption<T> multiOption(Ts&&... keys)
    {
        return addArgumentWithKeys<MultiOption<T>>(std::forward<Ts>(keys)...);
    }

    template <class T>
    Value<T> argument()
    {
        return addArgumentWithoutKeys<Value<T>>();
    }

    template <class T>
    MultiValue<T> multiArgument()
    {
        return addArgumentWithoutKeys<MultiValue<T>>();
    }

    void help(std::ostream& output) const
    {
        output << "usage: " << _programName;
        for (const auto& option : _options) {
            if (!option->isRequired()) {
                output << " [";
            }
            output << " " << option->firstKey();
            if (option->hasArgument()) {
                output << " " << option->metavar();
            }
            if (!option->isRequired()) {
                output << " ]";
            }
        }
        for (const auto& argument : _arguments) {
            if (!argument->isRequired()) {
                output << " [";
            }
            output << " " << argument->metavar();
            if (!argument->isRequired()) {
                output << " ]";
            }
        }

        if (!_options.empty()) {
            output << "\nOptions:\n";
            for (const auto& option : _options) {
                output << "  " << option->keyString();
                if (option->hasArgument()) {
                    output << " " << option->metavar();
                }
                output << "  " << option->help() << "\n";
            }
        }

        if (!_arguments.empty()) {
            output << "\nPositional arguments:\n";
            for (const auto& argument : _arguments) {
                output << argument->metavar() << "  " <<
                    argument->help() << "\n";
            }
        }
    }

    void parse(int argc, char** argv)
    {
        if (argc > 0) {
            _programName = argv[0];
        }

        std::vector<std::string> args;
        for (int i = 1; i < argc; i++) {
            args.push_back(argv[i]);
        }
        parse(args);
    }

    void parse(const std::vector<std::string>& args)
    {
        for (auto arg = args.begin(); arg != args.end(); ) {
            if (auto option = findOption(*arg); option) {
                if (option->hasArgument()) {
                    ++arg;
                    if (arg == args.end()) {
                        throw std::runtime_error{"no value for " + *arg};
                    }
                    option->addValue(*arg);
                } else {
                    option->raise();
                }
                ++arg;
                continue;
            }

            if (auto pair = parseKeyValue(*arg); pair) {
                if (auto option = findOption(pair->key); option) {
                    if (!option->hasArgument()) {
                        throw std::runtime_error{"option does not accept values"};
                    }
                    option->addValue(pair->value);
                    ++arg;
                    continue;
                }
            }

            if (auto pack = parsePack(*arg); pack) {
                // ...
                continue;
            }

            if (_position < _values.size()) {
                auto value = _values.at(_position);
                value->addValue(*arg);
                if (!value->multi()) {
                    _position++;
                }
                continue;
            }

            if (_collectLevtovers) {
                _leftovers.push_back(*arg);
            } else {
                // TODO: push error
            }
        }
    }

private:
    struct KeyValuePair {
        std::string key;
        std::string value;
    };

    struct KeyPack {
        std::vector<std::string> keys;
        std::string leftover;
    };

    template <class T>
    T addArgumentWithoutKeys()
    {
        T arg;
        attach(arg);
        return arg;
    }

    template <class T, class... Ts>
    T addArgumentWithKeys(Ts&&... keys)
    {
        T arg;
        arg.keys(std::forward<Ts>(keys)...);
        attach(arg);
        return arg;
    }

    // NOTE: linear search here, probably should replace with something better
    // someday
    std::unique_ptr<KeyAdapter> findOption(std::string_view key)
    {
        for (auto it = _options.rbegin(); it != _options.rend(); ++it) {
            if ((*it)->hasKey(key)) {
                return *it;
            }
        }
        return nullptr;
    }

    std::optional<KeyValuePair> parseKeyValue(std::string_view arg)
    {
        if (!_config.allowKeyValueSyntax) {
            return std::nullopt;
        }

        auto sep = arg.find(_config.keyValueSeparator);
        if (sep == std::string_view::npos) {
            return std::nullopt;
        }

        KeyValuePair keyValue;
        keyValue.key = arg.substr(0, sep);
        if (sep + 1 < arg.size()) {
            keyValue.value = arg.substr(sep + 1);
        }
        return keyValue;
    }

    std::optional<KeyPack> parsePack(std::string_view arg)
    {
        if (!_config.allowArgumentPacking) {
            return std::nullopt;
        }

        if (arg.find(_config.packPrefix) != 0) {
            return std::nullopt;
        }

        KeyPack keyPack;
        size_t i = _config.packPrefix.length();
        for (; i < arg.length(); i++) {
        }

    }

    std::vector<std::unique_ptr<KeyAdapter>> _options;
    std::vector<std::unique_ptr<ArgumentAdapter>> _arguments;
    std::string _programName = "<program>";
    Config _config;
};

namespace internal {

inline Parser globalParser;

} // namespace internal

template <class... Ts>
Flag flag(Ts&&... keys)
{
    return internal::globalParser.flag(std::forward<Ts>(keys)...);
}

template <class... Ts>
MultiFlag multiFlag(Ts&&... keys)
{
    return internal::globalParser.multiFlag(std::forward<Ts>(keys)...);
}

template <class T, class... Ts>
Option<T> option(Ts&&... keys)
{
    return internal::globalParser.option<T>(std::forward<Ts>(keys)...);
}

template <class T, class... Ts>
MultiOption<T> multiOption(Ts&&... keys)
{
    return internal::globalParser.multiOption<T>(std::forward<Ts>(keys)...);
}

template <class T>
Value<T> argument()
{
    return internal::globalParser.argument<T>();
}

template <class T>
MultiValue<T> multiArgument()
{
    return internal::globalParser.multiArgument<T>();
}

} // namespace arg
