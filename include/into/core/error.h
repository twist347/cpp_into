#pragma once

#include <expected>
#include <string_view>

namespace into {
    enum class Error {
        invalid_format, // input string cannot be parsed into the target type
        out_of_range, // value does not fit into the target type's range
        partial_parse, // value parsed, but trailing characters remained
        unrepresentable, // value cannot be represented (NaN -> int, etc.)
        unsupported, // conversion is not supported by any strategy
    };

    template<typename T>
    using Result = std::expected<T, Error>;

    using Status = Result<void>;

    constexpr auto to_string(Error e) noexcept -> std::string_view {
        switch (e) {
            case Error::invalid_format: return "invalid_format";
            case Error::out_of_range: return "out_of_range";
            case Error::partial_parse: return "partial_parse";
            case Error::unrepresentable: return "unrepresentable";
            case Error::unsupported: return "unsupported";
        }
        return "unknown";
    }
}
