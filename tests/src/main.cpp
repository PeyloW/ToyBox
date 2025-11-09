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

struct non_trivial_s {
    int value;
    int generation;
    bool moved;

    non_trivial_s() : value(0), generation(0), moved(false) {}

    explicit non_trivial_s(int v) : value(v), generation(0), moved(false) {}

    non_trivial_s(const non_trivial_s& other)
        : value(other.value), generation(other.generation + 1), moved(false) {}

    non_trivial_s(non_trivial_s&& other) noexcept
        : value(other.value), generation(other.generation), moved(false) {
        other.moved = true;
    }

    non_trivial_s& operator=(const non_trivial_s& other) {
        if (this != &other) {
            value = other.value;
            generation = other.generation + 1;
            moved = false;
        }
        return *this;
    }

    non_trivial_s& operator=(non_trivial_s&& other) {
        if (this != &other) {
            value = other.value;
            generation = other.generation;
            moved = false;
            other.moved = true;
        }
        return *this;
    }
};

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

static __neverinline void test_dynamic_vector() {
    // Test dynamic vector with Count == 0
    vector_c<int, 0> vec;
    hard_assert(vec.size() == 0 && "Initial size should be 0");
    hard_assert(vec.capacity() == 0 && "Initial capacity should be 0");

    // Test automatic growth on push_back
    vec.push_back(1);
    hard_assert(vec.size() == 1 && "Size should be 1 after first push");
    hard_assert(vec.capacity() >= 1 && "Capacity should be at least 1");
    hard_assert(vec[0] == 1 && "First element should be 1");

    // Test growth to default capacity
    for (int i = 2; i <= 10; ++i) {
        vec.push_back(i);
    }
    hard_assert(vec.size() == 10 && "Size should be 10");
    hard_assert(vec.capacity() >= 10 && "Capacity should be at least 10");

    // Verify all elements
    for (int i = 0; i < 10; ++i) {
        hard_assert(vec[i] == i + 1 && "Element values should be correct");
    }

    // Test reserve
    vec.reserve(100);
    hard_assert(vec.capacity() >= 100 && "Capacity should be at least 100 after reserve");
    hard_assert(vec.size() == 10 && "Size should remain 10 after reserve");

    // Test that elements are preserved after reserve
    for (int i = 0; i < 10; ++i) {
        hard_assert(vec[i] == i + 1 && "Elements should be preserved after reserve");
    }

    // Test emplace_back
    vec.emplace_back(11);
    hard_assert(vec.size() == 11 && "Size should be 11 after emplace");
    hard_assert(vec[10] == 11 && "Emplaced element should be 11");

    // Test insert
    vec.insert(vec.begin(), 0);
    hard_assert(vec.size() == 12 && "Size should be 12 after insert");
    hard_assert(vec[0] == 0 && "First element should be 0 after insert");
    hard_assert(vec[1] == 1 && "Second element should be 1 after insert");

    // Test erase
    vec.erase(vec.begin());
    hard_assert(vec.size() == 11 && "Size should be 11 after erase");
    hard_assert(vec[0] == 1 && "First element should be 1 after erase");

    // Test front and back
    hard_assert(vec.front() == 1 && "Front should be 1");
    hard_assert(vec.back() == 11 && "Back should be 11");

    // Test pop_back
    vec.pop_back();
    hard_assert(vec.size() == 10 && "Size should be 10 after pop_back");
    hard_assert(vec.back() == 10 && "Back should be 10 after pop_back");

    // Test clear
    vec.clear();
    hard_assert(vec.size() == 0 && "Size should be 0 after clear");
    hard_assert(vec.capacity() >= 100 && "Capacity should remain allocated after clear");

    // Test growth after clear
    for (int i = 0; i < 5; ++i) {
        vec.push_back(i * 2);
    }
    hard_assert(vec.size() == 5 && "Size should be 5 after refill");
    for (int i = 0; i < 5; ++i) {
        hard_assert(vec[i] == i * 2 && "Elements should be correct after refill");
    }

    printf("test_dynamic_vector pass.\n\r");
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

static __neverinline void test_object_lifetime() {
    // Test direct copy semantics
    non_trivial_s obj1(42);
    hard_assert(obj1.value == 42 && "Initial value should be 42");
    hard_assert(obj1.generation == 0 && "Initial generation should be 0");
    hard_assert(!obj1.moved && "Initial object should not be moved");

    non_trivial_s obj2(obj1);
    hard_assert(obj2.value == 42 && "Copied value should be 42");
    hard_assert(obj2.generation == 1 && "Copied generation should be 1");
    hard_assert(!obj2.moved && "Copied object should not be marked as moved");
    hard_assert(!obj1.moved && "Original object should not be marked as moved after copy");

    non_trivial_s obj3(100);
    obj3 = obj2;
    hard_assert(obj3.value == 42 && "Assigned value should be 42");
    hard_assert(obj3.generation == 2 && "Assigned generation should be 2");
    hard_assert(!obj3.moved && "Assigned object should not be marked as moved");

    // Test direct move semantics
    non_trivial_s obj4(move(obj3));
    hard_assert(obj4.value == 42 && "Moved value should be 42");
    hard_assert(obj4.generation == 2 && "Moved generation should remain 2");
    hard_assert(!obj4.moved && "Destination object should not be marked as moved");
    hard_assert(obj3.moved && "Source object should be marked as moved");

    non_trivial_s obj5(200);
    obj5 = move(obj4);
    hard_assert(obj5.value == 42 && "Move-assigned value should be 42");
    hard_assert(obj5.generation == 2 && "Move-assigned generation should remain 2");
    hard_assert(!obj5.moved && "Destination object should not be marked as moved");
    hard_assert(obj4.moved && "Source object should be marked as moved");

    // Test with static vector_c
    vector_c<non_trivial_s, 5> static_vec;

    // 1. Push back
    static_vec.push_back(non_trivial_s(10));
    hard_assert(static_vec.size() == 1 && "Static vector size should be 1");
    hard_assert(static_vec[0].value == 10 && "Back element value should be 10");
    hard_assert(static_vec[0].generation == 1 && "Back element generation should be 1 (copied from temporary)");
    hard_assert(!static_vec[0].moved && "Back element should not be moved after push_back");

    // 2. Insert front (back was moved during shift)
    static_vec.insert(static_vec.begin(), non_trivial_s(5));
    hard_assert(static_vec.size() == 2 && "Static vector size should be 2");
    hard_assert(static_vec[0].value == 5 && "Front element should be 5");
    hard_assert(static_vec[0].generation == 1 && "Front element generation should be 1 (copied from temporary)");
    hard_assert(!static_vec[0].moved && "Front element should not be moved");
    hard_assert(static_vec[1].value == 10 && "Back element should be 10");
    hard_assert(static_vec[1].generation == 1 && "Back element generation should remain 1 (moved, not copied)");
    hard_assert(!static_vec[1].moved && "Back element is destination of move, not marked as moved");

    // 3. Push back (back is not moved)
    static_vec.push_back(non_trivial_s(20));
    hard_assert(static_vec.size() == 3 && "Static vector size should be 3");
    hard_assert(static_vec[2].value == 20 && "New back element value should be 20");
    hard_assert(static_vec[2].generation == 1 && "New back element generation should be 1 (copied from temporary)");
    hard_assert(!static_vec[2].moved && "New back element should not be moved after push_back");

    // 4. Erase front (elements moved during shift)
    static_vec.erase(static_vec.begin());
    hard_assert(static_vec.size() == 2 && "Static vector size should be 2 after erase");
    hard_assert(static_vec[0].value == 10 && "First element should be 10 after erase");
    hard_assert(static_vec[0].generation == 1 && "First element generation should remain 1 (moved, not copied)");
    hard_assert(!static_vec[0].moved && "First element is destination of move, not marked as moved");
    hard_assert(static_vec[1].value == 20 && "Back element should be 20");
    hard_assert(static_vec[1].generation == 1 && "Back element generation should remain 1 (moved, not copied)");
    hard_assert(!static_vec[1].moved && "Back element is destination of move, not marked as moved");

    // Test with dynamic vector_c
    vector_c<non_trivial_s, 0> dynamic_vec;
    hard_assert(dynamic_vec.size() == 0 && "Dynamic vector initial size should be 0");

    // 1. Push back
    dynamic_vec.push_back(non_trivial_s(100));
    hard_assert(dynamic_vec.size() == 1 && "Dynamic vector size should be 1");
    hard_assert(dynamic_vec[0].value == 100 && "Back element value should be 100");
    hard_assert(dynamic_vec[0].generation == 1 && "Back element generation should be 1 (copied from temporary)");
    hard_assert(!dynamic_vec[0].moved && "Back element should not be moved after push_back");

    // 2. Insert front (back was moved during shift)
    dynamic_vec.insert(dynamic_vec.begin(), non_trivial_s(50));
    hard_assert(dynamic_vec.size() == 2 && "Dynamic vector size should be 2");
    hard_assert(dynamic_vec[0].value == 50 && "Front element should be 50");
    hard_assert(dynamic_vec[0].generation == 1 && "Front element generation should be 1 (copied from temporary)");
    hard_assert(!dynamic_vec[0].moved && "Front element should not be moved");
    hard_assert(dynamic_vec[1].value == 100 && "Back element should be 100");
    hard_assert(dynamic_vec[1].generation == 1 && "Back element generation should remain 1 (moved, not copied)");
    hard_assert(!dynamic_vec[1].moved && "Back element is destination of move, not marked as moved");

    // 3. Push back (back is not moved)
    dynamic_vec.push_back(non_trivial_s(200));
    hard_assert(dynamic_vec.size() == 3 && "Dynamic vector size should be 3");
    hard_assert(dynamic_vec[2].value == 200 && "New back element value should be 200");
    hard_assert(dynamic_vec[2].generation == 1 && "New back element generation should be 1 (copied from temporary)");
    hard_assert(!dynamic_vec[2].moved && "New back element should not be moved after push_back");

    // 4. Erase front (elements moved during shift)
    dynamic_vec.erase(dynamic_vec.begin());
    hard_assert(dynamic_vec.size() == 2 && "Dynamic vector size should be 2 after erase");
    hard_assert(dynamic_vec[0].value == 100 && "First element should be 100 after erase");
    hard_assert(dynamic_vec[0].generation == 1 && "First element generation should remain 1 (moved, not copied)");
    hard_assert(!dynamic_vec[0].moved && "First element is destination of move, not marked as moved");
    hard_assert(dynamic_vec[1].value == 200 && "Back element should be 200");
    hard_assert(dynamic_vec[1].generation == 1 && "Back element generation should remain 1 (moved, not copied)");
    hard_assert(!dynamic_vec[1].moved && "Back element is destination of move, not marked as moved");

    // Test with list_c
    list_c<non_trivial_s, 5> list;
    list.push_front(non_trivial_s(1000));
    hard_assert(list.size() == 1 && "List size should be 1");
    hard_assert(list.front().value == 1000 && "List front value should be 1000");

    // Test insert_after with lvalue (copy)
    non_trivial_s temp3(2000);
    list.insert_after(list.begin(), temp3);
    hard_assert(list.size() == 2 && "List size should be 2");
    auto it = list.begin();
    ++it;
    hard_assert(it->value == 2000 && "Second element value should be 2000");
    hard_assert(it->generation == 1 && "Second element should be copied (generation 1)");

    // Test emplace_after
    list.emplace_after(it, 3000);
    hard_assert(list.size() == 3 && "List size should be 3");

    // Verify list order
    it = list.begin();
    hard_assert(it->value == 1000 && "First list element should be 1000");
    ++it;
    hard_assert(it->value == 2000 && "Second list element should be 2000");
    ++it;
    hard_assert(it->value == 3000 && "Third list element should be 3000");

    printf("test_object_lifetime pass.\n\r");
}

int main(int argc, const char * argv[]) {
    test_array_and_vector();
    test_dynamic_vector();
    test_display_list();
    test_algorithms();
    test_math();
    test_math_functions();
    test_object_lifetime();
    printf("All pass.\n\r");
#ifndef TOYBOX_HOST
    while (getc(stdin) != ' ');
#endif
    return 0;
}
