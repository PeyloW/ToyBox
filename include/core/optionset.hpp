//
//  optionset.hpp
//  toybox
//
//  Created by Fredrik on 2025-10-05.
//

#pragma once

#include "core/concepts.hpp"

namespace toybox {

    // Specialize for true_type for enum class that is an option set to enable bitwise operators and comparison to bool.
    template<typename T>
    struct is_optionset : false_type {};
    
    template<typename T>
    concept optionset_type = enum_type<T> && is_optionset<T>::value;

    template<typename T>
    __forceinline constexpr auto operator==(T a, bool b) -> enable_if<optionset_type<T>, bool>::type {
        using U = __underlying_type(T);
        return ((U)a != 0) == b;
    }

    template<typename T>
    __forceinline constexpr auto operator==(bool a, T b) -> enable_if<optionset_type<T>, bool>::type {
        using U = __underlying_type(T);
        return ((U)b != 0) == a;
    }

    template<optionset_type T>
    __forceinline constexpr auto operator|(T a, T b) {
        using U = __underlying_type(T);
        return (T)((U)a | (U)b);
    }

    template<optionset_type T>
    __forceinline constexpr auto operator+(T a, T b) {
        using U = __underlying_type(T);
        return (T)((U)a | (U)b);
    }

    template<optionset_type T>
    __forceinline constexpr auto operator&(T a, T b) {
        using U = __underlying_type(T);
        return (T)((U)a & (U)b);
    }

    template<optionset_type T>
    __forceinline constexpr auto operator-(T a, T b) {
        using U = __underlying_type(T);
        return (T)((U)a & ~(U)b);
    }

    template<optionset_type T>
    constexpr T& operator|=(T& a, T b) {
        a = a | b;
        return a;
    }

    template<optionset_type T>
    constexpr T& operator+=(T& a, T b) {
        a = a + b;
        return a;
    }

    template<optionset_type T>
    constexpr T& operator&=(T& a, T b) {
        a = a & b;
        return a;
    }

    template<optionset_type T>
    constexpr T& operator-=(T& a, T b) {
        a = a - b;
        return a;
    }

}
