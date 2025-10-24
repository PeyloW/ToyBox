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
     This file contains a minimal set of functionality from C++ stdlib.
     */

    template<const_forward_iterator I, forward_iterator J>
    J copy(I first, I last, J d_first) {
        while (first != last) {
            *(d_first) = *(first);
            ++d_first; ++first;
        }
        return d_first;
    }

    template<const_backward_iterator I, backward_iterator J>
    J copy_backward(I first, I last, J d_last) {
        while (first != last) {
            *(--d_last) = *(--last);
        }
        return d_last;
    }

    template<const_forward_iterator I, forward_iterator J>
    J move(I first, I last, J d_first) {
        while (first != last) {
            *(d_first) = move(*(first));
            ++d_first; ++first;
        }
        return d_first;
    }

    template<const_backward_iterator I, backward_iterator J>
    I move_backward(I first, I last, J d_last) {
        while (first != last) {
            *(--d_last) = move(*(--last));
        }
        return d_last;
    }
    
    template<const_random_access_iterator FI, typename T>
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

    template<const_random_access_iterator FI, typename T>
    bool binary_search(FI first, FI last, const T& value) {
        const FI found = lower_bound(first, last, value);
        return (!(found == last) && !(value < *found));
    }

    template<const_forward_iterator FI, typename P>
    requires predicate<P, typename iterator_traits<FI>::value_type> ||
        predicate<P, const typename iterator_traits<FI>::reference>
    FI find_if(FI first, FI last, P pred)
    {
        for (; first != last; ++first) {
            if (pred(*first)) {
                return first;
            }
        }
        return last;
    }
           
    template<forward_iterator I>
    void sort(I first, I last) {
        for (auto i = first; i != last; ++i) {
            auto min = i;
            for (auto j = i; ++j != last; ) {
                if (*j < *min) {
                    min = j;
                }
            }
            if (min != i) {
                swap(*min, *i);
            }
        }
    }
    
    template<const_forward_iterator I>
    I is_sorted_until(I first, I last) {
        if (first == last) return last;
        for (I next = first; ++next != last; first = next) {
            if (*next < *first) {
                return next;
            }
        }
        return last;
    }
    
    template<const_forward_iterator I>
    bool is_sorted(I first, I last) {
        return is_sorted_until(first, last) == last;
    }

}
