#pragma once

#include <concepts>
#include <type_traits>

#include "into/core/error.h"
#include "into/core/traits.h"
#include "into/strategies/optional.h"
#include "into/strategies/tuple.h"
#include "into/strategies/variant.h"

namespace into {
    // Any implicit conversion, except:
    //   - arithmetic <-> arithmetic (numeric strategy, with range checking)
    //   - anything involving an enum (enum strategy, via the underlying type)
    //   - anything involving std::optional / std::variant / tuple-like
    //     (handled by their dedicated strategies)
    template<typename To, typename From>
        requires (!std::same_as<To, From>)
                 && std::convertible_to<const From &, To>
                 && (!(std::is_arithmetic_v<To> && std::is_arithmetic_v<From>))
                 && (!std::is_enum_v<To>)
                 && (!std::is_enum_v<From>)
                 && (!detail::is_optional_v<To>)
                 && (!detail::is_optional_v<From>)
                 && (!detail::is_variant_v<To>)
                 && (!detail::is_variant_v<From>)
                 && (!detail::is_tuple_like_v<To>)
                 && (!detail::is_tuple_like_v<From>)
    struct Caster<To, From> {
        static constexpr auto cast(const From &v) -> Result<To> {
            return static_cast<To>(v);
        }
    };
}
