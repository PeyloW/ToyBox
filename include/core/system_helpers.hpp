//
//  system_helpers.hpp
//  toybox
//
//  Created by Fredrik on 2024-02-11.
//

#pragma once

#include "core/cincludes.hpp"

namespace toybox {
       
#define DEBUG_CPU_CLOCK_INTERUPT 0x011
#define DEBUG_CPU_MOUSE_INTERUPT 0x101
#if TOYBOX_DEBUG_CPU
    static __forceinline void debug_cpu_color(uint16_t c) {
        __asm__ volatile ("move.w %[d],0xffff8240.w" :  : [d] "g" (c) : );
    }
#else
    static void debug_cpu_color(uint16_t) { }
#endif

    static __forceinline void hard_crash() {
#ifdef __M68000__
        __asm__ volatile ("move.w #1,0xffff8241.w" :  :  : );
#else
        hard_assert(false);
#endif
    }

    template<typename T, const int Bytes = sizeof(T)>
    __forceinline void move_inc_to(T src, void* &dst) {
        static_assert(Bytes == 1 || Bytes == 2 || Bytes == 4);
#ifdef __M68000__
        if constexpr (Bytes == 1) {
            __asm__ volatile ("move.b %[src],(%[dst])+" : [dst] "+a" (dst) : [src] "dmi" (src) : );
        } else if constexpr (Bytes == 2) {
            __asm__ volatile ("move.w %[src],(%[dst])+" : [dst] "+a" (dst) : [src] "g" (src) : );
        } else {
            __asm__ volatile ("move.l %[src],(%[dst])+"  : [dst] "+a" (dst) : [src] "g" (src) : );
        }
#else
        memcpy(dst, &src, Bytes);
        dst = (void*)((uint8_t*)dst + Bytes);
#endif
    }
    template<typename T, const int Bytes = sizeof(T)>
    __forceinline void move_inc_to(T src, T* &dst) {
        move_inc_to(src, reinterpret_cast<void*&>(dst));
    }

    template<typename T, const int Bytes = sizeof(T)>
    __forceinline void move_inc_from(void*& src, T& dst) {
        static_assert(Bytes == 1 || Bytes == 2 || Bytes == 4);
#ifdef __M68000__
        if constexpr (Bytes == 1) {
            __asm__ volatile ("move.b (%[src])+,%[dst]" : [dst] "+dmi" (dst) : [src] "+a" (src) : );
        } else if constexpr (Bytes == 2) {
            __asm__ volatile ("move.w (%[src])+,%[dst]" : [dst] "+g" (dst) : [src] "+a" (src) : );
        } else {
            __asm__ volatile ("move.l (%[src])+,%[dst]" : [dst] "+g" (dst) : [src] "+a" (src) : );
        }
#else
        memcpy(&dst, src, Bytes);
        src = (void*)((uint8_t*)src + Bytes);
#endif
    }

    template<typename T, const int Bytes = sizeof(T)>
    __forceinline void move_inc_from_to(void* &src, void* &dst) {
        static_assert(Bytes == 1 || Bytes == 2 || Bytes == 4);
#ifdef __M68000__
        if constexpr (Bytes == 1) {
            __asm__ volatile ("move.b (%[src])+,(%[dst])+" : [dst] "+a" (dst) : [src] "+a" (src) : );
        } else if constexpr (Bytes == 2) {
            __asm__ volatile ("move.w (%[src])+,(%[dst])+" : [dst] "+a" (dst) : [src] "+a" (src) : );
        } else {
            __asm__ volatile ("move.l (%[src])+,(%[dst])+" : [dst] "+a" (dst) : [src] "+a" (src) : );
        }
#else
        memcpy(dst, src, Bytes);
        src = (void*)((uint8_t*)src + Bytes);
        dst = (void*)((uint8_t*)dst + Bytes);
#endif
    }

    template<typename T, const int Bytes = sizeof(T)>
    __forceinline void or_inc_to(T src, void* &dst) {
        static_assert(Bytes == 1 || Bytes == 2 || Bytes == 4);
#ifdef __M68000__
        if constexpr (Bytes == 1) {
            __asm__ volatile ("or.b %[src],(%[dst])+" : [dst] "+a" (dst) : [src] "d" (src) : );
        } else if constexpr (Bytes == 2) {
            __asm__ volatile ("or.w %[src],(%[dst])+" : [dst] "+a" (dst) : [src] "d" (src) : );
        } else {
            __asm__ volatile ("or.l %[src],(%[dst])+"  : [dst] "+a" (dst) : [src] "d" (src) : );
        }
#else
        memcpy(dst, &src, Bytes);
        dst = (void*)((uint8_t*)dst + Bytes);
#endif
    }
    template<typename T, const int Bytes = sizeof(T)>
    __forceinline void or_inc_to(T src, T* &dst) {
        return or_inc_to(src, reinterpret_cast<void*&>(dst));
    }

    template<typename T, const int Bytes = sizeof(T)>
    __forceinline void move_dec_from_to(void* &src, void* &dst) {
        static_assert(Bytes == 1 || Bytes == 2 || Bytes == 4);
#ifdef __M68000__
        if constexpr (Bytes == 1) {
            __asm__ volatile ("move.b -(%[src]),-(%[dst])" : [dst] "+a" (dst) : [src] "+a" (src) : );
        } else if constexpr (Bytes == 2) {
            __asm__ volatile ("move.w -(%[src])+,-(%[dst])" : [dst] "+a" (dst) : [src] "+a" (src) : );
        } else {
            __asm__ volatile ("move.l -(%[src])+,-(%[dst])" : [dst] "+a" (dst) : [src] "+a" (src) : );
        }
#else
        src = (void*)((uint8_t*)src - Bytes);
        dst = (void*)((uint8_t*)dst - Bytes);
        memcpy(dst, src, Bytes);
#endif
    }

#if __M68000__
#define do_dbra(var, from) \
    var = from; \
    __asm__ volatile ("" : : "d"(var) :); \
    do
#define while_dbra_count(var,count) \
    var = count; \
    __asm__ volatile ("" : : "d"(var) :); \
    while (--var != ((decltype(var))~0))
#else
#define do_dbra(var, from) \
    var = from; \
    hard_assert(var >= 0); \
    do
#define while_dbra_count(var,count) \
    var = count; \
    while (--var != ((decltype(var))~0))
#endif
#define while_dbra(var) \
    while (--var != ((decltype(var))~0))

#ifdef __M68000__
    struct codegen_s {
        // Buffer must be 16 bytes
        static void make_trampoline(void* buffer, void* func, bool all_regs) {
            //movem.l d3-d7/a2-a6,-(sp)
            //jsr     [func].l
            //movem.l (sp)+,d3-d7/a2-a6
            //rts
            if (all_regs) {
                move_inc_to((int32_t)0x48e7fffe, buffer);
            } else {
                move_inc_to((int32_t)0x48e71f3e, buffer);
            }
            move_inc_to((int16_t)0x4eb9, buffer);
            move_inc_to((int32_t)func, buffer);
            if (all_regs) {
                move_inc_to((int32_t)0x4cdf7fff, buffer);
            } else {
                move_inc_to((int32_t)0x4cdf7cf8, buffer);
            }
            move_inc_to((int16_t)0x4e75, buffer);
        }
    };
        
#endif
   
}
