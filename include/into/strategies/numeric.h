#pragma once

#include <cmath>
#include <concepts>
#include <limits>
#include <type_traits>
#include <utility>

#include "into/core/error.h"
#include "into/core/traits.h"

namespace into {
    // Rust TryFrom semantics: error if the value cannot be represented losslessly.
    //   - int   -> int   : range check
    //   - int   -> float : allowed (matches Rust's From)
    //   - float -> float : allowed
    //   - float -> int   : error on NaN/inf, out-of-range, or non-zero fractional part
    template<typename To, typename From>
        requires std::is_arithmetic_v<To>
                 && std::is_arithmetic_v<From>
                 && (!std::same_as<To, From>)
    struct Caster<To, From> {
        static constexpr auto cast(const From &v) noexcept -> Result<To> {
            using lim = std::numeric_limits<To>;

            if constexpr (std::is_floating_point_v<From> && std::is_integral_v<To>) {
                if (!std::isfinite(v)) {
                    return std::unexpected{Error::unrepresentable};
                }
                if (v != std::trunc(v)) {
                    return std::unexpected{Error::unrepresentable};
                }
                using cmp = long double;
                if (static_cast<cmp>(v) < static_cast<cmp>(lim::lowest()) ||
                    static_cast<cmp>(v) > static_cast<cmp>(lim::max())) {
                    return std::unexpected{Error::out_of_range};
                }
                return static_cast<To>(v);
            } else if constexpr (std::is_floating_point_v<To>) {
                return static_cast<To>(v);
            } else {
                // int -> int: std::in_range handles signed/unsigned and width safely.
                if (!std::in_range<To>(v)) {
                    return std::unexpected{Error::out_of_range};
                }
                return static_cast<To>(v);
            }
        }
    };
}
