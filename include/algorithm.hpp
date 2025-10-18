//
//  algorithm.hpp
//  toybox
//
//  Created by Fredrik on 2024-03-26.
//

#pragma once

#include "concepts.hpp"
#include "utility.hpp"

namespace toybox {
    
    /*
     This file containes a minimal set of cuntionality from C++ stdlib.
     */

    template<input_iterator I, output_iterator<I> J>
    __forceinline J copy(I first, I last, J d_first) {
        while (first != last) {
            *(d_first++) = *(first++);
        }
        return d_first;
    }

    template<input_iterator I, output_iterator<I> J>
    __forceinline J copy_backward(I first, I last, J d_last) {
        while (first != last) {
            *(--d_last) = *(--last);
        }
        return d_last;
    }

    template<input_iterator I, output_iterator<I> J>
    __forceinline J move(I first, I last, J d_first) {
        while (first != last) {
            *(d_first++) = move(*(first++));
        }
        return d_first;
    }

    template<bidirectional_iterator I, bidirectional_iterator J>
    requires output_iterator<J, decltype(*I{})>
    __forceinline I move_backward(I first, I last, J d_last) {
        while (first != last) {
            *(--d_last) = move(*(--last));
        }
        return d_last;
    }
    
    template<random_access_iterator FI, typename T>
    FI lower_bound(FI first, FI last, const T& value) {
        int16_t count = last - first;
        while (count > 0) {
            const int16_t step = count / 2;
            const FI it = first + step;
            if (*it < value) {
                first = it + 1;
                count -= step + 1;
            } else {
                count = step;
            }
        }
        return first;
    }

    template<random_access_iterator FI, typename T>
    bool binary_search(FI first, FI last, const T& value) {
        const FI found = lower_bound(first, last, value);
        return (!(found == last) && !(value < *found));
    }

    template<random_access_iterator FI, typename P>
    FI find_if(FI first, FI last, P pred) {
        for (; first != last; ++first) {
            if (p(*first)) {
                return first;
            }
        }
        return last;
    }
    
    template<random_access_iterator I>
    void sort(I first, I last) {
        for (auto i = first; i != last; i++) {
            auto min = i;
            for (auto j = i + 1; j != last; j++) {
                if (*j < *min) {
                    min = j;
                }
            }
            swap(*min, *i);
        }
    }
       
    template<input_iterator I>
    I is_sorted_until(I first, I last) {
        if (first != last) {
            I next = first;
            while (++next != last) {
                if (*next < *first)
                    return next;
                first = next;
            }
        }
        return last;
    }
    
    template<input_iterator I>
    bool is_sorted(I first, I last) {
        return is_sorted_until(first, last) == last;
    }

}
