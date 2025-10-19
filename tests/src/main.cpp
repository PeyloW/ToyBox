//
//  main.cpp
//  toybox - tests
//
//  Created by Fredrik on 2025-10-12.
//

#include "cincludes.hpp"
#include "types.hpp"
#include "variant.hpp"
#include "memory.hpp"

#include "display_list.hpp"
#include "screen.hpp"

using namespace toybox;

static void test_variant() {
    using ptr_t = shared_ptr_c<char>;
    ptr_t p((char*)malloc(4));
    assert(p.use_count() == 1);
    // Use scope to verify RAII
    {
        // Use bool for trivial type with default constructor.
        // Use shared ptr for verifying non trivial construction and destruction.
        // Use rect for type larger than pointer
        variant_c<bool, ptr_t, rect_s> v1;
        assert(v1.index() == 0);
        assert(v1.get<bool>() == false);
        v1.emplace<rect_s>(2, 2, 8, 16);
        assert(v1.index() == 2);
        assert(v1.get_if<ptr_t>() == nullptr);
        assert(p.use_count() == 1);
        v1.emplace<ptr_t>(p);
        assert(p.use_count() == 2);
    }
    assert(p.use_count() == 1);
    p.reset();
    assert(p.use_count() == 0);
    printf("test_variant pass.\n");
}

static void test_display_list() {
    display_list_c list;
    screen_c screen(size_s(320, 16));
    palette_c p1, p2, p3;
    assert(is_sorted(list.begin(), list.end()));
    list.insert_sorted(display_item_c(2, screen));
    assert(is_sorted(list.begin(), list.end()));
    list.insert_sorted(display_item_c(0, p1));
    assert(is_sorted(list.begin(), list.end()));
    list.insert_sorted(display_item_c(2, p2));
    assert(is_sorted(list.begin(), list.end()));
    list.insert_sorted(display_item_c(20, p3));
    assert(is_sorted(list.begin(), list.end()));

    constexpr pair_c<int, int> expections[] = {
        {0, 1}, {2, 1}, {2, 0}, {20, 1}
    };
    int i = 0;
    for (const auto& item : list) {
        assert(item.first == expections[i].first);
        assert(item.second.index() == expections[i].second);
        i++;
    }
    printf("test_display_list pass.\n");
}

void test_algorithms() {
    constexpr int numbers[4] = { 1, 7, 2, 0 };
    int buffer[4];
    
    assert(!is_sorted(begin(numbers), end(numbers)));
    
    copy(begin(numbers), end(numbers), begin(buffer));
    assert(!is_sorted(begin(buffer), end(buffer)));
    sort(begin(buffer), end(buffer));
    assert(is_sorted(begin(buffer), end(buffer)));

    assert(binary_search(begin(buffer), end(buffer), 2));
    assert(!binary_search(begin(buffer), end(buffer), 3));

    copy_backward(begin(numbers), end(numbers), end(buffer));
    assert(!is_sorted(begin(buffer), end(buffer)));

    auto f = find_if(begin(buffer), end(buffer), [](auto&v) { return v == 2; });
    assert(*f == 2);
    f = find_if(begin(buffer), end(buffer), [](auto&v) { return v == 3; });
    assert(f == end(buffer));
    
    list_c<int, 4> list;
    list.push_front(4);
    list.push_front(5);
    list.push_front(0);
    assert(!is_sorted(list.begin(), list.end()));
    
    copy(list.begin(), list.end(), begin(buffer));
    assert(buffer[0] == 0 && buffer[1] == 5 && buffer[2] == 4);
    sort(list.begin(), list.end());
    move(list.begin(), list.end(), begin(buffer));
    assert(buffer[0] == 0 && buffer[1] == 4 && buffer[2] == 5);

    printf("test_algorithms pass.\n");
}

int main(int argc, const char * argv[]) {
    test_variant();
    test_display_list();
    test_algorithms();
    printf("All pass.\n");
}
