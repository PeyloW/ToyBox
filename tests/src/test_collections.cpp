//
//  test_collections.cpp
//  toybox - tests
//
//  Created by Fredrik on 2025-11-09.
//

#include "shared.hpp"

#include "array.hpp"
#include "vector.hpp"
#include "list.hpp"

__neverinline void test_array_and_vector() {
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

__neverinline void test_dynamic_vector() {
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
