//
//  static_allocator.hpp
//  toybox
//
//  Created by Fredrik Olsson on 2024-03-24.
//

#pragma once

#include "algorithm.hpp"

namespace toybox {

/**
 A `static_allocator_c` is vaguely related to `std::allocator`, but always
 allocates single blocks of a static size.
 This exists for performance, as `malloc` can be expensive.
 */
template <class T, size_t Count>
class static_allocator_c {
    struct block_t;
public:
    static constexpr size_t alloc_size = sizeof(T);
    static constexpr size_t max_alloc_count = Count;
    using type = block_t *;
    static void *allocate() {
        assert(first_block && "Allocator pool exhausted");
#ifndef __M68000__
        _alloc_count++;
        _peak_alloc_count = MAX(_peak_alloc_count, _alloc_count);
#endif
        auto ptr = reinterpret_cast<T*>(&first_block->data[0]);
        first_block = first_block->next;
        return ptr;
    };
    static void deallocate(void *ptr) {
#ifndef __M68000__
        _alloc_count--;
#endif
        block_t *block = reinterpret_cast<block_t *>(static_cast<void **>(ptr) - 1);
        block->next = first_block;
        first_block = block;
    }
#ifndef __M68000__
    static int peak_alloc_count() { return _peak_alloc_count; }
#endif
private:
    struct block_t {
        block_t *next;
        uint8_t data[alloc_size];
    };
#ifndef __M68000__
    static inline int _alloc_count = 0;
    static inline int _peak_alloc_count = 0;
#endif
    static inline block_t *first_block = [] {
        static block_t s_blocks[Count];
        for (int i = 0; i < Count - 1; i++) {
            s_blocks[i].next = &s_blocks[i + 1];
        }
        s_blocks[Count - 1].next = nullptr;
        return &s_blocks[0];
    }();
};

}
