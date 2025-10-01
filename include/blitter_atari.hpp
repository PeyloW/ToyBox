//
//  blitter_atari.hpp
//  ChromaGrid
//
//  Created by Fredrik on 2024-02-25.
//

#ifndef blitter_hpp
#define blitter_hpp

#include "cincludes.hpp"
#include "types.hpp"

namespace toybox {
    
    using namespace toybox;

#if TOYBOX_TARGET_ATARI
    
    /**
     A `blitter_s` is an abstraction of the Atari blitter.
     Emulated on emulation host.
     */
    struct blitter_s {
        enum class hop_e : uint8_t {
            one = 0,
            halftone = 1,
            src = 2,
            src_and_halftone = 3
        };
        
        enum class lop_e : uint8_t {
            zero = 0,
            src = 3,
            notsrc_and_dst = 4,
            src_or_dst = 7,
            one = 15
        };

        static const uint8_t skew_mask = 0x0f;
        static const uint8_t nfsr_bit = (1<<6);
        static const uint8_t fxsr_bit = (1<<7);
        
        static const uint8_t hog_bit = (1<<6);
        static const uint8_t busy_bit = (1<<7);

        volatile uint16_t halftoneRAM[16];
        volatile int16_t srcIncX;
        volatile int16_t srcIncY;
        volatile uint16_t *pSrc;
        volatile uint16_t endMask[3];
        volatile int16_t dstIncX;
        volatile int16_t dstIncY;
        volatile uint16_t *pDst;
        volatile uint16_t countX;
        volatile uint16_t countY;
        volatile hop_e  HOP;
        volatile lop_e  LOP;
        volatile uint8_t  mode;
        volatile uint8_t  skew;
        
        __forceinline uint8_t get_skew() const {
            return skew & skew_mask;
        }
        __forceinline bool is_nfsr() const {
            return (skew & nfsr_bit) != 0;
        }
        __forceinline bool is_fxsr() const {
            return (skew & fxsr_bit) != 0;
        }
        __forceinline uint16_t halftone() const {
            return halftoneRAM[mode & 0xf];
        }
#ifdef __M68000__
        inline void start(bool hog = false) {
            if (hog) {
                __asm__ volatile ("move.b #0xc0,0xffff8A3C.w \n\t"  : : : );
            } else {
                __asm__ volatile (
                                  "move.b #0x80,0xffff8A3C.w \n\t"
                                  "nop \n"
                                  ".Lrestart: bset.b #7,0xffff8A3C.w \n\t"
                                  "nop \n\t"
                                  "bne.s .Lrestart \n\t" : : : );
            }
        }
#else
        bool debug;
        void start(bool hog = false);
#endif
    };
    
#ifdef __M68000__
    static struct blitter_s *pBlitter = (struct blitter_s *)0xffff8a00;
#else
    extern struct blitter_s *pBlitter;
#endif
    
#else
#   error "Unsupported target"
#endif
    
}

#endif /* blitter_hpp */
