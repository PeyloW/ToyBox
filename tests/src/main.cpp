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
#include "math.hpp"

using namespace toybox;

static __neverinline void test_array_and_vector() {
    array_s<int, 4> arr = { 1, 5, 2, 1 };
    hard_assert(arr.size() == 4);
    hard_assert(!is_sorted(arr.begin(), arr.end()));
    
    vector_c<int, 5> vec = { 1, 2, 3 };
    hard_assert(vec.size() == 3);
    vec.push_back(4);
    hard_assert(vec.size() == 4);
    vec.emplace(0, 0);
    hard_assert(vec.size() == 5);
    hard_assert(is_sorted(vec.begin(), vec.end()));
    hard_assert(binary_search(vec.begin(), vec.end(), 0));
    hard_assert(binary_search(vec.begin(), vec.end(), 2));
    hard_assert(binary_search(vec.begin(), vec.end(), 4));
    hard_assert(*vec.erase(vec.begin()) == 1);
    hard_assert(vec.erase(vec.end() - 1) == vec.end());
    hard_assert(*vec.erase(1) == 3);
    hard_assert(vec.size() == 2);
    hard_assert(vec[0] == 1);
    hard_assert(vec[1] == 3);
    vec.clear();
    hard_assert(vec.size() == 0);

    for (const auto i : arr) {
        vec.insert(vec.begin(), i);
    }
    hard_assert(vec.size() == 4);
    for (int i = 0; i < 4; i++) {
        hard_assert(arr[3 - i] == vec[i]);
    }
    
    printf("test_array_and_vector pass.\n\r");
}

