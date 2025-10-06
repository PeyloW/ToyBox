//
//  optionset.hpp
//  toybox
//
//  Created by Fredrik on 2025-10-05.
//

#ifndef optionset_h
#define optionset_h

#include "type_traits.hpp"

namespace toybox {

    template<typename T>
    concept optionset_enum = is_enum<T>::value && is_optionset<T>::value;

    __forceinline constexpr bool operator==(optionset_enum auto a, bool b) {
        using T = decltype(a);
        using U = __underlying_type(T);
        return ((U)a != 0) == b;
    }

    __forceinline constexpr bool operator==(bool a, optionset_enum auto b) {
        using T = decltype(b);
        using U = __underlying_type(T);
        return ((U)b != 0) == a;
    }

    __forceinline constexpr auto operator|(optionset_enum auto a, optionset_enum auto b) -> decltype(a) {
        using T = decltype(a);
        using U = __underlying_type(T);
        return (T)((U)a | (U)b);
    }

    __forceinline constexpr auto operator&(optionset_enum auto a, optionset_enum auto b) -> decltype(a) {
        using T = decltype(a);
        using U = __underlying_type(T);
        return (T)((U)a & (U)b);
    }

    __forceinline constexpr auto operator|=(optionset_enum auto& a, optionset_enum auto b) -> decltype(a)& {
        a = a | b;
        return a;
    }

    __forceinline constexpr auto operator&=(optionset_enum auto& a, optionset_enum auto b) -> decltype(a)& {
        a = a & b;
        return a;
    }
    
}

#endif /* optionset_h */
