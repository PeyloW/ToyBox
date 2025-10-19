//
//  display_list.hpp
//  toybox
//
//  Created by Fredrik on 2025-10-18.
//

#pragma once

#include "list.hpp"
#include "variant.hpp"

namespace toybox {

    class screen_c;
    class palette_c;
    
    using display_item_variant_c = variant_c<
        reference_wrapper_c<screen_c>,
        reference_wrapper_c<palette_c>
    >;
    class display_item_c : public pair_c<int, display_item_variant_c> {
    public:
        template<typename T>
        display_item_c(int i, T& item) : pair_c(i, display_item_variant_c(reference_wrapper_c<T>(item))) {}
        //display_item_c(int i, T& item) : pair_c(i, display_item_variant_c(item)) {}
    };
    static constexpr bool operator<(const display_item_c &lhs, const display_item_c &rhs) {
        return lhs.first < rhs.first;
    }

    
    class display_list_c : public list_c<display_item_c> {
    public:

        inline const_iterator insert_sorted(const_reference value) {
            auto pos = iterator_before(value.first);
            return insert_after(pos, value);
        }
        template<class ...Args>
        inline iterator emplace_sorted(int first, Args&& ...args) {
            auto pos = iterator_before(first);
            return emplace_after(pos, first, forward<Args>(args)...);
        }
                
    private:
        const_iterator iterator_before(int index) const {
            auto iter = before_begin();
            while (iter._node->next) {
                if (iter._node->next->value.first >= index) {
                    break;
                }
                ++iter;
            }
            return iter;
        }
        
    };
    
}