static __neverinline void test_display_list() {
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

static __neverinline void test_algorithms() {
    constexpr int numbers[4] = { 1, 7, 2, 0 };
    int buffer[4];

    hard_assert(!is_sorted(begin(numbers), end(numbers)) && "Unsorted array should not be sorted");

    copy(begin(numbers), end(numbers), begin(buffer));
    hard_assert(!is_sorted(begin(buffer), end(buffer)) && "Copied unsorted array should not be sorted");
    sort(begin(buffer), end(buffer));
    hard_assert(is_sorted(begin(buffer), end(buffer)) && "Array should be sorted after sort");

    hard_assert(binary_search(begin(buffer), end(buffer), 2) && "Binary search should find 2");
    hard_assert(!binary_search(begin(buffer), end(buffer), 3) && "Binary search should not find 3");

    copy_backward(begin(numbers), end(numbers), end(buffer));
    hard_assert(!is_sorted(begin(buffer), end(buffer)) && "Array should not be sorted after copy_backward");

    auto f = find_if(begin(buffer), end(buffer), [](auto&v) { return v == 2; });
    hard_assert(*f == 2 && "find_if should find value 2");
    f = find_if(begin(buffer), end(buffer), [](auto&v) { return v == 3; });
    hard_assert(f == end(buffer) && "find_if should not find value 3");
    
    list_c<int, 4> list;
    list.push_front(4);
    list.push_front(5);
    list.push_front(0);
    hard_assert(!is_sorted(list.begin(), list.end()) && "List should not be sorted initially");

    copy(list.begin(), list.end(), begin(buffer));
    hard_assert(buffer[0] == 0 && buffer[1] == 5 && buffer[2] == 4 && "Copied list values should match");
    sort(list.begin(), list.end());
    move(list.begin(), list.end(), begin(buffer));
    hard_assert(buffer[0] == 0 && buffer[1] == 4 && buffer[2] == 5 && "Moved sorted list values should match");

    printf("test_algorithms pass.\n\r");
}

static __neverinline void test_math() {
    auto r1 = mul_fast((uint16_t)50000, (uint16_t)2);
    auto r2 = mul_fast((int16_t)30000, (int16_t)-3);
    hard_assert(sizeof(r1) == 4 && sizeof(r2) == 4 && "Result is 32 bit");
    hard_assert(r1 == 100000 && "Result is did not overflow");
    hard_assert(r2 == -90000 && "Result is did not overflow");

    auto r3 = div_fast((uint32_t)100000, (uint16_t)2);
    auto r4 = div_fast((int32_t)-90000, (int16_t)3);
    hard_assert(sizeof(r3.quot) == 2 && sizeof(r4.quot) == 2 && "Result is 16 bit");
    hard_assert(r3.quot == 50000 && "Result is correct");
    hard_assert(r4.quot == -30000 && "Result is correct");

    hard_assert(numbers::one);
    hard_assert(!fix16_t(0));
    
    fix16_t f1 = numbers::one;
    hard_assert(f1);
    hard_assert(f1.raw == 0x10);
    fix16_t zero = 0;
    hard_assert(!zero);
    
    hard_assert(fix16_t(1.5) * fix16_t(1.5) == 2.25);
    hard_assert(fix16_t(1.5) * 2 == 3);
    hard_assert(fix16_t(-10) * fix16_t(-3.1415) == fix16_t(31.25)); // Should be 31.375, but * does not round.

    hard_assert(fix16_t(10) / 3 == fix16_t(3.333f));
    hard_assert(fix16_t(10) % 3 == fix16_t(1.0f));
    hard_assert(fix16_t(12.25) / 3 == fix16_t(4.0833f));
    hard_assert(fix16_t(12.25) % 3 == fix16_t(0.25f));
    hard_assert(fix16_t(7.5) / fix16_t(3.25) == fix16_t(2.25f)); // Should be 2.3125, but / does not round.
    hard_assert(fix16_t(7.5) % fix16_t(3.25) == 1);

    hard_assert(trunc(3.14) == 3);
    hard_assert(round(3.14) == 3);
    hard_assert(trunc(2.72) == 2);
    hard_assert(round(2.72) == 3);
    hard_assert(trunc(-3.14) == -3);
    hard_assert(round(-3.14) == -3);
    hard_assert(trunc(-2.72) == -2);
    hard_assert(round(-2.72) == -3);
    hard_assert(round(-0.914518 * 16) / 16 == -0.937500);

    auto pi2 = numbers::pi * 2;
    hard_assert(pi2.raw == 100);

    hard_assert(trunc(numbers::pi) == 3);
    hard_assert(floor(numbers::pi) == 3);
    hard_assert(ceil(numbers::pi) == 4);
    hard_assert(round(numbers::pi) == 3);

    hard_assert(trunc(numbers::e) == 2);
    hard_assert(floor(numbers::e) == 2);
    hard_assert(ceil(numbers::e) == 3);
    hard_assert(round(numbers::e) == 3);

    
    printf("test_math pass.\n\r");
}

static __neverinline void test_math_functions() {
    using namespace rel_ops;
    using namespace numbers;
    
    hard_assert(pow(2, 2) == fix16_t(4));
    hard_assert(pow(3.0f, 3.0f) == fix16_t(27.0f));
    hard_assert(pow(25, 0.5f) == fix16_t(5));
    hard_assert(pow(10, 1.5f) == fix16_t(31.6228));

    hard_assert(sqrti(25) == 5);
    hard_assert(sqrti(22500) == 150);
    hard_assert(sqrti(10) == 3);

    hard_assert(sqrt(25) == 5);
    hard_assert(sqrt(10) == fix16_t(3.1622f));
    hard_assert(sqrt(2) == fix16_t(1.4142f));
    
    hard_assert(sin(0) == fix16_t(0));
    hard_assert(sin(1) == fix16_t(0.841471f));
    hard_assert(sin(2) == fix16_t(0.909297f));
    hard_assert(sin(3) == fix16_t(0.141120f));
    hard_assert(sin(4) == fix16_t(-0.756802f));
    hard_assert(sin(pi) == fix16_t(0));
    hard_assert(sin(pi2x) == fix16_t(0));
    hard_assert(sin(pi_2) == fix16_t(1));
    hard_assert(sin(-1) == fix16_t(-0.8750f)); // Should be 0.8125, but double rounding
    hard_assert(sin(-pi) == fix16_t(0));
    hard_assert(sin(-pi2x) == fix16_t(0));
    hard_assert(sin(-pi_2) == fix16_t(-1));
    
    hard_assert(sin(0) == cos(pi_2));
    
    hard_assert(tan(0) == 0);
    hard_assert(tan(pi_2) > fix16_t(1000));
    hard_assert(tan(-(pi / 4)) == -tan(pi / 4));
    hard_assert(tan(pi) == 0);
    
    printf("test_math_functions pass.\n\r");
}

int main(int argc, const char * argv[]) {
    test_array_and_vector();
    test_display_list();
    test_algorithms();
    test_math();
    test_math_functions();
    printf("All pass.\n\r");
#ifndef TOYBOX_HOST
    while (getc(stdin) != ' ');
#endif
    return 0;
}
