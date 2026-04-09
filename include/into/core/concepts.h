#pragma once

#include <concepts>
#include <type_traits>

#include "into/core/error.h"
#include "into/core/traits.h"

namespace into {
    template<typename To, typename From>
    concept Castable = requires(const std::remove_cvref_t<From> &v)
    {
        {
            Caster<std::remove_cvref_t<To>, std::remove_cvref_t<From> >::cast(v)
        }
        -> std::same_as<Result<std::remove_cvref_t<To> > >;
    };
}
