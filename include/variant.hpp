//
//  variant.hpp
//  toybox
//
//  Created by Fredrik on 2025-10-17.
//

#pragma once

#include "utility.hpp"

namespace toybox {
    
    template<typename... Ts>
    class variant_c {
    public:
        static_assert(sizeof...(Ts) > 0);

        static constexpr int size = sizeof...(Ts);
        
        template<int I>
        using type_at_t = toybox::type_at<I, Ts...>::type;
        template<typename T>
        static inline constexpr int index_of = toybox::index_of<typename remove_cvref<T>::type, Ts...>::value;
        
        constexpr variant_c() {
            emplace<0>();
        }

        template<typename T>
        constexpr variant_c(const T& arg) {
            emplace<index_of<T>>(const_cast<T&>(arg));
        }
        
        template<int I, typename... Args>
        constexpr variant_c(Args&&... args) {
            emplace<I>(forward<Args>(args)...);
        }
        template<typename T, typename... Args>
        constexpr variant_c(Args&&... args) {
            emplace<index_of<T>>(forward<Args>(args)...);
        }

        ~variant_c() {
            reset();
        }
        
        constexpr int index() const { return _index; }

        template<int I, typename... Args>
        constexpr auto& emplace(Args&&... args) {
            static_assert(I >= 0 && I < (int)sizeof...(Ts));
            using T = type_at_t<I>;
            reset();
            new(&_storage) T(forward<Args>(args)...);
            _index = I;
            return *_storage.template ptr<I>();
        }
        template<typename T, typename... Args>
        constexpr T& emplace(Args&&... args) {
            return emplace<index_of<T>>(forward<Args>(args)...);
        }

        template<int I>
        constexpr type_at_t<I>& get() {
            return *_storage.template ptr<I>();
        }
        template<int I>
        constexpr const type_at_t<I>& get() const {
            return *_storage.template ptr<I>();
        }

        template<typename T>
        constexpr T& get() {
            return get<index_of<T>>();
        }
        template<typename T>
        constexpr const T& get() const {
            return get<index_of<T>>();
        }

        template<int I>
        constexpr type_at_t<I>* get_if() {
            return _index == I ? _storage.template ptr<I>() : nullptr;
        }
        template<int I>
        constexpr const type_at_t<I>* get_if() const {
            return _index == I ? _storage.template ptr<I>() : nullptr;
        }

        template<typename T>
        constexpr T* get_if() {
            return get_if<index_of<T>>();
        }
        template<typename T>
        constexpr const T* get_if() const {
            return get_if<index_of<T>>();
        }

    private:
        aligned_membuf_s<Ts...> _storage;
        int _index = -1;
        
        constexpr void reset() {
            if (_index >= 0) {
                destroy(_index);
                _index = -1;
            }
        }

        constexpr void destroy(int i) {
            ((i == index_of<Ts> ? ( _storage.template ptr<index_of<Ts>>()->~Ts(), 0) : 0), ...);
        }
    };
    
}
