//
//  array.hpp
//  toybox
//
//  Created by Fredrik on 2025-10-23.
//

#pragma once

#include "cincludes.hpp"

namespace toybox {
    
    template<typename Type, int Count>
    struct array_s {
        using value_type = Type;
        using pointer = value_type* ;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = value_type*;
        using const_iterator = const value_type*;

        Type _data[Count];
        
        __forceinline iterator begin() __pure { return &_data[0]; }
        __forceinline const_iterator begin() const __pure { return &_data[0]; }
        __forceinline iterator end() __pure { return &_data[Count]; }
        __forceinline const_iterator end() const __pure { return &_data[Count]; }
        __forceinline int size() const __pure { return Count; }
        __forceinline pointer data() { return _data[0].ptr(); }
        __forceinline const_pointer data() const { return _data[0].ptr(); }

        __forceinline reference operator[](int i) __pure {
            assert(i >= 0 && i < Count && "Index out of bounds");
            return _data[i];
        }
        __forceinline const_reference operator[](int i) const __pure {
            assert(i >= 0 && i < Count && "Index out of bounds");
            return _data[i];
        }
        __forceinline reference front() __pure {
            assert(Count > 0 && "Vector is empty");
            return _data[0];
        }
        __forceinline const_reference front() const __pure {
            assert(Count > 0 && "Vector is empty");
            return _data[0];
        }
        __forceinline reference back() __pure {
            assert(Count > 0 && "Vector is empty");
            return _data[Count - 1];
        }
        __forceinline const_reference back() const __pure {
            assert(Count > 0 && "Vector is empty");
            return _data[Count - 1];
        }
        
        bool operator==(const array_s &other) const {
            if (this == &other) {
                return true;
            } else {
                int i;
                while_dbra_count(i, Count) {
                    if (_data[i] != other._data[i]) {
                        return false;
                    }
                }
                return true;
            }
        }
        
    };

}
