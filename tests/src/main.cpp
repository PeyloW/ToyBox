//
//  main.cpp
//  toybox - tests
//
//  Created by Fredrik on 2025-10-12.
//

#include "cincludes.hpp"
#include "types.hpp"
#include "memory.hpp"

#include "display_list.hpp"
#include "screen.hpp"

using namespace toybox;

static void test_display_list() {
    display_list_c list;
    screen_c screen(size_s(320, 16));
    palette_c p1, p2, p3;
    assert(is_sorted(list.begin(), list.end()) && "Empty list should be sorted");
    list.insert_sorted({PRIMARY_SCREEN, 2, screen});
    assert(is_sorted(list.begin(), list.end()) && "List should remain sorted after first insert");
    list.insert_sorted({PRIMARY_PALETTE, 0, p1});
    assert(is_sorted(list.begin(), list.end()) && "List should remain sorted after second insert");
    list.insert_sorted({1, 2, p2});
    assert(is_sorted(list.begin(), list.end()) && "List should remain sorted after third insert");
    list.insert_sorted({2, 20, p3});
    assert(is_sorted(list.begin(), list.end()) && "List should remain sorted after fourth insert");

    constexpr pair_c<int, display_item_c::type_e> expections[] = {
        {0, display_item_c::palette}, {2, display_item_c::palette}, {2, display_item_c::screen}, {20, display_item_c::palette}
    };
    int i = 0;
    for (const auto& item : list) {
        assert(item.row == expections[i].first && "Display list item order incorrect");
        assert(item.item.display_type() == expections[i].second && "Display list item type incorrect");
        i++;
    }
    printf("test_display_list pass.\n");
}

void test_algorithms() {
    constexpr int numbers[4] = { 1, 7, 2, 0 };
    int buffer[4];

    assert(!is_sorted(begin(numbers), end(numbers)) && "Unsorted array should not be sorted");

    copy(begin(numbers), end(numbers), begin(buffer));
    assert(!is_sorted(begin(buffer), end(buffer)) && "Copied unsorted array should not be sorted");
    sort(begin(buffer), end(buffer));
    assert(is_sorted(begin(buffer), end(buffer)) && "Array should be sorted after sort");

    assert(binary_search(begin(buffer), end(buffer), 2) && "Binary search should find 2");
    assert(!binary_search(begin(buffer), end(buffer), 3) && "Binary search should not find 3");

    copy_backward(begin(numbers), end(numbers), end(buffer));
    assert(!is_sorted(begin(buffer), end(buffer)) && "Array should not be sorted after copy_backward");

    auto f = find_if(begin(buffer), end(buffer), [](auto&v) { return v == 2; });
    assert(*f == 2 && "find_if should find value 2");
    f = find_if(begin(buffer), end(buffer), [](auto&v) { return v == 3; });
    assert(f == end(buffer) && "find_if should not find value 3");
    
    list_c<int, 4> list;
    list.push_front(4);
    list.push_front(5);
    list.push_front(0);
    assert(!is_sorted(list.begin(), list.end()) && "List should not be sorted initially");

    copy(list.begin(), list.end(), begin(buffer));
    assert(buffer[0] == 0 && buffer[1] == 5 && buffer[2] == 4 && "Copied list values should match");
    sort(list.begin(), list.end());
    move(list.begin(), list.end(), begin(buffer));
    assert(buffer[0] == 0 && buffer[1] == 4 && buffer[2] == 5 && "Moved sorted list values should match");

    printf("test_algorithms pass.\n");
}

int main(int argc, const char * argv[]) {
    test_display_list();
    test_algorithms();
    printf("All pass.\n");
}
