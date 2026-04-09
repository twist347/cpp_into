#pragma once

#include "into/core/error.h"

namespace into {
    // Main extension point.
    // To teach `into` how to parse a new type, specialize this template:
    //
    //   template<>
    //   struct into::Caster<MyType, std::string_view> {
    //       static into::Result<MyType> parse(std::string_view);
    //   };
    //
    // Built-in strategies are also partial specializations of `Caster<>`,
    // split by file under include/into/strategies/.
    template<typename To, typename From>
    struct Caster; // primary: undefined
}
