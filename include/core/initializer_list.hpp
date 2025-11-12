//
//  initializer_list.hpp
//  toybox
//
//  Created by Fredrik on 2025-10-24.
//

#pragma once

#include "core/cincludes.hpp"

#ifndef TOYBOX_HOST
namespace std {
    template<typename T>
    class initializer_list {
    public:
        using value_type      = T;
        using reference       = const T&;
        using const_reference = const T&;
        using size_type       = size_t;
        using iterator        = const T*;
        using const_iterator  = const T*;
        
    private:
        iterator _data;
        size_type _size;
        
        // Note: constructors are private in std::initializer_list,
        // and the compiler injects them when using brace-init syntax.
        constexpr initializer_list(const_iterator data, size_type size)
        : _data(data), _size(size) {}
        
    public:
        constexpr initializer_list() : _data(nullptr), _size(0) {}
        
        constexpr size_type size() const { return _size; }
        constexpr const_iterator begin() const { return _data; }
        constexpr const_iterator end() const { return _data + _size; }
        constexpr const_iterator data() const { return _data; }
    };
    
}
#else
// I do not like having to do this, but machine_sdl2.cpp uses standard library
// for mutex and other functionality I do not want to replicate, redefining
// initializer_list would cause compiler errors there.
#include <initializer_list>
#endif

namespace toybox {
    using std::initializer_list;
}
