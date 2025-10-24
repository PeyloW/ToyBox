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

    class display_item_c {
    public:
        enum class type_e : uint8_t {
            screen, palette
        };
        using enum type_e;
        virtual type_e display_type() const __pure = 0;
    };
    
    enum {
        PRIMARY_SCREEN = -1,
        PRIMARY_PALETTE = -2
    };
    struct display_list_entry_s {
        int id;
        int row;
        display_item_c &item;
        __forceinline screen_c& screen() const {
            assert(item.display_type() == display_item_c::screen && "Display item is not a screen");
            return (screen_c&)item;
        }
        __forceinline palette_c& palette() const {
            assert(item.display_type() == display_item_c::palette && "Display item is not a palette");
            return (palette_c&)item;
        }
        __forceinline bool operator<(const display_list_entry_s &rhs) const {
            return row < rhs.row;
        }
    };
    
    class display_list_c : public list_c<display_list_entry_s> {
    public:
        const_iterator insert_sorted(const_reference value) {
            auto pos = iterator_before(value.row);
            return insert_after(pos, value);
        }
        template<class ...Args>
        iterator emplace_sorted(int id, int row, Args&& ...args) {
            auto pos = iterator_before(row);
            return emplace_after(pos, id, row, forward<Args>(args)...);
        }

        __forceinline display_list_entry_s& get(int id) const {
            return *get_if(id);
        }

        display_list_entry_s* get_if(int id) const {
            for (auto& item : *this) {
                if (item.id == id) return const_cast<display_list_entry_s*>(&item);
            }
            return nullptr;
        }


    private:
        const_iterator iterator_before(int row) const {
            auto iter = before_begin();
            while (iter._node->next) {
                if (iter._node->next->value.row >= row) {
                    break;
                }
                ++iter;
            }
            return iter;
        }
        
    };
    
}
