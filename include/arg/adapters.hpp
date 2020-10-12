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
T read(std::string_view s)
{
    auto value = T{};
    auto stream = std::istringstream{std::string{s}};
    while (stream.good()) {
        stream >> value;
    }
    return value;
}

class KeyAdapter {
public:
    virtual ~KeyAdapter() {}

    virtual bool hasArgument() const = 0;
    virtual bool isRequired() const = 0;
    virtual const std::vector<std::string>& keys() const = 0;
    virtual std::string metavar() const = 0;
    virtual const std::string& help() const = 0;

    virtual void raise() {}
    virtual void addValue(std::string_view) {}

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
                stream << " " << *it;
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
    virtual std::string metavar() const = 0;
    virtual const std::string& help() const = 0;
    virtual bool multi() const = 0;
    virtual void addValue(std::string_view) = 0;
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

    void raise() override
    {
        _flag = true;
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

    void raise() override
    {
        _multiFlag = true;
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

    void addValue(std::string_view s) override
    {
        _option = read<T>(s);
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

    void addValue(std::string_view s) override
    {
        _multiOption.push(read<T>(s));
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

    void addValue(std::string_view s) override
    {
        _value = read<T>(s);
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

    void addValue(std::string_view s) override
    {
        _multiValue.push(read<T>(s));
    }

private:
    MultiValue<T> _multiValue;
};

} // namespace arg
