#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

#include "into/core/error.h"
#include "into/core/cast.h"
#include "into/core/traits.h"

namespace into {
    // enum -> arithmetic: take the underlying value, then delegate to the numeric strategy.
    template<typename To, typename From>
        requires std::is_enum_v<From> && std::is_arithmetic_v<To>
    struct Caster<To, From> {
        static constexpr auto cast(const From &v) noexcept -> Result<To> {
            return into::cast<To>(std::to_underlying(v));
        }
    };

    // arithmetic -> enum: range-check against the underlying type, then cast.
    template<typename To, typename From>
        requires std::is_enum_v<To> && std::is_arithmetic_v<From>
    struct Caster<To, From> {
        static constexpr auto cast(const From &v) noexcept -> Result<To> {
            using U = std::underlying_type_t<To>;
            const auto u = into::cast<U>(v);
            if (!u) {
                return std::unexpected{u.error()};
            }
            return static_cast<To>(*u);
        }
    };

    // enum -> enum (different): go through the source's underlying type.
    template<typename To, typename From>
        requires std::is_enum_v<To> && std::is_enum_v<From> && (!std::same_as<To, From>)
    struct Caster<To, From> {
        static constexpr auto cast(const From &v) noexcept -> Result<To> {
            return into::cast<To>(std::to_underlying(v));
        }
    };
}
