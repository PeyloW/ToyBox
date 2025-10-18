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
}

int main(int argc, const char * argv[]) {
    test_variant();
    printf("pass.\n");
}
