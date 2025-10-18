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

        constexpr variant_c() {
            emplace<0>();
        }

        ~variant_c() {
            reset();
        }

        constexpr int index() const { return _index; }

        template<int I, typename... Args>
        constexpr typename type_at<I, Ts...>::type& emplace(Args&&... args) {
            static_assert(I >= 0 && I < (int)sizeof...(Ts));
            reset();
            using T = typename type_at<I, Ts...>::type;
            new(&_storage) T(forward<Args>(args)...);
            _index = I;
            return *_storage.template ptr<I>();
        }
        template<typename T, typename... Args>
        constexpr T& emplace(Args&&... args) {
            constexpr int I = index_of<T, Ts...>::value;
            return emplace<I>(forward<Args>(args)...);
        }

        template<int I>
        constexpr typename type_at<I, Ts...>::type& get() {
            return *_storage.template ptr<I>();
        }
        template<int I>
        constexpr const typename type_at<I, Ts...>::type& get() const {
            return *_storage.template ptr<I>();
        }

        template<typename T>
        constexpr T& get() {
            constexpr int I = index_of<T, Ts...>::value;
            return get<I>();
        }
        template<typename T>
        constexpr const T& get() const {
            constexpr int I = index_of<T, Ts...>::value;
            return get<I>();
        }

        template<int I>
        constexpr typename type_at<I, Ts...>::type* get_if() {
            return _index == I ? _storage.template ptr<I>() : nullptr;
        }
        template<int I>
        constexpr const typename type_at<I, Ts...>::type* get_if() const {
            return _index == I ? _storage.template ptr<I>() : nullptr;
        }

        template<typename T>
        constexpr T* get_if() {
            constexpr int I = index_of<T, Ts...>::value;
            return get_if<I>();
        }
        template<typename T>
        constexpr const T* get_if() const {
            constexpr int I = index_of<T, Ts...>::value;
            return get_if<I>();
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
            ((i == index_of<Ts, Ts...>::value ? ( _storage.template ptr<index_of<Ts, Ts...>::value>()->~Ts(), 0) : 0), ...);
        }
    };
    
}
