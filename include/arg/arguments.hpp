#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace arg {

class Flag {
public:
    template <class... Args>
    Flag keys(Args&&... args)
    {
        _data->keys = {std::forward<Args>(args)...};
        return *this;
    }

    const std::vector<std::string>& keys() const
    {
        return _data->keys;
    }

    Flag help(std::string_view s)
    {
        _data->help = s;
        return *this;
    }

    const std::string& help() const
    {
        return _data->help;
    }

    bool operator*() const
    {
        return _data->value;
    }

    operator bool() const
    {
        return **this;
    }

    Flag& operator=(bool value)
    {
        _data->value = value;
        return *this;
    }

private:
    struct Data {
        std::vector<std::string> keys;
        std::string help;
        bool value = false;
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

class MultiFlag {
public:
    template <class... Args>
    MultiFlag keys(Args&&... args)
    {
        _data->keys = {std::forward<Args>(args)...};
        return *this;
    }

    const std::vector<std::string>& keys() const
    {
        return _data->keys;
    }

    MultiFlag help(std::string_view s)
    {
        _data->help = s;
        return *this;
    }

    const std::string& help() const
    {
        return _data->help;
    }

    size_t operator*() const
    {
        return _data->count;
    }

    operator size_t() const
    {
        return **this;
    }

    MultiFlag& operator=(size_t count)
    {
        _data->count = count;
        return *this;
    }

private:
    struct Data {
        std::vector<std::string> keys;
        std::string help;
        size_t count = 0;
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

template <class T>
class Option {
public:
    template <class... Args>
    Option keys(Args&&... args)
    {
        _data->keys = {std::forward<Args>(args)...};
        return *this;
    }

    const std::vector<std::string>& keys() const
    {
        return _data->keys;
    }

    Option help(std::string_view s)
    {
        _data->help = s;
        return *this;
    }

    const std::string& help() const
    {
        return _data->help;
    }

    Option metavar(std::string_view s)
    {
        _data->metavar = s;
        return *this;
    }

    const std::string& metavar() const
    {
        return _data->metavar;
    }

    Option markRequired()
    {
        _data->required = true;
        return *this;
    }

    bool isRequired() const
    {
        return _data->required;
    }

    Option defaultValue(T&& value)
    {
        _data->value = std::forward<T>(value);
        return *this;
    }

    const T& operator*() const
    {
        return _data->value;
    }

    operator const T&() const
    {
        return **this;
    }

    Option& operator=(T&& value)
    {
        _data->value = std::forward<T>(value);
        return *this;
    }

private:
    struct Data {
        std::vector<std::string> keys;
        std::string help;
        std::string metavar;
        bool required = false;
        T value = T{};
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

template <class T>
class MultiOption {
public:
    template <class... Args>
    MultiOption keys(Args&&... args)
    {
        _data->keys = {std::forward<Args>(args)...};
        return *this;
    }

    const std::vector<std::string>& keys() const
    {
        return _data->keys;
    }

    MultiOption help(std::string_view s)
    {
        _data->help = s;
        return *this;
    }

    const std::string& help() const
    {
        return _data->help;
    }

    auto begin() const
    {
        return _data->value.begin();
    }

    auto begin()
    {
        return _data->value.begin();
    }

    auto end() const
    {
        return _data->value.end();
    }

    auto end()
    {
        return _data->value.end();
    }

    void push(T&& value)
    {
        _data->values.push_back(std::forward<T>(value));
    }

private:
    struct Data {
        std::vector<std::string> keys;
        std::string help;
        std::string metavar;
        std::vector<T> values;
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

template <class T>
class Value {
public:
    Value help(std::string_view s)
    {
        _data->help = s;
        return *this;
    }

    const std::string& help() const
    {
        return _data->help;
    }

    Value metavar(std::string_view s)
    {
        _data->metavar = s;
        return *this;
    }

    const std::string& metavar() const
    {
        return _data->metavar;
    }

    Value markRequired()
    {
        _data->required = true;
        return *this;
    }

    bool isRequired() const
    {
        return _data->required;
    }

    Value defaultValue(T&& value)
    {
        _data->value = std::forward<T>(value);
        return *this;
    }

    const T& operator*() const
    {
        return _data->value;
    }

    operator const T&() const
    {
        return **this;
    }

    Value& operator=(T&& value)
    {
        _data->value = std::forward<T>(value);
        return *this;
    }

private:
    struct Data {
        std::string help;
        std::string metavar;
        bool required = false;
        T value = T{};
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

template <class T>
class MultiValue {
public:
    MultiValue help(std::string_view s)
    {
        _data->help = s;
        return *this;
    }

    const std::string& help() const
    {
        return _data->help;
    }

    auto begin() const
    {
        return _data->value.begin();
    }

    auto begin()
    {
        return _data->value.begin();
    }

    auto end() const
    {
        return _data->value.end();
    }

    auto end()
    {
        return _data->value.end();
    }

    void push(T&& value)
    {
        _data->values.push_back(std::forward<T>(value));
    }

private:
    struct Data {
        std::string help;
        std::string metavar;
        std::vector<T> values;
    };

    std::shared_ptr<Data> _data = std::make_shared<Data>();
};

template <class T> struct is_argument : std::false_type {};
template <class T> inline constexpr bool is_argument_v = is_argument<T>::value;

template <> struct is_argument<Flag> : std::true_type {};
template <> struct is_argument<MultiFlag> : std::true_type {};
template <class T> struct is_argument<Option<T>> : std::true_type {};
template <class T> struct is_argument<MultiOption<T>> : std::true_type {};
template <class T> struct is_argument<Value<T>> : std::true_type {};
template <class T> struct is_argument<MultiValue<T>> : std::true_type {};

} // namespace arg
