//
//  test_display.cpp
//  toybox - tests
//
//  Created by Fredrik on 2025-11-09.
//

#include "shared.hpp"

#include "media/display_list.hpp"
#include "media/screen.hpp"

__neverinline void test_display_list() {
    printf("== Start: test_display_list\n\r");
    display_list_c list;
    screen_c screen(size_s(320, 16));
    palette_c p1, p2, p3;
    hard_assert(is_sorted(list.begin(), list.end()) && "Empty list should be sorted");
    list.insert_sorted({PRIMARY_SCREEN, 2, screen});
    hard_assert(is_sorted(list.begin(), list.end()) && "List should remain sorted after first insert");
    list.insert_sorted({PRIMARY_PALETTE, 0, p1});
    hard_assert(is_sorted(list.begin(), list.end()) && "List should remain sorted after second insert");
    list.insert_sorted({1, 2, p2});
    hard_assert(is_sorted(list.begin(), list.end()) && "List should remain sorted after third insert");
    list.insert_sorted({2, 20, p3});
    hard_assert(is_sorted(list.begin(), list.end()) && "List should remain sorted after fourth insert");

    constexpr pair_c<int, display_item_c::type_e> expections[] = {
        {0, display_item_c::palette}, {2, display_item_c::palette}, {2, display_item_c::screen}, {20, display_item_c::palette}
    };
    int i = 0;
    for (const auto& item : list) {
        hard_assert(item.row == expections[i].first && "Display list item order incorrect");
        hard_assert(item.item.display_type() == expections[i].second && "Display list item type incorrect");
        i++;
    }
    printf("test_display_list pass.\n\r");
}
