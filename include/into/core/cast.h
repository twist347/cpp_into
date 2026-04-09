#pragma once

#include <type_traits>

#include "into/core/concepts.h"
#include "into/core/error.h"
#include "into/core/traits.h"

namespace into {
    template<typename To, typename From>
        requires Castable<To, From>
    [[nodiscard]] constexpr auto cast(From &&from)
        -> Result<std::remove_cvref_t<To> > {
        using T = std::remove_cvref_t<To>;
        using F = std::remove_cvref_t<From>;
        return Caster<T, F>::cast(static_cast<const F &>(from));
    }
}
