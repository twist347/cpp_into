#pragma once

#include <charconv>
#include <concepts>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>

#include "into/core/error.h"
#include "into/core/traits.h"

namespace into {
    // string_view -> arithmetic (via from_chars)
    template<typename To>
        requires std::is_arithmetic_v<To>
    struct Caster<To, std::string_view> {
        static constexpr auto cast(std::string_view s) noexcept -> Result<To> {
            To out{};
            const auto *first = s.data();
            const auto *last = s.data() + s.size();
            const auto [p, ec] = std::from_chars(first, last, out);
            if (ec == std::errc::invalid_argument) {
                return std::unexpected{Error::invalid_format};
            }
            if (ec == std::errc::result_out_of_range) {
                return std::unexpected{Error::out_of_range};
            }
            if (p != last) {
                return std::unexpected{Error::partial_parse};
            }
            return out;
        }
    };

    // std::string -> arithmetic (delegates to string_view)
    template<typename To>
        requires std::is_arithmetic_v<To>
    struct Caster<To, std::string> {
        static auto cast(const std::string &s) noexcept -> Result<To> {
            return Caster<To, std::string_view>::cast(std::string_view{s});
        }
    };

    // string_view -> std::string: std::string's converting ctor is explicit, so the
    // implicit strategy skips it; provide it directly here.
    template<>
    struct Caster<std::string, std::string_view> {
        static auto cast(std::string_view s) -> Result<std::string> {
            return std::string{s};
        }
    };

    // arithmetic -> std::string (via to_chars)
    template<typename From>
        requires std::is_arithmetic_v<From>
    struct Caster<std::string, From> {
        static auto cast(const From &v) -> Result<std::string> {
            char buf[64];
            const auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), v);
            if (ec != std::errc{}) {
                return std::unexpected{Error::unrepresentable};
            }
            return std::string(buf, p);
        }
    };
}
