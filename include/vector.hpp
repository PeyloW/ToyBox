//
//  vector.hpp
//  toybox
//
//  Created by Fredrik on 2024-03-22.
//

#pragma once

#include "algorithm.hpp"

namespace toybox {
    
    /**
     `vector_c` is a minimal implementation of `std::vector` with a statically
     allocated backing store, for performance reasons.
     TODO: Treat Count of 0 as a dynamic vector.
     */
    template<class Type, int Count>
    class vector_c : public nocopy_c {
    public:
        using value_type = Type;
        using pointer = value_type* ;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        
        vector_c() : _size(0) {}
        
        __forceinline iterator begin() __pure { return _data[0].ptr(); }
        __forceinline const_iterator begin() const __pure { return _data[0].ptr(); }
        __forceinline iterator end() __pure { return _data[_size].ptr(); }
        __forceinline const_iterator end() const __pure { return _data[_size].ptr(); }
        __forceinline int size() const __pure { return _size; }
        
        __forceinline reference operator[](const int i) __pure {
            assert(i < _size && "Index out of bounds");
            assert(i >= 0 && "Index must be non-negative");
            return *_data[i].ptr();
        }
        __forceinline const_reference operator[](const int i) const __pure {
            assert(i < _size && "Index out of bounds");
            assert(i >= 0 && "Index must be non-negative");
            return *_data[i].ptr();
        }
        __forceinline reference front() __pure {
            assert(_size > 0 && "Vector is empty");
            return *_data[0].ptr();
        }
        __forceinline const_reference front() const __pure {
            assert(_size > 0 && "Vector is empty");
            return *_data[0].ptr();
        }
        __forceinline reference back() __pure {
            assert(_size > 0 && "Vector is empty");
            return *_data[_size - 1].ptr();
        }
        __forceinline const_reference back() const __pure {
            assert(_size > 0 && "Vector is empty");
            return *_data[_size - 1].ptr();
        }

        __forceinline void push_back(const_reference value) {
            assert(_size < Count && "Vector capacity exceeded");
            *_data[_size++].ptr() = value;
        }
        iterator insert(const_iterator pos, const_reference value) {
            assert(_size < Count && pos >= begin() && pos <= end() && "Invalid insert position or capacity exceeded");
            move_backward(pos, end(), end() + 1);
            *pos = value;
            _size++;
            return ++pos;
        }
        template<class... Args>
        __forceinline reference emplace_back(Args&&... args) {
            assert(_size < Count && "Vector capacity exceeded");
            return *new (_data[_size++].addr()) Type(forward<Args>(args)...);
        }
        template<class... Args>
        iterator emplace(Type *pos, Args&&... args) {
            assert(_size < Count && pos >= begin() && pos <= end() && "Invalid emplace position or capacity exceeded");
            move_backward(pos, end(), end() + 1);
            new (static_cast<void *>(pos)) Type(forward<Args>(args)...);
            _size++;
            return ++pos;
        }

        iterator erase(const_iterator pos) {
            assert(_size > 0 && pos >= begin() && pos < end() && "Invalid erase position or vector is empty");
            destroy_at(pos);
            move(pos + 1, this->end(), pos);
            _size--;
            return iterator(pos);
        }
        void clear() {
            while (_size) {
                destroy_at(_data[--_size].ptr());
            }
        }
        __forceinline void pop_back() {
            assert(_size > 0 && "Vector is empty");
            destroy_at(_data[--_size].ptr());
        }
        
    private:
        aligned_membuf_s<Type> _data[Count];
        int _size;
    };
    
}
