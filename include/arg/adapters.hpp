#pragma once

#include "arg/arguments.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

namespace arg {

template <class T>
bool read(std::string_view input, T& value)
{
    auto stream = std::istringstream{std::string{input}};
    stream >> value;
    return !!stream;
}

bool read(std::string_view input, std::string& string)
{
    auto stream = std::istringstream{std::string{input}};
    // This is probably inefficient.
    string = {std::istreambuf_iterator<char>(stream), {}};
    return true;
}

class KeyAdapter {
public:
    virtual ~KeyAdapter() {}

    virtual bool hasArgument() const = 0;
    virtual bool isRequired() const = 0;
    virtual bool isSet() const = 0;
    virtual const std::vector<std::string>& keys() const = 0;
    virtual std::string metavar() const = 0;
    virtual const std::string& help() const = 0;

    virtual void raise() = 0;
    virtual bool addValue(std::string_view) = 0;

    std::string firstKey() const
    {
        return keys().empty() ? "<no key>" : keys().front();
    }

    std::string keyString() const
    {
        std::ostringstream stream;
        if (auto it = keys().begin(); it != keys().end()) {
            stream << *it++;
            for (; it != keys().end(); ++it) {
                stream << ", " << *it;
            }
        }
        return stream.str();
    }

    bool hasKey(std::string_view s) const
    {
        return std::find(keys().begin(), keys().end(), s) != keys().end();
    }
};

class ArgumentAdapter {
public:
    virtual ~ArgumentAdapter() {}

    virtual bool isRequired() const = 0;
    virtual bool isSet() const = 0;
    virtual std::string metavar() const = 0;
    virtual const std::string& help() const = 0;
    virtual bool multi() const = 0;
    virtual bool addValue(std::string_view) = 0;
};

class FlagAdapter : public KeyAdapter {
public:
    FlagAdapter(Flag&& flag)
        : _flag(std::move(flag))
    { }

    bool hasArgument() const override
    {
        return false;
    }

    bool isRequired() const override
    {
        return false;
    }

    bool isSet() const override
    {
        throw std::logic_error{"FlagAdapter's isSet() must not be called"};
    }

    void raise() override
    {
        _flag = true;
    }

    bool addValue(std::string_view) override
    {
        throw std::logic_error{"FlagAdapter's addValue must not be called"};
    }

    const std::vector<std::string>& keys() const override
    {
        return _flag.keys();
    }

    std::string metavar() const override
    {
        return "";
    }

    const std::string& help() const override
    {
        return _flag.help();
    }

private:
    Flag _flag;
};

class MultiFlagAdapter : public KeyAdapter {
public:
    MultiFlagAdapter(MultiFlag multiFlag)
        : _multiFlag(std::move(multiFlag))
    { }

    bool hasArgument() const override
    {
        return false;
    }

    bool isRequired() const override
    {
        return false;
    }

    bool isSet() const override
    {
        throw std::logic_error{"MultiFlagAdapter's isSet() must not be called"};
    }

    void raise() override
    {
        _multiFlag = true;
    }

    bool addValue(std::string_view) override
    {
        throw std::logic_error{"MultiFlagAdapter's addValue must not be called"};
    }

    const std::vector<std::string>& keys() const override
    {
        return _multiFlag.keys();
    }

    std::string metavar() const override
    {
        return "";
    }

    const std::string& help() const override
    {
        return _multiFlag.help();
    }

private:
    MultiFlag _multiFlag;
};

template <class T>
class OptionAdapter : public KeyAdapter {
public:
    OptionAdapter(Option<T>&& option)
        : _option(std::move(option))
    { }

    bool hasArgument() const override
    {
        return true;
    }

    bool isRequired() const override
    {
        return _option.isRequired();
    }

    bool isSet() const override
    {
        return _option.isSet();
    }

    void raise() override
    {
        throw std::logic_error{"OptionAdapter's addValue must not be called"};
    }

    bool addValue(std::string_view s) override
    {
        auto value = T{};
        if (read(s, value)) {
            _option = std::move(value);
            return true;
        } else {
            return false;
        }
    }

    const std::vector<std::string>& keys() const override
    {
        return _option.keys();
    }

    std::string metavar() const override
    {
        return _option.metavar();
    }

    const std::string& help() const override
    {
        return _option.help();
    }

private:
    Option<T> _option;
};

template <class T>
class MultiOptionAdapter : public KeyAdapter {
public:
    MultiOptionAdapter(MultiOption<T>&& multiOption)
        : _multiOption(std::move(multiOption))
    { }

    bool hasArgument() const override
    {
        return true;
    }

    bool isRequired() const override
    {
        return false;
    }

    bool isSet() const override
    {
        throw std::logic_error{"MultiOptionAdapter's isSet() must not be called"};
    }

    void raise() override
    {
        throw std::logic_error{"MultiOptionAdapter's addValue must not be called"};
    }

    bool addValue(std::string_view s) override
    {
        auto value = T{};
        if (read(s, value)) {
            _multiOption.push(std::move(value));
            return true;
        } else {
            return false;
        }
    }

    const std::vector<std::string>& keys() const override
    {
        return _multiOption.keys();
    }

    std::string metavar() const override
    {
        return _multiOption.metavar();
    }

    const std::string& help() const override
    {
        return _multiOption.help();
    }

private:
    MultiOption<T> _multiOption;
};

template <class T>
class ValueAdapter : public ArgumentAdapter {
public:
    ValueAdapter(Value<T>&& value)
        : _value(std::move(value))
    { }

    bool isRequired() const override
    {
        return _value.isRequired();
    }

    bool isSet() const override
    {
        return _value.isSet();
    }

    std::string metavar() const override
    {
        return _value.metavar();
    }

    const std::string& help() const override
    {
        return _value.help();
    }

    bool multi() const override
    {
        return false;
    }

    bool addValue(std::string_view s) override
    {
        auto value = T{};
        if (read(s, value)) {
            _value = std::move(value);
            return true;
        } else {
            return false;
        }
    }

private:
    Value<T> _value;
};

template <class T>
class MultiValueAdapter : public ArgumentAdapter {
    MultiValueAdapter(MultiValue<T>&& multiValue)
        : _multiValue(std::move(multiValue))
    { }

    bool isRequired() const override
    {
        return _multiValue.isRequired();
    }

    bool isSet() const override
    {
        throw std::logic_error{"MultiValueAdapter's isSet() must not be called"};
    }

    std::string metavar() const override
    {
        return _multiValue.metavar();
    }

    const std::string& help() const override
    {
        return _multiValue.help();
    }

    bool multi() const override
    {
        return true;
    }

    bool addValue(std::string_view s) override
    {
        auto value = T{};
        if (read(s, value)) {
            _multiValue.push(std::move(value));
            return true;
        } else {
            return false;
        }
    }

private:
    MultiValue<T> _multiValue;
};

} // namespace arg
