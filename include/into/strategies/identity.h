#pragma once

#include "into/core/error.h"
#include "into/core/traits.h"

namespace into {
    template<typename T>
    struct Caster<T, T> {
        static constexpr auto cast(const T &v) -> Result<T> {
            return v;
        }
    };
}
