//
//  optionset.hpp
//  toybox
//
//  Created by Fredrik on 2025-10-05.
//

#pragma once

#include "concepts.hpp"

namespace toybox {

    // Specialize for true_type for enum class that is an option set to enable bitwise operators and comparison to bool.
    template<typename T>
    struct is_optionset : false_type {};
    
    template<typename T>
    concept optionset_type = enum_type<T> && is_optionset<T>::value;

    __forceinline constexpr bool operator==(optionset_type auto a, bool b) {
        using T = decltype(a);
        using U = __underlying_type(T);
        return ((U)a != 0) == b;
    }

    __forceinline constexpr bool operator==(bool a, optionset_type auto b) {
        using T = decltype(b);
        using U = __underlying_type(T);
        return ((U)b != 0) == a;
    }

    __forceinline constexpr auto operator|(optionset_type auto a, optionset_type auto b) -> decltype(a) {
        using T = decltype(a);
        using U = __underlying_type(T);
        return (T)((U)a | (U)b);
    }

    __forceinline constexpr auto operator&(optionset_type auto a, optionset_type auto b) -> decltype(a) {
        using T = decltype(a);
        using U = __underlying_type(T);
        return (T)((U)a & (U)b);
    }

    __forceinline constexpr auto operator|=(optionset_type auto& a, optionset_type auto b) -> decltype(a)& {
        a = a | b;
        return a;
    }

    __forceinline constexpr auto operator&=(optionset_type auto& a, optionset_type auto b) -> decltype(a)& {
        a = a & b;
        return a;
    }
    
}
