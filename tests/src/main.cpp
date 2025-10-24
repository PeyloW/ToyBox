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
#include "array.hpp"

using namespace toybox;

static void test_array_and_vector() {
    array_s<int, 4> arr = { 1, 5, 2, 1 };
    assert(arr.size() == 4);
    assert(!is_sorted(arr.begin(), arr.end()));
    
    vector_c<int, 5> vec = { 1, 2, 3 };
    assert(vec.size() == 3);
    vec.push_back(4);
    assert(vec.size() == 4);
    vec.emplace(0, 0);
    assert(vec.size() == 5);
    assert(is_sorted(vec.begin(), vec.end()));
    assert(binary_search(vec.begin(), vec.end(), 0));
    assert(binary_search(vec.begin(), vec.end(), 2));
    assert(binary_search(vec.begin(), vec.end(), 4));
    assert(*vec.erase(vec.begin()) == 1);
    assert(vec.erase(vec.end() - 1) == vec.end());
    assert(*vec.erase(1) == 3);
    assert(vec.size() == 2);
    assert(vec[0] == 1);
    assert(vec[1] == 3);
    vec.clear();
    assert(vec.size() == 0);

    for (const auto i : arr) {
        vec.insert(vec.begin(), i);
    }
    assert(vec.size() == 4);
    for (int i = 0; i < 4; i++) {
        assert(arr[3 - i] == vec[i]);
    }
    
    printf("test_array_and_vector pass.\n\r");
}

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
    printf("test_display_list pass.\n\r");
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

    printf("test_algorithms pass.\n\r");
}

int main(int argc, const char * argv[]) {
    test_array_and_vector();
    test_display_list();
    test_algorithms();
    printf("All pass.\n\r");
#ifndef TOYBOX_HOST
    while (getc(stdin) != ' ');
#endif
    return 0;
}
