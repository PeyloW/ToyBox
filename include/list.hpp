//
//  list.hpp
//  toybox
//
//  Created by Fredrik Olsson on 2024-03-24.
//

#pragma once

#include "static_allocator.hpp"
#include "utility.hpp"

namespace toybox {
    
    /**
     `list_c` is a minimal implementation of `std::forward_list` with a
     statically allocated backing store, for performance reasons.
     TODO: Treat Count of 0 as a dynamic list.
     */
    template<class Type, size_t Count = 16>
    class list_c {
    public:
        using value_type = Type;
        using pointer = value_type* ;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        struct _node_s {
            _node_s *next;
            value_type value;
            template<class... Args>
            _node_s(_node_s *next, Args&&... args) : next(next), value(forward<Args>(args)...) {}
            inline ~_node_s() = default;
            void *operator new(size_t count) {
                assert(allocator::alloc_size >= count);
                return allocator::allocate();
            }
            void operator delete(void *ptr) noexcept {
                allocator::deallocate(ptr);
            }
        };
        using allocator= static_allocator_c<_node_s, Count>;
        template<class TypeI>
        struct _iterator_s {
            using value_type = TypeI;
            using pointer = value_type*;
            using reference = value_type&;

            _iterator_s() = delete;
            _iterator_s(const _iterator_s& o) = default;
            __forceinline _iterator_s(_node_s *node) : _node(node) {}
            __forceinline _iterator_s(const _iterator_s<Type> &other) requires (!same_as<Type, TypeI>) : _node(other._node) {}

            __forceinline reference operator*() const { return _node->value; }
            __forceinline pointer operator->() const { return &_node->value; }
            __forceinline _iterator_s& operator++() { _node = _node->next; return *this; }
            __forceinline _iterator_s operator++(int) { auto tmp = *this; _node = _node->next; return tmp; }
            __forceinline bool operator==(const _iterator_s& o) const { return _node == o._node; }
            __forceinline bool operator!=(const _iterator_s& o) const { return _node != o._node; }
            
            _node_s *_node;
        };
        using iterator = _iterator_s<Type>;
        using const_iterator = _iterator_s<const Type>;
        
        list_c() { _head = nullptr; }
        ~list_c() { clear(); }
        
        inline bool empty() const __pure { return _head == nullptr; }
        void clear() {
            auto it = before_begin();
            while (it._node->next) {
                erase_after(it);
            }
        }
        
        inline reference front() __pure { return _head->value; }
        inline const_reference front() const __pure { return _head->value; }
        
        inline iterator before_begin() __pure {
            auto before_head = const_cast<_node_s**>(&_head);
            return iterator(reinterpret_cast<_node_s*>(before_head));
        }
        inline const_iterator before_begin() const __pure {
            auto before_head = const_cast<_node_s**>(&_head);
            return const_iterator(reinterpret_cast<_node_s*>(before_head));
        }
        inline iterator begin() __pure { return iterator(_head); }
        inline const_iterator begin() const __pure { return const_iterator(_head); }
        inline iterator end() __pure { return iterator(nullptr); }
        inline const_iterator end() const __pure { return const_iterator(nullptr); }
    
        inline void push_front(const_reference value) {
            insert_after(before_begin(), value);
        }
        template<class ...Args>
        inline reference emplace_front(Args&& ...args) {
            return *emplace_after(before_begin(), forward<Args>(args)...);
        }
        inline iterator insert_after(const_iterator pos, const_reference value) {
            assert(owns_node(pos._node));
            pos._node->next = new _node_s{pos._node->next, value};
            return iterator(pos._node->next);
        }
        template<class ...Args>
        inline iterator emplace_after(const_iterator pos, Args&& ...args) {
            assert(owns_node(pos._node));
            pos._node->next = new _node_s(pos._node->next, forward<Args>(args)...);
            return iterator(pos._node->next);
        }
        inline void pop_front() {
            erase_after(before_begin());
        }
        inline iterator erase_after(const_iterator pos) {
            assert(owns_node(pos._node));
            auto tmp = pos._node->next;
            pos._node->next = tmp->next;
            delete tmp;
            return iterator(pos._node->next);
        }
        void splice_after(const_iterator pos, list_c &other, const_iterator it) {
            assert(owns_node(pos._node));
            assert(other.owns_node(it._node));
            auto tmp = it._node->next;
            it._node->next = tmp->next;
            tmp->next = pos._node->next;
            pos._node->next = tmp;
        }
    private:
        bool owns_node(_node_s * node) const __pure {
            auto before_head = const_cast<_node_s**>(&_head);
            auto n = reinterpret_cast<_node_s*>(before_head);
            while (n) {
                if (n == node) return true;
                n = n->next;
            }
            return false;;
        }
        _node_s *_head;
    };
    
}
